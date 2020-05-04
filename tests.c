#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "IDT.h"
#include "rtc.h"
#include "filesys.h"
#include "terminal_driver.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* page_test 1
 * 
 * test whether page is not present or presetn in some palace
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: MAY Will trigger IDT
 * 
 * 
 */

int page_test1(){
	TEST_HEADER;

	int *ptr=(int *)0xc00000;
	int a = *ptr; 
	a++; 

	return PASS;
}
/* page_test 2
 * 
 * test whether page is not present or presetn in some palace
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: MAY Will trigger IDT
 * 
 * 
 */

int page_test2(){
	TEST_HEADER;

	int *ptr=(int *)0x00001;
	int a = *ptr; 
	a++; 

	return PASS;
}
/* page_test 3
 * 
 * test whether page is not present or presetn in some palace
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: MAY Will trigger IDT
 * 
 * 
 */

int page_test3(){
	TEST_HEADER;

	int *ptr=(int *)0x500000;
	int a = *ptr; 
	a++; 

	return PASS;
}

	



/* IDT Test - 1
 * 
 * Asserts 15 IDT entries are NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */

int idt_test_1(){
	TEST_HEADER;
	int result = PASS;
		if ((idt[15].offset_15_00 != NULL) || 
			(idt[15].offset_31_16 != NULL)){
			assertion_failure();
			result = FAIL;
		}
	return result;
}

/* IDT Test - 2
 * 
 * Asserts 10 ~14 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test_2(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 10; i < 15; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* IDT Test - 3
 * 
 * Asserts 16~19 IDT entries are NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test_3(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 16; i < 20; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}



/* Checkpoint 2 tests */

/* RTC Test
 * print number on the screen to show it has the right frequency after open()
 * then write RTC frequenies, and test it with the same method with test open, 
 * the frequency should not greater than 512Hz, or it will not work normally(a TA tells me ). 
 * Inputs: the frequency we want to test
 * Outputs: the time we start the system, it is smaller than 5
 * Side Effects: None
 * Coverage: rtc driver
 * Files: rtc.c/h
 *//*
void rtc_test(){
	TEST_HEADER;
	int time = 0;
	int frequency = 2;
	int count = frequency;
	rtc_open();
	while(time != 4){			//test rtc_open function whether set the frequecny to 2Hz
		rtc_read();				// it will call the read function to test it. so if it works, means read and open 
		count--;				// is ok
		if(count == 0){
			printf("%d\n",time);
			time++;
			count = frequency;
		}
	}
	// int i;
	// for (i=0;i<10000;i++){
	// 	time = 0;
	// 	if (0==rtc_write(i)){			//change the frequency to what we want
	// 		count = i;
	// 		printf("now the accept frequency is %d",i);
	// 		while(time != 4){
	// 			rtc_read();
	// 			count--;
	// 			if(count == 0){
	// 			printf("%d\n",time);
	// 			time++;
	// 			count = i;
	// 			}
	// 		}
	// 	}	
	// }
	
	rtc_close();			//close(), do nothing
	//int i;
	// for (i = 0; i< 9000; i++){
	// 	if (0 == rtc_write(i)){
	// 		cli();
	// 		outb(INDEX_REG_A,RTC_PORT);
	// 		int rate = 0xF&inb(CMOS_PORT);
	// 		printf("frequency is %d and the rate is %d                              \n",i,rate);
	// 	}
	// }

}
*/

/* RTC Test-2
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc driver
 * Files: rtc.c/h
 */

// void filesystem_test(){
// 	dentry_t temp;
// 	read_dentry_by_index(7,&temp);
// 	printf("%s ",temp.name);
// 	printf("%d",temp.inode);
// 	read_dentry_by_index(8,&temp);
// 	printf("%s ",temp.name);
// 	printf("%d",temp.inode);
// 	read_dentry_by_index(9,&temp);
// 	printf("%s ",temp.name);
// 	printf("%d",temp.inode);
// 	read_dentry_by_index(10,&temp);
// 	printf("%s ",temp.name);
// 	printf("%d",temp.inode);
	
