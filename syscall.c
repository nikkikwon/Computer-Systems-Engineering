#include "syscall.h"
#include "terminal_driver.h"






/*Initialized file operations table here*/

int32_t current_process_number=-1;
//uint8_t process[6];
file_operations_table std_in_function = {terminal_read, do_nothing, terminal_open, terminal_close};
file_operations_table std_out_function = {do_nothing, terminal_write, terminal_open, terminal_close};
file_operations_table rtc_function = {rtc_read, rtc_write, rtc_open, rtc_close};
file_operations_table dir_function = {dir_read, dir_write, dir_open, dir_close};
file_operations_table file_function = {file_read, file_write, file_open, file_close};
file_operations_table do_nothing_function = {do_nothing, do_nothing, do_nothing, do_nothing};
/* 
*	Function halt
*	Description: end a process and return to its parent process
*	input: 	status = value which gets returned to the parent process
*	output: return status
*	effect: ending a current process
*/
int halt (uint8_t status){
    uint8_t index_1=terminal[curren_terminal_executing].index;
    uint8_t halt_process=terminal[curren_terminal_executing].process_in_terminal[index_1];
    pcb_t* pcb_now = get_pcb_by_process_number(halt_process);
    pcb_t* parent_pcb =  get_pcb_by_process_number(pcb_now->parent_process_number);
    int i;
    for(i = 0; i < 8; i++){
        if(pcb_now->file_descriptor_table[i].flags == ACTIVE){
            close(i);
            pcb_now->file_descriptor_table[i].flags = NOT_ACTIVE;
            pcb_now->file_descriptor_table[i].fops_table_ptr = &do_nothing_function; 
        }
    }
    //process_id_array[pcb_now->process_number] = 0;
    terminal[curren_terminal_executing].index=terminal[curren_terminal_executing].index-1;
    if (index_1 == 0){
        process_id_array[halt_process]=0;
        printf("the last process, can not be halt\n");
        execute((uint8_t *)"shell");   
        }
    if(index_1 > 0){
        process_id_array[halt_process]=0;
       
    }
  
    
    ////////////////////////////////////////////////////////////////
    repage(_128_MB, _8MB+ parent_pcb->process_number * _4MB);
    ////////////////////////////////////////////////////////////////
    tss.esp0 = pcb_now->parent_ksp;

    asm volatile(
				 ""
                 "mov %0, %%eax;"
                 "mov %1, %%esp;"
                 "mov %2, %%ebp;"
                 "jmp RETURN_FROM_HALT;"
                 :                      /* no outputs */
                 :"r"((uint32_t)status), "r"(pcb_now->parent_ksp), "r"(pcb_now->parent_kbp)   /* inputs */
                 :"%eax"                 /* clobbered registers */
                 );
    /* Should never reach this point due to iret *
   	 * ----------------------------------------- */
	return 0;
    

}



