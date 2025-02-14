#include "LicenseSettingsPage.h"

#include "SectionManager.h"

#include "../system/SaveManager.h"

#include <stdio.h>

static const Page_vt s_LicenseSettingsPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    LicenseSettingsPage *page = CONTAINER_OF(this, LicenseSettingsPage, onBack);
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onSettingControlFront(RadioButtonControlHandler *this, RadioButtonControl *control,
        u32 UNUSED(localPlayerId), s32 selected) {
    SpSaveLicense *license = s_saveManager->spLicenses[s_saveManager->spCurrentLicense];
    switch (control->index) {
    case 0:
        license->settingHudLabels = selected;
        break;
    case 1:
        license->setting169Fov = selected;
        break;
    case 2:
        license->settingMapIcons = selected;
        break;
    case 3:
        license->settingPageTransitions = selected;
        break;
    case 4:
        license->settingRaceInputDisplay = selected;
        break;
    }

    LicenseSettingsPage *page = CONTAINER_OF(this, LicenseSettingsPage, onSettingControlFront);
    if (control->index < ARRAY_SIZE(page->settingControls) - 1) {
        RadioButtonControl_select(&page->settingControls[control->index + 1], 0);
    } else {
        page->resetSelection = true;
        // TODO delay?
        Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
    }
}

static const RadioButtonControlHandler_vt onSettingControlFront_vt = {
    .handle = onSettingControlFront,
};

static void onSettingControlSelect(RadioButtonControlHandler *this, RadioButtonControl *control,
        u32 UNUSED(localPlayerId), s32 selected) {
    if (selected < 0) {
        return;
    }

    LicenseSettingsPage *page = CONTAINER_OF(this, LicenseSettingsPage, onSettingControlSelect);

    if (page->enableInstructionText) {
        u32 messageIds[][2] = {
            { 10007, 10008 },
            { 10012, 10013 },
            { 10017, 10018 },
            { 10059, 10060 },
            { 10064, 10065 },
        };
        u32 messageId = messageIds[control->index][selected];
        CtrlMenuInstructionText_setMessage(&page->instructionText, messageId, NULL);
    }
}

