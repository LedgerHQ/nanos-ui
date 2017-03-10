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

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))

bolos_ux_context_t G_bolos_ux_context;

unsigned short io_timeout(unsigned short last_timeout) {
    UNUSED(last_timeout);
    // infinite timeout
    return 1;
}

void screen_hex_identifier_string_buffer(const unsigned char *buffer,
                                         unsigned int total) {
    SPRINTF(G_bolos_ux_context.string_buffer, "%.*H...%.*H",
            BOLOS_UX_HASH_LENGTH / 2, buffer, BOLOS_UX_HASH_LENGTH / 2,
            buffer + total - BOLOS_UX_HASH_LENGTH / 2);
}

/*
unsigned char rng_u8_modulo(unsigned char modulo) {
  unsigned int rng_max = 256 % modulo;
  unsigned int rng_limit = 256 - rng_max;
  unsigned char candidate;
  while ((candidate = cx_rng_u8()) > rng_limit);
  return (candidate % modulo);
}
*/

// common code for all screens
void screen_state_init(unsigned int stack_slot) {
    // reinit ux behavior (previous touched element, button push state)
    io_seproxyhal_init_ux(); // glitch upon screen_display_init for a button
                             // being pressed in a previous screen

    // wipe the slot to be displayed just in case
    os_memset(&G_bolos_ux_context.screen_stack[stack_slot], 0,
              sizeof(G_bolos_ux_context.screen_stack[0]));

    // init current screen state
    G_bolos_ux_context.screen_stack[stack_slot]
        .exit_code_after_elements_displayed = BOLOS_UX_CONTINUE;
}

// check to process keyboard callback before screen generic callback
const bagl_element_t *
screen_display_element_callback(const bagl_element_t *element) {
    const bagl_element_t *el;
    if (G_bolos_ux_context.screen_stack_count) {
        if (G_bolos_ux_context
                .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                .screen_before_element_display_callback) {
            el = G_bolos_ux_context
                     .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                     .screen_before_element_display_callback(element);
            if (!el) {
                return 0;
            }
            if ((unsigned int)el != 1) {
                element = el;
            }
        }
    }
    // consider good to be displayed by default
    return element;
}

// common code for all screens
void screen_display_init(unsigned int stack_slot) {
    // don't display any elements of a previous screen replacement
    if (G_bolos_ux_context.screen_stack_count > 0 &&
        stack_slot == G_bolos_ux_context.screen_stack_count - 1) {
        io_seproxyhal_init_ux();

        if (!io_seproxyhal_spi_is_status_sent() &&
            G_bolos_ux_context.screen_stack[stack_slot]
                .element_arrays[0]
                .element_array_count) {
            G_bolos_ux_context.screen_stack[stack_slot].element_index =
                1; // prepare displaying next element
            screen_display_element(&G_bolos_ux_context.screen_stack[stack_slot]
                                        .element_arrays[0]
                                        .element_array[0]);
        }
    }
    // asking to redraw below top screen (likely the app below the ux)
    else if (stack_slot == -1UL || G_bolos_ux_context.screen_stack_count == 0) {
        if (G_bolos_ux_context.exit_code == BOLOS_UX_OK) {
            G_bolos_ux_context.exit_code = BOLOS_UX_REDRAW;
        }
    }
}

