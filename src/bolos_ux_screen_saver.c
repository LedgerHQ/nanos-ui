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

#define KONAMI_CODE_ENABLED 5

const bagl_element_t screen_saver_elements[] = {
    // type                               userid    x    y   w    h  str rad
    // fill      fg        bg      fid iid  txt   touchparams...       ]
    {{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
      0, 0},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},

//{{BAGL_LABELINE                       , 0x01,  56,  19, 128,  32, 0, 0, 0
//, 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px, 0  }, "Locked", 0, 0,
//0, NULL, NULL, NULL },

// konami
//{{BAGL_ICON                           , 0x02,  9,
//32/2-GLYPH_fish_left_HEIGHT/2,  GLYPH_fish_left_WIDTH, GLYPH_fish_left_HEIGHT,
//0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL
//},

// not konami
#ifdef BOUNCING_LOCK
    {{BAGL_ICON, 0x01, 128 / 2 - 14 / 2, 32 / 2 - 14 / 2, 14, 14, 0, 0, 0,
      0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_LOCK_BADGE},
     NULL,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
#else
    //{{BAGL_LABELINE                       , 0x02,  23,  19,  82,  12,
    ///*0x80|*/10, 0, 0        , 0xFFFFFF, 0x000000,
    //BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 26  }, "
    //vires in numeris                                                        ",
    //0, 0, 0, NULL, NULL, NULL },
    {{BAGL_LABELINE, 0x02, 23, 21, 82, 17, /*0x80|*/ 1, 0, 0, 0xFFFFFF,
      0x000000, BAGL_FONT_OPEN_SANS_LIGHT_16px | BAGL_FONT_ALIGNMENT_CENTER,
      26},
     "                        vires in numeris                        ",
     0,
     0,
     0,
     NULL,
     NULL,
     NULL},
//{{BAGL_LABELINE                       , 0x02,  14,  20, 100,  17, /*0x80|*/5,
//0, 0        , 0xFFFFFF, 0x000000,
//BAGL_FONT_OPEN_SANS_LIGHT_16px|BAGL_FONT_ALIGNMENT_CENTER, 26  }, "
//vires in numeris                        ", 0, 0, 0, NULL, NULL, NULL },
#endif // BOUNCING_LOCK
};

#define HORIZONTAL_SPAN (128 - GLYPH_fish_left_WIDTH)

unsigned int screen_saver_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
    /*
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      if (G_bolos_ux_context.saver_konami==KONAMI_CODE_ENABLED) {
        // fear the fish
        if (G_bolos_ux_context.saver_step_x<HORIZONTAL_SPAN) {
          G_bolos_ux_context.saver_step_x =
    (2*HORIZONTAL_SPAN)-G_bolos_ux_context.saver_step_x;
        }
      }
      else if (G_bolos_ux_context.saver_konami >= 2) {
        G_bolos_ux_context.saver_konami++;
        // enable the ticker when konami code is ok
        if (G_bolos_ux_context.saver_konami==5) {
          G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count-1].ticker_value
    = 100;
        }
      }
      break;
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      if (G_bolos_ux_context.saver_konami==KONAMI_CODE_ENABLED) {
        // fear the fish
        if (G_bolos_ux_context.saver_step_x>=HORIZONTAL_SPAN) {
          G_bolos_ux_context.saver_step_x =
    (2*HORIZONTAL_SPAN)-G_bolos_ux_context.saver_step_x;
        }
      }
      else if (G_bolos_ux_context.saver_konami <= 1) {
        G_bolos_ux_context.saver_konami++;
      }
      break;
    */

    case BUTTON_EVT_RELEASED | BUTTON_LEFT:
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
        G_bolos_ux_context
            .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
            .exit_code_after_elements_displayed = BOLOS_UX_OK;
        screen_display_init(screen_stack_pop());
        break;
    }
    return 0;
}

