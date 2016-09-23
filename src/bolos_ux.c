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

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))

bolos_ux_context_t G_bolos_ux_context;


unsigned short io_timeout(unsigned short last_timeout) {
    UNUSED(last_timeout);
    // infinite timeout
    return 1;
}

const unsigned char hex_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void array_hexstr(char *strbuf, const void *bin, unsigned int len) {
    while (len--) {
        *strbuf++ = hex_digits[((*((char *)bin)) >> 4) & 0xF];
        *strbuf++ = hex_digits[(*((char *)bin)) & 0xF];
        bin = (const void *)((unsigned int)bin + 1);
    }
    *strbuf = 0; // EOS
}

// common code for all screens
void screen_state_init(void) {
    G_bolos_ux_context.screen_before_element_display_callback = NULL;
    G_bolos_ux_context.screen_displayed_callback = NULL;
    G_bolos_ux_context.exit_code_after_elements_displayed = 0;
    G_bolos_ux_context.screen_displayed = 0; // element not yet displayed
    G_bolos_ux_context.screen_current_element = 0;
    G_bolos_ux_context.screen_current_element_arrays_count = 0;
    G_bolos_ux_context.button_push_callback = NULL;
}

// common code for all screens
void screen_display_init(void) {
    G_bolos_ux_context.screen_displayed = 0;
    G_bolos_ux_context.screen_current_element =
        1; // prepare displaying next element

    if (G_bolos_ux_context.screen_before_element_display_callback) {
        if (!G_bolos_ux_context.screen_before_element_display_callback(
                &G_bolos_ux_context.screen_current_element_arrays[0]
                     .element_array[0])) {
            return;
        }
    }
    // display current element
    io_seproxyhal_display(
        &G_bolos_ux_context.screen_current_element_arrays[0].element_array[0]);
}

void screen_return_after_displayed_touched_element(unsigned int exit_code) {
    G_bolos_ux_context.screen_current_element = 0;
    G_bolos_ux_context.screen_displayed = 0;
    G_bolos_ux_context.screen_current_element_arrays_count = 0;
    G_bolos_ux_context.exit_code_after_elements_displayed = exit_code;
}

/* prepare an icon the in the string buffer, with the heading length U2BE */
void screen_prepare_custom_icon(unsigned char *bitmap,
                                unsigned int bitmap_length) {
    // can't do more than the total size of string buffer
    bitmap_length =
        MIN(bitmap_length, sizeof(G_bolos_ux_context.string_buffer) - 2);

    // setup in the string buffer
    G_bolos_ux_context.string_buffer[0] = (bitmap_length + 9) >> 8;
    G_bolos_ux_context.string_buffer[1] = (bitmap_length + 9);

    // bitmap
    os_memmove(G_bolos_ux_context.string_buffer + 2, bitmap, bitmap_length);
}

const unsigned char const C_app_empty_bitmap[] = {
    // color index table
    0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,

    // icon mask
    0x00, 0x00, 0xF0, 0x0F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F,
    0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F,
    0xFC, 0x3F, 0xFC, 0x3F, 0xF0, 0x0F, 0x00, 0x00,
};

void screen_prepare_masked_icon(unsigned char *icon_bitmap,
                                unsigned int icon_bitmap_length) {
    unsigned int i, bitmap_length, inversemode;

    // can't do more than the total size of string buffer
    bitmap_length =
        MIN(icon_bitmap_length, sizeof(G_bolos_ux_context.string_buffer) - 2);

    // setup in the string buffer
    G_bolos_ux_context.string_buffer[0] = (32 + 9) >> 8;
    G_bolos_ux_context.string_buffer[1] = (32 + 9);

    // when first color of the bitmap is not 0, then, must inverse the icon's
    // bit to
    // match the C_app_empty_bitmap bit value
    inversemode = 0;
    if (icon_bitmap[1] != 0 || icon_bitmap[2] != 0 || icon_bitmap[3] != 0 ||
        icon_bitmap[4] != 0) {
        inversemode = 1;
    }

    os_memcpy(G_bolos_ux_context.string_buffer + 2, C_app_empty_bitmap,
              sizeof(C_app_empty_bitmap));

    for (i = 9; i < sizeof(C_app_empty_bitmap) && i < bitmap_length; i++) {
        if (inversemode) {
            G_bolos_ux_context.string_buffer[2 + i] =
                C_app_empty_bitmap[i] & (~icon_bitmap[i]);
        } else {
            G_bolos_ux_context.string_buffer[2 + i] =
                C_app_empty_bitmap[i] & icon_bitmap[i];
        }
    }
}

