//interrupts.h - Function Header 
//for "interrupts.S"

#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

/* Keyboard interrupt asm wrapper */
extern void Keyboard_handler();

/* Clock interrupt asm wrapper */
extern void RTC_handler();

/* PIT interrupt asm wrapper */
extern void PIT_handler();

/* System Call asm wrapper */
extern void system_call_handler();


#endif

