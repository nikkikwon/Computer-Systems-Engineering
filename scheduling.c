#include "scheduling.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "syscall.h"
#include "page.h"
#include "x86_desc.h"
#include "terminal_driver.h"


int next_terminal = 0;
int8_t pit_time=0;



/*
init_pit interrupt to 100hz
input :none
output:none
SIDE EFFECT:set pit interrupt frequencey to 100hz
*/
void init_pit(){
    
    cli();
    outb(PIT_MODE_2, PIT_COMMAND_REG );
    outb(F_MASKER & (FREQUENCY_DIVIDER), PIT_CHANNEL_0);    //this is from osdev
    outb(FREQUENCY_DIVIDER >>8 ,PIT_CHANNEL_0);
    enable_irq(PIT_IRQ);
    //init process_id_array
    //putc('s');
    curren_terminal_displaying=0;
    curren_terminal_executing=2;
    process_id_array[0]=0;
    process_id_array[1]=0;
    process_id_array[2]=0;
    process_id_array[3]=0;
    process_id_array[4]=0;
    process_id_array[5]=0;

    sti();
    
}
/*
pit_interrupt
handle pit_interrupt
input :none
output:none
SIDE EFFECT:none
*/
void pit_interrupt(){
    int8_t next_process;
    
    send_eoi(PIT_IRQ);
    cli();
    

    if (pit_time==0){
        
        pit_time++;
        curren_terminal_executing=(curren_terminal_executing+1)%3;  //open three termianl
        switch_terminal(curren_terminal_executing);
        sti();
        execute((uint8_t*)"shell");
        
        return;

    }
    if (pit_time<3){
        pcb_t* current_pcb=get_pcb_by_process_number(terminal[curren_terminal_executing].process_in_terminal[terminal[curren_terminal_executing].index]);
         asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(current_pcb->esp), "=b"(current_pcb->ebp)    /* outputs */
                 :                                          /* no input */
                 );
        pit_time++;
        curren_terminal_executing=(curren_terminal_executing+1)%3;
        switch_terminal(curren_terminal_executing);
        sti();

        execute((uint8_t*)"shell");
       
        return;
    }
    if (pit_time==3){
        switch_terminal(0); //come back to the terminal 0
        pit_time++;
    }


    ////////////////
    
    next_terminal=(curren_terminal_executing+1)%3;
    
    next_process=terminal[next_terminal].process_in_terminal[ terminal[next_terminal].index];
    //decide next process here;
    ////////////////
    sti();
    switch_process(next_process);
    
    return;
}
/*
switch_process
help interrupt handler switch to next process 
input :none
output:none
SIDE EFFECT:none
*/

void switch_process(int process_id){
    int8_t process_now;
    next_terminal=(curren_terminal_executing+1)%3;

    repage(_128_MB,_8_MB + process_id * _4_MB);
    ////////////////////////////
    //remap video memory
    // if we want to switch a terminal that is showing 
    if (next_terminal==curren_terminal_displaying){
        revideomemorypage(0);
        re_user_vidpage(0);
    }else
    {
        revideomemorypage(next_terminal+1);
        re_user_vidpage(next_terminal+1);
    }
    //////////////////////////

    process_now=terminal[curren_terminal_executing].process_in_terminal[terminal[curren_terminal_executing].index];
    pcb_t * current_pcb = get_pcb_by_process_number(process_now);
    pcb_t * next_pcb = get_pcb_by_process_number(process_id);
    ///////////////////////////
    //get the terminal process_id belongs to
    //////////////////////////
    
    // check whether the same terminal we display now
    // if not the same, we need to write the things into video buffer
    
    
    curren_terminal_executing=(curren_terminal_executing+1)%3;
    tss.ss0 = KERNEL_DS;
    //the next line is not sure yet
    tss.esp0 = _8_MB - _8KB * (process_id) -4;

    asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(current_pcb->esp), "=b"(current_pcb->ebp)    /* outputs */
                 :                                          /* no input */
                 );
                 
    asm volatile(
                 "movl %%eax, %%esp;"
                 "movl %%ebx, %%ebp;"
                 :                                          /* no outputs */
                 :"a"(next_pcb->esp), "b"(next_pcb->ebp)    /* input */
                 );
    

    return;
}




