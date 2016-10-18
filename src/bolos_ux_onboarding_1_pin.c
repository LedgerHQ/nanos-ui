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

#include "bolos_ux.h"
#include "bolos_ux_common.h"

#ifdef OS_IO_SEPROXYHAL

const bagl_element_t screen_onboarding_1_pin_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 19, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
     "1.",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 38, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Choose your",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 38, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "PIN code",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_2_pin_elements[] = {
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
     "PIN code",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_2_pin_nomatch_elements[] = {
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

    {{BAGL_ICON, 0x00, 16, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 37, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "PIN codes",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 37, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "do not match",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_onboarding_1_2_pin_entry_elements[] = {
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

    // pin digit holders
    {{BAGL_RECTANGLE, 0x00, 25, 4, 18, 24, 0, 6, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px | BAGL_FONT_ALIGNMENT_CENTER |
          BAGL_FONT_ALIGNMENT_MIDDLE,
      0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_RECTANGLE, 0x00, 45, 4, 18, 24, 1, 6, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_RECTANGLE, 0x00, 65, 4, 18, 24, 1, 6, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_RECTANGLE, 0x00, 85, 4, 18, 24, 1, 6, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
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

// changing digit
// FOREACH: x += validated digit_count * 20
const bagl_element_t screen_onboarding_1_2_pin_entry_edit_elements[] = {
    {{BAGL_RECTANGLE, 0x00, 25, 4, 18, 24, 0, 6, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px | BAGL_FONT_ALIGNMENT_CENTER |
          BAGL_FONT_ALIGNMENT_MIDDLE,
      0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

// validated input
// FOREACH: x += validating digit_count * 20
const bagl_element_t screen_onboarding_1_2_pin_entry_set_elements[] = {
    {{BAGL_RECTANGLE, 0x00, 25, 4, 18, 24, 1, 6, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL}, // rounded rect
    {{BAGL_CIRCLE, 0x00, 32, 14, 4, 4, 0, 2, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL}, // dot circle
    {{BAGL_RECTANGLE, 0x00, 45, 4, 18, 24, 0, 6, BAGL_FILL, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px | BAGL_FONT_ALIGNMENT_CENTER |
          BAGL_FONT_ALIGNMENT_MIDDLE,
      0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL}, // next digit
};

#define SCREEN_PIN_DIGIT_SPACE (18 + 2)

unsigned int
screen_onboarding_1_2_pin_entry_button(unsigned int button_mask,
                                       unsigned int button_mask_counter);

unsigned int
screen_onboarding_1_2_pin_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // display pin entry form
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_1_2_pin_entry_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_1_2_pin_entry_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.button_push_callback =
            screen_onboarding_1_2_pin_entry_button;
        screen_display_init();
        break;
    }
    return 0;
}

unsigned int
screen_onboarding_2_pin_nomatch_button(unsigned int button_mask,
                                       unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // re-enter the first pin
        screen_onboarding_1_2_pin_init(1);
        break;
    }
    return 1;
}

unsigned int
screen_onboarding_1_2_pin_entry_button(unsigned int button_mask,
                                       unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);

    unsigned int digit_count = strlen(G_bolos_ux_context.pin_buffer +
                                      G_bolos_ux_context.onboarding_index);

    G_bolos_ux_context.exit_code_after_elements_displayed = 0;

    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT |
        BUTTON_RIGHT: // NEXT DIGIT or CONTINUE
        // append the current displayed digit
        G_bolos_ux_context
            .pin_buffer[G_bolos_ux_context.onboarding_index + digit_count] =
            G_bolos_ux_context.string_buffer[0];
        // reset for next digit
        if (os_setting_get(OS_SETTING_SHUFFLE_PIN) == 0) {
            G_bolos_ux_context.string_buffer[0] = '5';
            G_bolos_ux_context.string_buffer[1] = '\0';
        } else {
            G_bolos_ux_context.string_buffer[0] = '0' + rng_u8_modulo(10);
            G_bolos_ux_context.string_buffer[1] = '\0';
        }

        if (digit_count == 3) {
            if (G_bolos_ux_context.onboarding_index == 0) {
                // ask for pin confirmation
                screen_onboarding_1_2_pin_init(2);
                return 1;
            } else {
                // check if pin and confirmation match, if not, error message
                if (os_memcmp(G_bolos_ux_context.pin_buffer,
                              G_bolos_ux_context.pin_buffer + 4, 4) != 0) {
                    G_bolos_ux_context.screen_current_element_arrays[0]
                        .element_array =
                        screen_onboarding_2_pin_nomatch_elements;
                    G_bolos_ux_context.screen_current_element_arrays[0]
                        .element_array_count =
                        ARRAYLEN(screen_onboarding_2_pin_nomatch_elements);
                    G_bolos_ux_context.screen_current_element_arrays_count = 1;
                    G_bolos_ux_context.button_push_callback =
                        screen_onboarding_2_pin_nomatch_button;
                    screen_display_init();
                    return 1;
                }

                if (G_bolos_ux_context.last_ux_id ==
                    BOLOS_UX_CHANGE_ALTERNATE_PIN) {
                    // Save the alternate PIN
                    os_perso_set_alternate_pin(G_bolos_ux_context.pin_buffer,
                                               4);
                    G_bolos_ux_context.exit_code = BOLOS_UX_OK;
                } else {
                    // if match, then goto onboarding step
                    switch (G_bolos_ux_context.onboarding_kind) {
                    case BOLOS_UX_ONBOARDING_NEW:
                        screen_onboarding_3_new_init();
                        return 1;

                    case BOLOS_UX_ONBOARDING_RESTORE:
                        screen_onboarding_3_restore_init();
                        return 1;

                    default:
                        // error ?
                        break;
                    }
                }
            }
        } else {
            // draw current digit as validated
            os_memmove(G_bolos_ux_context.screen_volatile_elements,
                       screen_onboarding_1_2_pin_entry_set_elements,
                       sizeof(screen_onboarding_1_2_pin_entry_set_elements));
            // draw at the correct position
            G_bolos_ux_context.screen_volatile_elements[0].component.x +=
                digit_count * SCREEN_PIN_DIGIT_SPACE;
            G_bolos_ux_context.screen_volatile_elements[1].component.x +=
                digit_count * SCREEN_PIN_DIGIT_SPACE;
            G_bolos_ux_context.screen_volatile_elements[2].component.x +=
                digit_count * SCREEN_PIN_DIGIT_SPACE;

            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                G_bolos_ux_context.screen_volatile_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_1_2_pin_entry_set_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            screen_display_init();
        }
        break;

    case BUTTON_LEFT:
        if (button_mask_counter >= FAST_LIST_THRESHOLD_CS &&
            (button_mask_counter % FAST_LIST_ACTION_CS) == 0) {
            goto case_BUTTON_EVT_RELEASED_BUTTON_LEFT;
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT: // DOWN
        // don't interpret we're releasing a long press
        if (button_mask_counter > FAST_LIST_THRESHOLD_CS) {
            return 0;
        }
    case_BUTTON_EVT_RELEASED_BUTTON_LEFT:
        if (G_bolos_ux_context.string_buffer[0] == '0') {
            G_bolos_ux_context.string_buffer[0] = '9';
        } else {
            G_bolos_ux_context.string_buffer[0]--;
        }
        goto redraw_digit;

    case BUTTON_RIGHT:
        if (button_mask_counter >= FAST_LIST_THRESHOLD_CS &&
            (button_mask_counter % FAST_LIST_ACTION_CS) == 0) {
            goto case_BUTTON_EVT_RELEASED_BUTTON_RIGHT;
        }
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // UP
        // don't interpret we're releasing a long press
        if (button_mask_counter > FAST_LIST_THRESHOLD_CS) {
            return 0;
        }
    case_BUTTON_EVT_RELEASED_BUTTON_RIGHT:
        if (G_bolos_ux_context.string_buffer[0] == '9') {
            G_bolos_ux_context.string_buffer[0] = '0';
        } else {
            G_bolos_ux_context.string_buffer[0]++;
        }

    redraw_digit:
        // draw current digit as validated
        os_memmove(G_bolos_ux_context.screen_volatile_elements,
                   screen_onboarding_1_2_pin_entry_edit_elements,
                   sizeof(screen_onboarding_1_2_pin_entry_edit_elements));
        // draw at the correct position
        G_bolos_ux_context.screen_volatile_elements[0].component.x +=
            digit_count * SCREEN_PIN_DIGIT_SPACE;

        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            G_bolos_ux_context.screen_volatile_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_onboarding_1_2_pin_entry_edit_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        screen_display_init();
        break;
    }
    return 1;
}

void screen_onboarding_1_2_pin_init(unsigned int initial) {
    screen_state_init();

    // register action callbacks
    G_bolos_ux_context.button_push_callback = screen_onboarding_1_2_pin_button;

    // initial typed digit is always this one
    /*
    if (os_setting_get(OS_SETTING_SHUFFLE_PIN) == 0) {
    */
    G_bolos_ux_context.string_buffer[0] = '5';
    G_bolos_ux_context.string_buffer[1] = '\0';
    /*
    }
    else {
      G_bolos_ux_context.string_buffer[0] = '0' + rng_u8_modulo(10);
      G_bolos_ux_context.string_buffer[1] = '\0';
    }
    */

    if (initial == 1) {
        // blank pins, first step first
        os_memset(G_bolos_ux_context.pin_buffer, 0,
                  sizeof(G_bolos_ux_context.pin_buffer));

        G_bolos_ux_context.onboarding_index =
            0; // where the pin is stored in pin_buffer

        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_1_pin_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count = ARRAYLEN(screen_onboarding_1_pin_elements);
    } else {
        G_bolos_ux_context.onboarding_index =
            4; // where the pin confirm is stored in pin_buffer

        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_onboarding_2_pin_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count = ARRAYLEN(screen_onboarding_2_pin_elements);
    }
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