unsigned int screen_saver_ticker(unsigned int ignored) {
    UNUSED(ignored);
#ifdef BOUNCING_LOCK
    G_bolos_ux_context.screen_stack[G_bolos_ux_context.screen_stack_count - 1]
        .element_index = 1; // prepare displaying next element
    screen_display_element(
        &G_bolos_ux_context
             .screen_stack[G_bolos_ux_context.screen_stack_count - 1]
             .element_arrays[0]
             .element_array[0]);
#else
    screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
#endif // BOUNCING_LOCK
    return 0;
}

const bagl_element_t *
screen_saver_before_display_callback(const bagl_element_t *element) {
    // display other elements only if screen setup, else, only redraw words
    // value
    return element;
}

void screen_saver_init(void) {
    unsigned int stack_slot = 0;

    // NOTE: on modal, we can take the place of the non modal version (before
    // displaying dashboard, or on top of app requesting a pin validation)

    // will push back the poweroff on top of screen stack
    stack_slot = screen_stack_is_element_array_present(screen_saver_elements);
    if (stack_slot) {
        screen_stack_remove(stack_slot - 1);

        /*
        // won't redisplay to enable exiting upon any button press and to avoid
        the race
        // button press => set inactivity mode = NONE
        // ticker event => inactive state => display screen saver => state_init
        => reset button state
        // button release => nothing happens :(, state has been reset
        return;
        */
        // need to be repushed over pin lock (pin is wiped, therefore we need to
        // repaint a saver over it)
    }

    // this is a modal
    stack_slot = screen_stack_push();

    screen_state_init(stack_slot);

#ifdef BOUNCING_LOCK
    G_bolos_ux_context.saver_step_x = cx_rng_u8() % 2 ? 3 : -3;
    G_bolos_ux_context.saver_step_y = cx_rng_u8() % 2 ? 1 : -1;
    os_memmove(&G_bolos_ux_context.tmp_element, &screen_saver_elements[1],
               sizeof(G_bolos_ux_context.tmp_element));
#endif // BOUNCING_LOCK

    G_bolos_ux_context.screen_stack[stack_slot]
        .screen_before_element_display_callback =
        screen_saver_before_display_callback;
    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[0]
        .element_array = screen_saver_elements;
    G_bolos_ux_context.screen_stack[stack_slot]
        .element_arrays[0]
        .element_array_count = ARRAYLEN(screen_saver_elements);
    G_bolos_ux_context.screen_stack[stack_slot].element_arrays_count = 1;
    G_bolos_ux_context.screen_stack[stack_slot].button_push_callback =
        screen_saver_button;

#ifdef BOUNCING_LOCK
    // timer before switching to konami automatically, or animate the fish
    G_bolos_ux_context.screen_stack[stack_slot].ticker_value =
        G_bolos_ux_context.screen_stack[stack_slot].ticker_interval =
            100; // reload value when 0 is reached
    G_bolos_ux_context.screen_stack[stack_slot].ticker_callback =
        screen_saver_ticker;
#else  // BOUNCING_LOCK
    G_bolos_ux_context.screen_stack[stack_slot].ticker_value =
        G_bolos_ux_context.screen_stack[stack_slot].ticker_interval =
            7800; // reload value when 0 is reached // "vires in numeris" 16px
    G_bolos_ux_context.screen_stack[stack_slot].ticker_callback =
        screen_saver_ticker;
#endif // BOUNCING_LOCK
    screen_display_init(stack_slot);
}

void screen_saver_deinit(void) {
    unsigned int stack_slot = 0;
    // will push back the poweroff on top of screen stack
    stack_slot = screen_stack_is_element_array_present(screen_saver_elements);
    if (stack_slot) {
        G_bolos_ux_context.screen_stack[stack_slot - 1]
            .exit_code_after_elements_displayed = BOLOS_UX_OK;
        screen_stack_remove(stack_slot - 1);
        screen_display_init(G_bolos_ux_context.screen_stack_count - 1);
    }
}

#endif // OS_IO_SEPROXYHAL
