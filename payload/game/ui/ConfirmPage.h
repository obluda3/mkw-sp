#pragma once

#include "Page.h"

struct ConfirmPage;

struct ConfirmPageHandler;

typedef struct {
    u8 _0[0x8 - 0x0];
    void (*handle)(struct ConfirmPageHandler *handler, struct ConfirmPage *page, f32 delay);
} ConfirmPageHandler_vt;
static_assert(sizeof(ConfirmPageHandler_vt) == 0xc);

typedef struct ConfirmPageHandler {
    const ConfirmPageHandler_vt *vt;
} ConfirmPageHandler;

typedef struct ConfirmPage {
    Page;
    u8 _044[0x820 - 0x044];
    ConfirmPageHandler *onConfirm;
    ConfirmPageHandler *onCancel;
    u8 _828[0xa54 - 0x828];
    s32 replacement;
} ConfirmPage;
static_assert(sizeof(ConfirmPage) == 0xa58);

void ConfirmPage_reset(ConfirmPage *this);

void ConfirmPage_setTitleMessage(ConfirmPage *this, u32 messageId, MessageInfo *info);

void ConfirmPage_setWindowMessage(ConfirmPage *this, u32 messageId, MessageInfo *info);
