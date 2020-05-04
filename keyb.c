#include "keyb.h"
#include "terminal_driver.h"
#include "lib.h"
#include "i8259.h"

// Keyboard flags(shift, capslock, ctrl)
static uint8_t shift_pressed = 0;
static uint8_t capslock_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;

//uint8_t enter_flag = {0,0,0};

//uint8_t ascii_value = 0; //the value to be put into the buffer(printed on the screen)

//variable declaration

// uint8_t buffer[CHARACTER_LIMIT];  

// Variable to check if special character has been typed. 1 = typed, 0 = not typed


/* we use the scancode set 1 to create this table*/
static uint8_t scancode_map[KEYBOARD_ARRAY_SIZE] = 
{  
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, 
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0   
};

/* when shift "alone" is pressed */
static uint8_t shift_scancode_map[KEYBOARD_ARRAY_SIZE] = 
{
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0
};
  
/* when caps lock "alone" is pressed */
static uint8_t caps_scancode_map[KEYBOARD_ARRAY_SIZE] = 
{
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'','`', 0,
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, 0, 0, ' ', 0
};
    
/* when caps lock + shift is pressed */
static uint8_t capsshift_scancode_map[KEYBOARD_ARRAY_SIZE] = 
{
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0,
    '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, 0, 0, ' ', 0
};

/* init_keyboard
 * description: initialize the keyboard with IRQ number
 * inputs: none
 * outputs: none
 * side effect: Having Keyboard initialized */

void init_keyboard(){
    enable_irq(KEYBOARD_IRQ);
    clear_all_buffer();
   // init_keyboard_buffer();
}

void keyboard_interrupt()
{
    cli();
    send_eoi(KEYBOARD_IRQ);
    uint8_t data;
    while(1)
    {
        data = inb(KEYBOARD_DATA_PORT);
        if(data > 0){ //get the value from the port
            break;
        }
    }
     //prevent key released(key up) to be interpreted
        //default ascii_value = consider no special charactes pressed
        //ascii_value = scancode_map[data];
        /* now the 'data' is the index in the scancode_map array. */
        /* Find out if special keys are pressed */
        //printf("before switch");
        switch(data)
        {      
            case CAPS_LOCK_PRESSED:
            if (capslock_pressed == 0)
                capslock_pressed = 1;
            else
                capslock_pressed = 0;
            break;
            case LEFT_SHIFT_PRESSED:
            shift_pressed = 1;
            break;
            case RIGHT_SHIFT_PRESSED:
            shift_pressed = 1;
            break;
            case LEFT_SHIFT_RELEASED:
            shift_pressed = 0;
            break;
            case RIGHT_SHIFT_RELEASED:
            shift_pressed = 0;
            break;
            case LEFT_CTRL_PRESSED:
            ctrl_pressed = 1;
            break;
            case LEFT_CTRL_RELEASED:
            ctrl_pressed = 0;
            break;
            case LEFT_ALT_PRESSED:
            alt_pressed = 1;
            break;
            case LEFT_ALT_RELEASED:
            alt_pressed = 0;
            break;
            case F1:
            if(alt_pressed)
            {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(TERMINAL_ONE);
            }
            break;
            case F2:
            if(alt_pressed)
            {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(TERMINAL_TWO);
            }
            break;
            case F3:
            if(alt_pressed)
            {
                send_eoi(KEYBOARD_IRQ);
                switch_terminal(TERMINAL_THREE);
            }
            break;

            default:
            break;
        }
        //printf(" after switch");

         //CTRL + L
        if(ctrl_pressed == 1 && data == 0x26 )
        { 
            clear_screen();
        }

        else if( data == ENTER_PRESSED )
        {
           // enter_flag = 1;
            enter();
        }

        //BACKSPACE
        else if( data == BACKSPACE_PRESSED)
        {
            handle_backspace();
        }

        //ALT
        else if(alt_pressed)
         {
             //do nothing by now
         }

        else if(data < KEYBOARD_ARRAY_SIZE)
        {
            unsigned int select_mode;

            if ((!capslock_pressed) && (!shift_pressed))
                select_mode = 0;
            if ((!capslock_pressed) && (shift_pressed))
                select_mode = 1;
            if ((capslock_pressed) && (!shift_pressed))
                select_mode = 2;
            if ((capslock_pressed) && (shift_pressed))
                select_mode = 3;
            if(terminal[curren_terminal_displaying].buf_index < (CHARACTER_LIMIT-1) )
            {
                print_and_add_char_to_buffer(data, select_mode);
            }
        }

}

