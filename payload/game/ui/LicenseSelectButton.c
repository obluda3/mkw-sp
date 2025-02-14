#include "LicenseSelectButton.h"

#include "LicenseSelectPage.h"
#include "SectionManager.h"

#include "../system/SaveManager.h"

#include <stdio.h>
#include <string.h>

static void onFront(PushButtonHandler *UNUSED(this), PushButton *base, u32 UNUSED(localPlayerId)) {
    LicenseSelectButton *button = (LicenseSelectButton *)base;

    GlobalContext *cx = s_sectionManager->globalContext;
    GlobalContext_changeLicense(cx);

    u32 index = button->index;
    if (index < s_saveManager->spLicenseCount) {
        s_saveManager->spCurrentLicense = index;
        const Mii *mii = MiiGroup_get(&button->miiGroup, 0);
        if (mii) {
            SaveManager_createLicense(s_saveManager, 0, &mii->id, mii->name);
            SaveManager_selectLicense(s_saveManager, 0);
            MiiGroup_copy(&cx->localPlayerMiis, &button->miiGroup, 0, 0);
            LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
            page->replacement = PAGE_ID_TOP_MENU;
            f32 delay = PushButton_getDelay(button);
            Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
        } else {
            Section *currentSection = s_sectionManager->currentSection;
            ConfirmPage *confirmPage = (ConfirmPage *)currentSection->pages[PAGE_ID_CONFIRM];
            ConfirmPage_reset(confirmPage);
            ConfirmPage_setTitleMessage(confirmPage, 2200, NULL);
            ConfirmPage_setWindowMessage(confirmPage, 2205, NULL);
            confirmPage->onConfirm = &button->onChangeConfirm;
            confirmPage->onCancel = &button->onCancel;
            LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
            page->replacement = PAGE_ID_CONFIRM;
            f32 delay = PushButton_getDelay(button);
            Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
        }
    } else if (index == s_saveManager->spLicenseCount) {
        s_saveManager->spCurrentLicense = -1;
        Section *currentSection = s_sectionManager->currentSection;
        ConfirmPage *confirmPage = (ConfirmPage *)currentSection->pages[PAGE_ID_CONFIRM];
        ConfirmPage_reset(confirmPage);
        ConfirmPage_setTitleMessage(confirmPage, 2102, NULL);
        ConfirmPage_setWindowMessage(confirmPage, 2101, NULL);
        confirmPage->onConfirm = &button->onCreateConfirm;
        confirmPage->onCancel = &button->onCancel;
        LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
        page->replacement = PAGE_ID_CONFIRM;
        f32 delay = PushButton_getDelay(button);
        Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
    }
}

static const PushButtonHandler_vt onFront_vt = {
    .handle = onFront,
};

static void onCreateConfirm(ConfirmPageHandler *UNUSED(this), ConfirmPage *UNUSED(confirmPage),
        f32 delay) {
    SectionManager_setNextSection(s_sectionManager, SECTION_ID_MII_SELECT_CREATE,
            PAGE_ANIMATION_NEXT);
    SectionManager_startChangeSection(s_sectionManager, delay, 0x000000ff);
}

static const ConfirmPageHandler_vt onCreateConfirm_vt = {
    .handle = onCreateConfirm,
};

static void onChangeConfirm(ConfirmPageHandler *UNUSED(this), ConfirmPage *UNUSED(confirmPage),
        f32 delay) {
    SectionManager_setNextSection(s_sectionManager, SECTION_ID_MII_SELECT_CHANGE,
            PAGE_ANIMATION_NEXT);
    SectionManager_startChangeSection(s_sectionManager, delay, 0x000000ff);
}

static const ConfirmPageHandler_vt onChangeConfirm_vt = {
    .handle = onChangeConfirm,
};

static void onCancel(ConfirmPageHandler *UNUSED(this), ConfirmPage *confirmPage,
        f32 UNUSED(delay)) {
    confirmPage->replacement = PAGE_ID_LICENSE_SELECT;
}

static const ConfirmPageHandler_vt onCancel_vt = {
    .handle = onCancel,
};

LicenseSelectButton *LicenseSelectButton_ct(LicenseSelectButton *this) {
    PushButton_ct(this);

    MiiGroup_ct(&this->miiGroup);
    this->onFront.vt = &onFront_vt;
    this->onCreateConfirm.vt = &onCreateConfirm_vt;
    this->onChangeConfirm.vt = &onChangeConfirm_vt;
    this->onCancel.vt = &onCancel_vt;

    return this;
}

void LicenseSelectButton_dt(LicenseSelectButton *this, s32 type) {
    MiiGroup_dt(&this->miiGroup, -1);

    PushButton_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

void LicenseSelectButton_load(LicenseSelectButton *this, u32 index) {
    char variant[0x9];
    snprintf(variant, sizeof(variant), "License%lu", index);
    PushButton_load(this, "button", "LicenseSelect", variant, 0x1, false, false);
    MiiGroup_init(&this->miiGroup, 1, 0x1, NULL);
    if (index < s_saveManager->spLicenseCount) {
        LayoutUIControl_setPaneVisible(this, "new", false);
        MiiGroup_insertFromId(&this->miiGroup, 0, &s_saveManager->spLicenses[index]->miiId);
        LayoutUIControl_setPaneVisible(this, "mii", true);
        LayoutUIControl_setMiiPicture(this, "mii", &this->miiGroup, 0, 0);
        MessageInfo info = {
            .miis[0] = MiiGroup_get(&this->miiGroup, 0),
        };
        LayoutUIControl_setMessage(this, "player", 9501, &info);
    } else if (index == s_saveManager->spLicenseCount) {
        LayoutUIControl_setPaneVisible(this, "new", true);
        LayoutUIControl_setMessage(this, "new", 6017, NULL);
        LayoutUIControl_setPaneVisible(this, "mii", false);
    } else {
        this->isHidden = true;
        PushButton_setPlayerFlags(this, 0x0);
    }
    this->index = index;
    PushButton_setFrontHandler(this, &this->onFront, false);
}
