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

#include "glyphs.h"

#ifdef OS_IO_SEPROXYHAL

#define BRIGHTNESS_DEFAULT 3
#define ROTATION_DEFAULT 0
#define INVERSION_DEFAULT 0
#define SHUFFLE_PIN_DEFAULT 0

#define AUTO_LOCK_DEFAULT 600000

typedef struct screen_desc_e {
    const bagl_element_t *elements;
    unsigned int elements_count;
    bagl_element_callback_t before_display_callback;
    button_push_callback_t button_push_callback;
    appmain_t setting_init;
} screen_desc_t;

extern const screen_desc_t screen_settings_desc[];

const ux_menu_entry_t menu_settings[];
const ux_menu_entry_t menu_settings_not_onboarded[];
const ux_menu_entry_t menu_settings_display[];
const ux_menu_entry_t menu_settings_security[];
const ux_menu_entry_t menu_settings_passphrase[];
const ux_menu_entry_t menu_settings_device[];
const ux_menu_entry_t menu_settings_device_not_onboard[];
const ux_menu_entry_t menu_settings_invert[];
const ux_menu_entry_t menu_settings_rotate[];
const ux_menu_entry_t menu_settings_shufflepin[];
const ux_menu_entry_t menu_settings_autolock[];

void screen_settings_apply_internal(unsigned int use_persisted,
                                    unsigned int brightness,
                                    unsigned int rotation,
                                    unsigned int invert) {
    // apply default settings
    if (!os_perso_isonboarded()) {
        brightness = BRIGHTNESS_DEFAULT;
        rotation = ROTATION_DEFAULT;
        invert = INVERSION_DEFAULT;
        // shufflePin = SHUFFLE_PIN_DEFAULT;
        G_bolos_ux_context.setting_auto_lock_delay_ms = AUTO_LOCK_DEFAULT;
    }

    if (os_perso_isonboarded() && use_persisted) {
        brightness = os_setting_get(OS_SETTING_BRIGHTNESS);
        rotation = os_setting_get(OS_SETTING_ROTATION);
        invert = os_setting_get(OS_SETTING_INVERT);
        // shufflePin = os_setting_get(OS_SETTING_SHUFFLE_PIN);

        // load
        G_bolos_ux_context.setting_auto_lock_delay_ms = AUTO_LOCK_DEFAULT;
        if (os_setting_get(OS_SETTING_AUTO_LOCK_DELAY)) {
            G_bolos_ux_context.setting_auto_lock_delay_ms =
                os_setting_get(OS_SETTING_AUTO_LOCK_DELAY);
        }
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
        G_io_seproxyhal_spi_buffer[4] = 20;
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

void screen_settings_display_current(void) {
    // only setup when not displaying help (or quit)
    if (screen_settings_desc[G_bolos_ux_context.settings_index]
            .elements_count != 0) {
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array = (const bagl_element_t *)PIC(
            screen_settings_desc[G_bolos_ux_context.settings_index].elements);
        G_bolos_ux_context.screen_stack[0]
            .element_arrays[0]
            .element_array_count =
            screen_settings_desc[G_bolos_ux_context.settings_index]
                .elements_count;
        G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;
        G_bolos_ux_context.screen_stack[0]
            .screen_before_element_display_callback =
            (bagl_element_callback_t)PIC(
                screen_settings_desc[G_bolos_ux_context.settings_index]
                    .before_display_callback);
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            (button_push_callback_t)PIC(
                screen_settings_desc[G_bolos_ux_context.settings_index]
                    .button_push_callback);
        screen_display_init(0);
    }
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

const bagl_element_t *
screen_settings_brightness_before_display(const bagl_element_t *element) {
    if (element->component.userid) {
        if (G_bolos_ux_context.settings_value < element->component.userid) {
            return NULL;
        }
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return element;
}

unsigned int
screen_settings_brightness_button(unsigned int button_mask,
                                  unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_FAST | BUTTON_LEFT:
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        if (G_bolos_ux_context.settings_value > 1) {
            G_bolos_ux_context.settings_value--;
        }
        goto redraw;

    case BUTTON_EVT_FAST | BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        if (G_bolos_ux_context.settings_value < 6) {
            G_bolos_ux_context.settings_value++;
        }

    redraw:
        screen_settings_apply_internal(0, G_bolos_ux_context.settings_value,
                                       os_setting_get(OS_SETTING_ROTATION),
                                       os_setting_get(OS_SETTING_INVERT));
        screen_settings_display_current();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        os_setting_set(OS_SETTING_BRIGHTNESS,
                       G_bolos_ux_context.settings_value);
        screen_settings_apply_internal(1, 0, 0, 0);
        UX_MENU_DISPLAY(UX_MENU_UNCHANGED_ENTRY, menu_settings_display, NULL);
        break;
    }
    return 0;
}

void screen_settings_brightness_init(void) {
    G_bolos_ux_context.settings_value = os_setting_get(OS_SETTING_BRIGHTNESS);
    if (G_bolos_ux_context.settings_value == 0) {
        G_bolos_ux_context.settings_value = BRIGHTNESS_DEFAULT;
    }
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
      BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
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

const bagl_element_t *
screen_settings_version_before_display(const bagl_element_t *element) {
    switch (element->component.userid) {
    // previous setting
    case 0x01:
        switch (G_bolos_ux_context.settings_value) {
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
        switch (G_bolos_ux_context.settings_value) {
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
        return (G_bolos_ux_context.settings_value != 0) ? element : NULL;

    case 0x11:
        return (G_bolos_ux_context.settings_value != 1) ? element : NULL;
    }

    // display other elements only if screen setup, else, only redraw words
    // value
    return element;
}

unsigned int screen_settings_version_button(unsigned int button_mask,
                                            unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.settings_value = 0;
        goto redraw;

    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        G_bolos_ux_context.settings_value = 1;

    redraw:
        screen_settings_display_current();
        break;

    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        // go back to settings list
        UX_MENU_DISPLAY(UX_MENU_UNCHANGED_ENTRY, menu_settings_device, NULL);
        break;
    }
    return 0;
}

void screen_settings_version_init(void) {
    G_bolos_ux_context.settings_value = 0;
}

// ==========================================================================

void menu_settings_rotate_init(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(os_setting_get(OS_SETTING_ROTATION), menu_settings_rotate,
                    NULL);
}

// ==========================================================================

void menu_settings_invert_init(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(os_setting_get(OS_SETTING_INVERT), menu_settings_invert,
                    NULL);
}

// ==========================================================================

void menu_settings_autolock_init(unsigned int index) {
    index = 0;
    switch (os_setting_get(OS_SETTING_AUTO_LOCK_DELAY)) {
    case -1UL:
        index = 0;
        break;
    case 60000:
        index = 1;
        break;
    case 180000:
        index = 2;
        break;
    case 300000:
        index = 3;
        break;
    case 0: // the undefined setting value
    case 6000000:
        index = 4;
        break;
    }
    UX_MENU_DISPLAY(index, menu_settings_autolock, NULL);
}

// ==========================================================================

void menu_settings_shufflepin_init(unsigned int ignored) {
    UNUSED(ignored);
    UX_MENU_DISPLAY(os_setting_get(OS_SETTING_SHUFFLE_PIN),
                    menu_settings_shufflepin, NULL);
}

// ==========================================================================

void screen_settings_help_ended(void) {
    UX_MENU_DISPLAY(os_perso_isonboarded() ? 3 : 1,
                    os_perso_isonboarded() ? menu_settings
                                           : menu_settings_not_onboarded,
                    NULL);
}

// ==========================================================================

void screen_settings_autolock_change(unsigned int duration) {
    os_setting_set(OS_SETTING_AUTO_LOCK_DELAY, duration);
    screen_settings_apply_internal(1, 0, 0, 0);
    UX_MENU_DISPLAY(0, menu_settings_security, NULL);
}

// ==========================================================================

void screen_settings_rotate_change(unsigned int enabled) {
    os_setting_set(OS_SETTING_ROTATION, enabled);
    screen_settings_apply_internal(1, 0, 0, 0);
    UX_MENU_DISPLAY(1, menu_settings_display, NULL);
}

// ==========================================================================

void screen_settings_invert_change(unsigned int enabled) {
    os_setting_set(OS_SETTING_INVERT, enabled);
    screen_settings_apply_internal(1, 0, 0, 0);
    UX_MENU_DISPLAY(2, menu_settings_display, NULL);
}

// ==========================================================================

void screen_settings_shufflepin_change(unsigned int enabled) {
    os_setting_set(OS_SETTING_SHUFFLE_PIN, enabled);
    screen_settings_apply_internal(1, 0, 0, 0);
    UX_MENU_DISPLAY(2, menu_settings_security, NULL);
}

// ==========================================================================

const screen_desc_t screen_settings_desc[] = {
    {screen_settings_brightness_elements,
     ARRAYLEN(screen_settings_brightness_elements),
     screen_settings_brightness_before_display,
     screen_settings_brightness_button, screen_settings_brightness_init},

    {screen_settings_version_elements,
     ARRAYLEN(screen_settings_version_elements),
     screen_settings_version_before_display, screen_settings_version_button,
     screen_settings_version_init},
};

// ==========================================================================

void menu_callback(unsigned int userid) {
    G_bolos_ux_context.settings_index = userid;
    switch (userid) {
    case -6: // change pin
        screen_settings_change_pin_1_2_pin_init(1);
        return;

    case -5: // erase all
        screen_settings_erase_all_init();
        return;

    case -4: // set persistent passphrase
        screen_settings_passphrase_attach_1_init();
        return;

    case -3: // set temporary passphrase
        screen_settings_passphrase_temporary_1_init();
        return;

    case -2: // help
        screen_help_init(screen_settings_help_ended);
        return;

    case -1:
        // request animation when settings exits
        G_bolos_ux_context.dashboard_redisplayed = 1;
        // display dashboard
        screen_dashboard_init();
        return;
    }
    // display current settings
    ((appmain_t)PIC(screen_settings_desc[G_bolos_ux_context.settings_index]
                        .setting_init))();
    screen_settings_display_current();
}

/*
*Display
*  Brightness
*    7 levels
*  Invert colors
*    Enabled/Disabled
*  Rotate screen
*    0 / 180
*  Back
*Security
*  Auto lock
*    OFF
*    1 minute
*    3 minutes
*    5 minutes
*    10 minutes
*  Shuffle PIN
*    Enabled/Disabled
*  Change PIN
*    Flow
*  Passphrase
*    Set temporary
*      Flow
*    Attach to new PIN
*      Flow
*    Back
*  Back
Device
  Firmware
    SE/MCU
  Factory reset
    Flow
  Back
Help
Quit app
*/

const ux_menu_entry_t menu_settings_rotate[] = {
    {NULL, screen_settings_rotate_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, screen_settings_rotate_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_invert[] = {
    {NULL, screen_settings_invert_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, screen_settings_invert_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_shufflepin[] = {
    {NULL, screen_settings_shufflepin_change, 0, NULL, "No", NULL, 0, 0},
    {NULL, screen_settings_shufflepin_change, 1, NULL, "Yes", NULL, 0, 0},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_display[] = {
    {NULL, menu_callback, 0, NULL, "Brightness", NULL, 0, 0},
    {NULL, menu_settings_rotate_init, 0, NULL, "Rotate screen", NULL, 0, 0},
    {NULL, menu_settings_invert_init, 0, NULL, "Invert colors", NULL, 0, 0},
    {menu_settings, NULL, 0, &C_badge_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_passphrase[] = {
    {NULL, menu_callback, -3, NULL, "Set temporary", NULL, 0, 0},
    {NULL, menu_callback, -4, NULL, "Attach to a PIN", NULL, 0, 0},
    {menu_settings_security, NULL, 3, &C_badge_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_autolock[] = {
    {NULL, screen_settings_autolock_change, -1UL, NULL, "OFF", NULL, 0, 0},
    {NULL, screen_settings_autolock_change, 60000, NULL, "1 minute", NULL, 0,
     0},
    {NULL, screen_settings_autolock_change, 180000, NULL, "3 minutes", NULL, 0,
     0},
    {NULL, screen_settings_autolock_change, 300000, NULL, "5 minutes", NULL, 0,
     0},
    {NULL, screen_settings_autolock_change, 6000000, NULL, "10 minutes", NULL,
     0, 0},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_security[] = {
    {NULL, menu_settings_autolock_init, 0, NULL, "Auto-lock", NULL, 0, 0},
    {NULL, menu_callback, -6, NULL, "Change PIN", NULL, 0, 0},
    {NULL, menu_settings_shufflepin_init, 0, NULL, "Shuffle PIN", NULL, 0, 0},
    {menu_settings_passphrase, NULL, 0, NULL, "Passphrase", NULL, 0, 0},
    {menu_settings, NULL, 1, &C_badge_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_device[] = {
    {NULL, menu_callback, 1, NULL, "Firmware", NULL, 0, 0},
    {NULL, menu_callback, -5, NULL, "Reset all", NULL, 0, 0},
    {menu_settings, NULL, 2, &C_badge_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_device_not_onboard[] = {
    {NULL, menu_callback, 1, NULL, "Firmware", NULL, 0, 0},
    {NULL, menu_callback, -5, NULL, "Reset all", NULL, 0, 0},
    {menu_settings_not_onboarded, NULL, 0, &C_badge_back, "Back", NULL, 61, 40},
    UX_MENU_END};

const ux_menu_entry_t menu_settings[] = {
    {menu_settings_display, NULL, 0, NULL, "Display", NULL, 0, 0},
    {menu_settings_security, NULL, 0, NULL, "Security", NULL, 0, 0},
    {menu_settings_device, NULL, 0, NULL, "Device", NULL, 0, 0},
    {NULL, menu_callback, -2, NULL, "Assistance", NULL, 0, 0},
    {NULL, menu_callback, -1, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END};

const ux_menu_entry_t menu_settings_not_onboarded[] = {
    {menu_settings_device_not_onboard, NULL, 0, NULL, "Device", NULL, 0, 0},
    {NULL, menu_callback, -2, NULL, "Assistance", NULL, 0, 0},
    {NULL, menu_callback, -1, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END};

void screen_settings_attach_to_pin(void) {
    UX_MENU_DISPLAY(1, menu_settings_passphrase, NULL);
}

void screen_settings_set_temporary(void) {
    UX_MENU_DISPLAY(0, menu_settings_passphrase, NULL);
}

void screen_settings_change_pin(void) {
    UX_MENU_DISPLAY(1, menu_settings_security, NULL);
}

void screen_settings_erase_all(void) {
    UX_MENU_DISPLAY(1,
                    os_perso_isonboarded() ? menu_settings_device
                                           : menu_settings_device_not_onboard,
                    NULL);
}

void screen_settings_init(unsigned int initial) {
    UX_MENU_DISPLAY(initial ? 0 : UX_MENU_UNCHANGED_ENTRY,
                    os_perso_isonboarded() ? menu_settings
                                           : menu_settings_not_onboarded,
                    NULL);
}

#endif // OS_IO_SEPROXYHAL
