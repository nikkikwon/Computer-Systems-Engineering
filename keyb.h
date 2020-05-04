#ifndef _KEYB_H
#define _KEYB_H
#include "types.h"
#include "terminal_driver.h"
#include "scheduling.h"

#define KEYBOARD_IRQ              1
#define KEYBOARD_DATA_PORT        0x60
#define KEYBOARD_ARRAY_SIZE       59

#define CAPS_LOCK_PRESSED         0x3A
#define CAPS_LOCK_RELEASED		  0xBA

#define LEFT_CTRL_PRESSED         0x1D
#define LEFT_CTRL_RELEASED        0x9D

#define LEFT_SHIFT_PRESSED        0x2A
#define LEFT_SHIFT_RELEASED       0xAA
#define RIGHT_SHIFT_PRESSED       0x36
#define RIGHT_SHIFT_RELEASED      0xB6

#define BACKSPACE_PRESSED		  0x0E
#define BACKSPACE_RELEASED		  0x8E

#define LEFT_ALT_PRESSED          0x38
#define LEFT_ALT_RELEASED         0xB8

#define ENTER_PRESSED             0x1C
#define ENTER_RELEASED			  0x9C

#define BACKSPACE_PRESSED		  0x0E
#define BACKSPACE_RELEASED		  0x8E

#define F1		                  0x3B
#define F2		                  0x3C
#define F3                  	  0x3D    
#define TERMINAL_ONE              0
#define TERMINAL_TWO              1
#define TERMINAL_THREE            2                   

uint8_t buffer[CHARACTER_LIMIT]; 
volatile uint8_t enter_flag[3];

extern void init_keyboard();
extern void clear_all_buffer() ;
extern void keyboard_interrupt();
extern uint8_t single_char_input(unsigned int key_press, unsigned int select_mode);
extern void print_and_add_char_to_buffer(uint8_t data, unsigned int select_mode);
void enter();
void handle_backspace();
extern void init_keyboard_buffer();


//extern volatile uint8_t *buffer;













#endif
