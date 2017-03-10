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

#include "bolos_ux.h"
#include "bolos_ux_common.h"

#include "glyphs.h"

#ifdef OS_IO_SEPROXYHAL

#define FLAG_TEMPORARY 0x80
#define FLAG_MASK 0x7F

const bagl_element_t
    screen_settings_passphrase_type_and_review_1_persistent_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 15, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "3.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 34, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Enter a secret",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 34, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_1_temporary_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 14, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "1.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 33, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Enter a secret",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 33, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_2_persistent_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 17, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "4.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 36, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Confirm your",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 36, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_2_temporary_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 17, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "2.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 36, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Confirm your",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 36, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_3_review_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
         "Passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        // icons
        {{BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
          BAGL_GLYPH_ICON_CROSS},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
          BAGL_GLYPH_ICON_CHECK},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        //                                                                         once|pause in cs                                                                                           pixels/s
        {{BAGL_LABELINE, 0x01, 23, 26, 82, 12, /*0x80|*/ 10, 0, 0, 0xFFFFFF,
          0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_4_persistent_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 21, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "5.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 40, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Enter your",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 40, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "current PIN",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_4_temporary_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_LABELINE, 0x00, 21, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
         "3.",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 40, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Enter your",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 40, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "current PIN",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_5_error_pin_id0_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_ICON, 0x00, 17, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
          BAGL_GLYPH_ICON_CROSS_BADGE},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 39, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Secret PIN is",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 39, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "already used",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

const bagl_element_t
    screen_settings_passphrase_type_and_review_error_empty_pp_elements[] = {
        // erase
        {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
          0xFFFFFF, 0, 0},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},

        {{BAGL_ICON, 0x00, 17, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
          BAGL_GLYPH_ICON_CROSS_BADGE},
         NULL,
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 39, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "Passphrase",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
        {{BAGL_LABELINE, 0x00, 39, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
          BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
         "is empty",
         0,
         0,
         0,
         NULL,
         NULL,
         NULL},
};

#define PIN_BUFFER (G_bolos_ux_context.words_buffer)
#define PP_BUFFER                                                              \
    ((char *)(G_bolos_ux_context.words_buffer +                                \
              G_bolos_ux_context.words_buffer[0] + 1))

const bagl_element_t *
screen_settings_passphrase_type_and_review_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    // word index element
    case 0x01:
        os_memmove(&G_bolos_ux_context.tmp_element, element,
                   sizeof(bagl_element_t));
        G_bolos_ux_context.tmp_element.text = PP_BUFFER;
        return &G_bolos_ux_context.tmp_element;
    }
    return element;
}

unsigned int screen_settings_passphrase_type_and_review_5_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        if (G_bolos_ux_context.onboarding_index & FLAG_TEMPORARY) {
            screen_settings_set_temporary();
        } else {
            screen_settings_attach_to_pin();
        }
        break;
    }
    return 0;
}

const char *const screen_settings_passphrase_classes_elements[] = {
    // when first letter is already entered
    "abcdefghijklmnopqrstuvwxyz\b\n\r", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\b\n\r",
    "0123456789 '\"`&/?!:;.,~*$=+-[](){}<>\\_#@|%\b\n\r",
    // when first letter is not entered yet
    "abcdefghijklmnopqrstuvwxyz\r", "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r",
    "0123456789 '\"`&/?!:;.,~*$=+-[](){}<>\\_#@|%\r",
};
#define GET_CHAR(char_class, char_idx)                                         \
    ((char *)PIC(                                                              \
        screen_settings_passphrase_classes_elements[char_class]))[char_idx]

// these icons will be centered
const bagl_icon_details_t *const screen_settings_passphrase_classes_icons[] = {
    &C_icon_lowercase, &C_icon_uppercase, &C_icon_digits,
    &C_icon_backspace, &C_icon_validate,  &C_icon_classes,
};

const bagl_element_t *
screen_settings_passphrase_type_and_review_keyboard_class_callback(
    unsigned int event, unsigned int value);

void screen_settings_passphrase_type_and_review_validate_passphrase(void) {
    // if no passphrase set, then nothing to continue
    if (strlen(PP_BUFFER) == 0) {
        screen_state_init(0);
        // empty passphrase
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_settings_passphrase_type_and_review_error_empty_pp_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count = ARRAYLEN(
            screen_settings_passphrase_type_and_review_error_empty_pp_elements);
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_settings_passphrase_type_and_review_5_button;
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        screen_display_init(0);
        return;
    }
    // append a dummy space for scroll animation to go smooth
    PP_BUFFER[strlen(PP_BUFFER)] = ' ';
    // validate the passphrase
    screen_settings_passphrase_type_and_review_init(
        G_bolos_ux_context.onboarding_index + 1);
}

