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

#define BRIGHTNESS_DEFAULT_PERCENT 20
#define BRIGHTNESS_DEFAULT 3
#define ROTATION_DEFAULT 0
#define INVERSION_DEFAULT 0

#define IMMEDIATE_SETTING_SAVE

typedef struct screen_desc_e {
    const char *name;
    const bagl_element_t *elements;
    unsigned int elements_count;
    bagl_element_callback_t before_display_callback;
    button_push_callback_t button_push_callback;
    appmain_t setting_init;
} screen_desc_t;

extern const screen_desc_t screen_settings_desc[];

void screen_settings_apply_internal(unsigned int use_persisted,
                                    unsigned int brightness,
                                    unsigned int rotation,
                                    unsigned int invert) {
    // apply default settings
    if (!os_perso_isonboarded() && use_persisted) {
        brightness = BRIGHTNESS_DEFAULT;
        rotation = ROTATION_DEFAULT;
        invert = INVERSION_DEFAULT;
    } else if (use_persisted) {
        brightness = os_setting_get(OS_SETTING_BRIGHTNESS);
        rotation = os_setting_get(OS_SETTING_ROTATION);
        invert = os_setting_get(OS_SETTING_INVERT);
    }

#ifdef ALWAYS_INVERT // fast discriminant for UX trigger check // debug
    invert = 1;
#endif // ALWAYS_INVERT

    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_SCREEN_CONFIG;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 2;
    G_io_seproxyhal_spi_buffer[3] = 0x80 /*power on screen*/
                                    | (rotation ? 4 : 0) | (invert ? 1 : 0);
    switch (brightness) {
    case 1:
        G_io_seproxyhal_spi_buffer[4] = 0;
        break;
    case 2:
        G_io_seproxyhal_spi_buffer[4] = 10;
        break;
    default:
    case 3:
        G_io_seproxyhal_spi_buffer[4] = BRIGHTNESS_DEFAULT_PERCENT;
        break;
    case 4:
        G_io_seproxyhal_spi_buffer[4] = 30;
        break;
    case 5:
        G_io_seproxyhal_spi_buffer[4] = 50;
        break;
    case 6:
        G_io_seproxyhal_spi_buffer[4] = 100;
        break;
    }
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}

void screen_settings_apply(void) {
    // use NVRAM values
    screen_settings_apply_internal(1, 0, 0, 0);
}

// draw screen
const bagl_element_t screen_settings_elements[] = {
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

    // application name
    {{BAGL_LABELINE, 0x01, 0, 3, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x02, 0, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x12, 29, 9, 14, 14, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_DASHBOARD_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x12, 50, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x03, 0, 35, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // icons
    {{BAGL_ICON, 0x21, 3, 14, 7, 4, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_UP},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x22, 118, 14, 7, 4, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_DOWN},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

void screen_settings_display_current(void) {
    // only setup when not displaying help (or quit)
    if (screen_settings_desc[G_bolos_ux_context.hslider3_current +
                             G_bolos_ux_context.onboarding_step]
            .elements_count != 0) {
        G_bolos_ux_context.screen_current_element_arrays[0].element_array =
            (const bagl_element_t *)PIC(
                screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                     G_bolos_ux_context.onboarding_step]
                    .elements);
        G_bolos_ux_context.screen_current_element_arrays[0]
            .element_array_count =
            screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                 G_bolos_ux_context.onboarding_step]
                .elements_count;
        G_bolos_ux_context.screen_current_element_arrays_count = 1;
        G_bolos_ux_context.screen_before_element_display_callback =
            (bagl_element_callback_t)PIC(
                screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                     G_bolos_ux_context.onboarding_step]
                    .before_display_callback);
        G_bolos_ux_context.button_push_callback = (button_push_callback_t)PIC(
            screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                 G_bolos_ux_context.onboarding_step]
                .button_push_callback);
        screen_display_init();
    }
}

