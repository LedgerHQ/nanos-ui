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

#ifdef HAVE_ELECTRUM

#include "os.h"
#include "cx.h"

#include "bolos_ux_common.h"

extern unsigned int
bolos_ux_mnemonic_to_seed_hash_length128(unsigned char *mnemonic,
                                         unsigned int mnemonicLength);

int cx_math_shiftr_11(unsigned char *r, unsigned int len) {
    unsigned int j, b11;
    b11 = r[len - 1] | ((r[len - 2] & 7) << 8);

    for (j = len - 2; j > 0; j--) {
        r[j + 1] = (r[j] >> 3) | (r[j - 1] << 5);
    }
    r[1] = r[0] >> 3;
    r[0] = 0;

    return b11;
}


unsigned int bolos_ux_electrum_mnemonic_encode(unsigned char *seed17,
                                               unsigned char *out,
                                               unsigned int outLength) {
    unsigned char tmp[17];
    unsigned int i;
    unsigned int offset = 0;
    os_memmove(tmp, seed17, sizeof(tmp));
    for (i = 0; i < 12; i++) {
        unsigned char wordLength;
        unsigned int idx = cx_math_shiftr_11(tmp, sizeof(tmp));
        wordLength =
            BIP39_WORDLIST_OFFSETS[idx + 1] - BIP39_WORDLIST_OFFSETS[idx];
        if ((offset + wordLength) > outLength) {
            THROW(INVALID_PARAMETER);
        }
        os_memmove(out + offset, BIP39_WORDLIST + BIP39_WORDLIST_OFFSETS[idx],
                   wordLength);
        offset += wordLength;
        if (i < 11) {
            if (offset > outLength) {
                THROW(INVALID_PARAMETER);
            }
            out[offset++] = ' ';
        }
    }
    return offset;
}

unsigned int bolos_ux_electrum_new_mnemonic(unsigned int version,
                                            unsigned char *out,
                                            unsigned int outLength) {
    unsigned char seed[17];
    unsigned int nonce;
    unsigned int offset;
    // Initialize a proper seed <= 132 bits
    for (;;) {
        cx_rng(seed, sizeof(seed));
        if (seed[0] < 0x10) {
            break;
        }
    }
    nonce = (seed[sizeof(seed) - 4] << 24) | (seed[sizeof(seed) - 3] << 16) |
            (seed[sizeof(seed) - 2] << 8) | (seed[sizeof(seed) - 1]);
    // Find a nonce that matches the version
    for (;;) {
        nonce++;
        seed[sizeof(seed) - 4] = (nonce >> 24);
        seed[sizeof(seed) - 3] = (nonce >> 16);
        seed[sizeof(seed) - 2] = (nonce >> 8);
        seed[sizeof(seed) - 1] = nonce;
        offset = bolos_ux_electrum_mnemonic_encode(seed, out, outLength);
        if (bolos_ux_electrum_mnemonic_check(version, out, offset)) {
            break;
        }
    }
    return offset;
}

unsigned int bolos_ux_electrum_mnemonic_check(unsigned int version,
                                              unsigned char *mnemonic,
                                              unsigned int mnemonicLength) {
    unsigned char tmp[64];
    cx_hmac_sha512(ELECTRUM_SEED_VERSION, ELECTRUM_SEED_VERSION_LENGTH,
                   mnemonic, mnemonicLength, tmp);
    return (tmp[0] == version);
}

void bolos_ux_electrum_mnemonic_to_seed(unsigned char *mnemonic,
                                        unsigned int mnemonicLength,
                                        unsigned char *seed) {
    unsigned char passphrase[ELECTRUM_MNEMONIC_LENGTH + 4];
    mnemonicLength =
        bolos_ux_mnemonic_to_seed_hash_length128(mnemonic, mnemonicLength);

    os_memmove(passphrase, ELECTRUM_MNEMONIC, ELECTRUM_MNEMONIC_LENGTH);
    cx_pbkdf2_sha512(mnemonic, mnemonicLength, passphrase,
                     ELECTRUM_MNEMONIC_LENGTH +
                         4 /*for round index, set in pbkdf2*/,
                     ELECTRUM_PBKDF2_ROUNDS, seed, 64);
}

#endif
