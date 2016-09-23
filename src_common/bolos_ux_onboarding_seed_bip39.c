/*******************************************************************************
*   Ledger Blue - Secure firmware
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

#include "bolos_ux_common.h"


unsigned int bolos_ux_mnemonic_from_data(unsigned char *in,
                                         unsigned int inLength,
                                         unsigned char outIndex,
                                         unsigned char *out,
                                         unsigned int outLength) {
    unsigned char bits[32 + 1];
    unsigned int mlen = inLength * 3 / 4;
    unsigned int i, j, idx, offset;

    if ((inLength % 4) || (inLength < 16) || (inLength > 32)) {
        THROW(INVALID_PARAMETER);
    }
    cx_hash_sha256(in, inLength, bits);
    bits[inLength] = bits[0];
    os_memmove(bits, in, inLength);
    offset = 0;
    for (i = 0; i < mlen; i++) {
        unsigned char wordLength;
        idx = 0;
        for (j = 0; j < 11; j++) {
            idx <<= 1;
            idx +=
                (bits[(i * 11 + j) / 8] & (1 << (7 - ((i * 11 + j) % 8)))) > 0;
        }
        if (!outIndex) {
            wordLength =
                BIP39_WORDLIST_OFFSETS[idx + 1] - BIP39_WORDLIST_OFFSETS[idx];
            if ((offset + wordLength) > outLength) {
                THROW(INVALID_PARAMETER);
            }
            os_memmove(out + offset,
                       BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[idx],
                       wordLength);
            offset += wordLength;
            if (i < mlen - 1) {
                if (offset > outLength) {
                    THROW(INVALID_PARAMETER);
                }
                out[offset++] = ' ';
            }
        } else {
            if ((offset + 2) > outLength) {
                THROW(INVALID_PARAMETER);
            }
            out[offset++] = ((idx >> 8) & 0xff);
            out[offset++] = (idx & 0xff);
        }
    }
    return offset;
}

unsigned int bolos_ux_mnemonic_indexes_to_words(unsigned char *indexes,
                                                unsigned char *words) {
    unsigned char i;
    unsigned int offset = 0;
    for (i = 0; i < 24; i++) {
        unsigned char wordLength;
        unsigned int idx = ((*indexes) << 8) | (*(indexes + 1));
        wordLength =
            BIP39_WORDLIST_OFFSETS[idx + 1] - BIP39_WORDLIST_OFFSETS[idx];
        os_memmove(words + offset, BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[idx],
                   wordLength);
        offset += wordLength;
        if (i != 23) {
            words[offset++] = ' ';
        }
        indexes += 2;
    }
    return offset;
}

// separated function to lower the stack usage when jumping into pbkdf algorithm
unsigned int
bolos_ux_mnemonic_to_seed_hash_length128(unsigned char *mnemonic,
                                         unsigned int mnemonicLength) {
    if (mnemonicLength > 128) {
        cx_hash_sha512(mnemonic, mnemonicLength, mnemonic);
        // new mnemonic length
        mnemonicLength = 64;
    }
    return mnemonicLength;
}

void bolos_ux_mnemonic_to_seed(unsigned char *mnemonic,
                               unsigned int mnemonicLength,
                               unsigned char *passphrase,
                               unsigned int passphraseLength,
                               unsigned char *seed) {
    if (passphraseLength > 200) {
        THROW(INVALID_PARAMETER);
    }
    mnemonicLength =
        bolos_ux_mnemonic_to_seed_hash_length128(mnemonic, mnemonicLength);

    os_memmove(passphrase, BIP39_MNEMONIC, BIP39_MNEMONIC_LENGTH);

    cx_pbkdf2_sha512(mnemonic, mnemonicLength, passphrase,
                     8 + passphraseLength +
                         4 /*for round index, set in pbkdf2*/,
                     BIP39_PBKDF2_ROUNDS, seed, 64);

    // what happen to the second block for a very short seed ?
}

