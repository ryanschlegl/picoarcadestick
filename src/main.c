/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ryan Schlegl
 * Derived from tinyusb hid_multiple_interface example by Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"


//--------------------------------------------------------------------+
// Configuration
//--------------------------------------------------------------------+

#define PLAYERS CFG_TUD_HID // Define player count in tusb_config.h CFG_TUD_HID between 1 and 4
#define BUTTONS 8
#define DIRECTIONS 4
//#define DEBOUNCE         // Debounce for DEBOUNCE_TIME Uncomment to activate, Comment to deactivate
#define DEBOUNCE_TIME 10   // Debounce time in milliseconds

// define gpio numbers for input
// Player 1
#define GPIO_P1_1K 6     
#define GPIO_P1_2K 7     
#define GPIO_P1_3K 8     
//#define GPIO_P2_4K 9
#define GPIO_P1_1P 10    
#define GPIO_P1_2P 11    
#define GPIO_P1_3P 12    
//#define GPIO_P2_4P 13
#define GPIO_P1_SE 16    
#define GPIO_P1_ST 17 
#define GPIO_P1_UP 2     
#define GPIO_P1_DOWN 3   
#define GPIO_P1_RIGHT 4  
#define GPIO_P1_LEFT 5 

// Player 2
#define GPIO_P2_1K 14 
#define GPIO_P2_2K 15
#define GPIO_P2_3K 28
//#define GPIO_P2_4K 
#define GPIO_P2_1P 27
#define GPIO_P2_2P 26
#define GPIO_P2_3P 22
//#define GPIO_P2_4P 
#define GPIO_P2_SE 0
#define GPIO_P2_ST 1
#define GPIO_P2_UP 21
#define GPIO_P2_DOWN 20
#define GPIO_P2_RIGHT 19
#define GPIO_P2_LEFT 18

/*
// Player 3
#define GPIO_P3_1K  
#define GPIO_P3_2K 
#define GPIO_P3_ST 
#define GPIO_P3_UP 
#define GPIO_P3_DOWN 
#define GPIO_P3_RIGHT 
#define GPIO_P3_LEFT 

// Player 4
#define GPIO_P4_1K  
#define GPIO_P4_2K 
#define GPIO_P4_ST 
#define GPIO_P4_UP 
#define GPIO_P4_DOWN 
#define GPIO_P4_RIGHT 
#define GPIO_P4_LEFT
*/

typedef union {
  struct {
    uint32_t buttonA;
    uint32_t buttonB;
    uint32_t buttonRightTrigger;
    //uint32_t buttonLeftTrigger;
    uint32_t buttonX;
    uint32_t buttonY;
    uint32_t buttonRightButton;
    //uint32_t buttonLeftButton;
    uint32_t buttonSelect;
    uint32_t buttonStart;
    uint32_t up;
    uint32_t down;
    uint32_t right;
    uint32_t left;
    };
    int array[BUTTONS + DIRECTIONS];
} inputMap;


