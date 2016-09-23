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

// show intro
const bagl_element_t screen_onboarding_4_restore_word_intro_elements[] = {
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

    {{BAGL_LABELINE, 0x31, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Enter first letters",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x32, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Next, enter letters",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x33, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Finally, enter letters",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x30, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

// draw screen
const bagl_element_t screen_onboarding_4_restore_word_elements[] = {
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

    // entered word
    {{BAGL_LABELINE, 0x13, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // slider elements
    {{BAGL_LABELINE, 0x01, 28, 14, 10, 13, 0, 0, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LINE, 0x11, 46, 10, 3, 1, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x02, 57, 16, 14, 19, 0, 0, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LINE, 0x12, 79, 10, 3, 1, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x03, 90, 14, 10, 13, 0, 0, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
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

    // the word's number
    //{{BAGL_LABELINE                       , 0x06,  16,  26, 128,  32, 0, 0,
    //BAGL_FILL, 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0  },
    //G_bolos_ux_context.string_buffer, 0, 0, 0, NULL, NULL, NULL },

};

// word selection
const bagl_element_t screen_onboarding_4_restore_word_select_elements[] = {
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

    {{BAGL_LABELINE, 0x21, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x20, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // left/rights icons
    {{BAGL_ICON, 0x22, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x23, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

// word reenter
const bagl_element_t screen_onboarding_4_restore_word_select_last_elements[] = {
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

    {{BAGL_ICON, 0x00, 23, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 43, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Clear word",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // left/rights icons
    {{BAGL_ICON, 0x22, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x23, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

/*
// word confirmation (single hit)
const bagl_element_t screen_onboarding_4_restore_word_single_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0,
BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  {{BAGL_LABELINE                       , 0x21,   0,  12, 128,  32, 0, 0, 0
, 0xFFFFFF, 0x000000,
BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  },
G_bolos_ux_context.string_buffer, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x20,   0,  26, 128,  32, 0, 0, 0
, 0xFFFFFF, 0x000000,
BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  },
G_bolos_ux_context.string_buffer, 0, 0, 0, NULL, NULL, NULL },

  // left/rights icons
  {{BAGL_ICON                           , 0x00,   3,  12,   7,   7, 0, 0, 0
, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CROSS  }, NULL, 0, 0, 0, NULL, NULL,
NULL },
  {{BAGL_ICON                           , 0x00, 117,  13,   8,   6, 0, 0, 0
, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CHECK  }, NULL, 0, 0, 0, NULL, NULL,
NULL },
};
*/

// recovery phrase error
const bagl_element_t screen_onboarding_4_restore_word_invalid_elements[] = {
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

    {{BAGL_ICON, 0x00, 6, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 27, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Recovery phrase",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 27, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "is not valid",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

#define EMPTY_WORD "_ _ _ _ _ _ _ _"

unsigned int screen_onboarding_4_restore_word_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    case 0x30:
// word index
#define WORD_X "of word #"
        os_memmove(G_bolos_ux_context.string_buffer, WORD_X, strlen(WORD_X));
        if (G_bolos_ux_context.onboarding_step + 1 < 10) {
            G_bolos_ux_context.string_buffer[strlen(WORD_X) + 0] =
                '0' + G_bolos_ux_context.onboarding_step + 1;
            // G_bolos_ux_context.string_buffer[strlen(WORD_X)+1] = '.';
            G_bolos_ux_context.string_buffer[strlen(WORD_X) + 1] = 0;
        } else {
            G_bolos_ux_context.string_buffer[strlen(WORD_X) + 0] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) / 10);
            G_bolos_ux_context.string_buffer[strlen(WORD_X) + 1] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) % 10);
            // G_bolos_ux_context.string_buffer[strlen(WORD_X)+2] = '.';
            G_bolos_ux_context.string_buffer[strlen(WORD_X) + 2] = 0;
        }
        break;

    case 0x31:
        if (G_bolos_ux_context.onboarding_step != 0) {
            return 0; // don't display
        }
        break;
    case 0x32:
        if (G_bolos_ux_context.onboarding_step == 0 ||
            G_bolos_ux_context.onboarding_step ==
                G_bolos_ux_context.onboarding_kind - 1) {
            return 0; // don't display
        }
        break;
    case 0x33:
        if (G_bolos_ux_context.onboarding_step !=
            G_bolos_ux_context.onboarding_kind - 1) {
            return 0; // don't display
        }
        break;

    case 0x01:
        G_bolos_ux_context.string_buffer[0] = 0;
        if (G_bolos_ux_context.hslider3_before != BOLOS_UX_HSLIDER3_NONE) {
            G_bolos_ux_context.string_buffer[0] =
                G_bolos_ux_context
                    .string_buffer[32 + G_bolos_ux_context.hslider3_before] -
                'a' + 'A';
            G_bolos_ux_context.string_buffer[1] = 0;
        }
        break;

    case 0x02:
        G_bolos_ux_context.string_buffer[0] =
            G_bolos_ux_context
                .string_buffer[32 + G_bolos_ux_context.hslider3_current] -
            'a' + 'A';
        G_bolos_ux_context.string_buffer[1] = 0;
        break;

    case 0x03:
        G_bolos_ux_context.string_buffer[0] = 0;
        if (G_bolos_ux_context.hslider3_after != BOLOS_UX_HSLIDER3_NONE) {
            G_bolos_ux_context.string_buffer[0] =
                G_bolos_ux_context
                    .string_buffer[32 + G_bolos_ux_context.hslider3_after] -
                'a' + 'A';
            G_bolos_ux_context.string_buffer[1] = 0;
        }
        break;

    // left button
    case 0x11:
    case 0x04:
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE) {
            return 0; // don't display
        }
        break;

    // right button
    case 0x12:
    case 0x05:
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE) {
            return 0; // don't display
        }
        break;

    // current entered word
    case 0x13: {
        unsigned int l = strlen(G_bolos_ux_context.string_buffer + 16);
        strcpy(G_bolos_ux_context.string_buffer, EMPTY_WORD);
        while (l--) {
            G_bolos_ux_context.string_buffer[2 * l] =
                G_bolos_ux_context.string_buffer[16 + l];
        }
        break;
    }

    case 0x06:
        // word index
        if (G_bolos_ux_context.onboarding_step + 1 < 10) {
            G_bolos_ux_context.string_buffer[0] =
                '0' + G_bolos_ux_context.onboarding_step + 1;
            G_bolos_ux_context.string_buffer[1] = '.';
            G_bolos_ux_context.string_buffer[2] = 0;
        } else {
            G_bolos_ux_context.string_buffer[0] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) / 10);
            G_bolos_ux_context.string_buffer[1] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) % 10);
            G_bolos_ux_context.string_buffer[2] = '.';
            G_bolos_ux_context.string_buffer[3] = 0;
        }
        break;

    case 0x07:
        // last letter of the word current buffer
        G_bolos_ux_context.string_buffer[0] =
            G_bolos_ux_context.string_buffer
                [16 + strlen(G_bolos_ux_context.string_buffer + 16) - 1];
        G_bolos_ux_context.string_buffer[1] = 0;
        break;

    case 0x20:
        // display matching word from the slider's current index
        bolos_ux_bip39_idx_strcpy(
            G_bolos_ux_context.onboarding_index +
                G_bolos_ux_context.hslider3_current,
            (unsigned char *)G_bolos_ux_context.string_buffer);
        break;

    case 0x21: {
#define SELECT_WORD_X "Select word #"
        os_memmove(G_bolos_ux_context.string_buffer, SELECT_WORD_X,
                   strlen(SELECT_WORD_X));
        if (G_bolos_ux_context.onboarding_step + 1 < 10) {
            G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X) + 0] =
                '0' + G_bolos_ux_context.onboarding_step + 1;
            // G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X)+1] = '.';
            G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X) + 1] = 0;
        } else {
            G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X) + 0] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) / 10);
            G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X) + 1] =
                '0' + ((G_bolos_ux_context.onboarding_step + 1) % 10);
            // G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X)+2] = '.';
            G_bolos_ux_context.string_buffer[strlen(SELECT_WORD_X) + 2] = 0;
        }
    } break;

    case 0x22:
        // no left button on first item
        if (G_bolos_ux_context.hslider3_current == 0) {
            return 0;
        }
        break;
    case 0x23:
        // no right button on last item
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        break;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int screen_onboarding_4_restore_word_invalid_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // enter a new recovery phrase
        screen_onboarding_3_restore_init();
        break;
    }
    return 0;
}