// return true (stack slot +1) if an element
unsigned int
screen_stack_is_element_array_present(const bagl_element_t *element_array) {
    unsigned int i, j;
    for (i = 0;
         i < /*ARRAYLEN(G_bolos_ux_context.screen_stack)*/ G_bolos_ux_context
                 .screen_stack_count;
         i++) {
        for (j = 0; j < G_bolos_ux_context.screen_stack[i].element_arrays_count;
             j++) {
            if (G_bolos_ux_context.screen_stack[i]
                    .element_arrays[j]
                    .element_array == element_array) {
                return i + 1;
            }
        }
    }
    return 0;
}
unsigned int screen_stack_push(void) {
    // only push if an available slot exists
    if (G_bolos_ux_context.screen_stack_count <
        ARRAYLEN(G_bolos_ux_context.screen_stack)) {
        os_memset(&G_bolos_ux_context
                       .screen_stack[G_bolos_ux_context.screen_stack_count],
                  0, sizeof(G_bolos_ux_context.screen_stack[0]));
        G_bolos_ux_context.screen_stack_count++;
    }
    // return the stack top index
    return G_bolos_ux_context.screen_stack_count - 1;
}
unsigned int screen_stack_pop(void) {
    unsigned int exit_code = BOLOS_UX_OK;
    // only pop if more than two stack entry (0 and 1,top is an index not a
    // count)
    if (G_bolos_ux_context.screen_stack_count > 0) {
        G_bolos_ux_context.screen_stack_count--;
        exit_code = G_bolos_ux_context
                        .screen_stack[G_bolos_ux_context.screen_stack_count]
                        .exit_code_after_elements_displayed;
        // wipe popped slot
        os_memset(&G_bolos_ux_context
                       .screen_stack[G_bolos_ux_context.screen_stack_count],
                  0, sizeof(G_bolos_ux_context.screen_stack[0]));
    }

    // prepare output code when popping the last stack screen
    if (G_bolos_ux_context.screen_stack_count == 0) {
        G_bolos_ux_context.exit_code = exit_code;
    }

    // ask for a complete redraw (optimisation due to blink must be avoided as
    // we're returning from a modal, and within the bolos ux screen stack)
    G_bolos_ux_context.screen_redraw = 1;
    // return the stack top index
    return G_bolos_ux_context.screen_stack_count - 1;
}

void screen_stack_remove(unsigned int stack_slot) {
    if (stack_slot > ARRAYLEN(G_bolos_ux_context.screen_stack) - 1) {
        stack_slot = ARRAYLEN(G_bolos_ux_context.screen_stack) - 1;
    }

    // removing something not in stack
    if (stack_slot >= G_bolos_ux_context.screen_stack_count) {
        return;
    }

    // before: | screenz | removed screen | other screenz |
    // after:  | screenz | other screenz |

    if (stack_slot != ARRAYLEN(G_bolos_ux_context.screen_stack) - 1) {
        os_memmove(
            &G_bolos_ux_context.screen_stack[stack_slot],
            &G_bolos_ux_context.screen_stack[stack_slot + 1],
            (ARRAYLEN(G_bolos_ux_context.screen_stack) - (stack_slot + 1)) *
                sizeof(G_bolos_ux_context.screen_stack[0]));
    }

    // wipe last slot
    screen_stack_pop();
}

void screen_display_element(const bagl_element_t *element) {
    const bagl_element_t *el = screen_display_element_callback(element);
    if (!el) {
        return;
    }
    if ((unsigned int)el != 1) {
        element = el;
    }
    // display current element
    io_seproxyhal_display(element);
}

void screen_wake_up(void) {
    // only reactivate backlight when dimmed, to avoid blink ...b
    if (G_bolos_ux_context.inactivity_state != INACTIVITY_NONE) {
        // not inactive anymore, interpret touch/button
        G_bolos_ux_context.inactivity_state = INACTIVITY_NONE;
        // wake backlight, don't touch the current state
        // io_seproxyhal_backlight(0, BACKLIGHT_FULL_LEVEL);
        screen_saver_deinit();
    }

    // user activity detected
    G_bolos_ux_context.ms_last_activity = G_bolos_ux_context.ms;
}

void screen_return_after_displayed_touched_element(unsigned int exit_code) {
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .element_index = 0;
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .displayed = 0;
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .element_arrays_count = 0;
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .exit_code_after_elements_displayed = exit_code;
}

const unsigned char const C_app_empty_colors[] = {
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
};

const unsigned char const C_app_empty_bitmap[] = {
    // color index table
    0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,

    // icon mask
    0x00, 0x00, 0xF0, 0x0F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F,
    0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F,
    0xFC, 0x3F, 0xFC, 0x3F, 0xF0, 0x0F, 0x00, 0x00,
};

