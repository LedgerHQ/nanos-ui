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

#include "bolos_ux.h"

#ifdef OS_IO_SEPROXYHAL

#define COLOR_BG_1 0xF9F9F9
//#define COLOR_BG_1 0xF80000
//#define ALWAYS_INVERT

#define KEYCODE_SWITCH '\1'
#define KEYCODE_BACKSPACE '\r'

// avoid typing the size each time
#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)

#define ONBOARDING_CONFIRM_WORD_COUNT 2
#define ONBOARDING_WORD_COMPLETION_MAX_ITEMS 8
#define BOLOS_UX_HASH_LENGTH 4 // as on the blue

#define CONSENT_INTERVAL_MS 3000

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))
#define INARRAY(elementptr, array)                                             \
    ((unsigned int)elementptr >= (unsigned int)array &&                        \
     (unsigned int)elementptr < ((unsigned int)array) + sizeof(array))

extern bolos_ux_context_t G_bolos_ux_context;

extern const unsigned char hex_digits[];

unsigned char rng_u8_modulo(unsigned char modulo);

void screen_hex_identifier_string_buffer(const unsigned char *buffer,
                                         unsigned int total);

// common code for all screens
// reset the screen asynch display machine
void screen_state_init(unsigned int stack_slot);

// common code for all screens
// start display of first declared element
void screen_display_init(unsigned int stack_slot);

// request display of the element (taking care of calling screen displayed
// preprocessors)
void screen_display_element(const bagl_element_t *element);

// prepare to return the exit code after the next display asynch ack is received
// (0d 00 00)
void screen_return_after_displayed_touched_element(unsigned int exit_code);

// screen keyboard helper
void screen_keyboard_init(unsigned int screen_current_element_arrays_index,
                          unsigned int mode);

void screen_pin_keyboard_init(unsigned int screen_current_element_arrays_index);

// all screens
void screen_saver_init(void);
void screen_saver_deinit(void);

void screen_not_personalized_init(void);
void screen_boot_recovery_init(void);
void screen_dashboard_init(void);
void screen_dashboard_prepare(void);
void screen_modal_validate_pin_init(void);
void screen_consent_upgrade_init(void);
void screen_consent_add_init(void);
void screen_consent_del_init(void);
void screen_consent_issuer_key_init(void);
void screen_consent_foreign_key_init(void);
void screen_consent_get_device_name_init(void);
void screen_consent_set_device_name_init(void);
void screen_boot_unsafe_wipe_init(void);
void screen_loader_init(void);
void screen_consent_ux_not_signed_init(void);
void screen_consent_customca_key_init(void);
void screen_consent_reset_customca_init(void);
void screen_consent_setup_customca_init(void);

void screen_random_boarding_init(void);

void screen_onboarding_0_welcome_init(void);
void screen_onboarding_1_2_pin_init(unsigned int step);
void screen_onboarding_3_new_init(void);
void screen_onboarding_4_confirm_init(void);

void screen_onboarding_3_restore_init(void);
void screen_onboarding_4_restore_word_init(unsigned int firstWord);

void screen_onboarding_5_passphrase_init(void);

void screen_onboarding_7_processing_init(void);

void screen_settings_init(unsigned int initial);
// apply settings @ boot time
void screen_settings_apply(void);
void screen_settings_change_pin(void);
void screen_settings_erase_all(void);
void screen_settings_set_temporary(void);
void screen_settings_attach_to_pin(void);
void screen_settings_change_pin_1_2_pin_init(unsigned int initial);
void screen_settings_passphrase_attach_1_init(void);
void screen_settings_passphrase_attach_2_init(unsigned int initial);
void screen_settings_passphrase_temporary_1_init(void);
void screen_settings_passphrase_type_and_review_init(unsigned int kind);
void screen_settings_erase_all_init(void);

void screen_help_init(appmain_t help_ended_callback);

void screen_processing_init(void);

void screen_prepare_masked_icon(unsigned char *icon_bitmap,
                                unsigned int icon_bitmap_length);

#ifdef BOLOS_OS_UPGRADER
void screen_os_upgrader(void);
#endif // BOLOS_OS_UPGRADER

unsigned int screen_consent_button(unsigned int button_mask,
                                   unsigned int button_mask_counter);
unsigned int screen_consent_ticker(unsigned int ignored);
void screen_consent_ticker_init(unsigned int number_of_steps,
                                unsigned int interval_ms,
                                unsigned int check_pin_to_confirm);
void screen_consent_set_interval(unsigned int interval_ms);

typedef unsigned int (*pin_callback_t)(unsigned char *pin_buffer,
                                       unsigned int pin_length);
void screen_common_pin_init(unsigned int stack_slot,
                            pin_callback_t end_callback);

#define COMMON_KEYBOARD_INDEX_UNCHANGED (-1UL)
void screen_common_keyboard_init(unsigned int stack_slot,
                                 unsigned int current_element,
                                 unsigned int nb_elements,
                                 keyboard_callback_t callback);

void debug(unsigned int id, unsigned char *msg);

#endif // OS_IO_SEPROXYHAL
