#include "Section.h"

#include "License.h"
#include "TimeAttackGhostListPage.h"

PATCH_S16(Section_createPage, 0x92e, sizeof(LicenseSelectPage));
PATCH_S16(Section_createPage, 0xa4e, sizeof(TimeAttackGhostListPage));
