#ifndef _PAGE_H
#define _PAGE_H
#include "x86_desc.h"


#include "types.h"
#define FOUR_KB     4096
#define PAGE_SIZE   1024    
uint32_t page_directory[PAGE_SIZE] __attribute__((aligned (FOUR_KB)));
uint32_t page_table0[PAGE_SIZE] __attribute__((aligned (FOUR_KB)));
uint32_t page_table1[PAGE_SIZE] __attribute__((aligned (FOUR_KB)));
void  initial_page();
void  initial_page();
int  repage(uint32_t virtual_address,uint32_t physical_address);
int  revidpage();
int revideomemorypage(uint8_t input);
int  re_user_vidpage(uint8_t input);



#endif

