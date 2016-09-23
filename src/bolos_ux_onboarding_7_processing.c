/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016 Ledger
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

const bagl_element_t screen_onboarding_7_processing_0_elements[] = {
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

    {{BAGL_ICON, 0x00, 10, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LOADING_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 30, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Configuration...",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    /*
      {{BAGL_LINE                           , 0x00,  16,   9,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  12,  11,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  10,  15,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  12,  19,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  16,  21,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  20,  19,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  22,  15,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_LINE                           , 0x00,  20,  11,   2,   2, 0, 0, 0
      , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
    */
};

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

    // setup a ticker event to be able to switch to next screen after derivation
    // (the event will be received while derivating, and be replied afterward)
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 2;
    G_io_seproxyhal_spi_buffer[3] = 0;
    G_io_seproxyhal_spi_buffer[4] = 0xFF; // 255ms
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);

    // finalise the exchange
    io_seproxyhal_general_status();

    // ================================================================================
    // derivation

    // store the pin
    os_perso_set_pin((unsigned char *)G_bolos_ux_context.pin_buffer, 4);

    // store the seed and the word list
    os_perso_set_words((unsigned char *)G_bolos_ux_context.words_buffer,
                       G_bolos_ux_context.words_buffer_length);

    // TODO store the device's name

    // pbkdf2 the seed
    // TODO try/catch me
    bolos_ux_mnemonic_to_seed(
        (unsigned char *)G_bolos_ux_context.words_buffer,
        G_bolos_ux_context.words_buffer_length,
        (unsigned char *)G_bolos_ux_context.passphrase_buffer,
        strlen(G_bolos_ux_context.passphrase_buffer),
        (unsigned char *)G_bolos_ux_context.string_buffer);

#ifdef BOLOS_UX_ONBOARDING_DEBUG
    screen_printf("pbkdf2 %.*H\n", 64, G_bolos_ux_context.string_buffer);
#endif // BOLOS_UX_ONBOARDING_DEBUG

    // Store the derived seed
    os_perso_set_seed(G_bolos_ux_context.string_buffer, 64);

    // wipe passphrase & derivated seed
    os_memset(G_bolos_ux_context.words_buffer, 0,
              sizeof(G_bolos_ux_context.words_buffer));
    os_memset(G_bolos_ux_context.string_buffer, 0,
              sizeof(G_bolos_ux_context.string_buffer));

    // finalize onboarding
    os_perso_finalize();

    // validate pin here to ensure going straight to dashboard
    os_global_pin_check((unsigned char *)G_bolos_ux_context.pin_buffer, 4);

    os_memset(G_bolos_ux_context.pin_buffer, 0,
              sizeof(G_bolos_ux_context.pin_buffer));

    // ================================================================================
    // ASSERTION, an event (at least ticker) has been received here

    // kthx, but no
    if (G_bolos_ux_context.canary != CANARY_MAGIC) {
        reset();
    }

    // consume the timer event before being able to reply
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer,
                           sizeof(G_io_seproxyhal_spi_buffer), 0);

    // stop ticker
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 2;
    G_io_seproxyhal_spi_buffer[3] = 0;
    G_io_seproxyhal_spi_buffer[4] = 0;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);

    screen_state_init();

    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_onboarding_7_processing_1_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_7_processing_1_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    G_bolos_ux_context.button_push_callback =
        screen_onboarding_7_processing_1_button;

    screen_display_init();
    // we've emitted some data, don't consider display has finished
    return 0;
}

void screen_onboarding_7_processing_init(void) {
    screen_state_init();

    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_onboarding_7_processing_0_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_7_processing_0_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    G_bolos_ux_context.screen_displayed_callback =
        screen_onboarding_7_processing_displayed;

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
