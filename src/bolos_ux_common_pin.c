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

#define IS_SHUFFLED os_setting_get(OS_SETTING_SHUFFLE_PIN)
//#define IS_SHUFFLED 1
#define INIT_DIGIT_VALUE 5

const bagl_icon_details_t *const C_digit_font[] = {
    &C_digit_0,        &C_digit_1,
    &C_digit_2,        &C_digit_3,
    &C_digit_4,        &C_digit_5,
    &C_digit_6,        &C_digit_7,
    &C_digit_8,        &C_digit_9,
    &C_digit_dot,      &C_digit_underscore,
    &C_icon_backspace, &C_icon_validate_bold,
};

const bagl_element_t screen_common_pin_elements[] = {
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

    /*
       // pin digit holders
      {{BAGL_RECTANGLE                      , 0x00,  25,   4,  18,  24, 0, 6,
      BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px|BAGL_FONT_ALIGNMENT_CENTER|BAGL_FONT_ALIGNMENT_MIDDLE,
      0   }, G_bolos_ux_context.string_buffer, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_RECTANGLE                      , 0x00,  45,   4,  18,  24, 1, 6, 0
      , 0xFFFFFF, 0x000000, 0, 0   }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_RECTANGLE                      , 0x00,  65,   4,  18,  24, 1, 6, 0
      , 0xFFFFFF, 0x000000, 0, 0   }, NULL, 0, 0, 0, NULL, NULL, NULL },
      {{BAGL_RECTANGLE                      , 0x00,  85,   4,  18,  24, 1, 6, 0
      , 0xFFFFFF, 0x000000, 0, 0   }, NULL, 0, 0, 0, NULL, NULL, NULL },
    */

    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "PIN code",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x11, 25 /*-7*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x12, 35 /*-5*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x13, 45 /*-3*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x14, 55 /*-1*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x15, 65 /*+1*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x16, 75 /*+3*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x17, 85 /*+5*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x18, 95 /*+7*/, 18, GLYPH_digit_0_WIDTH, GLYPH_digit_0_HEIGHT,
      0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer + 4,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // left/rights icons
    {{BAGL_ICON, 0x00, 3, 14, 7, 4, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_DOWN},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x00, 118, 14, 7, 4, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_UP},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

};

const bagl_element_t *screen_common_pin_before_element_display_callback(
    const bagl_element_t *element) {
    unsigned int digit_count = strlen(G_bolos_ux_context.pin_buffer) + 1;
    switch (element->component.userid) {
    case 0x01: {
        unsigned int retries = os_global_pin_retries();
        SPRINTF(G_bolos_ux_context.string_buffer, "%d remaining attempt%s",
                retries, retries == 1 ? "" : "s");
        break;
    }

    default:
        if ((element->component.userid & 0x10)) {
            if (digit_count > (element->component.userid & 0xF)) {
                // display the * (validated digits)
                os_memmove(G_bolos_ux_context.string_buffer + 4,
                           PIC(C_digit_font[0xA]), sizeof(bagl_icon_details_t));
            } else if (digit_count < (element->component.userid & 0xF)) {
                // display the _ (not entered digits)
                os_memmove(G_bolos_ux_context.string_buffer + 4,
                           PIC(C_digit_font[0xB]), sizeof(bagl_icon_details_t));
            } else if (digit_count == (element->component.userid & 0xF)) {
                if (G_bolos_ux_context.string_buffer[0] == ':') {
                    // display the 'backspace'
                    os_memmove(G_bolos_ux_context.string_buffer + 4,
                               PIC(C_digit_font[0xC]),
                               sizeof(bagl_icon_details_t));
                    os_memmove(&G_bolos_ux_context.tmp_element, element,
                               sizeof(bagl_element_t));
                    G_bolos_ux_context.tmp_element.component.y = 19;
                    G_bolos_ux_context.tmp_element.component.x -= 1;
                    return &G_bolos_ux_context.tmp_element;
                } else if (G_bolos_ux_context.string_buffer[0] == ';') {
                    // display the 'check'
                    os_memmove(G_bolos_ux_context.string_buffer + 4,
                               PIC(C_digit_font[0xD]),
                               sizeof(bagl_icon_details_t));
                    // change the shape for the check
                    os_memmove(&G_bolos_ux_context.tmp_element, element,
                               sizeof(bagl_element_t));
                    G_bolos_ux_context.tmp_element.component.y = 20;
                    return &G_bolos_ux_context.tmp_element;
                } else {
                    // display the digit
                    os_memmove(
                        G_bolos_ux_context.string_buffer + 4,
                        PIC(C_digit_font[G_bolos_ux_context.string_buffer[0] -
                                         '0']),
                        sizeof(bagl_icon_details_t));
                }
            }
        }
        break;
    }
    return element;
}

