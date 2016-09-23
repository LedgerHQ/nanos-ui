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

#ifndef BOLOS_UX_H
#define BOLOS_UX_H

#include "os_io_seproxyhal.h"

#ifdef OS_IO_SEPROXYHAL

typedef unsigned int (*unsigned_int_callback_t)(unsigned int);

// bolos ux context (not mandatory if redesigning a bolos ux)
typedef struct bolos_ux_context {
// enum bolos_ux_state_e {
#ifndef STATE_INITIALIZED
#define STATE_INITIALIZED 0xB01055E5
#endif // STATE_INITIALIZED
    unsigned int state;

    // current displayed screen
    unsigned int screen;

    // arrays of element to be displayed (to automate when dealing with static
    // and dynamic elements)
    struct {
        const bagl_element_t *element_array;
        unsigned int element_array_count;
    } screen_current_element_arrays[4]; // for keyboard enabled screen [static
                                        // screen elements, keyboard elements,
                                        // volatile screen element (the caret
                                        // and text field)]
    unsigned int screen_current_element_arrays_count;
    unsigned int screen_current_element;

    unsigned int exit_code;

    unsigned int last_ux_id;

    unsigned int loader_step;

#define BOLOS_UX_ONBOARDING_NEW 1
#define BOLOS_UX_ONBOARDING_RESTORE 2
#define BOLOS_UX_ONBOARDING_RESTORE_12 12
#define BOLOS_UX_ONBOARDING_RESTORE_18 18
#define BOLOS_UX_ONBOARDING_RESTORE_24 24
    unsigned int onboarding_kind;
    unsigned int onboarding_step;
    unsigned int onboarding_index;
    unsigned int onboarding_words_checked;
    unsigned int onboarding_words_are_valid;
    unsigned int onboarding_step_checked_inc;
    unsigned int onboarding_step_checked;

    unsigned int exit_code_after_elements_displayed;
    unsigned int screen_displayed;
    unsigned_int_callback_t screen_displayed_callback;
    bagl_element_callback_t screen_before_element_display_callback;

    // callback for the keyboard touch tap (can be null)
    unsigned_int_callback_t keyboard_callback;

    // button callback, called upon button release with a mask of buttons
    button_push_callback_t button_push_callback;

    // ticker callback
    unsigned_int_callback_t ticker_callback;

// store the BIP39 mnemonic before the passphrase before starting pbkdf2
#define PASSPHRASE_MAX_SIZE 32
#define PASSPHRASE_OFFSET 8
    char passphrase_buffer[8 + PASSPHRASE_MAX_SIZE +
                           4]; // well this is already sufficient
    unsigned int passphrase_buffer_length;
    unsigned int words_buffer_length;

    char words_buffer[256]; // 128 of words (215 => hashed to 64, or 128) +
                            // HMAC_LENGTH*2 = 256
    char pin_buffer[8];

    char string_buffer[64]; // to store the seed wholy

    // filled up during os_ux syscall when called by user or bolos.
    bolos_ux_params_t parameters;

    // dynamic elements
    bagl_element_t screen_volatile_elements[3]; // 0x38 per bagl_element_t

    // slider management
    unsigned int hslider3_before;
    unsigned int hslider3_current;
    unsigned int hslider3_after;
    unsigned int hslider3_total;

    // dashboard last selected item
    unsigned int dashboard_last_selected;
    unsigned int dashboard_redisplayed; // to trigger animation when all
                                        // elements are displayed

    // to return to the current context after help screens have been validated
    appmain_t help_ended_callback;
    unsigned int help_screen_idx;

// detect stack/global variable overlap
#define CANARY_MAGIC 0x764DE9AB
    unsigned int canary;

} bolos_ux_context_t;

extern bolos_ux_context_t G_bolos_ux_context;

// update before, current, after index for horizontal slider with 3 positions
// slider distinguish handling from the data, to be more generic :)
#define BOLOS_UX_HSLIDER3_NONE (-1UL)
void bolos_ux_hslider3_init(unsigned int total_count);
void bolos_ux_hslider3_next(void);
void bolos_ux_hslider3_previous(void);

#define FAST_LIST_THRESHOLD_CS 8
#define FAST_LIST_ACTION_CS 2

#endif // #ifdef OS_IO_SEPROXYHAL

/**
 * Bolos system app internal UX entry point (could be overriden by a further
 * loaded BOLOS_UX application)
 */
void bolos_ux_main(void);

#endif // BOLOS_UX_H