void screen_onboarding_4_restore_word_validate(void) {
    bolos_ux_bip39_idx_strcpy(
        G_bolos_ux_context.onboarding_index +
            G_bolos_ux_context.hslider3_current,
        (unsigned char *)(G_bolos_ux_context.words_buffer +
                          G_bolos_ux_context.words_buffer_length));
    G_bolos_ux_context.words_buffer_length =
        strlen(G_bolos_ux_context.words_buffer);

    // a word has been added
    G_bolos_ux_context.onboarding_step++;

    if (G_bolos_ux_context.onboarding_step ==
        G_bolos_ux_context.onboarding_kind) {
        // if we've entered all the words, then check the phrase
        if (!bolos_ux_mnemonic_check(
                (unsigned char *)G_bolos_ux_context.words_buffer,
                G_bolos_ux_context.words_buffer_length)) {
            G_bolos_ux_context.button_push_callback =
                screen_onboarding_4_restore_word_invalid_button;
            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                screen_onboarding_4_restore_word_invalid_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_4_restore_word_invalid_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            screen_display_init();
        } else {
            // allright, the recovery phrase looks ok, finish onboarding
            screen_onboarding_7_processing_init();
        }
    } else {
        // add a space before next word
        G_bolos_ux_context
            .words_buffer[G_bolos_ux_context.words_buffer_length++] = ' ';

        // enter the next word
        screen_onboarding_4_restore_word_init(0);
    }
}

