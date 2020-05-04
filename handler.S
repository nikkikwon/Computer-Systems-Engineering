# handler.S - Set up interrupt handler assembly wrappers
#define ASM 1
#include "x86_desc.h"

.global Keyboard_handler
.global PIT_handler
.global RTC_handler
# keyboard_handler: interrupt handler for keyboard interrupts
# record all flags and regietrs before call this interrupt
Keyboard_handler:
    pushal
    pushfl
	call keyboard_interrupt
    popfl
    popal
	iret
# clock_handler: interrupt handler for rtc interrupts
# record all flags and regietrs before call this interrupt
RTC_handler:
    pushal
    pushfl
	call rtc_interrupt
    popfl
    popal
	iret
# PIT_handler: interrupt handler for pit interrupts
# record all flags and regietrs before call this interrupt
PIT_handler:
    pushal
    pushfl
	call pit_interrupt
    popfl
    popal
	iret