// prepare the app icon as if it was a icon_detail_t encoded structure in the
// string_buffer
void screen_prepare_masked_icon(unsigned char *icon_bitmap,
                                unsigned int icon_bitmap_length) {
    unsigned int i, inversemode;
    bagl_icon_details_t *icon_details =
        (bagl_icon_details_t *)G_bolos_ux_context.string_buffer;
    unsigned char *bitmap = (unsigned char *)G_bolos_ux_context.string_buffer +
                            sizeof(bagl_icon_details_t);

    icon_details->width = 16;
    icon_details->height = 16;
    // prepare the icon_details content
    icon_details->bpp = C_app_empty_bitmap[0];
    // use color table from the const
    icon_details->colors = (unsigned int *)C_app_empty_colors;
    icon_details->bitmap = bitmap;

    // when first color of the bitmap is not 0, then, must inverse the icon's
    // bit to
    // match the C_app_empty_bitmap bit value
    inversemode = 0;
    if (icon_bitmap[1] != 0 || icon_bitmap[2] != 0 || icon_bitmap[3] != 0 ||
        icon_bitmap[4] != 0) {
        inversemode = 1;
    }

    for (i = 1 + 8; i < sizeof(C_app_empty_bitmap) && i < icon_bitmap_length;
         i++) {
        if (inversemode) {
            bitmap[i - 1 - 8] = C_app_empty_bitmap[i] & (~icon_bitmap[i]);
        } else {
            bitmap[i - 1 - 8] = C_app_empty_bitmap[i] & icon_bitmap[i];
        }
    }

    // the string buffer is now ready to be displayed as an icon details
    // structure
}

void io_seproxyhal_display(const bagl_element_t *element) {
    io_seproxyhal_display_default((bagl_element_t *)element);
}

