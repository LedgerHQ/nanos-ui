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

#ifdef BOLOS_RELEASE
#define DASHBOARD_SETTINGS_APPNAME "Settings"
#else
#define DASHBOARD_SETTINGS_APPNAME "NOT A RELEASE"
#endif // BOLOS_RELEASE

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
    {{BAGL_LABELINE, 0x23, 13, 28, 102, 11, 10, 0, 0, 0xFFFFFF, 0x000000,
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

    {{BAGL_LABELINE, 0x10, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Open non",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x10, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "genuine app?",
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
    io_seproxyhal_disable_io();

    // pop all screens as we're running the app => reinited next call when first
    // screen will be requested to be displayed
    os_memset(G_bolos_ux_context.screen_stack, 0,
              sizeof(G_bolos_ux_context.screen_stack));
    G_bolos_ux_context.screen_stack_count = 0;
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

const bagl_element_t *
screen_dashboard_unsigned_app_before_element_display_callback(
    const bagl_element_t *element) {
    screen_dashboard_t db;
    int i = 0;

    if ((element->component.userid & 0x10) &&
        (element->component.userid & 0x0F) !=
            G_bolos_ux_context.onboarding_index) {
        return NULL;
    }

    switch (element->component.userid) {
    case 0x31:
        do {
            screen_dashboard_get_app(
                G_bolos_ux_context.dashboard_last_selected + i, &db);
            i++;
        } while (db.app.flags & APPLICATION_FLAG_BOLOS_UX);
        screen_hex_identifier_string_buffer(db.app.hash, 32);
        break;
    }
    return element;
}

unsigned int screen_dashboard_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_FAST | BUTTON_LEFT:
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        bolos_ux_hslider3_previous();
        goto redraw;

    case BUTTON_EVT_FAST | BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        bolos_ux_hslider3_next();

    redraw:
        G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
            screen_dashboard_elements;
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count = ARRAYLEN(screen_dashboard_elements);
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        screen_display_init(0);
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

            // requested non genuine validation
            if ((db.app.flags &
                 (APPLICATION_FLAG_ISSUER | APPLICATION_FLAG_CUSTOM_CA |
                  APPLICATION_FLAG_SIGNED)) == 0) {
                G_bolos_ux_context.screen_stack[0]
                    .element_arrays[0]
                    .element_array = screen_dashboard_unsigned_app_elements;
                G_bolos_ux_context.screen_stack[0]
                    .element_arrays[0]
                    .element_array_count =
                    ARRAYLEN(screen_dashboard_unsigned_app_elements);
                G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
                G_bolos_ux_context.screen_stack[0]
                    .screen_before_element_display_callback =
                    screen_dashboard_unsigned_app_before_element_display_callback;

                screen_consent_ticker_init(2, 3000, 0);
                // override the consent callback, just use the logic
                G_bolos_ux_context.screen_stack[0].button_push_callback =
                    screen_dashboard_unsigned_app_button;
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

const bagl_element_t *screen_dashboard_before_element_display_callback(
    const bagl_element_t *element) {
    screen_dashboard_t db;
    int i;
    switch (element->component.userid) {
    // left app icon
    case 0x01:
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_before ==
                G_bolos_ux_context.hslider3_total - 1) {
            return NULL;
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
            return NULL;
        }
        break;
    // left arrow
    case 0x41:
        /*
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE) {
          return NULL;
        }
        */
        break;
    // left '-'
    case 0x02:
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE) {
            return NULL;
        }
        break;

    // current app icon
    case 0x03:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return NULL;
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
            return NULL;
        }
        break;

    // right '-'
    case 0x04:
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE) {
            return NULL;
        }
        break;

    // right app icon
    case 0x05:
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_after ==
                G_bolos_ux_context.hslider3_total - 1) {
            return NULL;
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
            return NULL;
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
    return element;
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

void screen_dashboard_prepare(void) {
    unsigned int count;
    screen_dashboard_t db;

    screen_state_init(0);

    // static dashboard content
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array =
        screen_dashboard_elements;
    G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count =
        ARRAYLEN(screen_dashboard_elements);
    G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

    // ensure the string_buffer will be set before each button is displayed
    G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback =
        screen_dashboard_before_element_display_callback;
    G_bolos_ux_context.screen_stack[0].button_push_callback =
        screen_dashboard_button;

    // dashboard says ok when done displaying
    G_bolos_ux_context.screen_stack[0].exit_code_after_elements_displayed =
        BOLOS_UX_OK;
    G_bolos_ux_context.screen_stack[0].displayed_callback =
        screen_dashboard_displayed;

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
        bolos_ux_hslider3_set_current(
            G_bolos_ux_context.dashboard_last_selected);
    }
}

void screen_dashboard_init(void) {
    screen_dashboard_prepare();

    screen_display_init(0);
}

#endif // OS_IO_SEPROXYHAL
