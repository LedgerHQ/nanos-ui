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

const bagl_element_t screen_onboarding_7_processing_1_elements[] = {
    // erase
    {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x00, 17, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CHECK_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 39, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Your device",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 38, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "is now ready",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

unsigned int
screen_onboarding_7_processing_1_button(unsigned int button_mask,
                                        unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.exit_code = BOLOS_UX_OK;
        break;
    }
    return 0;
}

// screen displayed, start bip39 seed derivation
unsigned int screen_onboarding_7_processing_displayed(unsigned int i) {
    UNUSED(i);

    // finalise the exchange
    io_seproxyhal_general_status();

    // ================================================================================
    // derivation

    os_perso_derive_and_set_seed(0, NULL, 0, NULL, 0,
                                 G_bolos_ux_context.words_buffer,
                                 G_bolos_ux_context.words_buffer_length);

    // store the pin (the pin length has been stored as the first pin char)
    os_perso_set_pin(0, (unsigned char *)G_bolos_ux_context.pin_buffer + 1,
                     G_bolos_ux_context.pin_buffer[0]);

    // wipe passphrase & derivated seed
    os_memset(G_bolos_ux_context.words_buffer, 0,
              sizeof(G_bolos_ux_context.words_buffer));
    os_memset(G_bolos_ux_context.string_buffer, 0,
              sizeof(G_bolos_ux_context.string_buffer));

    // finalize onboarding
    os_perso_finalize();

    // validate pin here to ensure going straight to dashboard
    os_global_pin_check((unsigned char *)G_bolos_ux_context.pin_buffer + 1,
                        G_bolos_ux_context.pin_buffer[0]);
    // wipe pin memory; just in case
    os_memset(G_bolos_ux_context.pin_buffer, 0,
              sizeof(G_bolos_ux_context.pin_buffer));

    // ================================================================================
    // ASSERTION, an event (at least ticker) has been received here

    // kthx, but no overflow allowed
    if (G_bolos_ux_context.canary != CANARY_MAGIC) {
        reset();
    }

    // consume the timer event before being able to reply
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer,
                           sizeof(G_io_seproxyhal_spi_buffer), 0);

    screen_state_init(0);

    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_onboarding_7_processing_1_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_7_processing_1_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_onboarding_7_processing_1_button;

    screen_display_init(0);
    // we've emitted some data, don't consider display has finished
    return 0;
}

void screen_onboarding_7_processing_init(void) {
    screen_processing_init();

    G_bolos_ux_context.screen_stack[0].displayed_callback =
        screen_onboarding_7_processing_displayed;
    G_bolos_ux_context.screen_stack[0].exit_code_after_elements_displayed =
        BOLOS_UX_CONTINUE;
}

#endif // OS_IO_SEPROXYHAL