void bolos_ux_main(void) {
    G_bolos_ux_context.exit_code = BOLOS_UX_CONTINUE;

    switch (G_bolos_ux_context.state) {
    default:
        // init seproxyhal ux related globals
        io_seproxyhal_init_ux();
        // no button push so far
        io_seproxyhal_init_button();

        // init the ram context
        os_memset(&G_bolos_ux_context, 0, sizeof(G_bolos_ux_context));
        // setup the ram canary
        G_bolos_ux_context.canary = CANARY_MAGIC;
        // register the ux parameters pointer for the os side
        os_ux_register(&G_bolos_ux_context.parameters);
        G_bolos_ux_context.state = STATE_INITIALIZED;
        G_bolos_ux_context.dashboard_last_selected =
            -1UL; // initialize the current selected application to none., done
                  // only at boot

        // request animation when dashboard has finished displaying all the
        // elements (after onboarding OR the first time displayed)
        G_bolos_ux_context.dashboard_redisplayed = 1;

        // return, this should be the first and only call from the bolos task at
        // platform startup
        G_bolos_ux_context.exit_code = BOLOS_UX_OK;
        break;

    case STATE_INITIALIZED:
        // push the default screen to display the ux into it
        if (G_bolos_ux_context.screen_stack_count == 0
            // no need for a new stacked screen in the following cases (no
            // screen frame needed on top of apps for these calls)

            // BEGIN BOLOS MANAGER FLOW (use slot 0 implicitely)
            &&
            (G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_BOOT_NOT_PERSONALIZED ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_BOOT_ONBOARDING ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_BOOT_RECOVERY ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_DASHBOARD ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_LOADER ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_CONSENT_UPGRADE ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_CONSENT_APP_ADD
             //|| G_bolos_ux_context.parameters.ux_id ==
             //BOLOS_UX_CONSENT_APP_UPG
             ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_CONSENT_APP_DEL ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_ISSUER_KEY ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_CUSTOMCA_KEY ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_CONSENT_FOREIGN_KEY
             //|| G_bolos_ux_context.parameters.ux_id ==
             //BOLOS_UX_CHANGE_ALTERNATE_PIN
             ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_GET_DEVICE_NAME ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_SET_DEVICE_NAME ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_SETUP_CUSTOMCA_KEY ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_CONSENT_RESET_CUSTOMCA_KEY ||
             G_bolos_ux_context.parameters.ux_id ==
                 BOLOS_UX_BOOT_UX_NOT_SIGNED ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_PROCESSING ||
             G_bolos_ux_context.parameters.ux_id == BOLOS_UX_BOOT_UNSAFE_WIPE
             // END BOLOS MANAGER FLOW
             )) {
            screen_stack_push();
        }

        switch (G_bolos_ux_context.parameters.ux_id) {
        case BOLOS_UX_BOOT:
            // init seproxyhal ux related globals
            io_seproxyhal_init_ux();
            // no button push so far
            io_seproxyhal_init_button();

            // init the ram context
            os_memset(&G_bolos_ux_context, 0, sizeof(G_bolos_ux_context));
            // setup the ram canary
            G_bolos_ux_context.canary = CANARY_MAGIC;
            // register the ux parameters pointer for the os side
            os_ux_register(&G_bolos_ux_context.parameters);
            G_bolos_ux_context.state = STATE_INITIALIZED;
            G_bolos_ux_context.dashboard_last_selected =
                -1UL; // initialize the current selected application to none.,
                      // done only at boot

            // request animation when dashboard has finished displaying all the
            // elements (after onboarding OR the first time displayed)
            G_bolos_ux_context.dashboard_redisplayed = 1;

            // return, this should be the first and only call from the bolos
            // task at platform startup
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;

        case BOLOS_UX_BOLOS_START:

            screen_wake_up();
            // apply settings in the L4 (ble, brightness, etc)
            screen_settings_apply();

            // ensure ticker is present
            io_seproxyhal_setup_ticker(100);

            // request animation when dashboard has finished displaying all the
            // elements (after onboarding OR the first time displayed)
            G_bolos_ux_context.dashboard_redisplayed = 1;

        default:
            // nothing to do yet
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;
            break;

        case BOLOS_UX_BOOT_NOT_PERSONALIZED:
            screen_not_personalized_init();
            break;

#ifndef BOLOS_OS_UPGRADER
        case BOLOS_UX_BOOT_ONBOARDING:
            screen_wake_up();
            // re apply settings in the L4 (ble, brightness, etc) after exiting
            // application in case of wipe
            screen_settings_apply();

            // request animation when dashboard has finished displaying all the
            // elements (after onboarding OR the first time displayed)
            G_bolos_ux_context.dashboard_redisplayed = 1;

            // avoid reperso is already onboarded to avoid leaking data through
            // parameters due to user land call
            if (os_perso_isonboarded()) {
                G_bolos_ux_context.exit_code = BOLOS_UX_OK;
                break;
            }

            screen_onboarding_0_welcome_init();
            break;

        case BOLOS_UX_BOOT_RECOVERY:
        /*
        screen_boot_recovery_init();
        break;
        */
        case BOLOS_UX_DASHBOARD:
            screen_wake_up();

            // apply settings when redisplaying dashboard
            screen_settings_apply();

            // when returning from application, the ticker could have been
            // disabled
            io_seproxyhal_setup_ticker(100);
#ifdef BOLOS_AUTOSTART_FIRST
            // if not in recovery
            if (!(os_flags() & OS_FLAG_RECOVERY) &&
                !G_bolos_ux_context.app_auto_started) {
                // run the first non ux application
                unsigned int i = 0;
                while (i < os_registry_count()) {
                    application_t app;
                    os_registry_get(i, &app);
                    if (!(app.flags & APPLICATION_FLAG_BOLOS_UX)) {
                        G_bolos_ux_context.app_auto_started = 1;

                        screen_stack_pop();
                        io_seproxyhal_disable_io();

                        os_sched_exec(i); // no return
                    }
                }
            }
#endif // BOLOS_AUTOSTART_FIRST

            screen_dashboard_init();
            break;

        case BOLOS_UX_VALIDATE_PIN:
            screen_wake_up();
            screen_modal_validate_pin_init();
            break;

        case BOLOS_UX_CONSENT_APP_ADD:
            screen_wake_up();
            screen_consent_add_init();
            break;

        case BOLOS_UX_CONSENT_APP_DEL:
            screen_wake_up();
            screen_consent_del_init();
            break;

        case BOLOS_UX_CONSENT_ISSUER_KEY:
            screen_wake_up();
            screen_consent_issuer_key_init();
            break;

        case BOLOS_UX_CONSENT_CUSTOMCA_KEY:
            screen_wake_up();
            screen_consent_customca_key_init();
            break;

        case BOLOS_UX_CONSENT_FOREIGN_KEY:
            screen_wake_up();
            screen_consent_foreign_key_init();
            break;

        case BOLOS_UX_CONSENT_GET_DEVICE_NAME:
            screen_wake_up();
            screen_consent_get_device_name_init();
            break;

        case BOLOS_UX_CONSENT_SET_DEVICE_NAME:
            screen_wake_up();
            screen_consent_set_device_name_init();
            break;

        case BOLOS_UX_BOOT_UX_NOT_SIGNED:
            screen_wake_up();
            screen_consent_ux_not_signed_init();
            break;

        case BOLOS_UX_BOOT_UNSAFE_WIPE:
            screen_wake_up();
            screen_boot_unsafe_wipe_init();
            break;

        case BOLOS_UX_CONSENT_SETUP_CUSTOMCA_KEY:
            screen_wake_up();
            screen_consent_setup_customca_init();
            break;

        case BOLOS_UX_CONSENT_RESET_CUSTOMCA_KEY:
            screen_wake_up();
            screen_consent_reset_customca_init();
            break;

#else  // ! BOLOS_OS_UPGRADER
        // upgrader dashboard does not exists
        case BOLOS_UX_DASHBOARD:
            screen_wake_up();
            screen_os_upgrader();
            break;
#endif // ! BOLOS_OS_UPGRADER

        // only consent upgrade is common to os upgrader and normal os to avoid
        // being stuck if hash doesn't match
        case BOLOS_UX_CONSENT_UPGRADE:
            screen_wake_up();
            // reset global pin state in case was onboarded, must validate the
            // pin to proceed to the upgrade
            os_global_pin_invalidate();

            screen_consent_upgrade_init();
            break;

        // display a wait screen during application loading
        // if host computer bugs, then the token also remains in a loading state
        // (on screen only)
        case BOLOS_UX_PROCESSING:
            screen_wake_up();
            screen_processing_init();
            break;

        case BOLOS_UX_WAKE_UP:
            screen_wake_up();
            // if a screen is drawn (like the PIN) onto the current screen, then
            // avoid allowing the app to erase or whatever the current screen
            goto continue_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT;

        // continue processing of the current screen
        case BOLOS_UX_EVENT: {
            // retrieve the last message received by the application, cached by
            // the OS (to avoid complex and sluggish parameter copy interface in
            // syscall)
            io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer,
                                   sizeof(G_io_seproxyhal_spi_buffer),
                                   IO_CACHE);
            // process event
            // nothing done with the event, throw an error on the transport
            // layer if needed

            // just reply "amen"
            // add a "pairing ok" tag if necessary
            // can't have more than one tag in the reply, not supported yet.
            switch (G_io_seproxyhal_spi_buffer[0]) {
            case SEPROXYHAL_TAG_TICKER_EVENT: {
                unsigned int last_ms = G_bolos_ux_context.ms;
                unsigned int interval_ms = 0;
                if (G_io_seproxyhal_spi_buffer[2] == 4) {
                    G_bolos_ux_context.ms = U4BE(G_io_seproxyhal_spi_buffer, 3);
                } else {
                    G_bolos_ux_context.ms += 100; // ~ approx, just to avoid
                                                  // being stuck on blue dev
                                                  // edition
                }

                // compute time interval, handle overflow
                interval_ms = G_bolos_ux_context.ms - last_ms;
                if (G_bolos_ux_context.ms < last_ms) {
                    interval_ms = (-1UL) - interval_ms;
                }

                // request time extension of the MCU watchdog
                G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_MORE_TIME;
                G_io_seproxyhal_spi_buffer[1] = 0;
                G_io_seproxyhal_spi_buffer[2] = 0;
                io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);

                if (G_bolos_ux_context.screen_stack_count > 0 &&
                    G_bolos_ux_context
                        .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                        .ticker_callback &&
                    G_bolos_ux_context
                        .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                        .ticker_interval) {
                    G_bolos_ux_context
                        .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
                        .ticker_value -=
                        MIN(G_bolos_ux_context
                                .screen_stack
                                    [G_bolos_ux_context.screen_stack_count - 1]
                                .ticker_value,
                            interval_ms);
                    if (G_bolos_ux_context
                            .screen_stack
                                [G_bolos_ux_context.screen_stack_count - 1]
                            .ticker_value == 0) {
                        // rearm, and call the registered function
                        G_bolos_ux_context
                            .screen_stack
                                [G_bolos_ux_context.screen_stack_count - 1]
                            .ticker_value =
                            G_bolos_ux_context
                                .screen_stack
                                    [G_bolos_ux_context.screen_stack_count - 1]
                                .ticker_interval;
                        G_bolos_ux_context
                            .screen_stack
                                [G_bolos_ux_context.screen_stack_count - 1]
                            .ticker_callback(/*ignored*/ 0);
                    }
                }

                if (G_bolos_ux_context.ms_last_activity == 0) {
                    // initializing with no user action (at boot time, the user
                    // just ... wait)
                    G_bolos_ux_context.ms_last_activity = G_bolos_ux_context.ms;
                }
#ifndef BOLOS_NO_CONSENT
                else if (IS_SETTING_PRE_POWER_OFF() &&
                         G_bolos_ux_context.inactivity_state <
                             INACTIVITY_LOCK &&
                         G_bolos_ux_context.ms >
                             G_bolos_ux_context.ms_last_activity +
                                 INACTIVITY_MS_AUTO_LOCK) {
                    G_bolos_ux_context.inactivity_state = INACTIVITY_LOCK;
                    // prepare the lock screen
                    // don't lock screen on onboarding (at boot or by an app)
                    if (os_perso_isonboarded()) {
                        // stack pin lock, not cancellable, modal if not the
                        // only screen
                        screen_modal_validate_pin_init();

                        // yay, some fun, ensure saver is stacked over the pin
                        screen_saver_init();
                    }
                }
#endif // BOLOS_NO_CONSENT

                // in case more display to be finished (asynch timer during
                // display sequence)
                goto continue_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT;
            }

            // power off if long push, else pass to the application callback if
            // any
            case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT: {
                // reenable the screen lock state later on (when releasing a
                // button, not upon press)
                if (G_bolos_ux_context.inactivity_state) {
                    G_bolos_ux_context.inactivity_state = INACTIVITY_NONE;
                }

                // user activity detected
                G_bolos_ux_context.ms_last_activity = G_bolos_ux_context.ms;

                if (G_bolos_ux_context.screen_stack_count) {
                    // will use the exit code from the currently displayed
                    // screen
                    io_seproxyhal_button_push(
                        G_bolos_ux_context
                            .screen_stack
                                [G_bolos_ux_context.screen_stack_count - 1]
                            .button_push_callback,
                        G_io_seproxyhal_spi_buffer[3] >> 1);
                } else {
                    G_bolos_ux_context.exit_code = BOLOS_UX_OK;
                }

                if (io_seproxyhal_spi_is_status_sent() ||
                    G_bolos_ux_context.exit_code != BOLOS_UX_CONTINUE) {
                    break;
                }
                // in case more display to be finished (asynch push during
                // display sequence)
                goto continue_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT;
            }

            continue_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
            case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT: {
                // display next screen element
                unsigned int elem_idx;
                unsigned int total_element_count;
                const bagl_element_t *element;
                unsigned int i;
            next_elem:
                i = 0;
                total_element_count = 0;
                if (G_bolos_ux_context.screen_stack_count) {
                    if (!G_bolos_ux_context
                             .screen_stack
                                 [G_bolos_ux_context.screen_stack_count - 1]
                             .displayed) {
                        elem_idx =
                            G_bolos_ux_context
                                .screen_stack
                                    [G_bolos_ux_context.screen_stack_count - 1]
                                .element_index;
                        while (i < G_bolos_ux_context
                                       .screen_stack[G_bolos_ux_context
                                                         .screen_stack_count -
                                                     1]
                                       .element_arrays_count) {
                            if (!io_seproxyhal_spi_is_status_sent()) {
                                // check if we're sending from this array or not
                                if (elem_idx <
                                    G_bolos_ux_context
                                        .screen_stack[G_bolos_ux_context
                                                          .screen_stack_count -
                                                      1]
                                        .element_arrays[i]
                                        .element_array_count) {
                                    const bagl_element_t *el;
                                    // pre inc before callback to allow callback
                                    // to change the next element to be drawn
                                    G_bolos_ux_context
                                        .screen_stack[G_bolos_ux_context
                                                          .screen_stack_count -
                                                      1]
                                        .element_index++;

                                    element =
                                        &G_bolos_ux_context
                                             .screen_stack
                                                 [G_bolos_ux_context
                                                      .screen_stack_count -
                                                  1]
                                             .element_arrays[i]
                                             .element_array[elem_idx];
                                    el = screen_display_element_callback(
                                        element);
                                    if (!el) {
                                        // skip display if requested to
                                        if (!io_seproxyhal_spi_is_status_sent() &&
                                            G_bolos_ux_context.exit_code ==
                                                BOLOS_UX_CONTINUE) {
                                            goto next_elem;
                                        }
                                        goto return_exit_code;
                                    }
                                    if ((unsigned int)el != 1) {
                                        element = el;
                                    }

                                    io_seproxyhal_display(element);
                                    goto return_exit_code;
                                }
                                //  prepare for next array comparison
                                elem_idx -=
                                    G_bolos_ux_context
                                        .screen_stack[G_bolos_ux_context
                                                          .screen_stack_count -
                                                      1]
                                        .element_arrays[i]
                                        .element_array_count;
                            }
                            total_element_count +=
                                G_bolos_ux_context
                                    .screen_stack[G_bolos_ux_context
                                                      .screen_stack_count -
                                                  1]
                                    .element_arrays[i]
                                    .element_array_count;
                            i++;
                        }

                        if (G_bolos_ux_context
                                .screen_stack
                                    [G_bolos_ux_context.screen_stack_count - 1]
                                .element_index >= total_element_count) {
                            // pop screen redisplay operation ended
                            G_bolos_ux_context.screen_redraw = 0;

                            // if screen has special stuff todo on exit
                            // G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count-1].displayed
                            // = 1; // to be tested first
                            if (G_bolos_ux_context
                                    .screen_stack[G_bolos_ux_context
                                                      .screen_stack_count -
                                                  1]
                                    .displayed_callback) {
                                // if screen displayed callback requested one
                                // more round, then set CONTINUE exit code
                                if (!G_bolos_ux_context
                                         .screen_stack[G_bolos_ux_context
                                                           .screen_stack_count -
                                                       1]
                                         .displayed_callback(0)) {
                                    G_bolos_ux_context
                                        .screen_stack[G_bolos_ux_context
                                                          .screen_stack_count -
                                                      1]
                                        .displayed = 0;
                                    G_bolos_ux_context.exit_code =
                                        BOLOS_UX_CONTINUE;
                                    break;
                                }
                            }
                            G_bolos_ux_context.exit_code =
                                G_bolos_ux_context
                                    .screen_stack[G_bolos_ux_context
                                                      .screen_stack_count -
                                                  1]
                                    .exit_code_after_elements_displayed;
                        }
                    }
                } else {
                    // nothing to be done here
                    G_bolos_ux_context.exit_code = BOLOS_UX_OK;
                }
                break;
            }
            }
            // process exit code
            break;
        }
        }
        break;
    }

