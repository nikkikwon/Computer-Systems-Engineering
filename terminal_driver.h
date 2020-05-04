#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H

#include "lib.h"

#define VIDEO                   0xB8000
#define CHARACTER_LIMIT         128
#define NUM_OF_TERMINAL         3
#define NUM_COLS                80
#define NUM_ROWS                25
#define TERMINAL_ONE            0
#define TERMINAL_TWO            1
#define TERMINAL_THREE          2
#define VIDEO_BUFFER1       0xB9000
#define VIDEO_BUFFER2       0xBA000
#define VIDEO_BUFFER3       0xBB000
#define VIDEO_MEMORY        0xB7000

/*Structure for the multiple terminals*/
typedef struct
{
    
    volatile uint8_t buffer[CHARACTER_LIMIT];  
    volatile unsigned int buf_index;
    uint8_t x;
    uint8_t y;
    uint8_t num_term; //0, 1, 2 for 3 terminals
    uint8_t process_in_terminal[4];     // store which processes is running at this terminal
    int32_t index;
    
}terminal_t;


terminal_t terminal[NUM_OF_TERMINAL];

//static unsigned int buf_index = 0;

extern int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_open(const uint8_t * filename);
extern int32_t terminal_close(int32_t fd);

extern int32_t switch_terminal(int32_t next_termianl);


extern void delete_prev_char(void);
extern void reset_terminal(void);
extern void init_terminal(void);
extern void init_keyboard_buffer(void);
extern void print_backspace(void);
int32_t save_state(uint8_t term_idx);
int32_t term_switch(uint8_t term_prev, uint8_t term_new);
int32_t set_term(uint8_t term_idx);



 #endif /* _TERMINAL_DRIVER_H */
// #ifndef _TERMINAL_DRIVER_H
// #define _TERMINAL_DRIVER_H

// #include "lib.h"
// #include "scheduling.h"

// #define CHARACTER_LIMIT 128
// #define NUM_OF_TERMINAL 1
// #define NUM_COLS                80
// #define NUM_ROWS                25

// typedef struct
// {
//     uint8_t buffer[CHARACTER_LIMIT];  
//     unsigned int buf_index;
  
    

// }terminal_t;

// extern int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);
// extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
// extern int32_t terminal_open(const uint8_t * filename);
// extern int32_t terminal_close(int32_t fd);

// extern void delete_prev_char(void);
// extern void reset_terminal(void);
// extern void init_terminal(void);
// extern void init_keyboard_buffer(void);
// extern void print_backspace(void);
// int read_flag;
// extern static terminal_t terminal[NUM_OF_TERMINAL];

// #endif /* _TERMINAL_DRIVER_H */