unsigned int bolos_ux_get_word_ptr(unsigned char **word,
                                   unsigned int max_length,
                                   unsigned int word_index) {
    unsigned int word_length;

    // seek next word
    while (word_index--) {
        while (*(*word) != ' ' && max_length) {
            *word = (*word) + 1;
            max_length--;
        }
        // also skip the space
        *word = (*word) + 1;
        max_length--;
    }

    // seek next word's length
    word_length = 0; // could optim by using the smaller word length here (EOS
                     // or space as delim here)
    while (word_length < max_length && (*word)[word_length] != ' ' &&
           (*word)[word_length] != 0) {
        word_length++;
    }

    // word ptr is returned in the parameter
    return word_length;
}

unsigned char bolos_ux_word_check(unsigned char *word,
                                  unsigned int wordLength) {
    unsigned int i;
    for (i = 0; i < BIP39_WORDLIST_OFFSETS_LENGTH - 1; i++) {
        if ((unsigned int)(BIP39_WORDLIST_OFFSETS[i + 1] -
                           BIP39_WORDLIST_OFFSETS[i]) == wordLength &&
            os_memcmp(word, BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[i],
                      wordLength) == 0) {
            return 1;
        }
    }
    return 0;
}

unsigned int bolos_ux_mnemonic_check(unsigned char *mnemonic,
                                     unsigned int mnemonicLength) {
    unsigned int i, n = 0;
    unsigned int bi;
    unsigned char bits[32 + 1];
    unsigned char mask;

    for (i = 0; i < mnemonicLength; i++) {
        if (mnemonic[i] == ' ') {
            n++;
        }
    }
    n++;
    if (n != 12 && n != 18 && n != 24) {
        return 0;
    }
    os_memset(bits, 0, sizeof(bits));
    i = 0;
    bi = 0;
    while (i < mnemonicLength) {
        unsigned char current_word[10];
        unsigned int current_word_size = 0;
        unsigned int j, k, ki;
        j = 0;
        while (mnemonic[i] != ' ' && i < mnemonicLength) {
            if (j >= sizeof(current_word)) {
                return 0;
            }
            current_word[j] = mnemonic[i];
            current_word_size = j;
            i++;
            j++;
        }
        if (i < mnemonicLength) {
            i++;
        }
        current_word_size++;
        for (k = 0; k < BIP39_WORDLIST_OFFSETS_LENGTH - 1; k++) {
            if ((os_memcmp(current_word,
                           BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[k],
                           current_word_size) == 0) &&
                ((unsigned int)(BIP39_WORDLIST_OFFSETS[k + 1] -
                                BIP39_WORDLIST_OFFSETS[k]) ==
                 current_word_size)) {
                for (ki = 0; ki < 11; ki++) {
                    if (k & (1 << (10 - ki))) {
                        bits[bi / 8] |= 1 << (7 - (bi % 8));
                    }
                    bi++;
                }
                break;
            }
        }
        if (k == (unsigned int)(BIP39_WORDLIST_OFFSETS_LENGTH - 1)) {
            return 0;
        }
    }
    if (bi != n * 11) {
        return 0;
    }
    bits[32] = bits[n * 4 / 3];
    cx_hash_sha256(bits, n * 4 / 3, bits);
    switch (n) {
    case 12:
        mask = 0xF0;
        break;
    case 18:
        mask = 0xFC;
        break;
    default:
        mask = 0xFF;
        break;
    }
    if ((bits[0] & mask) != (bits[32] & mask)) {
        return 0;
    }

    // allright mnemonic is ok
    return 1;
}


unsigned int bolos_ux_bip39_idx_strcpy(unsigned int index,
                                       unsigned char *buffer) {
    if (index < BIP39_WORDLIST_OFFSETS_LENGTH - 1 && buffer) {
        unsigned char wordLength =
            BIP39_WORDLIST_OFFSETS[index + 1] - BIP39_WORDLIST_OFFSETS[index];
        os_memmove(buffer, BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[index],
                   wordLength);
        buffer[wordLength] = 0; // EOS
        return wordLength;
    }
    // no word at that index
    // buffer[0] = 0; // EOS
    return 0;
}