const bagl_element_t *
screen_settings_passphrase_type_and_review_keyboard_item_callback(
    unsigned int event, unsigned int value) {
    const bagl_icon_details_t *icon;
    switch (event) {
    case KEYBOARD_ITEM_VALIDATED:
        // depending on the chosen class, interpret the click
        if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) == '\b') {
            if (strlen(PP_BUFFER)) {
                PP_BUFFER[strlen(PP_BUFFER) - 1] = 0;
                goto redisplay_current_class;
            }
        } else if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) ==
                   '\r') {
            // go back to classes display
            screen_common_keyboard_init(
                0, G_bolos_ux_context.onboarding_step,
                (strlen(PP_BUFFER) ? 5 : 3),
                screen_settings_passphrase_type_and_review_keyboard_class_callback);
            return NULL;
        } else if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) ==
                   '\n') {
            screen_settings_passphrase_type_and_review_validate_passphrase();
            return NULL;
        } else {
            // too much character entered already
            if (strlen(PP_BUFFER) >=
                100 /*sizeof(G_bolos_ux_context.words_buffer) - 10*/ /*max pin + pin length*/) {
                return NULL;
            }
            // append the char and display classes again
            PP_BUFFER[strlen(PP_BUFFER)] =
                GET_CHAR(G_bolos_ux_context.onboarding_step, value);
            PP_BUFFER[strlen(PP_BUFFER)] = 0;

        redisplay_current_class:
            // redisplay the correct class depending on the current number of
            // entered digits
            G_bolos_ux_context.onboarding_step =
                (G_bolos_ux_context.onboarding_step % 3) +
                (strlen(PP_BUFFER) ? 0 : 3);
            screen_common_keyboard_init(
                0, (event == KEYBOARD_ITEM_VALIDATED && strlen(PP_BUFFER) == 0)
                       ? 0
                       : COMMON_KEYBOARD_INDEX_UNCHANGED,
                strlen((char *)PIC(screen_settings_passphrase_classes_elements
                                       [G_bolos_ux_context.onboarding_step])),
                screen_settings_passphrase_type_and_review_keyboard_item_callback);
            return NULL;
        }

    case KEYBOARD_RENDER_ITEM:
        G_bolos_ux_context.tmp_element.text = G_bolos_ux_context.string_buffer;
        os_memset(G_bolos_ux_context.string_buffer, 0, 3);
        if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) == '\b') {
            value = 3;
            goto set_bitmap;
        } else if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) ==
                   '\r') {
            value = 5;
            goto set_bitmap;
        } else if (GET_CHAR(G_bolos_ux_context.onboarding_step, value) ==
                   '\n') {
            value = 4;

        set_bitmap:
            icon = (bagl_icon_details_t *)PIC(
                screen_settings_passphrase_classes_icons[value]);
            G_bolos_ux_context.tmp_element.component.x +=
                G_bolos_ux_context.tmp_element.component.width / 2 -
                icon->width / 2;
            G_bolos_ux_context.tmp_element.component.width = icon->width;
            // G_bolos_ux_context.tmp_element.component.y += icon->height/2 -
            // G_bolos_ux_context.tmp_element.component.height/2;
            G_bolos_ux_context.tmp_element.component.y = 5;
            G_bolos_ux_context.tmp_element.component.height = icon->height;
            G_bolos_ux_context.tmp_element.component.type = BAGL_ICON;
            G_bolos_ux_context.tmp_element.component.icon_id = 0;
            G_bolos_ux_context.tmp_element.text = (const char *)icon;
        } else {
            G_bolos_ux_context.string_buffer[0] =
                GET_CHAR(G_bolos_ux_context.onboarding_step, value);
        }
        break;

    case KEYBOARD_RENDER_WORD: {
        unsigned int l = strlen(PP_BUFFER);

        G_bolos_ux_context.string_buffer[0] = '_';
        G_bolos_ux_context.string_buffer[1] = 0;

        if (value < 8) {
            if (l <= 8) {
                if (l > value) {
                    G_bolos_ux_context.string_buffer[0] = PP_BUFFER[value];
                } else {
                    G_bolos_ux_context.string_buffer[0] = '_';
                }
            } else {
                // first char is '...' to notify continuing
                if (value == 0) {
                    G_bolos_ux_context.string_buffer[0] = '.';
                    G_bolos_ux_context.string_buffer[1] = '.';
                    G_bolos_ux_context.string_buffer[2] = '.';
                    G_bolos_ux_context.string_buffer[3] = 0;
                } else {
                    G_bolos_ux_context.string_buffer[0] =
                        (PP_BUFFER + l - 8)[value];
                }
            }
        }
        // ensure font is left aligned
        G_bolos_ux_context.tmp_element.text = G_bolos_ux_context.string_buffer;
        break;
    }
    }
    // update element display
    return &G_bolos_ux_context.tmp_element;
}