/*execute
  system call funciton execute, execute a executable file in the filesystem 
  input file name
  output -1 if the command cannot be execute
            256 if the program dies by an exception
            0~255 if the program execute a halt system call

*/
int execute (const uint8_t * command){
    cli();
    
int32_t next_process_number;
//int32_t current_number;
dentry_t temp;
int i;
int j;
uint8_t parse_command[10];
uint8_t parse_arg[ARGSIZE];
uint8_t command_start;
uint8_t whether_arg;
uint8_t arg_start;
uint8_t arg_end;
uint8_t command_end;
uint8_t command_len;
uint8_t arg_len;
uint8_t buf[4];
uint32_t entry_point;

/**********************
 * 1: parse command   *
 **********************/

command_len=0;
command_start = 0;
arg_start=0;
arg_len=0;
if (command==NULL){
    return -1;              // nmsl 
}
while(command[command_start] == ' '){ //spaces before the command
    command_start += 1;
}
command_end = command_start;

while(command[command_end] != '\0' && command[command_end] != '\n' && command[command_end] != ' ' ){
    
    command_end += 1;
    command_len++;
}
if (command[command_end]==' '){
    whether_arg=1;          // if after cmd is a blank, there is chance that it have some arg
}
if (command_len>9){return -1;}

for(i = command_start; i < command_end; i++){
    parse_command[i - command_start] = command[i];
}

parse_command[command_len] = '\0'; //now parse_command is the string of the command

// arg part
if (whether_arg){
    arg_start=command_end;
    while(command[arg_start] == ' '){ //spaces before the command
        arg_start += 1;
    }
    arg_end=arg_start;
    while(command[arg_end] != '\0' && command[arg_end] != '\n' && command[arg_end] != ' ' ){
        
        arg_end += 1;
        arg_len++;

    }
    if (arg_len>ARGSIZE){return -1;}       // no such arg
    for(i = arg_start; i < arg_end; i++){
        parse_arg[i - arg_start] = command[i];
    }
    parse_arg[arg_len]='\0';

}


/**********************
 * 2:check executable *
 **********************/

if(0 != read_dentry_by_name((uint8_t*)parse_command, &temp)){
    return -1;//read this file failed.
}
//checking whether it is executable
read_data(temp.inode, 0, buf, 4);
if(buf[0] != 0x7F){return -1;} //is it 'delete'
if(buf[1] != 0x45){return -1;} //is it 'E'
if(buf[2] != 0x4C){return -1;} //is it 'L'
if(buf[3] != 0x46){return -1;} //is it 'F'

read_data(temp.inode, 24, buf, 4);
entry_point = *((uint32_t*)buf);

/**********************
 *     3: paging      *
 **********************/
// map the virtrual address 128MB~132MB to physical address based on the process number
next_process_number=-1;
for (j=0;j<MAX_PROCESSES;j++){
    if(process_id_array[j]==0){
        next_process_number=j;
        process_id_array[j]=1;  
        terminal[curren_terminal_executing].index++;
        terminal[curren_terminal_executing].process_in_terminal[terminal[curren_terminal_executing].index] = next_process_number;     
        break; 
    }

}
if (next_process_number==-1){
    printf("no more proccess\n");
    
    return -1   ;
}


// next_process_number=++current_process_number;
// if (next_process_number>=6){
//     current_process_number--;
//     printf("no more proccess\n");
//     return -1;
// }
repage(_128_MB,_8_MB+next_process_number*_4_MB);        //next_process_number should start with 0


/**********************
 *   4:program loader *
 **********************/

read_data(temp.inode,0,(uint8_t*)location_for_file,file_max_size);

/**********************
 *   5:create pcb *
 **********************/
//current_number = terminal[curren_terminal_executing].process_in_terminal[ terminal[curren_terminal_executing].index ];
pcb_t*  next_pcb=(pcb_t*)(_8_MB-(_8_KB)*(next_process_number+1)); // the location of the pcb
//pcb_t*  current_pcb=(pcb_t*)(_8_MB-(_8_KB)*(current_number+1)); // the location of the pcb

// Save the next ESP and EBP into the PCB  
	asm volatile(
				"movl %%ebp, %%eax; "
				"movl %%esp, %%ebx; "
			
			:"=a"(next_pcb->parent_kbp), "=b"(next_pcb->parent_ksp));

// Save the current ESP and EBP into the PCB  
//	asm volatile(
//				"movl %%ebp, %%eax; "
//				"movl %%esp, %%ebx; "
//			
//			:"=a"(current_pcb->ebp), "=b"(current_pcb->esp));




//pcb_t*  parent_pcb;
next_pcb->process_number=next_process_number;

if ((next_process_number==0) || (next_process_number==1) || (next_process_number==2)){
    next_pcb->parent_process_number=next_process_number;    //set to itself
    
}
else{
    //next_pcb->parent_process_number=current_process_number-1;

    //in this case, 'next_process_number' is not 0,1,2. In Other Words, index is not 0.
    next_pcb->parent_process_number = terminal[curren_terminal_executing].process_in_terminal[ terminal[curren_terminal_executing].index-1 ];
}

for (i=0;i<MAXFILE;i++){
    /*set flags to zero*/
    next_pcb->file_descriptor_table[i].flags=0;
    next_pcb->arg[0]='\0';
}
if (whether_arg){
    memcpy(next_pcb->arg,parse_arg,ARGSIZE);    
}



next_pcb->file_descriptor_table[0].fops_table_ptr=&(std_in_function);
next_pcb->file_descriptor_table[0].flags=1;
next_pcb->file_descriptor_table[1].fops_table_ptr=&(std_out_function);
next_pcb->file_descriptor_table[1].flags=1;


/**********************
 * 6 : context switch *
 **********************/
 tss.ss0 = KERNEL_DS; // in fact it is set in kernel.c
 tss.esp0 = _8_MB - (_8_KB * next_process_number) - 4;
 sti();
 


asm volatile(
                 "cli;"
                 "mov $0x2B, %%ax;"
                 "mov %%ax, %%ds;"
                 "movl $0x83FFFFC, %%eax;"
                 "pushl $0x2B;"
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;"
                 "pushl %%edx;"
                 "pushl $0x23;"
                 "pushl %0;"
                 "iret;"
                 "RETURN_FROM_HALT:;"
                 "LEAVE;"
                 "RET;"
                 :	/* no outputs */
                 :"r"(entry_point)	/* input */
                 :"%edx","%eax"	/* clobbered register */
                 );

//should reach this line
return 0;
}

