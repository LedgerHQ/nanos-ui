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

const bagl_element_t screen_consent_add_elements[] = {
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

    {{BAGL_ICON, 0x10, 23, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_DOWNLOAD_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x10, 44, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     "Install app",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x11, 56, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x31, 23, 28, 82, 11, 0x80 | 0x10, 0, 0, 0xFFFFFF,
      0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER,
      26},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x12, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Identifier",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x32, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

};

const bagl_element_t *screen_consent_add_before_element_display_callback(
    const bagl_element_t *element) {
    // for dashboard, setup the current application's name
    switch (element->component.userid) {
    case 0x11:
        screen_prepare_masked_icon(
            G_bolos_ux_context.parameters.u.appadd.appentry.icon,
            G_bolos_ux_context.parameters.u.appadd.appentry.icon_length);
        break;

    case 0x31:
        strcpy(
            G_bolos_ux_context.string_buffer,
            (const char *)G_bolos_ux_context.parameters.u.appadd.appentry.name);
        break;

    case 0x32:
        // compact hash of the app
        screen_hex_identifier_string_buffer(
            G_bolos_ux_context.parameters.u.appadd.appentry.hash, 32);
        break;
    }

    if (element->component.userid & 0x10) {
        if ((element->component.userid & 0x0F) !=
            G_bolos_ux_context.onboarding_index) {
            return 0;
        }
        switch (element->component.userid & 0x0F) {
        case 0:
            screen_consent_set_interval(2000);
            break;
        case 1:
            screen_consent_set_interval(
                MAX(3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
            break;
        case 2:
            screen_consent_set_interval(3000);
            break;
        }
    }

    return element;
}

void screen_consent_add_init(void) {
    screen_state_init(0);
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_consent_add_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_consent_add_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_consent_add_before_element_display_callback;

    screen_consent_ticker_init(3, 2000, 1 /* ask pin to */);
}

#endif // OS_IO_SEPROXYHAL
