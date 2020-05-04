
#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H
#include "lib.h"
#include "types.h"
#include "terminal_driver.h"
#include "rtc.h"
#include "filesys.h"
#include "types.h"
#include "page.h"
#include "x86_desc.h"
#include "scheduling.h"
#define ACTIVE 0x0001
#define NOT_ACTIVE 0x0000
#define MAXFILE 8
#define MAX_FILE_DESCRIPTOR 7
#define MIN_FILE_DESCRIPTOR 2
#define MAX_PROCESSES 6
#define MAX_FILE_NAME_SIZE 32
#define MAX_COMMAND_SIZE 10
#define MAX_BUFFER_SIZE 100
#define _128_MB 	0x8000000
#define _8_MB 	0x800000
#define _4_MB 	0x400000
#define _8_KB 	0x2000
#define location_for_file 0x8048000
#define file_max_size 0x3ff000

#define PCBPTR_MASK 0xFFFFE000

#define TYPE_RTC 	0
#define TYPE_DIR 	1
#define TYPE_FILE 	2
#define ARGSIZE 120
#define _12_MB 0xc00000

/*** Struct: file_operations_table

***/
 typedef struct {
	 int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
	 int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
	 int32_t (*open)(const uint8_t* filename);
	 int32_t (*close)(int32_t fd);
 } file_operations_table;
 
 
/*** Struct: file_desc_t

***/ 
typedef struct { 
	file_operations_table* fops_table_ptr; 
	int32_t inode; 
	uint32_t file_position; 
	int32_t flags; 
} file_descriptor;

/*** Struct: pcb_t

***/ 
typedef struct { 
	file_descriptor file_descriptor_table[MAXFILE]; 
	uint32_t parent_ksp; 
	uint32_t parent_kbp; 
	uint8_t process_number; 
	uint8_t parent_process_number; 
    uint32_t esp;
    uint32_t ebp;
	uint8_t  arg[ARGSIZE];
 } pcb_t; 
 
uint8_t process_id_array[MAX_PROCESSES];
int process_number_now;
/*System Call   Halt */
int32_t halt (uint8_t status);

/* System Call Execute  */
int32_t execute (const uint8_t* command);

/* System Call Read  */
int32_t read (int32_t fd, void* buf, int32_t  nbytes);

/* System Call Write  */
int32_t write (int32_t fd, const void* buf, int32_t nbytes);

/* System Call Open */
int32_t open (const uint8_t* filename);

/* System Call Close */
int32_t close (int32_t fd);


/* System Call Sigreturn  */
int32_t sigreturn (void);

/*  System Call Getargs */
int32_t getargs (uint8_t* buf, int32_t nbytes);

/*  System Call Vidmap */
int32_t vidmap (uint8_t** screen_start);

/*  Handler System Call  Set*/
int32_t set_handler (int32_t signum, void* handler_address);





pcb_t * get_pcb_by_process_number(uint32_t process);

int do_nothing();

pcb_t* get_pcbptr();






#endif
