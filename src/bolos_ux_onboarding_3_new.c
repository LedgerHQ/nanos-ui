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
    // NULL, NULL },
};

unsigned int screen_onboarding_3_new_before_element_display_callback(
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
            return 0; // don't display
        }
        break;

    // right button
    case 0x04:
        if (G_bolos_ux_context.onboarding_step == 23) {
            return 0; // don't display
        }
        break;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int
screen_onboarding_3_new_words_button(unsigned int button_mask,
                                     unsigned int button_mask_counter);

unsigned int screen_onboarding_3_new_button(unsigned int button_mask,
                                            unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_3_new_words_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_3_new_words_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.button_push_callback =
            screen_onboarding_3_new_words_button;
        screen_display_init();
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
        if (G_bolos_ux_context.onboarding_step != 23) {
            break;
        }
        // perform phrase validation
        screen_onboarding_4_confirm_init();
        break;

    case BUTTON_LEFT:
        if (button_mask_counter >= FAST_LIST_THRESHOLD_CS &&
            (button_mask_counter % FAST_LIST_ACTION_CS) == 0) {
            goto case_BUTTON_EVT_RELEASED_BUTTON_LEFT;
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        // don't interpret we're releasing a long press
        if (button_mask_counter > FAST_LIST_THRESHOLD_CS) {
            return 0;
        }
    case_BUTTON_EVT_RELEASED_BUTTON_LEFT:
        if (G_bolos_ux_context.onboarding_step <= 0) {
            // nothing :)
            break;
        }
        G_bolos_ux_context.onboarding_step--;
        goto redraw;

    case BUTTON_RIGHT:
        if (button_mask_counter >= FAST_LIST_THRESHOLD_CS &&
            (button_mask_counter % FAST_LIST_ACTION_CS) == 0) {
            goto case_BUTTON_EVT_RELEASED_BUTTON_RIGHT;
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        // don't interpret we're releasing a long press
        if (button_mask_counter > FAST_LIST_THRESHOLD_CS) {
            return 0;
        }
    case_BUTTON_EVT_RELEASED_BUTTON_RIGHT:
        // no auto pass to next step for last confirmation
        if (G_bolos_ux_context.onboarding_step == 23) {
            break;
        }
        G_bolos_ux_context.onboarding_step++;

    redraw:
        // display next word
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_3_new_words_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_3_new_words_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.button_push_callback =
            screen_onboarding_3_new_words_button;
        screen_display_init();
        break;
    }
    return 0;
}

void screen_onboarding_3_new_init(void) {
    screen_state_init();

    // generate a new seed, and the word list
    os_memset(G_bolos_ux_context.words_buffer, 0,
              sizeof(G_bolos_ux_context.words_buffer));
    cx_rng((unsigned char *)G_bolos_ux_context.string_buffer, 32);
    G_bolos_ux_context.words_buffer_length = bolos_ux_mnemonic_from_data(
        (unsigned char *)G_bolos_ux_context.string_buffer, 32, 0,
        (unsigned char *)G_bolos_ux_context.words_buffer,
        sizeof(G_bolos_ux_context.words_buffer));

    // start at word 0
    G_bolos_ux_context.onboarding_step = 0;

    // register action callbacks
    G_bolos_ux_context.button_push_callback = screen_onboarding_3_new_button;
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_onboarding_3_new_before_element_display_callback;

    // elements to be displayed
    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_onboarding_3_new_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_3_new_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
