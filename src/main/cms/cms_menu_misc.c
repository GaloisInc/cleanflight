/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"

#ifdef USE_CMS

#include "build/debug.h"
#include "build/version.h"

#include "drivers/time.h"

#include "cms/cms.h"
#include "cms/cms_types.h"
#include "cms/cms_menu_ledstrip.h"

#include "common/utils.h"

#include "config/feature.h"
#include "pg/pg.h"
#include "pg/pg_ids.h"
#include "pg/rx.h"

#include "fc/config.h"
#include "fc/rc_controls.h"

#include "flight/mixer.h"

#include "rx/rx.h"

#include "sensors/battery.h"

//
// Misc
//

static long cmsx_menuRcConfirmBack(const OSD_Entry *self)
{
    if (self && self->type == OME_Back)
        return 0;
    else
        return -1;
}

static OSD_INT16_t roll_data  = { &rcData[ROLL],     1, 2500, 0 };
static OSD_INT16_t pitch_data = { &rcData[PITCH],    1, 2500, 0 };
static OSD_INT16_t thr_data   = { &rcData[THROTTLE], 1, 2500, 0 };
static OSD_INT16_t yaw_data   = { &rcData[YAW],      1, 2500, 0 };

static OSD_INT16_t aux1_data  = { &rcData[AUX1],     1, 2500, 0 };
static OSD_INT16_t aux2_data  = { &rcData[AUX2],     1, 2500, 0 };
static OSD_INT16_t aux3_data  = { &rcData[AUX3],     1, 2500, 0 };
static OSD_INT16_t aux4_data  = { &rcData[AUX4],     1, 2500, 0 };


//
// RC preview
//
static OSD_Entry cmsx_menuRcEntries[] =
{
    { "-- RC PREV --", OME_Label, NULL, NULL, 0},

    { "ROLL",  OME_INT16, NULL, &roll_data , DYNAMIC },
    { "PITCH", OME_INT16, NULL, &pitch_data, DYNAMIC },
    { "THR",   OME_INT16, NULL, &thr_data  , DYNAMIC },
    { "YAW",   OME_INT16, NULL, &yaw_data  , DYNAMIC },

    { "AUX1",  OME_INT16, NULL, &aux1_data , DYNAMIC },
    { "AUX2",  OME_INT16, NULL, &aux2_data , DYNAMIC },
    { "AUX3",  OME_INT16, NULL, &aux3_data , DYNAMIC },
    { "AUX4",  OME_INT16, NULL, &aux4_data , DYNAMIC },

    { "BACK",  OME_Back, NULL, NULL, 0},
    {NULL, OME_END, NULL, NULL, 0}
};

CMS_Menu cmsx_menuRcPreview = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "XRCPREV",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = NULL,
    .onExit = cmsx_menuRcConfirmBack,
    .entries = cmsx_menuRcEntries
};

static uint16_t motorConfig_minthrottle;
static uint8_t motorConfig_digitalIdleOffsetValue;
static debugType_e systemConfig_debug_mode;

static long cmsx_menuMiscOnEnter(void)
{
    motorConfig_minthrottle = motorConfig()->minthrottle;
    motorConfig_digitalIdleOffsetValue = motorConfig()->digitalIdleOffsetValue / 10;
    systemConfig_debug_mode = systemConfig()->debug_mode;

    return 0;
}

static long cmsx_menuMiscOnExit(const OSD_Entry *self)
{
    UNUSED(self);

    motorConfigMutable()->minthrottle = motorConfig_minthrottle;
    motorConfigMutable()->digitalIdleOffsetValue = 10 * motorConfig_digitalIdleOffsetValue;
    systemConfigMutable()->debug_mode = systemConfig_debug_mode;

    return 0;
}

static OSD_UINT16_t min_thr_data      = { &motorConfig_minthrottle,              1000, 2000, 1    };
static OSD_UINT8_t  digital_idle_data = { &motorConfig_digitalIdleOffsetValue,      0,  200, 1    };
static OSD_TAB_t    debug_mode_data   = { &systemConfig_debug_mode, DEBUG_COUNT - 1, debugModeNames };

static OSD_Entry menuMiscEntries[]=
{
    { "-- MISC --", OME_Label, NULL, NULL, 0 },

    { "MIN THR",      OME_UINT16,  NULL, &min_thr_data,      0 },
    { "DIGITAL IDLE", OME_UINT8,   NULL, &digital_idle_data,      0 },
    { "DEBUG MODE",   OME_TAB,     NULL, &debug_mode_data,      0 },
    { "RC PREV",      OME_Submenu, cmsMenuChange, &cmsx_menuRcPreview, 0},

    { "BACK", OME_Back, NULL, NULL, 0},
    { NULL, OME_END, NULL, NULL, 0}
};

CMS_Menu cmsx_menuMisc = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "XMISC",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_menuMiscOnEnter,
    .onExit = cmsx_menuMiscOnExit,
    .entries = menuMiscEntries
};

#endif // CMS
