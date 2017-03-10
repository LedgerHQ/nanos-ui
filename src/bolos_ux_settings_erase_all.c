/*******************************************************************************
*   Ledger Blue - Secure firmware
*   (c) 2016, 2017 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "os.h"
#include "cx.h"

#include "os_io_seproxyhal.h"
#include "string.h"

#include "bolos_ux_common.h"

#ifdef OS_IO_SEPROXYHAL

const ux_turner_step_t screen_settings_erase_all_steps[] = {
    {NULL, 0, "Your device apps", 0, "and configuration", 0, 0, 3000},
    {NULL, 0, "will be erased", 0, "(PIN is required).", 0, 0, 3000},
    {NULL, 0, "Use your recovery", 0, "sheet to restore", 0, 0, 3000},
    {NULL, 0, "your accounts on", 0, "any Ledger device.", 0, 0, 3000},
    {NULL, 0, "Would you like", 0, "to continue?", 0, 0, 3000},
};

unsigned int screen_settings_erase_all_perform(unsigned int ignored) {
    UNUSED(ignored);
    // wait a bit
    // avoid future callback
    G_bolos_ux_context.screen_stack[0].ticker_callback = NULL;

    // perform the wipe and go to onboarding
    os_perso_erase_all();

    if (os_flags() & OS_FLAG_RECOVERY) {
        G_bolos_ux_context.dashboard_redisplayed = 1;
        screen_dashboard_init();
    } else {
        screen_onboarding_0_welcome_init();
    }
    return 0;
}

unsigned int
screen_settings_erase_all_display_processing(unsigned int ignored) {
    UNUSED(ignored);
    // erase all and reboot
    os_perso_erase_all();
    io_seproxyhal_se_reset();
    for (;;)
        ;
}

unsigned int
screen_settings_erase_all_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    // abort
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        // go back to the settings entry
        screen_settings_erase_all();
        break;

    // erase
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        if (os_perso_isonboarded()) {
            // ask pin !
            // display change pin
            screen_modal_validate_pin_init();
        }

        // when pin is correcly entered (screen stack slot is popped), then
        screen_state_init(0);
        G_bolos_ux_context.screen_stack[0].displayed_callback =
            screen_settings_erase_all_display_processing;
        break;
    }
    return 1;
}

void screen_settings_erase_all_init(void) {
    UX_TURNER_INIT();
    UX_TURNER_DISPLAY(0, screen_settings_erase_all_steps,
                      ARRAYLEN(screen_settings_erase_all_steps),
                      screen_settings_erase_all_button);
}

#endif // OS_IO_SEPROXYHAL