// 	//temp->name;
// }

// /* terminal_open_test
//  * Discription: test terminal_open.
//  * Inputs: none.
//  * Outputs: print "terminal_open success" if passed
//  * Side Effects: none.
//  */
// void terminal_open_test(){
// 	if(!terminal_open(0))
// 		printf("terminal_open success\n");
// }

// /* terminal_close_test
//  * Discription: test terminal_close
//  * Inputs: none.
//  * Outputs: print "terminal close success" if passes test.
//  * Side Effects: none.
//  */
// void terminal_close_test(){
// 	if(!terminal_close(0))
// 		printf("terminal_close success\n");
// }
// void filesystem_test1(){
// 	uint8_t buf[1024];
// 	//should file read consider the type
// 	read_data(38,10,buf,200);
// 	clear();
// 	printf("%s",buf);

// 	//temp->name;
	
// }
// void filesystem_test2(){
// 	// inmitate ls
// 	int i;
// 	clear();
// 	int size_of_entries=number_of_entries();
// 	for  (i=0;i<(size_of_entries);i++){
// 		unsigned int j=0;
// 		dentry_t temp;
// 		dir_read((uint8_t*) (&temp),i,sizeof(dentry_t));

// 		printf("file name:");
//    		 for (; j < 32; j++){
// 			//if ((temp.name[j])=='\0'){break;}
//         	printf("%c", temp.name[j]);
// 		}

// 		 printf(" file type: %d",temp.type);
// 		 printf(" file size: %d",size_of_file(temp.inode)); 
//     	 printf("\n");
		 

		
// 		//printf("%d",temp.inode);
// 		//printf("%d",temp.type);


		
// 	}
	

// }
// void filesystem_test3(){
	
// 	//print by name 
	
// 	uint32_t inode=44;
// 	uint8_t buf[5445];
// 	uint32_t offset=0;
// 	char filename[]="pingpong";
// 	uint32_t j=0;
// 	clear();
// 	file_open(&inode,filename);
	
// 	file_read(inode,&offset,buf,5445);		// big question ? how can i know i am doing right 
	
// 	 for (; j < 5445; j++){
// 			//if ((temp.name[j])=='\0'){break;}
//         	printf("%c", buf[j]);
// 		}



// }
// void filesystem_test4(){
	
// 	//print by name 
// 	dentry_t temp;
	
// 	uint8_t buf[6000];
// 	uint32_t offset=0;
// 	uint32_t result;
// 	//char filename[]="verylargetextwithverylongname.tx";
	
// 	clear();
// 	read_dentry_by_index(9,&temp);
// 	result=file_read(temp.inode,&offset,buf,4099);		// big question ? how can i know i am doing right 
// 	printf("%s \n",temp.name);
// //	printf ("%d",result);
// 	printf("%s",buf);



// }

/* terminal_read_write_test
 * Discription: test terminal_read and terminal_write
 * Inputs: none.
 * Outputs: print what we input on the keyboard to terminal twice.
 * Side Effects: none.
 */
// void terminal_read_write_test() {
// 	unsigned int buf[110];
// 	terminal_read(-1, buf, 110);
// 	terminal_write(-1, buf, 110);
// }


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("page_test3", page_test3());
	
	// TEST_OUTPUT("idt_test", idt_test());n
	// // TEST_OUTPUT("idt_test_1", idt_test_1());
	// TEST_OUTPUT("idt_test_2", idt_test_2());
	// TEST_OUTPUT("idt_test_3", idt_test_3());
	// launch your tests here 
	//	
	//rtc_test();
	//terminal_open_test();
	//terminal_close_test();
	//terminal_read_write_test();
	// TEST_OUTPUT("idt_test_3", idt_test_3());en
	// launch your tests here
	//rtc_test();
	//filesystem_test3();
		
	
}



//C:\qemu-1.5.0-win32-sdl\qemu-system-i386w.exe -hda "Z:\mp3_group_48\student-distrib\mp3.img"  -m 256 -gdb tcp:127.0.0.1:1234  -S -name mp3