unsigned int screen_onboarding_4_restore_word_select_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    switch (button_mask) {
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
        // not a rotating list
        if (G_bolos_ux_context.hslider3_current == 0) {
            return 0;
        }
        bolos_ux_hslider3_previous();
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
        // not a rotating list
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        bolos_ux_hslider3_next();

    redraw:
        // last item is a bit different
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                screen_onboarding_4_restore_word_select_last_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_4_restore_word_select_last_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            screen_display_init();
        } else {
            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                screen_onboarding_4_restore_word_select_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_4_restore_word_select_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            screen_display_init();
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            // clear current word
            screen_onboarding_4_restore_word_init(0);
        } else {
            // confirm word, and prepare entering a new one or validate the seed
            screen_onboarding_4_restore_word_validate();
        }
        break;
    }
    return 0;
}

unsigned int
screen_onboarding_4_restore_word_button(unsigned int button_mask,
                                        unsigned int button_mask_counter) {
    switch (button_mask) {
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
        bolos_ux_hslider3_previous();
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
        bolos_ux_hslider3_next();

    redraw:
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_4_restore_word_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_4_restore_word_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        screen_display_init();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: {
        unsigned int nb_words_matching_stem;
        // validate next letter of the word
        G_bolos_ux_context
            .string_buffer[16 + strlen(G_bolos_ux_context.string_buffer + 16)] =
            G_bolos_ux_context
                .string_buffer[32 + G_bolos_ux_context.hslider3_current];

        // continue displaying until less than X words matches the stem
        nb_words_matching_stem = bolos_ux_bip39_get_word_count_starting_with(
            (unsigned char *)G_bolos_ux_context.string_buffer + 16,
            strlen(G_bolos_ux_context.string_buffer + 16));
        if (nb_words_matching_stem > ONBOARDING_WORD_COMPLETION_MAX_ITEMS) {
            // too much words for slider word completion, await another letter

            // update te slider's possible completion letters
            bolos_ux_hslider3_init(
                bolos_ux_bip39_get_word_next_letters_starting_with(
                    (unsigned char *)G_bolos_ux_context.string_buffer + 16,
                    strlen(G_bolos_ux_context.string_buffer + 16),
                    (unsigned char *)G_bolos_ux_context.string_buffer + 32));

            // display added letter and refresh slider
            goto redraw;
        } else {
            // always init stem count
            // index of the first word matching the stem
            G_bolos_ux_context.onboarding_index =
                bolos_ux_bip39_get_word_idx_starting_with(
                    (unsigned char *)G_bolos_ux_context.string_buffer + 16,
                    strlen(G_bolos_ux_context.string_buffer + 16));

            {
                // account for the extre "last" (clear) item
                bolos_ux_hslider3_init(nb_words_matching_stem + 1);

                // multiple possibilities
                // update te slider's possible words
                G_bolos_ux_context.button_push_callback =
                    screen_onboarding_4_restore_word_select_button;
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array =
                    screen_onboarding_4_restore_word_select_elements;
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array_count =
                    ARRAYLEN(screen_onboarding_4_restore_word_select_elements);
                G_bolos_ux_context.screen_current_element_arrays_count = 1;
                screen_display_init();
            }
        }
        break;
    }
    }
    return 0;
}

unsigned int screen_onboarding_4_restore_word_intro_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.button_push_callback =
            screen_onboarding_4_restore_word_button;

        // update te slider's possible completion letters
        bolos_ux_hslider3_init(
            bolos_ux_bip39_get_word_next_letters_starting_with(
                (unsigned char *)G_bolos_ux_context.string_buffer + 16,
                strlen(G_bolos_ux_context.string_buffer + 16),
                (unsigned char *)G_bolos_ux_context.string_buffer + 32));

        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_4_restore_word_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_4_restore_word_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        screen_display_init();
        break;
    }
    return 0;
}

void screen_onboarding_4_restore_word_init(unsigned int firstWord) {
    screen_state_init();

    if (firstWord) {
        // start by restore first word (+1 when displayed)
        G_bolos_ux_context.onboarding_step = 0;

        // flush the words first
        os_memset(G_bolos_ux_context.words_buffer, 0,
                  sizeof(G_bolos_ux_context.words_buffer));
        G_bolos_ux_context.words_buffer_length = 0;
    }

    os_memset(G_bolos_ux_context.string_buffer, 0,
              sizeof(G_bolos_ux_context.string_buffer));
    // offset 0: the display buffer for various placement
    // offset 16: the entered stem for the current word restoration
    // offset 32: array of next letters possible after the current word's stem
    // in the dictionary (word completion possibilities)

    G_bolos_ux_context.button_push_callback =
        screen_onboarding_4_restore_word_intro_button;
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_onboarding_4_restore_word_before_element_display_callback;

    // elements to be displayed
    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_onboarding_4_restore_word_intro_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_onboarding_4_restore_word_intro_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;
    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