return_exit_code:
    // remember the last displayed screen for blanking
    if (G_bolos_ux_context.parameters.ux_id != BOLOS_UX_EVENT) {
        G_bolos_ux_context.last_ux_id = G_bolos_ux_context.parameters.ux_id;
    }

    // kthx, but no
    if (G_bolos_ux_context.canary != CANARY_MAGIC) {
        reset();
    }

    // return to the caller
    os_sched_exit(G_bolos_ux_context.exit_code);
}

void bolos_ux_hslider3_init(unsigned int total_count) {
    G_bolos_ux_context.hslider3_total = total_count;
    switch (total_count) {
    case 0:
        G_bolos_ux_context.hslider3_before = BOLOS_UX_HSLIDER3_NONE;
        G_bolos_ux_context.hslider3_current = BOLOS_UX_HSLIDER3_NONE;
        G_bolos_ux_context.hslider3_after = BOLOS_UX_HSLIDER3_NONE;
        break;
    case 1:
        G_bolos_ux_context.hslider3_before = BOLOS_UX_HSLIDER3_NONE;
        G_bolos_ux_context.hslider3_current = 0;
        G_bolos_ux_context.hslider3_after = BOLOS_UX_HSLIDER3_NONE;
        break;
    case 2:
        G_bolos_ux_context.hslider3_before = BOLOS_UX_HSLIDER3_NONE;
        // G_bolos_ux_context.hslider3_before = 1; // full rotate
        G_bolos_ux_context.hslider3_current = 0;
        G_bolos_ux_context.hslider3_after = 1;
        break;
    default:
        G_bolos_ux_context.hslider3_before = total_count - 1;
        G_bolos_ux_context.hslider3_current = 0;
        G_bolos_ux_context.hslider3_after = 1;
        break;
    }
}