/* 
*	Function read
*	Description: Reads file specified into the buffer
*	input: fd = file descriptor idx, buf = buffer, nbytes = number of bytes
*	output: -1 if read fails, else return value from file read
*	effect: File gets read and put into the buffer
*/

/* 
*	Function read
*	Description: Reads file specified into the buffer
*	input: fd = file descriptor idx, buf = buffer, nbytes = number of bytes
*	output: -1 if read fails, else return value from file read
*	effect: File gets read and put into the buffer
*/
int read (int32_t fd, void * buf, int32_t nbytes){

    pcb_t *pcbptr = get_pcbptr();
   
    if(fd < 0 || fd > MAX_FILE_DESCRIPTOR)
    {
        return -1;
    }

    //0 means it is not being used. 
    if(pcbptr -> file_descriptor_table[fd].flags == 0 || buf == NULL) 
    {
        return -1;
    } 
    //////////-> read parameter (*void)
    return pcbptr -> file_descriptor_table[fd].fops_table_ptr -> read(fd, (void*)buf, nbytes);   
}


/* 
*	Function write
*	Description: Determine which write function to call depending on the file type
*	input: fd = file descriptor idx, buf = buffer, nbytes = number of bytes
*	output: -1 if read fails, else return value from file write
*	effect: Appropriate write function to the file assigned
*/
int write (int32_t fd , const void* buf, int32_t nbytes){
    pcb_t *pcbptr = get_pcbptr();

    if(fd < 0 || fd > MAX_FILE_DESCRIPTOR)
    {
        return -1;
    }
    //0 means not being used
    if(pcbptr -> file_descriptor_table[fd].flags == 0 || buf == NULL)
        return -1;
    /////////->write parameter (*void)
    return pcbptr -> file_descriptor_table[fd].fops_table_ptr -> write(fd, (void*)buf, nbytes);   
}
/* 
*	Function open()
*	Description: opens a file into the current pcb
*	input: filename = file name associated with open
*	output: -1 if open fails, else index in file descriptor array
*	effect: Opens the file
*/

