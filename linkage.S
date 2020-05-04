#define ASM     1

#include "x86_desc.h"
.global system_call_linkage

system_call_linkage: 
    cmpl $1, %eax
  	jl invalid
  	cmpl $10, %eax
  	jg invalid

    pushfl 
    pushl %esi              #save register
	pushl %edi
	pushl %ebp
#argument    
    pushl %edx              #push argument
	pushl %ecx
	pushl %ebx
    call *system_call_jump_table(,%eax,4)
    popl %ebx               #pop argument
	popl %ecx
	popl %edx

    popl %ebp               #restore register value
	popl %edi
	popl %esi
    popfl
    jmp done



invalid:
 	movl $-1, %eax

done:
    iret


system_call_jump_table:
	.long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn







