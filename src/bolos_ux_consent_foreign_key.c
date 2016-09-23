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

const bagl_element_t screen_consent_foreign_key_elements[] = {
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

    {{BAGL_LABELINE, 0x10, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Trust unknown",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x10, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "app manager?",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_LABELINE, 0x11, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Public key",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x31, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
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
};

unsigned int screen_consent_foreign_key_before_element_display_callback(
    const bagl_element_t *element) {
    if ((element->component.userid & 0x10) &&
        (element->component.userid & 0x0F) !=
            G_bolos_ux_context.onboarding_index) {
        return 0;
    }

    switch (element->component.userid) {
    case 0x31:
        // compact hash of the app
        array_hexstr(G_bolos_ux_context.string_buffer,
                     G_bolos_ux_context.parameters.u.foreign_key.host_pubkey.W,
                     BOLOS_UX_HASH_LENGTH / 2);
        G_bolos_ux_context.string_buffer[BOLOS_UX_HASH_LENGTH / 2 * 2] = '.';
        G_bolos_ux_context.string_buffer[BOLOS_UX_HASH_LENGTH / 2 * 2 + 1] =
            '.';
        G_bolos_ux_context.string_buffer[BOLOS_UX_HASH_LENGTH / 2 * 2 + 2] =
            '.';
        array_hexstr(
            G_bolos_ux_context.string_buffer + BOLOS_UX_HASH_LENGTH / 2 * 2 + 3,
            G_bolos_ux_context.parameters.u.foreign_key.host_pubkey.W +
                G_bolos_ux_context.parameters.u.foreign_key.host_pubkey.W_len -
                BOLOS_UX_HASH_LENGTH / 2,
            BOLOS_UX_HASH_LENGTH / 2);
        break;
    }
    return 1;
}

void screen_consent_foreign_key_init(void) {
    screen_state_init();

    // display the welcome screen first

    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_consent_foreign_key_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_consent_foreign_key_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_consent_foreign_key_before_element_display_callback;

    screen_consent_ticker_init(2, CONSENT_INTERVAL_MS);
}

#endif // OS_IO_SEPROXYHAL
