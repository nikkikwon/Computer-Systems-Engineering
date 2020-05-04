#include "page.h"
#define VIDEO       184
#define FOUR_MB 	0x400000


/*
initiliaze the page in 
the memory
input :none
output:none
SIDE EFFECT:NO
*/

void initial_page_sturcture(){
	int i;
	for (i=0;i<PAGE_SIZE;i++){
            page_directory[i]=0x2;    // the last bit is 0,which means not present, the bit 1 is 1 which means we can read and write
            page_table0[i]=0x2;       // the last bit is 0,which means not present, the bit 1 is 1 which means we can read and write
			page_table1[i]=0x2;       // the last bit is 0,which means not present, the bit 1 is 1 which means we can read and write
			page_directory[i]=page_directory[i]|(FOUR_MB*i);
			page_table0[i]=page_table0[i]|(FOUR_KB*i);				// map the linear address to physical address 
        }

}

/*
initiliaze the page in 
the memory
input :none
output:none
SIDE EFFECT:NO
*/
void initial_page(){
    initial_page_sturcture();
        
    page_directory[1]=0x01;
    page_directory[1]=page_directory[1]|(0x400000)|(0x80);         //address equal to 4MB, page_size set to 1;
    page_table0[VIDEO]=0x3| (0xB8000);                           // set vedio memory 
	page_table0[VIDEO-1]=0x3| (0xB8000);                           // set vedio memory_always point to the b8000
	page_table0[VIDEO+1]=0x3| (0xB9000);                           // set vedio_buffer1
	page_table0[VIDEO+2]=0x3| (0xBA000);                           // set vedio_buffer2
	page_table0[VIDEO+3]=0x3| (0xBB000);                           // set vedio_buffer3
		
    page_directory[0]=((unsigned int)page_table0)|0x01;                        // set the first entry of directory be the address of first page table  

    asm volatile("mov %0, %%eax;"					
				"mov %%eax, %%cr3;"					//put the address of pdt to cr3
	
				"mov %%cr4, %%eax;"
				"or $0x00000010, %%eax;"
				"mov %%eax, %%cr4;"					//enable 4mb paging  
	
				"mov %%cr0, %%eax;"
				"or $0x80000000, %%eax;"
				"mov %%eax, %%cr0;"					//enable paging 
				:
				:"r" (page_directory)
				:"eax", "memory"
				);



}

/* 
	remap the page based on the input
	input virtual_address
		  physical_address should be the mutiple of 4mb
	output 1 for success 0 for fail


*/
int  repage(uint32_t virtual_address,uint32_t physical_address){

      // based on the virtual_address get the page_dir entry
	uint32_t test=physical_address % FOUR_MB;
	uint32_t index= virtual_address / FOUR_MB;
	if (test != 0){return -1;}
	page_directory[index]=0x07|physical_address|0x80;	// set present and address equal to physical_address
	
	asm volatile("mov %%cr3,%%eax;"
				"mov %%eax,%%cr3;"
				:
				://"r" (page_directory)		/*inputs*/
				:"eax"  //,"memory"
				);
	return 1;
}


/* 
	repage the video memory to a space that >8mb <128MB 
	input NULL
		  
	output 1 for success -1 for fail


*/
int  revidpage(){

      // based on the virtual_address get the page_dir entry
	page_table1[0]=0x7| (0xB8000);                           // set vedio memory
	page_directory[3]=((unsigned int)page_table1)|0x07;  
	
	
	// fresh tlb
	asm volatile("mov %%cr3,%%eax;"
				"mov %%eax,%%cr3;"
				:
				://"r" (page_directory)		/*inputs*/
				:"eax"  //,"memory"
				);
	return 1;
}

/* 
	repage the video memory to a space that >8mb <128MB 
	input NULL
		  
	output 1 for success -1 for fail


*/
/*
input equal to 0 video_memory page to video_memory
input bigger than 1 , vedio_memory page to corresponding_buffer
*/
int revideomemorypage(uint8_t input){
	if (input==0){
		page_table0[VIDEO]=0x3| (0xB8000);
	}else
	{
		page_table0[VIDEO]=0x3| (0xB8000+input*0x1000);
	}
	

	// fresh tlb
	asm volatile("mov %%cr3,%%eax;"
				"mov %%eax,%%cr3;"
				:
				://"r" (page_directory)		/*inputs*/
				:"eax"  //,"memory"
				);
	return 1;


}

/*
input equal to 0 video_memory page to video_memory
input bigger than 1 , vedio_memory page to corresponding_buffer
*/
int  re_user_vidpage(uint8_t input){

      // based on the virtual_address get the page_dir entry

	if (input==0){
		page_table1[0]=0x7| (0xB8000);				
	}else
	{
		page_table1[0]=0x7| (0xB8000+input*0x1000);
	}
	
	// fresh tlb
	asm volatile("mov %%cr3,%%eax;"
				"mov %%eax,%%cr3;"
				:
				://"r" (page_directory)		/*inputs*/
				:"eax"  //,"memory"
				);
	return 1;
}