void bolos_ux_hslider3_set_current(unsigned int current) {
    // index is reachable ?
    if (G_bolos_ux_context.hslider3_total > current) {
        // reach it
        while (G_bolos_ux_context.hslider3_current != current) {
            bolos_ux_hslider3_next();
        }
    }
}

void bolos_ux_hslider3_next(void) {
    switch (G_bolos_ux_context.hslider3_total) {
    case 0:
    case 1:
        break;
    case 2:
        switch (G_bolos_ux_context.hslider3_current) {
        case 0:
            G_bolos_ux_context.hslider3_before = 0;
            G_bolos_ux_context.hslider3_current = 1;
            G_bolos_ux_context.hslider3_after = BOLOS_UX_HSLIDER3_NONE;
            break;
        case 1:
            G_bolos_ux_context.hslider3_before = BOLOS_UX_HSLIDER3_NONE;
            G_bolos_ux_context.hslider3_current = 0;
            G_bolos_ux_context.hslider3_after = 1;
            break;
        }
        break;
    default:
        G_bolos_ux_context.hslider3_before =
            G_bolos_ux_context.hslider3_current;
        G_bolos_ux_context.hslider3_current = G_bolos_ux_context.hslider3_after;
        G_bolos_ux_context.hslider3_after =
            (G_bolos_ux_context.hslider3_after + 1) %
            G_bolos_ux_context.hslider3_total;
        break;
    }
}