void io_seproxyhal_display(const bagl_element_t *element) {
    // special processing for icons with custom bitmap
    if (element->component.type == BAGL_ICON &&
        element->component.icon_id == 0) {
        unsigned short length = sizeof(bagl_component_t) +
                                U2BE(G_bolos_ux_context.string_buffer, 0);
        G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
        G_io_seproxyhal_spi_buffer[1] = length >> 8;
        G_io_seproxyhal_spi_buffer[2] = length;
        io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
        io_seproxyhal_spi_send((void *)&element->component,
                               sizeof(bagl_component_t));
        io_seproxyhal_spi_send((void *)(G_bolos_ux_context.string_buffer + 2),
                               length - sizeof(bagl_component_t));
    } else {
        io_seproxyhal_display_default((bagl_element_t *)element);
    }
}

void bolos_ux_main(void) {
    G_bolos_ux_context.exit_code = 0;

    switch (G_bolos_ux_context.state) {
    default:
        // init seproxyhal ux related globals
        io_seproxyhal_init_ux();
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
        switch (G_bolos_ux_context.parameters.ux_id) {
        case BOLOS_UX_BOOT: {
            // nothing to do yet
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;
            break;
        }

        case BOLOS_UX_BOOT_NOT_PERSONALIZED:
            screen_not_personalized_init();
            break;

#ifndef BOLOS_OS_UPGRADER
        case BOLOS_UX_BOOT_UX_NOT_SIGNED:
            screen_consent_ux_not_signed_init();
            break;

        case BOLOS_UX_BOOT_UNSAFE_WIPE:
            screen_boot_unsafe_wipe_init();
            break;

        case BOLOS_UX_BOLOS_START:
            // apply settings in the L4 (ble, brightness, etc)
            screen_settings_apply();

            // request animation when dashboard has finished displaying all the
            // elements (after onboarding OR the first time displayed)
            G_bolos_ux_context.dashboard_redisplayed = 1;

            // nothing else to do
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;
            break;

        case BOLOS_UX_BOOT_ONBOARDING:
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

        case BOLOS_UX_BOOT_RECOVERY: // recovery is dashboard on nano s
        case BOLOS_UX_DASHBOARD:
            screen_settings_apply();

            screen_dashboard_init();
            break;

        case BOLOS_UX_LOADER:
            screen_loader_init();
            break;

        case BOLOS_UX_VALIDATE_PIN:
            screen_validate_pin_init();
            break;

        case BOLOS_UX_CONSENT_APP_ADD:
            screen_consent_add_init();
            break;

        case BOLOS_UX_CONSENT_APP_DEL:
            screen_consent_del_init();
            break;

        case BOLOS_UX_CONSENT_APP_UPG:
            break;

        case BOLOS_UX_CONSENT_ISSUER_KEY:
            screen_consent_issuer_key_init();
            break;

        case BOLOS_UX_CONSENT_FOREIGN_KEY:
            screen_consent_foreign_key_init();
            break;

        case BOLOS_UX_BLANK_PREVIOUS:
            break;

        case BOLOS_UX_APPEXIT:
        case BOLOS_UX_WIPED_DEVICE:
        case BOLOS_UX_SETTINGS:
            // could display, depending on the exit code, a nice banner to tell
            // the application exited
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;
            break;
#else  // ! BOLOS_OS_UPGRADER
        // upgrader dashboard does not exists
        case BOLOS_UX_DASHBOARD:
            screen_os_upgrader();
            break;

        // no ux/consent by default
        default:
            G_bolos_ux_context.exit_code = BOLOS_UX_OK;
            break;
#endif // ! BOLOS_OS_UPGRADER

        // display a wait screen during application loading
        // if host computer bugs, then the token also remains in a loading state
        // (on screen only)
        case BOLOS_UX_PROCESSING:
            screen_processing_init();
            break;

        // only consent upgrade is common to os upgrader and normal os to avoid
        // being stuck if hash doesn't match
        case BOLOS_UX_CONSENT_UPGRADE:
            screen_consent_upgrade_init();
            break;

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
            unsigned int i = 0;

            // just reply "amen"
            // add a "pairing ok" tag if necessary
            // can't have more than one tag in the reply, not supported yet.
            switch (G_io_seproxyhal_spi_buffer[0]) {
            case SEPROXYHAL_TAG_TICKER_EVENT:
                if (G_bolos_ux_context.ticker_callback) {
                    G_bolos_ux_context.ticker_callback(
                        0 /*later the elapsed seconds*/);
                }
                // in case more display to be finished (asynch timer during
                // display sequence)
                goto case_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT;

            // power off if long push, else pass to the application callback if
            // any
            case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
                io_seproxyhal_button_push(
                    G_bolos_ux_context.button_push_callback,
                    G_io_seproxyhal_spi_buffer[3] >> 1);
                // in case more display to be finished (asynch push during
                // display sequence)
                goto case_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT;

            case_SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
                if (io_seproxyhal_spi_is_status_sent() ||
                    G_bolos_ux_context.exit_code != 0) {
                    break;
                }
            // no break is intentional

            case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT: {
                // display next screen element
                unsigned int elem_idx;
                unsigned int total_element_count;
            next_elem:
                if (!G_bolos_ux_context.screen_displayed) {
                    elem_idx = G_bolos_ux_context.screen_current_element;
                    total_element_count = 0;
                    while (i < G_bolos_ux_context
                                   .screen_current_element_arrays_count) {
                        // check if we're sending from this array or not
                        if (elem_idx <
                            G_bolos_ux_context.screen_current_element_arrays[i]
                                .element_array_count) {
                            // pre inc before callback to allow callback to
                            // change the next element to be drawn
                            G_bolos_ux_context.screen_current_element++;

                            if (G_bolos_ux_context
                                    .screen_before_element_display_callback) {
                                if (!G_bolos_ux_context
                                         .screen_before_element_display_callback(
                                             &G_bolos_ux_context
                                                  .screen_current_element_arrays
                                                      [i]
                                                  .element_array[elem_idx])) {
                                    // skip display if requested to
                                    if (!io_seproxyhal_spi_is_status_sent() &&
                                        G_bolos_ux_context.exit_code == 0) {
                                        goto next_elem;
                                    }
                                    // display has been changed
                                    goto return_exit_code;
                                }
                            }

                            io_seproxyhal_display(
                                &G_bolos_ux_context
                                     .screen_current_element_arrays[i]
                                     .element_array[elem_idx]);
                            goto return_exit_code;
                        }
                        //  prepare for next array comparison
                        elem_idx -=
                            G_bolos_ux_context.screen_current_element_arrays[i]
                                .element_array_count;
                        total_element_count +=
                            G_bolos_ux_context.screen_current_element_arrays[i]
                                .element_array_count;
                        i++;
                    }

                    if (G_bolos_ux_context.screen_current_element >=
                        total_element_count) {
                        // if screen has special stuff todo on exit
                        if (G_bolos_ux_context.screen_displayed_callback) {
                            // consider another round if screen displayed
                            // callback ask for it
                            if (!G_bolos_ux_context.screen_displayed_callback(
                                    0)) {
                                G_bolos_ux_context.screen_displayed = 0;
                                goto check_gen_status;
                            }
                        }
                        G_bolos_ux_context.screen_displayed = 1;
                        G_bolos_ux_context.exit_code =
                            G_bolos_ux_context
                                .exit_code_after_elements_displayed;
                    }
                }

            // if no more elements, then general status

            check_gen_status:
                // no element changed, and no exit code, we must close the event
                if (!io_seproxyhal_spi_is_status_sent() &&
                    G_bolos_ux_context.exit_code == 0) {
                    goto general_status;
                }
                break;
            }

            default:
            general_status:
                // send a general status last command
                io_seproxyhal_general_status();
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
        G_bolos_ux_context.exit_code = BOLOS_UX_OK;
        break;
    }

    // stop ticker
    io_seproxyhal_setup_ticker(0);

    return 0;
}

unsigned int screen_consent_ticker(unsigned int ignored) {
    UNUSED(ignored);
    screen_display_init();

    // prepare displaying next screen
    G_bolos_ux_context.onboarding_index =
        (G_bolos_ux_context.onboarding_index + 1) %
        G_bolos_ux_context.onboarding_step;
    return 0;
}

void screen_consent_ticker_init(unsigned int number_of_steps,
                                unsigned int interval_ms) {
    // register action callbacks
    G_bolos_ux_context.ticker_callback = screen_consent_ticker;
    G_bolos_ux_context.button_push_callback = screen_consent_button;

    io_seproxyhal_setup_ticker(interval_ms);

    // start displaying
    G_bolos_ux_context.onboarding_index = number_of_steps - 1;
    G_bolos_ux_context.onboarding_step = number_of_steps;
    screen_consent_ticker(0);
}

#endif // OS_IO_SEPROXYHAL
