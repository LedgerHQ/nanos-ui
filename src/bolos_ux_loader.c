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

#define LOADER_STEP_COUNT 3
const bagl_element_t screen_loader_0_static_elements[] = {
    // current //
    {{BAGL_RECTANGLE, 0x00, 0, 0, 6, 6, 0, 3, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // smaller //
    {{BAGL_RECTANGLE, 0x00, 11, 1, 4, 4, 0, 2, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // erase //
    {{BAGL_RECTANGLE, 0x00, 20, 0, 6, 6, 0, 0, BAGL_FILL, COLOR_BG_1, 0x000000,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // smaller //
    {{BAGL_RECTANGLE, 0x00, 21, 1, 4, 4, 0, 2, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_loader_1_static_elements[] = {
    // erase //
    {{BAGL_RECTANGLE, 0x00, 0, 0, 6, 6, 0, 0, BAGL_FILL, COLOR_BG_1, 0x000000,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // smaller //
    {{BAGL_RECTANGLE, 0x00, 1, 1, 4, 4, 0, 2, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // current //
    {{BAGL_RECTANGLE, 0x00, 10, 0, 6, 6, 0, 3, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

const bagl_element_t screen_loader_2_static_elements[] = {
    // erase //
    {{BAGL_RECTANGLE, 0x00, 10, 0, 6, 6, 0, 0, BAGL_FILL, COLOR_BG_1, 0x000000,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // smaller //
    {{BAGL_RECTANGLE, 0x00, 11, 1, 4, 4, 0, 2, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
    // current //
    {{BAGL_RECTANGLE, 0x00, 20, 0, 6, 6, 0, 3, BAGL_FILL, 0x1d2028, 0x000000, 0,
      0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
};

void screen_loader_init(void) {
    G_bolos_ux_context.exit_code = BOLOS_UX_OK;
}

#endif // OS_IO_SEPROXYHAL
