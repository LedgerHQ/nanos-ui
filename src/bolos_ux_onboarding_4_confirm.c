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

const bagl_element_t screen_onboarding_4_confirm_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 7, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
     "4.",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 27, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Confirm your",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 27, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "recovery phrase",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_4_confirm_words_elements[] = {
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
    {{BAGL_ICON, 0x00, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x00, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_4_confirm_invalid_elements[] = {
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

    {{BAGL_ICON, 0x00, 8, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 29, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Recovery words",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 30, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "do not match",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t *
screen_onboarding_4_confirm_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    // word index element
    case 0x01:
        // change next button if last page
        strcpy(G_bolos_ux_context.string_buffer, "Select word #");
        if (G_bolos_ux_context.onboarding_step_checked + 1 < 10) {
            G_bolos_ux_context.string_buffer[13 + 0] =
                '0' + G_bolos_ux_context.onboarding_step_checked + 1;
            G_bolos_ux_context.string_buffer[13 + 1] = 0;
        } else {
            G_bolos_ux_context.string_buffer[13 + 0] =
                '0' + ((G_bolos_ux_context.onboarding_step_checked + 1) / 10);
            G_bolos_ux_context.string_buffer[13 + 1] =
                '0' + ((G_bolos_ux_context.onboarding_step_checked + 1) % 10);
            G_bolos_ux_context.string_buffer[13 + 2] = 0;
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
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return element;
}

unsigned int
screen_onboarding_4_confirm_words_button(unsigned int button_mask,
                                         unsigned int button_mask_counter);
unsigned int
screen_onboarding_4_confirm_invalid_button(unsigned int button_mask,
                                           unsigned int button_mask_counter);

void screen_onboarding_4_confirm_next_word(void) {
    // when last word has been confirmed, then continue to next screen
    if (G_bolos_ux_context.onboarding_index == 0) {
        // check if all words matched
        if (!G_bolos_ux_context.onboarding_words_are_valid) {
            // restart word confirmation
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_onboarding_4_confirm_invalid_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_4_confirm_invalid_elements);
            G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_onboarding_4_confirm_invalid_button;
            screen_display_init(0);
        } else {
            screen_onboarding_7_processing_init();
        }
    } else {
        G_bolos_ux_context.onboarding_index--; // a new word is being confirmed

        // select next word to check
        do {
            // randomize word to confirm
            G_bolos_ux_context.onboarding_step_checked =
                cx_rng_u8() % G_bolos_ux_context.onboarding_kind;
        } while (G_bolos_ux_context.onboarding_words_checked &
                 (1 << G_bolos_ux_context.onboarding_step_checked));
        // mark word index as checked
        G_bolos_ux_context.onboarding_words_checked |=
            1 << G_bolos_ux_context.onboarding_step_checked;

        // start display with any word (avoid guess :p)
        G_bolos_ux_context.onboarding_step =
            cx_rng_u8() % G_bolos_ux_context.onboarding_kind;
        G_bolos_ux_context.onboarding_step_checked_inc =
            /*(cx_rng_u8()%1)*2 +*/ 1; // ensure a completely random word order
                                       // display (giggle)

        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_4_confirm_words_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_4_confirm_words_elements);
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_onboarding_4_confirm_words_button;
        screen_display_init(0);
    }
}

unsigned int
screen_onboarding_4_confirm_button(unsigned int button_mask,
                                   unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_onboarding_4_confirm_next_word();
        break;
    }
    return 0;
}

unsigned int
screen_onboarding_4_confirm_words_button(unsigned int button_mask,
                                         unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: {
        // check if current word matches (content, not index, as the same word
        // might happen to occur more than once in a seed)
        unsigned char *word = (unsigned char *)G_bolos_ux_context.words_buffer;
        if (bolos_ux_get_word_ptr(&word, G_bolos_ux_context.words_buffer_length,
                                  G_bolos_ux_context.onboarding_step_checked) !=
                strlen(G_bolos_ux_context.string_buffer) ||
            memcmp(word, G_bolos_ux_context.string_buffer,
                   strlen(G_bolos_ux_context.string_buffer))) {
            // not matching since here, don't tell the user nao, to avoid
            // bruteforcing each word. and make him rewrite the words instead
            G_bolos_ux_context.onboarding_words_are_valid = 0;
        }

        // prepare to confirm next word or finish onboarding
        screen_onboarding_4_confirm_next_word();
        break;
    }

    case BUTTON_EVT_FAST | BUTTON_LEFT:
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.onboarding_step =
            ((G_bolos_ux_context.onboarding_step +
              G_bolos_ux_context.onboarding_kind) -
             G_bolos_ux_context.onboarding_step_checked_inc) %
            G_bolos_ux_context.onboarding_kind;
        goto redraw;

    case BUTTON_EVT_FAST | BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        G_bolos_ux_context.onboarding_step =
            (G_bolos_ux_context.onboarding_step +
             G_bolos_ux_context.onboarding_step_checked_inc) %
            G_bolos_ux_context.onboarding_kind;

    redraw:
        // display next word
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_4_confirm_words_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_4_confirm_words_elements);
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_onboarding_4_confirm_words_button;
        screen_display_init(0);
        break;
    }
    return 0;
}

unsigned int
screen_onboarding_4_confirm_invalid_button(unsigned int button_mask,
                                           unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // restart confirmation
        screen_onboarding_4_confirm_init();
        // DEBUG MODE // screen_onboarding_7_processing_init();
        break;
    }
    return 0;
}

void screen_onboarding_4_confirm_init(void) {
    screen_state_init(0);

    // reset word confirmation
    G_bolos_ux_context.onboarding_index = ONBOARDING_CONFIRM_WORD_COUNT;
    G_bolos_ux_context.onboarding_words_checked = 0;
    G_bolos_ux_context.onboarding_words_are_valid =
        1; // fused value for each checked word, to only display result after
           // all words have been validated by the user but minimizing memory
           // cost of handling it

    // register action callbacks
    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_onboarding_4_confirm_button;
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_onboarding_4_confirm_before_element_display_callback;

    // elements to be displayed
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_onboarding_4_confirm_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_4_confirm_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    screen_display_init(0);
}

#endif // OS_IO_SEPROXYHAL
