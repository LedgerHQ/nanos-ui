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

#ifdef HAVE_ELECTRUM

const bagl_element_t screen_onboarding_3_processing_0_elements[] = {
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
     "Processing...",
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

#endif

const bagl_element_t screen_onboarding_3_new_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 6, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
     "3.",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 26, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Write down your",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 25, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "recovery phrase",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_3_new_words_elements[] = {
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

    {{BAGL_LABELINE, 0x01, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x02, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // left/rights icons
    {{BAGL_ICON, 0x03, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x04, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    //{{BAGL_ICON                           , 0x05, 117,  13,   8,   6, 0, 0, 0
    //, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CHECK  }, NULL, 0, 0, 0, NULL,
    //NULL, NULL },
};

void screen_onboarding_3_new_init_internal(void);

#ifdef HAVE_ELECTRUM

// screen displayed, start Electrum seed generation
unsigned int screen_onboarding_3_processing_0_displayed(unsigned int i) {
    UNUSED(i);

    // finalise the exchange
    io_seproxyhal_general_status();

    // ================================================================================

    G_bolos_ux_context.words_buffer_length = bolos_ux_electrum_new_mnemonic(
        ELECTRUM_SEED_PREFIX_STANDARD,
        (unsigned char *)G_bolos_ux_context.words_buffer,
        sizeof(G_bolos_ux_context.words_buffer));

    // ================================================================================
    // ASSERTION, an event (at least ticker) has been received here

    // kthx, but no
    if (G_bolos_ux_context.canary != CANARY_MAGIC) {
        reset();
    }

    // consume the timer event before being able to reply
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer,
                           sizeof(G_io_seproxyhal_spi_buffer), 0);

    screen_state_init(0);

    screen_onboarding_3_new_init_internal();

    // we've emitted some data, don't consider display has finished
    return 0;
}

#endif

const bagl_element_t *screen_onboarding_3_new_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    // word index element
    case 0x01:
        // change next button if last page
        strcpy(G_bolos_ux_context.string_buffer, "Word #");
        if (G_bolos_ux_context.onboarding_step + 1 < 10) {
            G_bolos_ux_context.string_buffer[6 + 0] =
                '0' + G_bolos_ux_context.onboarding_step + 1;
            G_bolos_ux_context.string_buffer[6 + 1] = 0;
        } else {
            G_bolos_ux_context.string_buffer[6 + 0] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) / 10);
            G_bolos_ux_context.string_buffer[6 + 1] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) % 10);
            G_bolos_ux_context.string_buffer[6 + 2] = 0;
        }
        break;

    // word element
    case 0x02: {
        // word value
        unsigned char *word = (unsigned char *)G_bolos_ux_context.words_buffer;
        unsigned int word_length =
            bolos_ux_get_word_ptr(&word, G_bolos_ux_context.words_buffer_length,
                                  G_bolos_ux_context.onboarding_step);

        // copy the word to display
        os_memmove(G_bolos_ux_context.string_buffer, word, word_length);
        G_bolos_ux_context.string_buffer[word_length] = 0; // set EOS
        break;
    }

    // left button
    case 0x03:
        if (G_bolos_ux_context.onboarding_step == 0) {
            return NULL; // don't display
        }
        break;

    // right button
    case 0x04:
        if (G_bolos_ux_context.onboarding_step ==
            G_bolos_ux_context.onboarding_kind - 1) {
            return NULL; // don't display
        }
        break;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return element;
}

unsigned int
screen_onboarding_3_new_words_button(unsigned int button_mask,
                                     unsigned int button_mask_counter);

unsigned int screen_onboarding_3_new_button(unsigned int button_mask,
                                            unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_3_new_words_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_3_new_words_elements);
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_onboarding_3_new_words_button;
        screen_display_init(0);
        break;
    }
    return 0;
}

unsigned int
screen_onboarding_3_new_words_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // not on the last word
        if (G_bolos_ux_context.onboarding_step !=
            G_bolos_ux_context.onboarding_kind - 1) {
            break;
        }
        // perform phrase validation
        screen_onboarding_4_confirm_init();
        break;

    case BUTTON_EVT_FAST | BUTTON_LEFT:
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        if (G_bolos_ux_context.onboarding_step <= 0) {
            // nothing :)
            break;
        }
        G_bolos_ux_context.onboarding_step--;
        goto redraw;

    case BUTTON_EVT_FAST | BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        // no auto pass to next step for last confirmation
        if (G_bolos_ux_context.onboarding_step ==
            G_bolos_ux_context.onboarding_kind - 1) {
            break;
        }
        G_bolos_ux_context.onboarding_step++;

    redraw:
        // display next word
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_3_new_words_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_3_new_words_elements);
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_onboarding_3_new_words_button;
        screen_display_init(0);
        break;
    }
    return 0;
}

void screen_onboarding_3_new_init_internal(void) {
    // start at word 0
    G_bolos_ux_context.onboarding_step = 0;

    // register action callbacks
    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_onboarding_3_new_button;
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_onboarding_3_new_before_element_display_callback;

    // elements to be displayed
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_onboarding_3_new_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_3_new_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    screen_display_init(0);
}

void screen_onboarding_3_new_init(void) {
    screen_state_init(0);

    // generate a new seed, and the word list
    os_memset(G_bolos_ux_context.words_buffer, 0,
              sizeof(G_bolos_ux_context.words_buffer));

#ifdef HAVE_ELECTRUM

    if (G_bolos_ux_context.onboarding_algorithm ==
        BOLOS_UX_ONBOARDING_ALGORITHM_ELECTRUM) {
        G_bolos_ux_context.onboarding_kind = BOLOS_UX_ONBOARDING_NEW_12;

        screen_processing_init();
        G_bolos_ux_context.screen_stack[0].screen_displayed_callback =
            screen_onboarding_3_processing_0_displayed;
        G_bolos_ux_context.screen_stack[0].exit_code_after_elements_displayed =
            BOLOS_UX_CONTINUE;
    } else {
        G_bolos_ux_context.onboarding_kind = BOLOS_UX_ONBOARDING_NEW_24;
        cx_rng((unsigned char *)G_bolos_ux_context.string_buffer, 32);
        G_bolos_ux_context.words_buffer_length = bolos_ux_mnemonic_from_data(
            (unsigned char *)G_bolos_ux_context.string_buffer, 32,
            (unsigned char *)G_bolos_ux_context.words_buffer,
            sizeof(G_bolos_ux_context.words_buffer));
        screen_onboarding_3_new_init_internal();
    }

#else

    G_bolos_ux_context.onboarding_kind = BOLOS_UX_ONBOARDING_NEW_24;
    cx_rng((unsigned char *)G_bolos_ux_context.string_buffer, 32);
    G_bolos_ux_context.words_buffer_length = bolos_ux_mnemonic_from_data(
        (unsigned char *)G_bolos_ux_context.string_buffer, 32,
        (unsigned char *)G_bolos_ux_context.words_buffer,
        sizeof(G_bolos_ux_context.words_buffer));
    screen_onboarding_3_new_init_internal();

#endif
}

#endif // OS_IO_SEPROXYHAL