/* single_char_input
 * description: Chooses appropriate character from 4 tables above depending on the "select_mode"
 * inputs: key_press=key pressed data, select_mode = special key info
 * outputs: character selected from the corresponding table
 * side effect: Gets the character to be printed
 * select_mode = 0: none of the special key pressed
 * select_mode = 1: shift pressed
 * select_mode = 2: capslock pressed
 * select_mode = 3: both capslock and shift pressed
 */
uint8_t single_char_input(unsigned int key_press, unsigned int select_mode)
{
    uint8_t ascii_value;

    switch(select_mode)
    {
        //no special characters pressed
        case 0:
            ascii_value = (scancode_map[key_press]);
            return ascii_value;
            break;

        //shift key pressed
        case 1:
            ascii_value = (shift_scancode_map[key_press]);
            return ascii_value;
            break;

        //capslock pressed
        case 2:
            ascii_value = (caps_scancode_map[key_press]);
            return ascii_value;
            break;

        //both capslock and shift pressed
        case 3:
            ascii_value = (capsshift_scancode_map[key_press]);
            return ascii_value;
            break;
 
        default:
            printf("inappropriate select mode in scan code");
            return 0;
            break;
        }
        return 0;
}
/* char_to_buffer
 * description: Gets character to print from single_char_input function and put into the buffer
 * inputs: data = data read from the keyboard, select_mode = mode determined by the special key press
 * outputs: void
 * side effect: Puts character to be printed into the buffer
 */        
void print_and_add_char_to_buffer(uint8_t data, unsigned int select_mode)
{
    int character;
    character = single_char_input(data, select_mode);
    if(terminal[curren_terminal_displaying].buf_index>127){
        return;
    }
    if(character!= 0)
    {
        putc1(character);

        terminal[curren_terminal_displaying].buffer[terminal[curren_terminal_displaying].buf_index] = character;
        terminal[curren_terminal_displaying].buf_index++;
        terminal[curren_terminal_displaying].buffer[terminal[curren_terminal_displaying].buf_index] = '\0';
    }
    return;
}
/* handle_enter()
 * description: handle enter
 * input: none
 * output: none
 * side effect: change enter flag
 */
void enter(){
    enter_flag[curren_terminal_displaying] = 1;
    putc1('\n');
    //init_keyboard_buffer();
}
/* handle_backspace()
 * description: handle backspace 
 * input: none
 * output: none
 * side effect: backspace
 */
void handle_backspace(){
    if (terminal[curren_terminal_displaying].buf_index <= 0){
        return;
    }
    else{
        terminal[curren_terminal_displaying].buf_index--;
        terminal[curren_terminal_displaying].buffer[terminal[curren_terminal_displaying].buf_index] = '\0';
        backspace1();
        return;
    }
}
/* init_keyboard_buffer()
 * description: initialize the keyboard buffer 
 * input: none
 * output: none
 * side effect: initialize the keyboard buffer data and index to 0
 */
void init_keyboard_buffer()
{
    int i;
    
    for(i = 0; i < CHARACTER_LIMIT; i++)
    {
        terminal[curren_terminal_displaying].buffer[i] = '\0'; 
    }
    terminal[curren_terminal_displaying].buf_index= 0; 
}

/* clear_all_buffer()
 * description: initialize the terminal buffer 
 * input: none
 * output: none
 * side effect: initialize the terminal buffer data and index to 0
 */
void clear_all_buffer() {
    int i;
    enter_flag[0]=0;
    enter_flag[1]=0;
    enter_flag[2]=0;

     for(i = 0; i < CHARACTER_LIMIT; i++)
    {
        terminal[0].buffer[i] = '\0'; 
    }
    terminal[0].buf_index= 0; 
     for(i = 0; i < CHARACTER_LIMIT; i++)
    {
        terminal[1].buffer[i] = '\0'; 
    }
    terminal[1].buf_index= 0; 
     for(i = 0; i < CHARACTER_LIMIT; i++)
    {
        terminal[2].buffer[i] = '\0'; 
    }
    terminal[2].buf_index= 0; 
}