const bagl_element_t *
screen_settings_passphrase_type_and_review_keyboard_class_callback(
    unsigned int event, unsigned int value) {
    const bagl_icon_details_t *icon;
    switch (event) {
    case KEYBOARD_ITEM_VALIDATED:
        switch (value) {
        case 3:
            // backspace
            if (strlen(PP_BUFFER)) {
                PP_BUFFER[strlen(PP_BUFFER) - 1] = 0;
                screen_common_keyboard_init(
                    0, strlen(PP_BUFFER) == 0 ? 0
                                              : COMMON_KEYBOARD_INDEX_UNCHANGED,
                    (strlen(PP_BUFFER) ? 5 : 3),
                    screen_settings_passphrase_type_and_review_keyboard_class_callback);
                return NULL;
            }
            break;
        case 4:
            screen_settings_passphrase_type_and_review_validate_passphrase();
            return NULL;

        case 0:
        case 1:
        case 2:
            G_bolos_ux_context.onboarding_step =
                value + (strlen(PP_BUFFER) ? 0 : 3);
            screen_common_keyboard_init(
                0, 0, strlen(&GET_CHAR(G_bolos_ux_context.onboarding_step, 0)),
                screen_settings_passphrase_type_and_review_keyboard_item_callback);
            return NULL;

        default:
            // no validation
            break;
        }
        break;
    case KEYBOARD_RENDER_ITEM:
        icon = (bagl_icon_details_t *)PIC(
            screen_settings_passphrase_classes_icons[value]);
        G_bolos_ux_context.tmp_element.component.x +=
            G_bolos_ux_context.tmp_element.component.width / 2 -
            icon->width / 2;
        G_bolos_ux_context.tmp_element.component.width = icon->width;
        // G_bolos_ux_context.tmp_element.component.y += icon->height/2 -
        // G_bolos_ux_context.tmp_element.component.height/2;
        G_bolos_ux_context.tmp_element.component.y = 5;
        G_bolos_ux_context.tmp_element.component.height = icon->height;
        G_bolos_ux_context.tmp_element.component.type = BAGL_ICON;
        G_bolos_ux_context.tmp_element.component.icon_id = 0;
        G_bolos_ux_context.tmp_element.text = (const char *)icon;
        break;
    case KEYBOARD_RENDER_WORD:
        // same as when drawing items
        return screen_settings_passphrase_type_and_review_keyboard_item_callback(
            event, value);
    }
    // update element display
    return &G_bolos_ux_context.tmp_element;
}

unsigned int screen_settings_passphrase_type_and_review_1_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_common_keyboard_init(
            0, 0, 3,
            screen_settings_passphrase_type_and_review_keyboard_class_callback);
        break;
    }
    return 0;
}

unsigned int screen_settings_passphrase_type_and_review_2_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_settings_passphrase_type_and_review_init(
            G_bolos_ux_context.onboarding_index + 1);
        break;
    }
    return 0;
}

unsigned int
screen_settings_passphrase_type_and_review_apply(unsigned int ignored) {
    UNUSED(ignored);
    G_bolos_ux_context.screen_stack[0].ticker_callback = NULL;

    // pin is only for non temporary
    if (!(G_bolos_ux_context.onboarding_index & FLAG_TEMPORARY)) {
        BEGIN_TRY {
            TRY {
                if (PIN_BUFFER[0] == 0) {
                    THROW(INVALID_PARAMETER);
                }
                // set attached pin for that identity
                os_perso_set_pin(1, (unsigned char *)(PIN_BUFFER + 1),
                                 PIN_BUFFER[0]);
            }
            CATCH_ALL {
                screen_state_init(0);
                // display error (probably duplicate pin id 0)
                G_bolos_ux_context.screen_stack[0]
                    .element_arrays[0]
                    .element_array =
                    screen_settings_passphrase_type_and_review_5_error_pin_id0_elements;
                G_bolos_ux_context.screen_stack[0]
                    .element_arrays[0]
                    .element_array_count = ARRAYLEN(
                    screen_settings_passphrase_type_and_review_5_error_pin_id0_elements);
                G_bolos_ux_context.screen_stack[0].button_push_callback =
                    screen_settings_passphrase_type_and_review_5_button;
                G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
                screen_display_init(0);
                return 0;
            }
            FINALLY;
        }
        END_TRY;
    }

    // compute the seed and attach it to pin 1
    os_perso_derive_and_set_seed(
        (G_bolos_ux_context.onboarding_index & FLAG_TEMPORARY) ? 2 : 1,
        // standard BIP39 mnemonic prefix
        NULL, 0,
        // entered passphrase
        PP_BUFFER,
        strlen(PP_BUFFER) - 1 /*suffixed with a dummy space for display only*/,
        // use onboarded mnemonic
        NULL, 0);

    // prepare return after pin has been changed
    if (G_bolos_ux_context.onboarding_index & FLAG_TEMPORARY) {
        screen_settings_set_temporary();
    } else {
        screen_settings_attach_to_pin();
    }
    return 0;
}

