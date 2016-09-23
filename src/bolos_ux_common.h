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

#include "bolos_ux.h"

#ifdef OS_IO_SEPROXYHAL

#define COLOR_BG_1 0xF8F8F8
//#define COLOR_BG_1 0xF80000
//#define ALWAYS_INVERT

#define KEYCODE_SWITCH '\1'
#define KEYCODE_BACKSPACE '\r'

#define ONBOARDING_CONFIRM_WORD_COUNT 2
#define ONBOARDING_WORD_COMPLETION_MAX_ITEMS 8
#define BOLOS_UX_HASH_LENGTH 4 // as on the blue

#define CONSENT_INTERVAL_MS 3000

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))

extern bolos_ux_context_t G_bolos_ux_context;

extern const unsigned char hex_digits[];

void array_hexstr(char *strbuf, const void *bin, unsigned int len);

// common code for all screens
// reset the screen asynch display machine
void screen_state_init(void);

// common code for all screens
// start display of first declared element
void screen_display_init(void);

// prepare to return the exit code after the next display asynch ack is received
// (0d 00 00)
void screen_return_after_displayed_touched_element(unsigned int exit_code);

// screen keyboard helper
void screen_keyboard_init(unsigned int screen_current_element_arrays_index);

/* prepare an icon the in the string buffer, with the heading length U2BE */
void screen_prepare_custom_icon(unsigned char *bitmap,
                                unsigned int bitmap_length);

// all screens

void screen_not_personalized_init(void);
void screen_boot_recovery_init(void);
void screen_dashboard_init(void);
void screen_validate_pin_init(void);
void screen_consent_upgrade_init(void);
void screen_consent_add_init(void);
void screen_consent_del_init(void);
void screen_consent_issuer_key_init(void);
void screen_consent_foreign_key_init(void);
void screen_boot_unsafe_wipe_init(void);
void screen_loader_init(void);

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

void screen_help_init(appmain_t help_ended_callback);

void screen_consent_ux_not_signed_init(void);
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
                                unsigned int interval_ms);

// BIP39 helpers
#include "bolos_ux_onboarding_seed_rom_variables.h"

void bolos_ux_pbkdf2(unsigned char *password, unsigned int passwordlen,
                     unsigned char *salt, unsigned int saltlen,
                     unsigned int iterations, unsigned char *out,
                     unsigned int outLength);
unsigned char bolos_ux_get_random_bip39_word(unsigned char *word);
// return 0 if mnemonic is invalid
unsigned int bolos_ux_mnemonic_check(unsigned char *mnemonic,
                                     unsigned int mnemonicLength);
unsigned char bolos_ux_word_check(unsigned char *word, unsigned int wordLength);

unsigned int bolos_ux_get_word_ptr(unsigned char **word,
                                   unsigned int max_length,
                                   unsigned int word_index);

// passphrase will be prefixed with "MNEMONIC" from BIP39, the passphrase
// content shall start @ 8
void bolos_ux_mnemonic_to_seed(
    unsigned char *mnemonic, unsigned int mnemonicLength,
    unsigned char *passphrase, unsigned int passphraseLength,
    unsigned char *seed /*, unsigned char *workBuffer*/);
unsigned int bolos_ux_mnemonic_indexes_to_words(unsigned char *indexes,
                                                unsigned char *words);
unsigned int bolos_ux_mnemonic_from_data(unsigned char *in,
                                         unsigned int inLength,
                                         unsigned char outIndex,
                                         unsigned char *out,
                                         unsigned int outLength);

unsigned int
bolos_ux_bip39_get_word_idx_starting_with(unsigned char *prefix,
                                          unsigned int prefixlength);
unsigned int bolos_ux_bip39_idx_strcpy(unsigned int index,
                                       unsigned char *buffer);
unsigned int bolos_ux_bip39_idx_startswith(unsigned int idx,
                                           unsigned char *prefix,
                                           unsigned int prefixlength);
unsigned int
bolos_ux_bip39_get_word_count_starting_with(unsigned char *prefix,
                                            unsigned int prefixlength);
unsigned int bolos_ux_bip39_get_word_next_letters_starting_with(
    unsigned char *prefix, unsigned int prefixlength,
    unsigned char *next_letters_buffer);

#endif // OS_IO_SEPROXYHAL