static const RadioButtonControlHandler_vt onSettingControlSelect_vt = {
    .handle = onSettingControlSelect,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    LicenseSettingsPage *page = CONTAINER_OF(this, LicenseSettingsPage, onBackButtonFront);
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

extern void LicenseRecordsPage_ct;

static LicenseSettingsPage *LicenseSettingsPage_ct(LicenseSettingsPage *this) {
    Page_ct(this);
    this->vt = &s_LicenseSettingsPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    CtrlMenuInstructionText_ct(&this->instructionText);
    CtrlMenuBackButton_ct(&this->backButton);
    for (u32 i = 0; i < ARRAY_SIZE(this->settingControls); i++) {
        RadioButtonControl_ct(&this->settingControls[i]);
    }
    this->onBack.vt = &onBack_vt;
    this->onSettingControlFront.vt = &onSettingControlFront_vt;
    this->onSettingControlSelect.vt = &onSettingControlSelect_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    this->enableInstructionText = true;
    this->enableBackButton = true;
    this->replacementPage = PAGE_ID_LICENSE_SETTINGS;

    return this;
}
PATCH_B(LicenseRecordsPage_ct, LicenseSettingsPage_ct);

static void LicenseSettingsPage_dt(Page *base, s32 type) {
    LicenseSettingsPage *this = (LicenseSettingsPage *)base;

    for (u32 i = ARRAY_SIZE(this->settingControls); i --> 0;) {
        RadioButtonControl_dt(&this->settingControls[i], -1);
    }
    CtrlMenuBackButton_dt(&this->backButton, -1);
    CtrlMenuInstructionText_dt(&this->instructionText, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 LicenseSettingsPage_getReplacement(Page *base) {
    LicenseSettingsPage *this = (LicenseSettingsPage *)base;
    
    return this->replacementPage;
}

static void LicenseSettingsPage_onInit(Page *base) {
    LicenseSettingsPage *this = (LicenseSettingsPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 1 + this->enableInstructionText + this->enableBackButton + ARRAY_SIZE(this->settingControls));

    u32 id = 0;
    Page_insertChild(this, id++, &this->pageTitleText, 0);
    if (this->enableInstructionText) {
        Page_insertChild(this, id++, &this->instructionText, 0);
    }
    if (this->enableBackButton) {
        Page_insertChild(this, id++, &this->backButton, 0);
    }
    for (u32 i = 0; i < ARRAY_SIZE(this->settingControls); i++) {
        Page_insertChild(this, id++, &this->settingControls[i], 0);
    }

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    if (this->enableInstructionText) {
        CtrlMenuInstructionText_load(&this->instructionText);
    }
    if (this->enableBackButton) {
        PushButton_load(&this->backButton, "button", "Back", "ButtonBackPopup", 0x1, false, true);
    }

    const char *settingNames[] = {
        "HudLabels",
        "169Fov",
        "MapIcons",
        "PageTransitions",
        "RaceInputDisplay",
    };
    for (u32 i = 0; i < ARRAY_SIZE(this->settingControls); i++) {
        const SpSaveLicense *license = s_saveManager->spLicenses[s_saveManager->spCurrentLicense];
        u32 chosen;
        switch (i) {
        case 0:
            chosen = license->settingHudLabels;
            break;
        case 1:
            chosen = license->setting169Fov;
            break;
        case 2:
            chosen = license->settingMapIcons;
            break;
        case 3:
            chosen = license->settingPageTransitions;
            break;
        case 4:
            chosen = license->settingRaceInputDisplay;
            break;
        }
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Radio%s", settingNames[i]);
        char buffers[2][0x20];
        const char *buttonVariants[2];
        for (u32 j = 0; j < 2; j++) {
            snprintf(buffers[j], sizeof(*buffers), "Option%s%lu", settingNames[i], j);
            buttonVariants[j] = buffers[j];
        }
        RadioButtonControl_load(&this->settingControls[i], 2, chosen, "control",
                "LicenseSettingRadioBase", variant, "LicenseSettingRadioOption", buttonVariants,
                0x1, false, false);
        this->settingControls[i].index = i;
    }

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    for (u32 i = 0; i < ARRAY_SIZE(this->settingControls); i++) {
        RadioButtonControl *control = &this->settingControls[i];
        RadioButtonControl_setFrontHandler(control, &this->onSettingControlFront);
        RadioButtonControl_setSelectHandler(control, &this->onSettingControlSelect);
    }
    if (this->enableBackButton) {
        PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);
    }

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 2015, NULL);

    this->resetSelection = true;
}

static void LicenseSettingsPage_onDeinit(Page *UNUSED(base)) {
    SaveManagerProxy_markLicensesDirty(s_sectionManager->saveManagerProxy);
}

static void LicenseSettingsPage_onActivate(Page *base) {
    LicenseSettingsPage *this = (LicenseSettingsPage *)base;

    if (this->resetSelection) {
        RadioButtonControl_selectDefault(&this->settingControls[0], 0);
        if (this->enableInstructionText) {
            u32 messageId = 10007 + this->settingControls[0].selected;
            CtrlMenuInstructionText_setMessage(&this->instructionText, messageId, NULL);
        }
        this->resetSelection = false;
    }
}

static const Page_vt s_LicenseSettingsPage_vt = {
    .dt = LicenseSettingsPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = LicenseSettingsPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = LicenseSettingsPage_onInit,
    .onDeinit = LicenseSettingsPage_onDeinit,
    .onActivate = LicenseSettingsPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .beforeCalc = Page_beforeCalc,
    .afterCalc = Page_afterCalc,
    .vf_50 = &Page_vf_50,
    .onRefocus = Page_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .getTypeInfo = Page_getTypeInfo,
};
