#include "terminal_driver.h"
#include "keyb.h" 
#include "syscall.h"
#include "scheduling.h"
#include "lib.h"
#include "types.h"


//from the lib.h


// static uint8_t terminal_buffer[CHARACTER_LIMIT];
//terminal_t terminal[NUM_OF_TERMINAL];


//uint8_t keyboard_buffer[CHARACTER_LIMIT];
//uint8_t video_buffer[CHARACTER_LIMIT];

//volatile uint8_t cur_term_idx;




/* void terminal_read(uint8_t c);
 * Inputs:int32_t fd, void* buf, int32_t nbytes
 * Return Value: nbytes-1; 
 *  Function: Output a character to the console */
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes)
{
    
    uint8_t term_run=curren_terminal_executing;
    while (enter_flag[term_run] ==0){
    }
    memcpy(buf, (void*)terminal[term_run].buffer, nbytes);
    enter_flag[term_run] = 0;
    //memcpy(buf, buffer, nbytes);
    init_keyboard_buffer();


    return nbytes-1; 
}
/* void terminal_read(uint8_t c);
 * Inputs:int32_t fd, void* buf, int32_t nbytes
 * Return Value: nbytes-1; 
 *  Function: Output a character to the console */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
   // int32_t num_of_bytes;
    int i ;
    cli();

    if (curren_terminal_displaying==curren_terminal_executing){
        for(i = 0; i<nbytes; i++){
            putc1(((int8_t*)buf)[i]);
        }
        
    }else
    {
        for(i = 0; i<nbytes; i++){
            putc(((int8_t*)buf)[i]);
        }
    }
    

    sti();
   
    
    return nbytes;
}

/* 
 *   terminal_open
 *   DESCRIPTION: This function opens terminal
 *   INPUTS: uint8_t * filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0, terminal_open will be always successful
 *   SIDE EFFECTS: none
 */
 
int32_t terminal_open(const uint8_t * filename) 
{
    //Initialize the terminal
    init_terminal();  
	return 0; 															
}

/* 
 *   terminal_close
 *   DESCRIPTION: This function closes terminal
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0, terminal_close will be always successful
 *   SIDE EFFECTS: none
 */
 
int32_t terminal_close(int32_t fd)
{
	return 0;													
}  
/*
void delete_prev_char()
{
    int r = terminal.row_s;
    int c = terminal.col_s;

    if(terminal.length_s)
    {
        terminal.length_s --;
        if(c >=0)
        {
            c--;
        }
        else
        {
            c = NUM_COLS -1;
            r--; 
        }
        terminal.video_buffer[r * NUM_COLS + c] = 
        terminal.row_s = r;
        terminal.col_s = c;
        

    }
}*/
/* void init_terminal();
 * Inputs:none
 * Return Value: none
 *  Function: ininialize terminal */
void init_terminal()
{
    //looping index
    int i, j;

    for (i = 0; i < NUM_OF_TERMINAL; i++)
    {
        terminal[i].buf_index = 0;
        terminal[i].x = 0;
        terminal[i].y = 0;
        terminal[i].num_term = i;
        terminal[i].index=-1;
        terminal[i].process_in_terminal[0]=0;
        terminal[i].process_in_terminal[1]=0;
        terminal[i].process_in_terminal[2]=0;
        terminal[i].process_in_terminal[3]=0;


        for (j = 0; j < CHARACTER_LIMIT; j++)
        {
            //set the buffer value to 0
            terminal[i].buffer[j] = 0;
        }
        put_cursor(0,0);
    }

    for(i = 0; i < NUM_OF_TERMINAL; i++)
    {
        set_term(i);    
    }
}

/*
*   Function: switch_terminal
*   Description: switch between 3 terminals. 
*   inputs: terminal_idx = indicate which terminal to launch(either 0,1,2) 
*   outputs: return -1 : fail, return 0 : success
*/

// int32_t switch_terminal(uint8_t terminal_idx) 
// {
//     cli();
//     //no more terminals can be launched
//     if(terminal_idx >= NUM_OF_TERMINAL)
//     {
//         return -1;
//     }
//     //terminal already launched
//     if (terminal_idx == cur_term_idx)
//     {
//         return 0;
//     }
   
//     if(term_switch(cur_term_idx, terminal_idx) == -1)
//         return -1;
//     buf_index = terminal[terminal_idx].buf_index;
//     cur_term_idx = terminal_idx;

//     uint8_t * init_screen;
//     vidmap(&init_screen);
//     if (terminal[current_terminal_running].num_term != cur_term_idx) 
//     {
//         ///////////NEED TO MAKE FUNCTION CALL HERE///////////
//     }

//     asm volatile("			
//                  movl %%ebp, %%eax 	
//                  movl %%esp, %%ebx 	
//                  "
//                  :"=a"(old_pcb->ebp), "=b"(old_pcb->esp)
// 	);
// 	sti();
	
// 	return 0;
// }
// change current termianl to the terminal that passed in
// return 1 for success

/* void switch_terminal();
 * Inputs:next_termianl
 * Return Value: 1
 *  Function: switch to the terminal we want */
int32_t switch_terminal(int32_t next_termianl){
    void* video_buffer_current_address;
    void* video_buffer_next_address;
    if ((next_termianl)==(curren_terminal_displaying)){
        return 1;
    }
    cli();
    // copy viedo Memory to video_buffer_current_display
    // copy video_buffer_next to video memory 
    /*switch the display content*/
    video_buffer_current_address=(void*)(0xB9000+(0x1000*curren_terminal_displaying));
    video_buffer_next_address=(void*)(0xB9000+(0x1000*next_termianl));
    memcpy(video_buffer_current_address, (void*)VIDEO_MEMORY, 2*NUM_ROWS*NUM_COLS);
    memcpy((void*)VIDEO_MEMORY, video_buffer_next_address, 2*NUM_ROWS*NUM_COLS);

    // if we want to switch a termianl that is current_running, we should remap the page immediately

    if (curren_terminal_executing==next_termianl){
        revideomemorypage(0);
    }
    save_state(curren_terminal_displaying);
    set_term(next_termianl);
    curren_terminal_displaying=next_termianl;
    

    sti();
    return 1;
}



/*
*   Function: term_switch
*   Description: change current terminal to the other one
*   inputs: term_prev = previous terminal, term_new = new terminal to be set
*   outputs: returns 0 on success, -1 on failure
*/
int32_t term_switch(uint8_t term_prev, uint8_t term_new) 
{
	if (save_state(term_prev) == -1)
		return -1;

	if (set_term(term_new) == -1)
		return -1;

	return 0;
}

/*
*   Function: save_state
*   Description: saves the terminal state
*   inputs: term_idx = indicates which terminal state needs to be saved
*   outputs: return 0 = success, -1 otherwise
*/
int32_t save_state(uint8_t term_idx) 
{
    //terminal[term_idx].buf_index = buf_index;
    terminal[term_idx].x = screen_x;
    terminal[term_idx].y = screen_y;

   

	return 0;
}


/*
*   Function: set_term
*   Description: Set the state of the terminal with respect to the index
*   inputs: term_idx = terminal to set(put back the state)
*   outputs: return 0 = success, -1 otherwise
*/
int32_t set_term(uint8_t term_idx) 
{
	//buf_index = terminal[term_idx].buf_index;
    put_cursor(terminal[term_idx].x,terminal[term_idx].y);
    
   

	return 0;
}

