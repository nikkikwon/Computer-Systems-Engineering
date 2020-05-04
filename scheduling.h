#ifndef SCHEDULING_H
#define SCHEDULING_H
#include "types.h"
#include "syscall.h"
#include "terminal_driver.h"

#define PIT_IRQ 0
#define PIT_COMMAND_REG 0x43
#define PIT_MODE_2 0x36
#define PIT_CHANNEL_0 0x40

#define FREQUENCY_DIVIDER 11932
#define F_MASKER 0xFF


// 0,1,2  corresponding index in term[];
volatile uint8_t curren_terminal_executing;
volatile uint8_t curren_terminal_displaying;
extern void init_pit();
extern void pit_interrupt();
void switch_process(int process_number);

#endif