unsigned int screen_settings_button(unsigned int button_mask,
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
        if (G_bolos_ux_context.hslider3_current == 0) {
            return 0;
        }
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
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        bolos_ux_hslider3_next();

    redraw:
        screen_settings_init(0);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            // request animation when settings exits
            G_bolos_ux_context.dashboard_redisplayed = 1;

            screen_dashboard_init();
        } else {
            ((appmain_t)PIC(
                screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                     G_bolos_ux_context.onboarding_step]
                    .setting_init))();
            screen_settings_display_current();
        }
        break;
    }
    return 0;
}

unsigned int
screen_settings_before_element_display_callback(const bagl_element_t *element) {
    switch (element->component.userid) {
    // previous setting
    case 0x01:
        // don't display help when quit app is displayed
        if (G_bolos_ux_context.hslider3_before == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_current == 0 ||
            G_bolos_ux_context.hslider3_current ==
                G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        strcpy(G_bolos_ux_context.string_buffer,
               (const char *)PIC(
                   screen_settings_desc[G_bolos_ux_context.hslider3_before +
                                        G_bolos_ux_context.onboarding_step]
                       .name));
        break;

    // special quit app
    case 0x12:
        if (G_bolos_ux_context.hslider3_current !=
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        strcpy(G_bolos_ux_context.string_buffer,
               (const char *)PIC(
                   screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                        G_bolos_ux_context.onboarding_step]
                       .name));
        break;

    // current selected
    case 0x02:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        strcpy(G_bolos_ux_context.string_buffer,
               (const char *)PIC(
                   screen_settings_desc[G_bolos_ux_context.hslider3_current +
                                        G_bolos_ux_context.onboarding_step]
                       .name));
        break;

    // next setting
    case 0x03:
        // don't display quit app when displaying help
        if (G_bolos_ux_context.hslider3_after == BOLOS_UX_HSLIDER3_NONE ||
            G_bolos_ux_context.hslider3_current >=
                G_bolos_ux_context.hslider3_total - 2) {
            return 0;
        }
        strcpy(G_bolos_ux_context.string_buffer,
               (const char *)PIC(
                   screen_settings_desc[G_bolos_ux_context.hslider3_after +
                                        G_bolos_ux_context.onboarding_step]
                       .name));
        break;

    // up
    case 0x21:
        if (G_bolos_ux_context.hslider3_current == 0) {
            return 0;
        }
        break;

    // down
    case 0x22:
        if (G_bolos_ux_context.hslider3_current ==
            G_bolos_ux_context.hslider3_total - 1) {
            return 0;
        }
        break;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

// ==========================================================================

const bagl_element_t screen_settings_brightness_elements[] = {
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

    // application name
    {{BAGL_LABELINE, 0x00, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     "Brightness",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_CIRCLE, 0x00, 34, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x00, 45, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x00, 56, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x00, 67, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x00, 78, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x00, 89, 20, 7, 7, 1, 3, 0, 0xFFFFFF, 0x000000, 0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_CIRCLE, 0x00, 34, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x02, 45, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x03, 56, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x04, 67, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x05, 78, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_CIRCLE, 0x06, 89, 20, 6, 6, 0, 3, BAGL_FILL, 0xFFFFFF, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // icons
    {{BAGL_ICON, 0x00, 3, 15, 6, 1, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LESS},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x00, 118, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_PLUS},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

unsigned int
screen_settings_brightness_before_display(const bagl_element_t *element) {
    if (element->component.userid) {
        if (G_bolos_ux_context.onboarding_index < element->component.userid) {
            return 0;
        }
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int
screen_settings_brightness_button(unsigned int button_mask,
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
        if (G_bolos_ux_context.onboarding_index > 1) {
            G_bolos_ux_context.onboarding_index--;
        }
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
        if (G_bolos_ux_context.onboarding_index < 6) {
            G_bolos_ux_context.onboarding_index++;
        }

    redraw:
        screen_settings_apply_internal(0, G_bolos_ux_context.onboarding_index,
                                       os_setting_get(OS_SETTING_ROTATION),
                                       os_setting_get(OS_SETTING_INVERT));
        screen_settings_display_current();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        os_setting_set(OS_SETTING_BRIGHTNESS,
                       G_bolos_ux_context.onboarding_index);
        screen_settings_init(0);
        break;
    }
    return 0;
}

void screen_settings_brightness_init(void) {
    G_bolos_ux_context.onboarding_index = os_setting_get(OS_SETTING_BRIGHTNESS);
    if (G_bolos_ux_context.onboarding_index == 0) {
        G_bolos_ux_context.onboarding_index = BRIGHTNESS_DEFAULT;
    }
}

// ==========================================================================

const bagl_element_t screen_settings_invert_elements[] = {
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

    // application name
    {{BAGL_LABELINE, 0x00, 36, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px, 0},
     "Invert",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x01, 74, 11, 16, 10, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_TOGGLE_ON},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x02, 74, 11, 16, 10, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_TOGGLE_OFF},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // icons
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

unsigned int
screen_settings_invert_before_display(const bagl_element_t *element) {
    switch (element->component.userid) {
    case 0x01:
        return G_bolos_ux_context.onboarding_index;
    case 0x02:
        return !G_bolos_ux_context.onboarding_index;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int screen_settings_invert_button(unsigned int button_mask,
                                           unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.onboarding_index = 0;
        goto redraw;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        G_bolos_ux_context.onboarding_index = 1;

    redraw:
        os_setting_set(OS_SETTING_INVERT, G_bolos_ux_context.onboarding_index);
        screen_settings_apply_internal(0, os_setting_get(OS_SETTING_BRIGHTNESS),
                                       os_setting_get(OS_SETTING_ROTATION),
                                       G_bolos_ux_context.onboarding_index);
        screen_settings_init(0);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // save setting and redisplay list
        os_setting_set(OS_SETTING_INVERT, G_bolos_ux_context.onboarding_index);
        screen_settings_init(0);
        break;
    }
    return 0;
}

void screen_settings_invert_init(void) {
    G_bolos_ux_context.onboarding_index = os_setting_get(OS_SETTING_INVERT);
}

// ==========================================================================

const bagl_element_t screen_settings_rotation_elements[] = {
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

    // application name
    {{BAGL_LABELINE, 0x00, 35, 19, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px, 0},
     "Rotate",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    {{BAGL_ICON, 0x01, 76, 11, 16, 10, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_TOGGLE_ON},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x02, 76, 11, 16, 10, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_TOGGLE_OFF},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // icons
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

unsigned int
screen_settings_rotation_before_display(const bagl_element_t *element) {
    switch (element->component.userid) {
    case 0x01:
        return G_bolos_ux_context.onboarding_index;
    case 0x02:
        return !G_bolos_ux_context.onboarding_index;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int screen_settings_rotation_button(unsigned int button_mask,
                                             unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.onboarding_index = 0;
        goto redraw;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        G_bolos_ux_context.onboarding_index = 180;

    redraw:
        os_setting_set(OS_SETTING_ROTATION,
                       G_bolos_ux_context.onboarding_index);
        screen_settings_apply_internal(0, os_setting_get(OS_SETTING_BRIGHTNESS),
                                       G_bolos_ux_context.onboarding_index,
                                       os_setting_get(OS_SETTING_INVERT));
        screen_settings_init(0);
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // save setting and redisplay list
        os_setting_set(OS_SETTING_ROTATION,
                       G_bolos_ux_context.onboarding_index);
        screen_settings_init(0);
        break;
    }
    return 0;
}

void screen_settings_rotation_init(void) {
    G_bolos_ux_context.onboarding_index = os_setting_get(OS_SETTING_ROTATION);
}

// ==========================================================================

const bagl_element_t screen_settings_version_elements[] = {
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

    {{BAGL_LABELINE, 0x01, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_LABELINE, 0x02, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
      BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
     G_bolos_ux_context.string_buffer,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

    // icons
    {{BAGL_ICON, 0x10, 3, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_LEFT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    {{BAGL_ICON, 0x11, 121, 12, 4, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
      BAGL_GLYPH_ICON_RIGHT},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

unsigned int
screen_settings_version_before_display(const bagl_element_t *element) {
    switch (element->component.userid) {
    // previous setting
    case 0x01:
        switch (G_bolos_ux_context.onboarding_index) {
        // SE
        case 0:
            strcpy(G_bolos_ux_context.string_buffer, "Secure Element");
            break;

        // MCU
        case 1:
            strcpy(G_bolos_ux_context.string_buffer, "MCU");
            break;
        }
        break;
    case 0x02:
        switch (G_bolos_ux_context.onboarding_index) {
        // SE
        case 0:
            strcpy(G_bolos_ux_context.string_buffer, VERSION);
            break;

        // MCU
        case 1:
            os_memset(G_bolos_ux_context.string_buffer, 0,
                      sizeof(G_bolos_ux_context.string_buffer));
            os_seph_version((unsigned char *)G_bolos_ux_context.string_buffer,
                            sizeof(G_bolos_ux_context.string_buffer));
            break;
        }
        break;

    case 0x10:
        return G_bolos_ux_context.onboarding_index != 0;

    case 0x11:
        return G_bolos_ux_context.onboarding_index != 1;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return 1;
}

unsigned int screen_settings_version_button(unsigned int button_mask,
                                            unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.onboarding_index = 0;
        goto redraw;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        G_bolos_ux_context.onboarding_index = 1;

    redraw:
        screen_settings_display_current();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // go back to settings list
        screen_settings_init(0);
        break;
    }
    return 0;
}

void screen_settings_version_init(void) {
    G_bolos_ux_context.onboarding_index = 0;
}

// ==========================================================================

void screen_settings_help_ended(void) {
    screen_settings_init(0);
}

void screen_settings_help_init(void) {
    screen_help_init(screen_settings_help_ended);
}

// ==========================================================================

const screen_desc_t screen_settings_desc[] = {
    {"Brightness", screen_settings_brightness_elements,
     ARRAYLEN(screen_settings_brightness_elements),
     screen_settings_brightness_before_display,
     screen_settings_brightness_button, screen_settings_brightness_init},

    {"Rotate screen", screen_settings_rotation_elements,
     ARRAYLEN(screen_settings_rotation_elements),
     screen_settings_rotation_before_display, screen_settings_rotation_button,
     screen_settings_rotation_init},

    {"Invert colors", screen_settings_invert_elements,
     ARRAYLEN(screen_settings_invert_elements),
     screen_settings_invert_before_display, screen_settings_invert_button,
     screen_settings_invert_init},

    {"Firmware", screen_settings_version_elements,
     ARRAYLEN(screen_settings_version_elements),
     screen_settings_version_before_display, screen_settings_version_button,
     screen_settings_version_init},

    {"Help", NULL, 0, NULL, NULL, screen_settings_help_init},

    {"Quit app", NULL, 0, NULL, NULL, NULL},
};

void screen_settings_init(unsigned int initial) {
    screen_state_init();

    // static dashboard content
    G_bolos_ux_context.screen_current_element_arrays[0].element_array =
        screen_settings_elements;
    G_bolos_ux_context.screen_current_element_arrays[0].element_array_count =
        ARRAYLEN(screen_settings_elements);
    G_bolos_ux_context.screen_current_element_arrays_count = 1;

    // ensure the string_buffer will be set before each button is displayed
    G_bolos_ux_context.screen_before_element_display_callback =
        screen_settings_before_element_display_callback;
    G_bolos_ux_context.button_push_callback = screen_settings_button;

    if (initial) {
        G_bolos_ux_context.onboarding_step = os_perso_isonboarded() ? 0 : 3;
        bolos_ux_hslider3_init(ARRAYLEN(screen_settings_desc) -
                               G_bolos_ux_context.onboarding_step);
    }

    screen_display_init();
}

#endif // OS_IO_SEPROXYHAL