unsigned int
screen_settings_passphrase_type_and_review_processing(unsigned int ignored) {
    UNUSED(ignored);
    screen_processing_init();

    G_bolos_ux_context.screen_stack[0].ticker_value = 1;
    G_bolos_ux_context.screen_stack[0].ticker_interval = 1;
    G_bolos_ux_context.screen_stack[0].ticker_callback =
        screen_settings_passphrase_type_and_review_apply;
    G_bolos_ux_context.screen_stack[0].exit_code_after_elements_displayed =
        BOLOS_UX_CONTINUE;
    return 0;
}

unsigned int screen_settings_passphrase_type_and_review_4_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // display change pin
        screen_modal_validate_pin_init();

        // when pin is correcly entered (screen stack slot is popped), then
        screen_state_init(0);
        G_bolos_ux_context.screen_stack[0].displayed_callback =
            screen_settings_passphrase_type_and_review_processing;
        break;
    }
    return 0;
}

unsigned int screen_settings_passphrase_type_and_review_3_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        if (G_bolos_ux_context.onboarding_index & FLAG_TEMPORARY) {
            screen_settings_set_temporary();
        } else {
            screen_settings_attach_to_pin();
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        screen_settings_passphrase_type_and_review_init(
            G_bolos_ux_context.onboarding_index + 1);
        break;
    }
    return 0;
}

unsigned int
screen_settings_passphrase_type_and_review_validate_passphrase_redraw(
    unsigned int ignored) {
    UNUSED(ignored);
    screen_display_init(0);
    return 0;
}

void screen_settings_passphrase_type_and_review_init(unsigned int kind) {
    screen_state_init(0);

    G_bolos_ux_context.onboarding_index = kind;

    // blank passphrase, first step first
    if (!(kind & FLAG_TEMPORARY)) {
        switch (kind & FLAG_MASK) {
        case 0:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_1_persistent_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_1_persistent_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_1_button;
            break;
        case 1:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_2_persistent_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_2_persistent_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_2_button;
            break;
        case 2:
            goto passphrase_review;
        case 3:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_4_persistent_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_4_persistent_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_4_button;
            break;
        }
    } else {
        switch (kind & FLAG_MASK) {
        case 0:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_1_temporary_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_1_temporary_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_1_button;
            break;
        case 1:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_2_temporary_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_2_temporary_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_2_button;
            break;
        case 2:
        passphrase_review:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_3_review_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_3_review_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_3_button;

            // auto redisplay passphrase as the infinite roundtripping is
            // failing in the MCU
            G_bolos_ux_context.screen_stack[0].ticker_callback =
                screen_settings_passphrase_type_and_review_validate_passphrase_redraw;
            G_bolos_ux_context.screen_stack[0].ticker_interval =
                1000 +
                bagl_label_roundtrip_duration_ms_buf(
                    &screen_settings_passphrase_type_and_review_3_review_elements
                        [4],
                    PP_BUFFER, 7);
            G_bolos_ux_context.screen_stack[0].ticker_value =
                G_bolos_ux_context.screen_stack[0].ticker_interval;
            break;
        case 3:
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_settings_passphrase_type_and_review_4_temporary_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count = ARRAYLEN(
                screen_settings_passphrase_type_and_review_4_temporary_elements);
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_settings_passphrase_type_and_review_4_button;
            break;
        }
    }
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_settings_passphrase_type_and_review_before_element_display_callback;
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    screen_display_init(0);
}

#endif // OS_IO_SEPROXYHAL
