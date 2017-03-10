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

    {{BAGL_LABELINE, 0x00, 29, 22, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_LIGHT_16px, 0},
     "1.",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 48, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Choose a",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 48, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
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

unsigned int
screen_onboarding_2_pin_nomatch_button(unsigned int button_mask,
                                       unsigned int button_mask_counter);

//
unsigned int screen_onboarding_1_2_pin_entered(unsigned char *pin_buffer,
                                               unsigned int pin_length) {
    if (G_bolos_ux_context.onboarding_index == 0) {
        // copy pin to words_buffer to hold it.
        G_bolos_ux_context.words_buffer[0] = pin_length;
        os_memmove(G_bolos_ux_context.words_buffer + 1, pin_buffer, pin_length);

        // ask for pin confirmation
        screen_onboarding_1_2_pin_init(2);
        return 1;
    } else {
        // check if pin and confirmation match, if not, error message
        if (G_bolos_ux_context.words_buffer[0] != pin_length ||
            os_memcmp(pin_buffer, G_bolos_ux_context.words_buffer + 1,
                      pin_length) != 0) {
            G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
                screen_onboarding_2_pin_nomatch_elements;
            G_bolos_ux_context.screen_stack[0]
                .element_arrays[0]
                .element_array_count =
                ARRAYLEN(screen_onboarding_2_pin_nomatch_elements);
            G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
            G_bolos_ux_context.screen_stack[0].button_push_callback =
                screen_onboarding_2_pin_nomatch_button;
            screen_display_init(0);
            return 1;
        }

        // copy the variable pin with its prepend length
        os_memmove(G_bolos_ux_context.pin_buffer,
                   G_bolos_ux_context.words_buffer, pin_length + 1);

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
    return 0;
}

unsigned int
screen_onboarding_1_2_pin_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        screen_common_pin_init(0, screen_onboarding_1_2_pin_entered);
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

void screen_onboarding_1_2_pin_init(unsigned int initial) {
    screen_state_init(0);

    // register action callbacks
    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_onboarding_1_2_pin_button;

    // no pin shuffling during onboarding
    G_bolos_ux_context.string_buffer[0] = '5';
    G_bolos_ux_context.string_buffer[1] = '\0';

    if (initial == 1) {
        // blank pins, first step first
        os_memset(G_bolos_ux_context.pin_buffer, 0,
                  sizeof(G_bolos_ux_context.pin_buffer));
        G_bolos_ux_context.onboarding_index = 0;
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_1_pin_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count = ARRAYLEN(screen_onboarding_1_pin_elements);
    } else {
        G_bolos_ux_context.onboarding_index = 1;
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_onboarding_2_pin_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count = ARRAYLEN(screen_onboarding_2_pin_elements);
    }
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    screen_display_init(0);
}

#endif // OS_IO_SEPROXYHAL