unsigned int bolos_ux_bip39_idx_startswith(unsigned int index,
                                           unsigned char *prefix,
                                           unsigned int prefixlength) {
    unsigned int j = 0;
    if (index < BIP39_WORDLIST_OFFSETS_LENGTH - 1) {
        while (j < (unsigned int)(BIP39_WORDLIST_OFFSETS[index + 1] -
                                  BIP39_WORDLIST_OFFSETS[index]) &&
               BIP39_WORDLIST[BIP39_WORDLIST_OFFSETS[index] + j] == prefix[j]) {
            j++;
        }
        if (j == prefixlength) {
            return 1;
        }
    }
    return 0;
}

unsigned int
bolos_ux_bip39_get_word_idx_starting_with(unsigned char *prefix,
                                          unsigned int prefixlength) {
    unsigned int i;
    for (i = 0; i < BIP39_WORDLIST_OFFSETS_LENGTH - 1; i++) {
        unsigned int j = 0;
        while (j < (unsigned int)(BIP39_WORDLIST_OFFSETS[i + 1] -
                                  BIP39_WORDLIST_OFFSETS[i]) &&
               j < prefixlength &&
               BIP39_WORDLIST[BIP39_WORDLIST_OFFSETS[i] + j] == prefix[j]) {
            j++;
        }
        if (j == prefixlength) {
            return i;
        }
    }
    // no match, sry
    return BIP39_WORDLIST_OFFSETS_LENGTH;
}

unsigned int
bolos_ux_bip39_get_word_count_starting_with(unsigned char *prefix,
                                            unsigned int prefixlength) {
    unsigned int i;
    unsigned int count = 0;
    for (i = 0; i < BIP39_WORDLIST_OFFSETS_LENGTH - 1; i++) {
        unsigned int j = 0;
        while (j < (unsigned int)(BIP39_WORDLIST_OFFSETS[i + 1] -
                                  BIP39_WORDLIST_OFFSETS[i]) &&
               j < prefixlength &&
               BIP39_WORDLIST[BIP39_WORDLIST_OFFSETS[i] + j] == prefix[j]) {
            j++;
        }
        if (j == prefixlength) {
            count++;
        }
        // don't seek till the end, abort when the prefix is not matched anymore
        else if (count > 0) {
            break;
        }
    }
    // return number of matched word starting with the given prefix
    return count;
}

// allocate at most 26 letters for next possibilities
// alrogithm considers the bip39 words are alphabetically ordered in the
// wordlist
unsigned int bolos_ux_bip39_get_word_next_letters_starting_with(
    unsigned char *prefix, unsigned int prefixlength,
    unsigned char *next_letters_buffer) {
    unsigned int i;
    unsigned int letter_count = 0;
    for (i = 0; i < BIP39_WORDLIST_OFFSETS_LENGTH - 1; i++) {
        unsigned int j = 0;
        while (j < (unsigned int)(BIP39_WORDLIST_OFFSETS[i + 1] -
                                  BIP39_WORDLIST_OFFSETS[i]) &&
               j < prefixlength &&
               BIP39_WORDLIST[BIP39_WORDLIST_OFFSETS[i] + j] == prefix[j]) {
            j++;
        }
        if (j == prefixlength) {
            if (j < (unsigned int)(BIP39_WORDLIST_OFFSETS[i + 1] -
                                   BIP39_WORDLIST_OFFSETS[i])) {
                // j is inc during previous loop, don't touch it
                unsigned char next_letter =
                    BIP39_WORDLIST[BIP39_WORDLIST_OFFSETS[i] + j];
                // add the first next_letter inconditionnally
                if (letter_count == 0) {
                    next_letters_buffer[0] = next_letter;
                    letter_count = 1;
                }
                // the next_letter is different
                else if (next_letters_buffer[0] != next_letter) {
                    next_letters_buffer++;
                    next_letters_buffer[0] = next_letter;
                    letter_count++;
                }
            }
        }
        // don't seek till the end, abort when the prefix is not matched anymore
        else if (letter_count > 0) {
            break;
        }
    }
    // return number of matched word starting with the given prefix
    return letter_count;
}