inputMap player[] = {
  { // player 1
  1 << GPIO_P1_1K, 1 << GPIO_P1_2K, 1 << GPIO_P1_3K, //1 << GPIO_P1_4K,
  1 << GPIO_P1_1P, 1 << GPIO_P1_2P, 1 << GPIO_P1_3P, //1 << GPIO_P1_4P,
  1 << GPIO_P1_SE, 
  1 << GPIO_P1_ST, // buttons
  1 << GPIO_P1_UP, 1 << GPIO_P1_DOWN, 1 << GPIO_P1_RIGHT, 1 << GPIO_P1_LEFT, // directions
  },
  { // player 2
  1 << GPIO_P2_1K, 1 << GPIO_P2_2K, 1 << GPIO_P2_3K, //1 << GPIO_P2_4K,
  1 << GPIO_P2_1P, 1 << GPIO_P2_2P, 1 << GPIO_P2_3P, //1 << GPIO_P2_4P,
  1 << GPIO_P2_SE, 
  1 << GPIO_P2_ST, // buttons
  1 << GPIO_P2_UP, 1 << GPIO_P2_DOWN, 1 << GPIO_P2_RIGHT, 1 << GPIO_P2_LEFT, // directions
  }/*,  
  { // player 3
  1 << GPIO_P3_1K, 1 << GPIO_P3_2K, //1 << GPIO_P3_3K, 1 << GPIO_P2_4K,
  //1 << GPIO_P3_1P, 1 << GPIO_P3_2P, 1 << GPIO_P3_3P, 1 << GPIO_P2_4P,
  //1 << GPIO_P3_SE, 
  1 << GPIO_P3_ST, // buttons
  1 << GPIO_P3_UP, 1 << GPIO_P3_DOWN, 1 << GPIO_P3_RIGHT, 1 << GPIO_P3_LEFT, // directions
  },
  { // player 4
  1 << GPIO_P4_1K, 1 << GPIO_P4_2K, //1 << GPIO_P4_3K, 1 << GPIO_P2_4K,
  //1 << GPIO_P4_1P, 1 << GPIO_P4_2P, 1 << GPIO_P4_3P, 1 << GPIO_P2_4P,
  //1 << GPIO_P4_SE, 
  1 << GPIO_P4_ST, // buttons
  1 << GPIO_P4_UP, 1 << GPIO_P4_DOWN, 1 << GPIO_P4_RIGHT, 1 << GPIO_P4_LEFT, // directions
  }
  */
};

uint32_t input;
uint32_t inputMask;
uint32_t playerMask[PLAYERS];
uint32_t playerInputPrev[PLAYERS];
uint32_t dpadMask[PLAYERS];
hid_gamepad_report_t gp[PLAYERS];
#ifdef DEBOUNCE
uint32_t inputNew;
uint32_t debounceMask[PLAYERS][BUTTONS+DIRECTIONS];
uint32_t input_ms[PLAYERS][BUTTONS+DIRECTIONS];
#endif
void hid_task();

