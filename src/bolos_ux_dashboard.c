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

#define DASHBOARD_SETTINGS_APPNAME "Settings"

#ifdef OS_IO_SEPROXYHAL

// DESIGN NOTE: index 0 of the slider is the settings

// draw screen
const bagl_element_t screen_dashboard_elements[] = {
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

    {{BAGL_ICON, 0x01, 23, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON /*settings*/, 0x11, 23, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000,
      0, BAGL_GLYPH_ICON_COG_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x41, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_RECTANGLE, 0x02, 46, 10, 3, 1, 0, 0, BAGL_FILL, 0xFFFFFF, 0x000000,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x03, 56, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON /*settings*/, 0x13, 56, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000,
      0, BAGL_GLYPH_ICON_COG_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_RECTANGLE, 0x04, 79, 10, 3, 1, 0, 0, BAGL_FILL, 0xFFFFFF, 0x000000,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x05, 89, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000, 0, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON /*settings*/, 0x15, 89, 3, 16, 16, 0, 0, 0, 0xFFFFFF, 0x000000,
      0, BAGL_GLYPH_ICON_COG_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x45, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // potentially larger than screen, enable horizontal scrolling
    {{BAGL_LABELINE, 0x23, 0, 28, 128, 32, 10, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_dashboard_unsigned_app_elements[] = {
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

    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Open non",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x00, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "genuine app?",
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

typedef struct screen_dashboard_s {
    application_t app;
    unsigned int os_index;
} screen_dashboard_t;

unsigned int screen_dashboard_order_is_after(const unsigned char *cmp,
                                             const unsigned char *orig) {
    return strcmp((const char *)cmp, (const char *)orig) > 0;
}

// retrieve the application at index which match the app filter
unsigned int screen_dashboard_get_app(unsigned int nth,
                                      screen_dashboard_t *db) {
    if (nth >= os_registry_count()) {
        return 0;
    }
    unsigned int total;
    application_t tmp;
    unsigned char previous_app_name[sizeof(tmp.name)];

    db->os_index = -1UL;

    // find the first app in order
    os_registry_get(0, &tmp);
    os_memmove(previous_app_name, tmp.name, sizeof(tmp.name));
    total = os_registry_count();
    while (total--) {
        os_registry_get(total, &tmp);
        // app 'total' is before previous, select it
        if (db->os_index == -1UL ||
            screen_dashboard_order_is_after(previous_app_name, tmp.name)) {
            db->os_index = total;
            os_memmove(previous_app_name, tmp.name, sizeof(tmp.name));
        }
    }

    // if not requesting the first entry then seek for the next ones
    if (nth) {
        while (nth--) {
            unsigned int not_found = 1;
            unsigned char current_app_name[sizeof(tmp.name)];

        again:
            os_registry_get(db->os_index, &tmp);
            os_memmove(current_app_name, tmp.name, sizeof(tmp.name));
            total = os_registry_count();
            while (total--) {
                if (db->os_index != total) {
                    os_registry_get(total, &tmp);
                    // return ordered
                    if (db->os_index == -1UL ||
                        (screen_dashboard_order_is_after(tmp.name,
                                                         previous_app_name) &&
                         (not_found || screen_dashboard_order_is_after(
                                           current_app_name, tmp.name)))) {
                        db->os_index = total;
                        not_found = 0;
                        goto again;
                    }
                }
            }

            // store previous app name for next nth seek
            os_memmove(previous_app_name, current_app_name, sizeof(tmp.name));
        }
    }

    // ensure to return not found when os_index is not correctly inited
    if (db->os_index == -1UL) {
        return 0;
    }

    // get info on the last app
    os_registry_get(db->os_index, &db->app);
    return 1;
}

void screen_dashboard_disable_bolos_before_app(void) {
    // disable ticker
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 2;
    G_io_seproxyhal_spi_buffer[3] = 0;
    G_io_seproxyhal_spi_buffer[4] = 0;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);

    // ble off
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_RADIO_POWER;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 1;
    G_io_seproxyhal_spi_buffer[3] = 0;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);

    // usb off
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_USB_CONFIG;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 1;
    G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_USB_CONFIG_DISCONNECT;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);
}

unsigned int
screen_dashboard_unsigned_app_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    screen_dashboard_t db;
    int i = 0;
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        screen_dashboard_init();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        do {
            screen_dashboard_get_app(
                G_bolos_ux_context.dashboard_last_selected + i, &db);
            i++;
        } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);

        screen_dashboard_disable_bolos_before_app();

        os_sched_exec(db.os_index); // no return
        break;
    }
    return 0;
}

