#ifndef IDT
#define IDT
#include "lib.h" 
#define SYSCALL_VECTOR		0x80
#define RTC   0x28
#define KEYBOARD 0x21
#define EXCEPTION_NUMBER    32
#define USED_EXCEPTION_NUMBER 20
#define UNUSED  15
#define PIT_VECTOR 	0x20
//marco used to simplify IDT.c
//parameter function name: the name of function
// the expansion function will print the function name 
#define ITR_EXCEPTION(function_name)             \
void function_name(){                            \
        printf("%s\n",#function_name);           \
        while(1);                                \
}                                   \

//function used to initialize idt table
extern int initialize_idt();
#endif