void bolos_ux_hslider3_previous(void) {
    switch (G_bolos_ux_context.hslider3_total) {
    case 0:
    case 1:
        break;
    case 2:
        switch (G_bolos_ux_context.hslider3_current) {
        case 0:
            G_bolos_ux_context.hslider3_before = 0;
            G_bolos_ux_context.hslider3_current = 1;
            G_bolos_ux_context.hslider3_after = BOLOS_UX_HSLIDER3_NONE;
            break;
        case 1:
            G_bolos_ux_context.hslider3_before = BOLOS_UX_HSLIDER3_NONE;
            G_bolos_ux_context.hslider3_current = 0;
            G_bolos_ux_context.hslider3_after = 1;
            break;
        }
        break;
    default:
        G_bolos_ux_context.hslider3_after = G_bolos_ux_context.hslider3_current;
        G_bolos_ux_context.hslider3_current =
            G_bolos_ux_context.hslider3_before;
        G_bolos_ux_context.hslider3_before =
            (G_bolos_ux_context.hslider3_before +
             G_bolos_ux_context.hslider3_total - 1) %
            G_bolos_ux_context.hslider3_total;
        break;
    }
}

unsigned int screen_consent_button(unsigned int button_mask,
                                   unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.exit_code = BOLOS_UX_CANCEL;
        break;
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        screen_dashboard_prepare();
        G_bolos_ux_context.exit_code = BOLOS_UX_OK;
        break;
    }
    return 0;
}