unsigned int screen_common_pin_button(unsigned int button_mask,
                                      unsigned int button_mask_counter) {
    unsigned char currentdigit;
    unsigned int digit_count = strlen(G_bolos_ux_context.pin_buffer);
    UNUSED(button_mask_counter);

    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT |
        BUTTON_RIGHT: // NEXT DIGIT or CONTINUE

        // delete digit
        switch (G_bolos_ux_context.string_buffer[0]) {
        case ':':
            if (digit_count) {
                G_bolos_ux_context.pin_buffer[digit_count - 1] = 0;

                if (digit_count - 1 >= 4) {
                    currentdigit = ';';
                } else if (IS_SHUFFLED == 0) {
                    currentdigit = '0' + INIT_DIGIT_VALUE /*+digit_count*/;
                } else {
                    currentdigit = '0' + cx_rng_u8() % 10;
                }
                G_bolos_ux_context.string_buffer[0] = currentdigit;
            }
            goto redraw;

        case ';':
            goto validate;

        default:
            // append the current displayed digit
            G_bolos_ux_context.pin_buffer[digit_count] =
                G_bolos_ux_context.string_buffer[0];
            digit_count++;
            // reset for next digit

            // display the "check" by default when at least 4 digit have been
            // entered
            if (digit_count >= 4) {
                currentdigit = ';';
            } else if (IS_SHUFFLED == 0) {
                currentdigit = '0' + INIT_DIGIT_VALUE /*+digit_count*/;
            } else {
                currentdigit = '0' + cx_rng_u8() % 10;
            }
            G_bolos_ux_context.string_buffer[0] = currentdigit;
            break;
        }

        if (digit_count == MAX_PIN_LENGTH) {
        validate:
            return ((pin_callback_t)G_bolos_ux_context
                        .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                        .element_arrays[1]
                        .element_array)(
                (unsigned char *)G_bolos_ux_context.pin_buffer, digit_count);
        } else {
            goto redraw;
        }
        break;

    case BUTTON_EVT_FAST | BUTTON_LEFT:     // DOWN
    case BUTTON_EVT_RELEASED | BUTTON_LEFT: // DOWN
        switch (G_bolos_ux_context.string_buffer[0]) {
        case '0':
            if (digit_count == 0) {
                G_bolos_ux_context.string_buffer[0] = '9';
                break;
            } else if (digit_count < MIN_PIN_LENGTH) {
                G_bolos_ux_context.string_buffer[0] = ':';
            } else {
                G_bolos_ux_context.string_buffer[0] = ';';
            }
            break;
        default:
            G_bolos_ux_context.string_buffer[0]--;
            break;
        }
        goto redraw;

    case BUTTON_EVT_FAST | BUTTON_RIGHT:     // UP
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // UP
        switch (G_bolos_ux_context.string_buffer[0]) {
        case ':':
            if (digit_count < MIN_PIN_LENGTH) {
                G_bolos_ux_context.string_buffer[0] = '0';
                break;
            }
            goto inc;
        case ';':
            G_bolos_ux_context.string_buffer[0] = '0';
            break;
        case '9':
            if (digit_count == 0) {
                G_bolos_ux_context.string_buffer[0] = '0';
                break;
            }
            goto inc;
        default:
        inc:
            G_bolos_ux_context.string_buffer[0]++;
            break;
        }

    redraw:
        // redisplay pin screen
        screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        return 0;
    }
    return 1;
}

void screen_common_pin_init(unsigned int stack_slot,
                            pin_callback_t end_callback) {
    screen_state_init(stack_slot);

    // blank pins, first step first
    os_memset(G_bolos_ux_context.pin_buffer, 0,
              sizeof(G_bolos_ux_context.pin_buffer));

    // build the pin current digit
    G_bolos_ux_context.string_buffer[1] = '\0';
    // initialize first digit
    if (IS_SHUFFLED == 0) {
        G_bolos_ux_context.string_buffer[0] = '0' + INIT_DIGIT_VALUE;
    } else {
        G_bolos_ux_context.string_buffer[0] = '0' + cx_rng_u8() % 10;
    }

    // register action callbacks
    G_bolos_ux_context.screen_stack[stack_slot].button_push_callback =
        screen_common_pin_button;
    G_bolos_ux_context.screen_stack[stack_slot]
        .screen_before_element_display_callback =
        screen_common_pin_before_element_display_callback; // used for each
                                                           // screen of the
                                                           // validate pin flow

    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[0]
        .element_array = screen_common_pin_elements;
    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[0]
        .element_array_count = ARRAYLEN(screen_common_pin_elements);

    // marker slot to make sure avoiding consuming screen stack when playing
    // around with lock and relock etc
    // this marker also enable to discriminate between change pin ux and lock
    // validate pin ux when locking the device
    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[1]
        .element_array = (bagl_element_t *)end_callback;
    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[1]
        .element_array_count = 0; // avoid being used

    G_bolos_ux_context.screen_stack[stack_slot].element_arrays_count = 2;

    screen_display_init(stack_slot);
}

#endif