/*------------- MAIN -------------*/
int main(void)
{
  // calculate input masks
  for (int i = 0; i < PLAYERS; i++) {
    for (int j = 0; j < BUTTONS+DIRECTIONS; j++) {
      inputMask = inputMask | player[i].array[j];
      playerMask[i] = playerMask[i] | player[i].array[j];
      #ifdef DEBOUNCE
      debounceMask[i][j] = player[i].array[j];
      #endif
    }
  
    dpadMask[i] = player[i].up | player[i].down | player[i].left | player[i].right;
  }
  
  // init gpio
  gpio_init_mask(inputMask);
  // set gpio as inputs
  gpio_set_dir_in_masked(inputMask);
  // set gpio as pullup
  uint gpio_pullup_mask = inputMask;
  for(uint i=0;i<32;i++) {
    if (gpio_pullup_mask & 1) {
        gpio_pull_up(i);
    }
    gpio_pullup_mask >>= 1;
  }

  board_init();
  tusb_init();
  // turn on LED
  board_led_write(1);

  while (1)
  {
    hid_task();
    tud_task(); // tinyusb device task
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void hid_task(void)
{
  // Poll max every 1ms
  const uint32_t interval_ms = 1;
  static uint32_t start_ms = 0;
  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  // Read inputs
  #ifndef DEBOUNCE
  input = inputMask & ~gpio_get_all();
  #else
  inputNew = inputMask & ~gpio_get_all();
  for (int i=0; i < PLAYERS; i++) {
    for (int j=0; j < BUTTONS+DIRECTIONS; j++) {
      if((inputNew & debounceMask[i][j]) != (input & debounceMask[i][j]) && (board_millis() - input_ms[i][j]) > DEBOUNCE_TIME){
        input ^= debounceMask[i][j];
        input_ms[i][j] = board_millis();
      }
    }
  }
  #endif
  
  // Set "Gamepad" for USB HID report as defined in TinyUSB hid.h
  for (int i = 0; i < PLAYERS; i++) {
    // If input has changed
    if( (input & playerMask[i]) != playerInputPrev[i] ) {
      
      // TODO optionally set analog inputs e.g. with directions/buttons similar to buttons/hat or with analog pins
      gp[i].x       = 0;  ///< Delta x  movement of left analog-stick
      gp[i].y       = 0;  ///< Delta y  movement of left analog-stick
      gp[i].z       = 0;  ///< Delta z  movement of right analog-joystick
      gp[i].rz      = 0;  ///< Delta Rz movement of right analog-joystick
      gp[i].rx      = 0;  ///< Delta Rx movement of analog left trigger
      gp[i].ry      = 0;  ///< Delta Ry movement of analog right trigger
      
      // Buttons
      gp[i].buttons = 0
      | (((input & player[i].buttonA )             ? GAMEPAD_BUTTON_A : 0) ) // 1K, A, Cross
      | (((input & player[i].buttonB)              ? GAMEPAD_BUTTON_B : 0) ) // 2K, B, Circle
      | (((input & player[i].buttonRightTrigger)   ? GAMEPAD_BUTTON_TR2 : 0) ) // 3K, RT, 1`R2
      | (((input & player[i].buttonX)              ? GAMEPAD_BUTTON_X : 0) ) // 1P, X, Square
      | (((input & player[i].buttonY)              ? GAMEPAD_BUTTON_Y : 0) ) // 2P, Y, Triangle
      | (((input & player[i].buttonRightButton)    ? GAMEPAD_BUTTON_TR : 0) ) // 3P, RB, R1
      | (((input & player[i].buttonSelect)         ? GAMEPAD_BUTTON_SELECT : 0) )  // Coin, Select, Back
      | (((input & player[i].buttonStart)          ? GAMEPAD_BUTTON_START : 0)); // Start, Start, Start
      
      // Direction
      if ((input & dpadMask[i]) == (player[i].up | player[i].down))    {gp[i].hat = GAMEPAD_HAT_CENTERED;} // SOCD UP DOWN NEUTRAL
      if ((input & dpadMask[i]) == (player[i].left | player[i].right)) {gp[i].hat = GAMEPAD_HAT_CENTERED;} // SOCD LEFT RIGHT NEUTRAL
      if ((input & dpadMask[i]) == (player[i].up))                     {gp[i].hat = GAMEPAD_HAT_UP;} else 
      if ((input & dpadMask[i]) == (player[i].up | player[i].right))   {gp[i].hat = GAMEPAD_HAT_UP_RIGHT;} else
      if ((input & dpadMask[i]) == (player[i].right))                  {gp[i].hat = GAMEPAD_HAT_RIGHT;} else
      if ((input & dpadMask[i]) == (player[i].down | player[i].right)) {gp[i].hat = GAMEPAD_HAT_DOWN_RIGHT;} else
      if ((input & dpadMask[i]) == (player[i].down))                   {gp[i].hat = GAMEPAD_HAT_DOWN;} else
      if ((input & dpadMask[i]) == (player[i].down | player[i].left))  {gp[i].hat = GAMEPAD_HAT_DOWN_LEFT;} else
      if ((input & dpadMask[i]) == (player[i].left))                   {gp[i].hat = GAMEPAD_HAT_LEFT;} else
      if ((input & dpadMask[i]) == (player[i].up | player[i].left))    {gp[i].hat = GAMEPAD_HAT_UP_LEFT;} else
      gp[i].hat = GAMEPAD_HAT_CENTERED;

      // Send USB HID report
      if ( tud_hid_n_ready(i) )
      {
          tud_hid_n_gamepad_report(i, 0, gp[i].x,gp[i].y,gp[i].z,gp[i].rz,gp[i].rx,gp[i].ry,gp[i].hat,gp[i].buttons);
      }
    }
    // Save previous input
    playerInputPrev[i] = (input & playerMask[i]);
  }

}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // TODO set LED based on CAPLOCK, NUMLOCK etc...
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) bufsize;
}