unsigned int screen_dashboard_button(unsigned int button_mask,
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
        /*
        // don't go too far
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE) {
          return 0;
        }
        */
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
            screen_dashboard_elements;
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count = ARRAYLEN(screen_dashboard_elements);
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        screen_display_init();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // remember last booted
        G_bolos_ux_context.dashboard_last_selected =
            G_bolos_ux_context.hslider3_current;

        if (G_bolos_ux_context.hslider3_current <
            G_bolos_ux_context.hslider3_total - 1) {
            screen_dashboard_t db;

            // if application is not signed when installed, nor is using the
            // issuer key,
            // then ask if the user really is ok to run it, this solves the
            // security flaw
            // that apps are not wiped when seed is wiped after 3 wrong PIN
            // attempts.
            int i = 0;
            do {
                screen_dashboard_get_app(
                    G_bolos_ux_context.hslider3_current + i, &db);
                i++;
            } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
            if ((db.app.flags &
                 (APPLICATION_FLAG_ISSUER | APPLICATION_FLAG_SIGNED)) == 0) {
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array = screen_dashboard_unsigned_app_elements;
                G_bolos_ux_context.screen_current_element_arrays[0]
                    .element_array_count =
                    ARRAYLEN(screen_dashboard_unsigned_app_elements);
                G_bolos_ux_context.screen_current_element_arrays_count = 1;
                G_bolos_ux_context.button_push_callback =
                    screen_dashboard_unsigned_app_button;
                screen_display_init();
            } else {
                screen_dashboard_disable_bolos_before_app();

                // delegate boot
                os_sched_exec(db.os_index); // no return
            }
        } else {
            // show settings (init)
            screen_settings_init(1);
        }
        break;
    }
    return 0;
}

unsigned int screen_dashboard_before_element_display_callback(
    const bagl_element_t *element) {
    screen_dashboard_t db;
    int i;
    switch (element->component.userid) {
    // left app icon
    case 0x01:
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_before ==
                G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        i = 0;
        do {
            screen_dashboard_get_app(G_bolos_ux_context.hslider3_before + i,
                                     &db);
            i++;
        } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
        screen_prepare_masked_icon(db.app.icon, db.app.icon_length);
        break;
    // left settings icon
    case 0x11:
        if (G_bolos_ux_context.hslider3_before !=
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        break;
    // left arrow
    case 0x41:
        /*
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE) {
          return 0;
        }
        */
        break;
    // left '-'
    case 0x02:
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE) {
            return 0;
        }
        break;

    // current app icon
    case 0x03:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        i = 0;
        do {
            screen_dashboard_get_app(G_bolos_ux_context.hslider3_current + i,
                                     &db);
            i++;
        } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
        screen_prepare_masked_icon(db.app.icon, db.app.icon_length);
        break;
    // current settings icon
    case 0x13:
        if (G_bolos_ux_context.hslider3_current !=
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        break;

    // right '-'
    case 0x04:
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE) {
            return 0;
        }
        break;

    // right app icon
    case 0x05:
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_after ==
                G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        i = 0;
        do {
            screen_dashboard_get_app(G_bolos_ux_context.hslider3_after + i,
                                     &db);
            i++;
        } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
        screen_prepare_masked_icon(db.app.icon, db.app.icon_length);
        break;

    // right settings icon
    case 0x15:
        if (G_bolos_ux_context.hslider3_after !=
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        break;

    // right arrow
    case 0x45:
        break;

    // current application name
    case 0x23:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            strcpy(G_bolos_ux_context.string_buffer,
                   DASHBOARD_SETTINGS_APPNAME);
            break;
        } else {
            i = 0;
            do {
                screen_dashboard_get_app(
                    G_bolos_ux_context.hslider3_current + i, &db);
                i++;
            } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
            strcpy(G_bolos_ux_context.string_buffer, (const char *)db.app.name);
        }
        break;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int screen_dashboard_displayed(unsigned int i) {
    UNUSED(i);

    if (G_bolos_ux_context.dashboard_redisplayed) {
        G_bolos_ux_context.dashboard_redisplayed = 0;

        G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS;
        G_io_seproxyhal_spi_buffer[1] = 0;
        G_io_seproxyhal_spi_buffer[2] = 5;
        G_io_seproxyhal_spi_buffer[3] =
            SEPROXYHAL_TAG_SCREEN_ANIMATION_STATUS_VERTICAL_SPLIT_SLIDE;
        G_io_seproxyhal_spi_buffer[4] = 0;   // split y coordinate
        G_io_seproxyhal_spi_buffer[5] = 19;  // split y coordinate
        G_io_seproxyhal_spi_buffer[6] = 0;   // duration of the animation in ms
        G_io_seproxyhal_spi_buffer[7] = 200; // duration of the animation in ms
        io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 8);
        return 0; // don't consider screen ended, more data sent
    }
    return 1;
}

void screen_dashboard_init(void) {
    unsigned int count;
    screen_dashboard_t db;

    screen_state_init();

    // static dashboard content
    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_dashboard_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_dashboard_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    // ensure the string_buffer will be set before each button is displayed
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_dashboard_before_element_display_callback;
    G_bolos_ux_context.button_push_callback = screen_dashboard_button;

    // dashboard says ok when done displaying
    G_bolos_ux_context.exit_code_after_elements_displayed = BOLOS_UX_OK;
    G_bolos_ux_context.screen_displayed_callback = screen_dashboard_displayed;

    // count displayable apps
    G_bolos_ux_context.hslider3_total = 0;
    count = 0;
    while (screen_dashboard_get_app(count, &db)) {
        count++;
        if (!(db.app.flags & APPLICATION_FLAG_BOLOS_UX)) {
            G_bolos_ux_context.hslider3_total++;
        }
    }

    // add the fake settings app
    bolos_ux_hslider3_init(G_bolos_ux_context.hslider3_total + 1);

    // yolo last selected
    if (G_bolos_ux_context.dashboard_last_selected <
        G_bolos_ux_context.hslider3_total) {
        // don't modify directly to avoid invalid indexes
        while (G_bolos_ux_context.hslider3_current !=
               G_bolos_ux_context.dashboard_last_selected) {
            bolos_ux_hslider3_next();
        }
    }

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
