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

unsigned int
screen_modal_validate_pin_wiped_1_button(unsigned int button_mask,
                                         unsigned int button_mask_counter);
unsigned int
screen_modal_validate_pin_wiped_2_button(unsigned int button_mask,
                                         unsigned int button_mask_counter);
unsigned int
screen_modal_validate_pin_wiped_3_button(unsigned int button_mask,
                                         unsigned int button_mask_counter);
unsigned int screen_modal_validate_pin_remaining_attempts_button(
    unsigned int button_mask, unsigned int button_mask_counter);

const bagl_element_t screen_modal_validate_pin_error_elements[] = {
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

const bagl_element_t screen_modal_validate_pin_remaining_attempts_elements[] = {
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

const bagl_element_t screen_modal_validate_pin_wiped_1_elements[] = {
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

const bagl_element_t screen_modal_validate_pin_wiped_2_elements[] = {
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

const bagl_element_t screen_modal_validate_pin_wiped_3_elements[] = {
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

unsigned int screen_modal_validate_pin_remaining_attempts_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // re-enter the first pin
        screen_modal_validate_pin_init();
        break;
    }
    return 1;
}

unsigned int
screen_modal_validate_pin_error_button(unsigned int button_mask,
                                       unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:

        // if too much retries, then perform onboarding again
        if (!os_perso_isonboarded()) {
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .element_arrays[0]
                .element_array = screen_modal_validate_pin_wiped_1_elements;
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_modal_validate_pin_wiped_1_elements);
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .button_push_callback =
                screen_modal_validate_pin_wiped_1_button;
            screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        } else {
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .element_arrays[0]
                .element_array =
                screen_modal_validate_pin_remaining_attempts_elements;
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_modal_validate_pin_remaining_attempts_elements);
            G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .button_push_callback =
                screen_modal_validate_pin_remaining_attempts_button;
            screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        }
        break;
    }
    return 1;
}

unsigned int
screen_modal_validate_pin_wiped_1_button(unsigned int button_mask,
                                         unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array = screen_modal_validate_pin_wiped_2_elements;
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_modal_validate_pin_wiped_2_elements);
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .button_push_callback = screen_modal_validate_pin_wiped_2_button;
        screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        break;
    }
    return 1;
}

unsigned int
screen_modal_validate_pin_wiped_2_button(unsigned int button_mask,
                                         unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array = screen_modal_validate_pin_wiped_3_elements;
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_modal_validate_pin_wiped_3_elements);
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .button_push_callback = screen_modal_validate_pin_wiped_3_button;
        screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        break;
    }
    return 1;
}

void screen_modal_validate_pin_wiped_3_button_help_exit(void) {
    // pop the validate pin slot
    G_bolos_ux_context.exit_code = BOLOS_UX_CANCEL;
    screen_display_init(screen_stack_pop());
}

unsigned int
screen_modal_validate_pin_wiped_3_button(unsigned int button_mask,
                                         unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_help_init(screen_modal_validate_pin_wiped_3_button_help_exit);
        break;
    }
    return 1;
}

unsigned int screen_modal_validate_pin_end(unsigned char *pin_buffer,
                                           unsigned int pin_length) {
    UNUSED(pin_buffer);
    UNUSED(pin_length);
    // check if pin and confirmation match, if not, error message
    if (!os_global_pin_check((unsigned char *)G_bolos_ux_context.pin_buffer,
                             strlen(G_bolos_ux_context.pin_buffer))) {
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array = screen_modal_validate_pin_error_elements;
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_modal_validate_pin_error_elements);
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .button_push_callback = screen_modal_validate_pin_error_button;
        screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
        return 1;
    }

    // pin is ok, prepare return value
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .exit_code_after_elements_displayed = BOLOS_UX_OK;
    screen_display_init(screen_stack_pop());
    return 0;
}

void screen_modal_validate_pin_init(void) {
    unsigned int stack_slot = 0;

    // NOTE: on modal, we can take the place of the non modal version (before
    // displaying dashboard, or on top of app requesting a pin validation)

    // will push back the poweroff on top of screen stack
    stack_slot = screen_stack_is_element_array_present(
        (const bagl_element_t *)screen_modal_validate_pin_end);
    if (stack_slot) {
        screen_stack_remove(stack_slot - 1);
    }

    // this is a modal
    stack_slot = screen_stack_push();

    // SECURITY: reset global pin state, we are typing again
    os_global_pin_invalidate();

    // use the common pin ux
    screen_common_pin_init(stack_slot, screen_modal_validate_pin_end);
}

#endif // OS_IO_SEPROXYHAL
