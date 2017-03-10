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

const bagl_element_t screen_consent_ux_not_signed_1_elements[] = {
    // type                               userid    x    y   w    h  str rad
    // fill      fg        bg      fid iid  txt   touchparams...       ]
    {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x00, 13, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_WARNING_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x00, 34, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "User interface",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 34, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "is not genuine",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_consent_ux_not_signed_2_elements[] = {
    // type                               userid    x    y   w    h  str rad
    // fill      fg        bg      fid iid  txt   touchparams...       ]
    {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x11, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Identifier",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x31, 0, 26, 128, 11, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

unsigned int
screen_consent_ux_not_signed_2_button(unsigned int button_mask,
                                      unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.exit_code = BOLOS_UX_OK;
        break;
    }
    return 0;
}

unsigned int
screen_consent_ux_not_signed_1_button(unsigned int button_mask,
                                      unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_consent_ux_not_signed_2_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            ARRAYLEN(screen_consent_ux_not_signed_2_elements);
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_consent_ux_not_signed_2_button;
        screen_display_init(0);
        break;
    }
    return 0;
}

const bagl_element_t *
screen_consent_ux_not_signed_before_element_display_callback(
    const bagl_element_t *element) {
    switch (element->component.userid) {
    case 0x31:
        screen_hex_identifier_string_buffer(
            G_bolos_ux_context.parameters.u.ux_not_signed.ux_app.hash, 32);
        break;
    }
    return element;
}

void screen_consent_ux_not_signed_init(void) {
    screen_state_init(0);
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_consent_ux_not_signed_1_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_consent_ux_not_signed_1_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_consent_ux_not_signed_1_button;
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_consent_ux_not_signed_before_element_display_callback;
    screen_display_init(0);
}

#endif // OS_IO_SEPROXYHAL
