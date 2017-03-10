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

const ux_turner_step_t screen_settings_passphrase_attach_1_steps[] = {
    {NULL, 0, "This feature is for", 0, "advanced users.", 0, 0, 3000},
    {NULL, 0, "Read the F.A.Q.", 0, "before proceeding.", 0, 0, 3000},
    {NULL, 0, "Would you like", 0, "to continue?", 0, 0, 3000},
};

unsigned int
screen_settings_passphrase_attach_1_button(unsigned int button_mask,
                                           unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    // abort
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        // go back to the settings entry
        screen_settings_attach_to_pin();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        screen_settings_passphrase_attach_2_init(1);
        break;
    }
    return 1;
}

void screen_settings_passphrase_attach_1_init(void) {
    // wipe passphrase (we don't store the pin inside it in the first hand)
    os_memset(G_bolos_ux_context.words_buffer, 0,
              sizeof(G_bolos_ux_context.words_buffer));

    UX_TURNER_INIT();
    UX_TURNER_DISPLAY(0, screen_settings_passphrase_attach_1_steps,
                      ARRAYLEN(screen_settings_passphrase_attach_1_steps),
                      screen_settings_passphrase_attach_1_button);
}

#endif // OS_IO_SEPROXYHAL
