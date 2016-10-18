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

unsigned int
screen_validate_pin_wiped_1_button(unsigned int button_mask,
                                   unsigned int button_mask_counter);
unsigned int
screen_validate_pin_wiped_2_button(unsigned int button_mask,
                                   unsigned int button_mask_counter);
unsigned int
screen_validate_pin_wiped_3_button(unsigned int button_mask,
                                   unsigned int button_mask_counter);
unsigned int
screen_validate_pin_remaining_attempts_button(unsigned int button_mask,
                                              unsigned int button_mask_counter);

const bagl_element_t screen_validate_pin_error_elements[] = {
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

    {{BAGL_ICON, 0x00, 7, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_CROSS_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 28, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Invalid PIN code",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_validate_pin_remaining_attempts_elements[] = {
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

    {{BAGL_LABELINE, 0x01, 0, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_validate_pin_wiped_1_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Your device has been",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "reset (3 wrong PIN).",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_validate_pin_wiped_2_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "To restore your device",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "configuration,",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_validate_pin_wiped_3_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "use your confidential",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "recovery sheet.",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_validate_pin_elements[] = {
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
// FOREACH: x += validated digit_count * 18
const bagl_element_t screen_validate_pin_entry_edit_elements[] = {
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
// FOREACH: x += validating digit_count * 18
const bagl_element_t screen_validate_pin_entry_set_elements[] = {
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

unsigned int screen_validate_pin_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    case 0x01: {
        unsigned int retries = os_global_pin_retries();
        if (retries < 10) {
            G_bolos_ux_context.string_buffer[0] = '0' + retries;
            G_bolos_ux_context.string_buffer[1] = 0;
        } else {
            G_bolos_ux_context.string_buffer[0] = '0' + ((retries) / 10);
            G_bolos_ux_context.string_buffer[1] = '0' + ((retries) % 10);
            G_bolos_ux_context.string_buffer[2] = 0;
        }
        strcat(G_bolos_ux_context.string_buffer, " remaining attempt(s)");
        break;
    }
    }
    return 1;
}

unsigned int screen_validate_pin_remaining_attempts_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // re-enter the first pin
        screen_validate_pin_init();
        break;
    }
    return 1;
}

unsigned int
screen_validate_pin_error_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:

        // if too much retries, then perform onboarding again
        if (!os_perso_isonboarded()) {
            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                screen_validate_pin_wiped_1_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_validate_pin_wiped_1_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            G_bolos_ux_context.button_push_callback =
                screen_validate_pin_wiped_1_button;
            screen_display_init();
        } else {
            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                screen_validate_pin_remaining_attempts_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_validate_pin_remaining_attempts_elements);
            G_bolos_ux_context.screen_current_element_arrays_count = 1;
            G_bolos_ux_context.button_push_callback =
                screen_validate_pin_remaining_attempts_button;
            screen_display_init();
        }
        break;
    }
    return 1;
}

unsigned int
screen_validate_pin_wiped_1_button(unsigned int button_mask,
                                   unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_validate_pin_wiped_2_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_validate_pin_wiped_2_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.button_push_callback =
            screen_validate_pin_wiped_2_button;
        screen_display_init();
        break;
    }
    return 1;
}

unsigned int
screen_validate_pin_wiped_2_button(unsigned int button_mask,
                                   unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            screen_validate_pin_wiped_3_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_validate_pin_wiped_3_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.button_push_callback =
            screen_validate_pin_wiped_3_button;
        screen_display_init();
        break;
    }
    return 1;
}

void screen_validate_pin_wiped_3_button_help_exit(void) {
    G_bolos_ux_context.exit_code = BOLOS_UX_CANCEL;
}

unsigned int
screen_validate_pin_wiped_3_button(unsigned int button_mask,
                                   unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_help_init(screen_validate_pin_wiped_3_button_help_exit);
        break;
    }
    return 1;
}

unsigned int screen_validate_pin_button(unsigned int button_mask,
                                        unsigned int button_mask_counter) {
    unsigned int digit_count = strlen(G_bolos_ux_context.pin_buffer);

    G_bolos_ux_context.exit_code_after_elements_displayed = 0;

    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT |
        BUTTON_RIGHT: // NEXT DIGIT or CONTINUE
        // append the current displayed digit
        G_bolos_ux_context.pin_buffer[digit_count] =
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
            // check if pin and confirmation match, if not, error message
            if (!os_global_pin_check(
                    (unsigned char *)G_bolos_ux_context.pin_buffer,
                    digit_count + 1)) {
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array = screen_validate_pin_error_elements;
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array_count =
                    ARRAYLEN(screen_validate_pin_error_elements);
                G_bolos_ux_context.screen_current_element_arrays_count = 1;
                G_bolos_ux_context.button_push_callback =
                    screen_validate_pin_error_button;
                screen_display_init();
                return 1;
            }

            if (G_bolos_ux_context.last_ux_id ==
                BOLOS_UX_CHANGE_ALTERNATE_PIN) {
                // move to the next part
                screen_onboarding_1_2_pin_init(1);
            } else {
                // pin is ok, prepare return value
                G_bolos_ux_context.exit_code =
                    BOLOS_UX_OK; // no elements to be drawn, we can reply right
                                 // after even is processed
            }
        } else {
            // draw current digit as validated
            os_memmove(G_bolos_ux_context.screen_volatile_elements,
                       screen_validate_pin_entry_set_elements,
                       sizeof(screen_validate_pin_entry_set_elements));
            // draw at the correct position
            G_bolos_ux_context.screen_volatile_elements[0].component.x +=
                digit_count * (18 + 2);
            G_bolos_ux_context.screen_volatile_elements[1].component.x +=
                digit_count * (18 + 2);
            G_bolos_ux_context.screen_volatile_elements[2].component.x +=
                digit_count * (18 + 2);

            G_bolos_ux_context.screen_current_element_arrays[0].element_array =
                G_bolos_ux_context.screen_volatile_elements;
            G_bolos_ux_context.screen_current_element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_validate_pin_entry_set_elements);
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
                   screen_validate_pin_entry_edit_elements,
                   sizeof(screen_validate_pin_entry_edit_elements));
        // draw at the correct position
        G_bolos_ux_context.screen_volatile_elements[0].component.x +=
            digit_count * (18 + 2);

        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            G_bolos_ux_context.screen_volatile_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_validate_pin_entry_edit_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        screen_display_init();
        break;
    }
    return 1;
}

void screen_validate_pin_init(void) {
    screen_state_init();

    // register action callbacks
    G_bolos_ux_context.button_push_callback = screen_validate_pin_button;
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_validate_pin_before_element_display_callback;

    // initialize first digit
    if (os_setting_get(OS_SETTING_SHUFFLE_PIN) == 0) {
        G_bolos_ux_context.string_buffer[0] = '5';
        G_bolos_ux_context.string_buffer[1] = '\0';
    } else {
        G_bolos_ux_context.string_buffer[0] = '0' + rng_u8_modulo(10);
        G_bolos_ux_context.string_buffer[1] = '\0';
    }

    // blank pins, first step first
    os_memset(G_bolos_ux_context.pin_buffer, 0,
              sizeof(G_bolos_ux_context.pin_buffer));

    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_validate_pin_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_validate_pin_elements);

    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
