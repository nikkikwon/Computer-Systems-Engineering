#include "IDT.h"
#include "x86_desc.h"
#include "types.h"
#include "keyb.h"
#include "rtc.h"
#include "scheduling.h"
#include "handler.h"
#include "linkage.h"
//this is the marcos used to print the exception message, see IDT.h
ITR_EXCEPTION(Divide_Error);
ITR_EXCEPTION(RESERVED);
ITR_EXCEPTION(NMI_interrupt);
ITR_EXCEPTION(Breakpoint);
ITR_EXCEPTION(Overflow);
ITR_EXCEPTION(BOUND_Range_Exceeded);
ITR_EXCEPTION(Invalid_Opcode);
ITR_EXCEPTION(Device_Not_Available);
ITR_EXCEPTION(Double_Fault);
ITR_EXCEPTION(Coprocessor_Segment_Overrun );
ITR_EXCEPTION(Invalid_TSS);
ITR_EXCEPTION(Segment_Not_Present);
ITR_EXCEPTION(Stack_Segment_Fault);
ITR_EXCEPTION(General_Protection);
ITR_EXCEPTION(Page_Fault);
ITR_EXCEPTION(x87_FPU_Floating_Point_Error);
ITR_EXCEPTION(Alignment_Check);
ITR_EXCEPTION(Machine_Check);
ITR_EXCEPTION(SIMD_Floating_Point_Exception);



/*	Function: initialize_idt()
*	Description: intializes the interrupts when boot
*	input: none
*	output: none
*   return value:0
*	effects: sets the IDT entries to be the associated exceptions
*/
int initialize_idt(){
    int i;   // Initialize index
for (i = 0;i< NUM_VEC;i++){     //use 256 loop to initialize all the entries
    
    idt[i].seg_selector = KERNEL_CS;           //this is initialized based on 
    idt[i].reserved4 = 0;                      //the IA-32 Intel® Architecture Software Developer’s Manual Volume 3: 
    idt[i].reserved3 = 0;                      //System Programming Guide.
    idt[i].reserved2 = 1;                      //Figure 5-2. IDT Gate Descriptors
    idt[i].reserved1 = 1;
    idt[i].size = 1;
    idt[i].reserved0 = 0;
    idt[i].dpl = 0;
    idt[i].present = 0;                         //at beginning, initialize all to non-present
    idt[i].offset_31_16 = 0;
    idt[i].offset_15_00 = 0;
    
    if(i < EXCEPTION_NUMBER){                                   
        idt[i].reserved3 = 1;
        if(i<USED_EXCEPTION_NUMBER&&i!=UNUSED){
            idt[i].present = 1;                 //the first 20 exist
        }
    }
    if(i == SYSCALL_VECTOR){
        idt[i].reserved3 = 1;
        idt[i].dpl = 3;                         //userspace systemcall present
        idt[i].present = 1;
    }
    if(i == KEYBOARD){
        idt[i].dpl = 3;                         //keyboard interrupt  present
        idt[i].present = 1;
    }
    if(i == RTC){
        idt[i].dpl = 3;                         //RTC interrupt present
        idt[i].present = 1;
    }
      if(i == PIT_VECTOR){
        idt[i].dpl = 3;                         //RTC interrupt present
        idt[i].present = 1;
    }

}

SET_IDT_ENTRY(idt[0], Divide_Error);             //this is set based on 
SET_IDT_ENTRY(idt[1], RESERVED);                 //the IA-32 Intel® Architecture Software Developer’s Manual Volume 3: 
SET_IDT_ENTRY(idt[2], NMI_interrupt);            //System Programming Guide.
SET_IDT_ENTRY(idt[3], Breakpoint);               //Figure 5-1. Table 5-1. Protected-Mode Exceptions and Interrupts
SET_IDT_ENTRY(idt[4], Overflow);
SET_IDT_ENTRY(idt[5], BOUND_Range_Exceeded);
SET_IDT_ENTRY(idt[6], Invalid_Opcode);
SET_IDT_ENTRY(idt[7], Device_Not_Available);
SET_IDT_ENTRY(idt[8], Double_Fault);
SET_IDT_ENTRY(idt[9], Coprocessor_Segment_Overrun);
SET_IDT_ENTRY(idt[10], Invalid_TSS);
SET_IDT_ENTRY(idt[11], Segment_Not_Present);
SET_IDT_ENTRY(idt[12], Stack_Segment_Fault);
SET_IDT_ENTRY(idt[13], General_Protection);
SET_IDT_ENTRY(idt[14], Page_Fault);
SET_IDT_ENTRY(idt[16], x87_FPU_Floating_Point_Error);
SET_IDT_ENTRY(idt[17], Alignment_Check);
SET_IDT_ENTRY(idt[18], Machine_Check);
SET_IDT_ENTRY(idt[19], SIMD_Floating_Point_Exception);		
SET_IDT_ENTRY(idt[SYSCALL_VECTOR],system_call_linkage);
SET_IDT_ENTRY(idt[KEYBOARD],Keyboard_handler);
SET_IDT_ENTRY(idt[RTC],RTC_handler);
SET_IDT_ENTRY(idt[PIT_VECTOR],PIT_handler);
// SET_IDT_ENTRY(idt[PIC_VECTOR],PIC_handler);
lidt(idt_desc_ptr);                                              //load idt
return 0;
}