/* 
*	Function open()
*	Description: opens a file into the current pcb
*	input: filename = file name associated with open
*	output: -1 if open fails, else index in file descriptor array
*	effect: Opens the file
*/
int open (const uint8_t* filename){
    uint16_t file_des_idx;
    pcb_t *pcbptr = get_pcbptr();
    dentry_t dir_entries;

    if(read_dentry_by_name(filename, &dir_entries) == -1)
        return -1;

    file_des_idx = MIN_FILE_DESCRIPTOR;

    while(file_des_idx <= MAX_FILE_DESCRIPTOR)
    {
        //0 means not being used
        if(pcbptr -> file_descriptor_table[file_des_idx].flags == 0)
        {
            pcbptr -> file_descriptor_table[file_des_idx].flags = 1;
            //0x0000, starting position of the file
            pcbptr -> file_descriptor_table[file_des_idx].file_position = 0x0000;
            break;
        }
        else if(file_des_idx == MAX_FILE_DESCRIPTOR)
        {
            return -1;
        }
        file_des_idx++;
    }

    switch(dir_entries.type)
    {
        case TYPE_DIR:
            if(dir_open(filename) != 0)
                return -1;
            else
            {   
                pcbptr -> file_descriptor_table[file_des_idx].inode = NULL;
                pcbptr -> file_descriptor_table[file_des_idx].fops_table_ptr = &dir_function;
                break;
            }
        case TYPE_RTC:
            if(rtc_open(filename) != 0)
                return -1;
            else
            {
                pcbptr -> file_descriptor_table[file_des_idx].inode = NULL;
                pcbptr -> file_descriptor_table[file_des_idx].fops_table_ptr = &rtc_function;
                break;
            }
        case TYPE_FILE:
        if(file_open(filename) != 0)
            return -1;
        else
        {
            pcbptr -> file_descriptor_table[file_des_idx].inode = dir_entries.inode;
            pcbptr -> file_descriptor_table[file_des_idx].fops_table_ptr = &file_function;
            break;
        }

    }
    return file_des_idx;
}
/* 
*	Function close()
*	Description: File specficied by the fd gets closed
*	input: fd= file descriptor index
*	output: -1 if close fails, 0 otherwise
*	effect: Closes a file
*/

/* 
*	Function close()
*	Description: File specficied by the fd gets closed
*	input: fd= file descriptor index
*	output: -1 if close fails, 0 otherwise
*	effect: Closes a file
*/
int close(int32_t fd){
    pcb_t *pcbptr = get_pcbptr();
    
    if(fd < MIN_FILE_DESCRIPTOR || fd > MAX_FILE_DESCRIPTOR)
       return -1;

    //0 means not being used
    if(pcbptr -> file_descriptor_table[fd].flags == 0)
        return -1;

    pcbptr -> file_descriptor_table[fd].flags = 0;

    if(pcbptr -> file_descriptor_table[fd].fops_table_ptr -> close(fd) != 0)
        return -1;

    return 0;
}
/* function getargs 
    reads the program's command line arguments into a user-level buffer
    these arguments is stored in the pcb
    return -1 :if there are no arguments,or if the arguments and a terminal NULL do not fit in the buffer 
    

*/
int32_t getargs (uint8_t* buf, int32_t nbytes){
    pcb_t *pcbptr = get_pcbptr();
    if (buf==NULL){
        return -1;
        }
    if (pcbptr->arg[0]=='\0'){
        return -1;
    }
    
    memcpy(buf,pcbptr->arg,nbytes);
    return 0;
}
/*vid map

call maps the text-mode video memory into user space at a pre-set virtual address
Although the address returned is always the same
return 0 if work
return -1 if the adrress passed in is not vaild
*/

int32_t vidmap (uint8_t** screen_start){
    if (screen_start==NULL){return -1;}
    if ((int32_t)screen_start>=_4_MB &&(int32_t)screen_start<=_8MB){return -1;}
    revidpage();
    *screen_start=(uint8_t*)_12_MB;
    return 0;
}

/* Set Handler System Call */
int32_t set_handler (int32_t signum, void* handler_address){
    return -1;
}

/* Sigreturn System Call */
int32_t sigreturn (void){
    return -1;
}

int do_nothing(){
    return -1;
}
/* 
*	Function: get_pcb_by_process_number
*	Description: gets pcb with the process number
*	input: process = process number
*	output: returns pointer to the pcb
*	effect: none
*/
pcb_t * get_pcb_by_process_number(uint32_t process)
{
	return (pcb_t *)(_8MB - (process + 1) * _8KB);
}

/*
*	Function get_pcbptr
*	Description: gets a pointer to the current pcb
*	input: void
*	output: pcb pointer
*	effect: none
*/
pcb_t* get_pcbptr(){
    pcb_t* p;
    asm volatile("				   \n\
				andl %%esp, %%eax  \n\
				"
				:"=a"(p)
				:"a"(PCBPTR_MASK)
				:"cc"
				);
	return p;
}