unsigned int
screen_consent_button_with_final_pin(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        G_bolos_ux_context.exit_code = BOLOS_UX_CANCEL;
        break;
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
        // ensure to ask for pin validation before continuing with upgrade to
        // ensure seed won't be at risk
        screen_modal_validate_pin_init();
        // ensure dashboard is scheduled to be redisplayed
        screen_dashboard_prepare();
        break;
    }
    return 0;
}

unsigned int screen_consent_ticker(unsigned int ignored) {
    UNUSED(ignored);
    screen_display_init(0);

    // prepare displaying next screen
    G_bolos_ux_context.onboarding_index =
        (G_bolos_ux_context.onboarding_index + 1) %
        G_bolos_ux_context.onboarding_step;
    return 0;
}

void screen_consent_set_interval(unsigned int interval_ms) {
    G_bolos_ux_context.screen_stack[0].ticker_value = interval_ms;
    G_bolos_ux_context.screen_stack[0].ticker_interval = interval_ms;
}

void screen_consent_ticker_init(unsigned int number_of_steps,
                                unsigned int interval_ms,
                                unsigned int check_pin_to_confirm) {
    // register action callbacks
    G_bolos_ux_context.screen_stack[0].ticker_value = interval_ms;
    G_bolos_ux_context.screen_stack[0].ticker_interval = interval_ms;
    G_bolos_ux_context.screen_stack[0].ticker_callback = screen_consent_ticker;
    if (!check_pin_to_confirm || !os_perso_isonboarded()) {
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_consent_button;
    } else {
        G_bolos_ux_context.screen_stack[0].button_push_callback =
            screen_consent_button_with_final_pin;
    }

    // start displaying
    G_bolos_ux_context.onboarding_index = number_of_steps - 1;
    G_bolos_ux_context.onboarding_step = number_of_steps;
    screen_consent_ticker(0);
}

#endif // OS_IO_SEPROXYHAL
