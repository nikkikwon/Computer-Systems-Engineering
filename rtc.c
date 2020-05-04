// #include "rtc.h"
#include "i8259.h"
#include "lib.h"
#include "scheduling.h"

volatile int rtc_interupt_occur_t0 = 0;
volatile int rtc_interupt_occur_t1 = 0;
volatile int rtc_interupt_occur_t2 = 0;

/*
*   Function: init_rtc
*   Description: Initializes ports on the RTC, control registers and sets frequency to 2Hz.
*	inputs: void
*   outputs: void
*   effects: enables IRQ and sends value to RTC
*/
void rtc_init(){
    cli();
    char prev;
    outb(INDEX_REG_B, RTC_PORT);		// select register B, and disable NMI
    prev=inb(CMOS_PORT);	// read the current value of register B
    outb(INDEX_REG_B, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ);
    sti();
}





/*
*	Function: rtc_interrupt()
*	Description: RTC interrupt handler, for checkpoint 1, it calls test_interrupts()
*	input: void
*	output: void
*	effects: Gets video memory updated. 
*/


void rtc_interrupt(){
    /* Clear interrupt flag */
    cli();
    rtc_interupt_occur_t0 = 1;
    rtc_interupt_occur_t1 = 1;
    rtc_interupt_occur_t2 = 1;
    outb(INDEX_REG_C, RTC_PORT); // select register C
    inb(CMOS_PORT); // just throw away contents

    //test_interrupts(); //temperory do this, checkpoint1 needed.

    send_eoi(RTC_IRQ);

    /* Set interrupt flag */
    sti();
}


//this is from the wiki.OSDev.org.com as a reference
// rate &= 0x0F;			// rate must be above 2 and not over 15
// disable_ints();
// outportb(0x70, 0x8A);		// set index to register A, disable NMI
// char prev=inportb(0x71);	// get initial value of register A
// outportb(0x70, 0x8A);		// reset index to A
// outportb(0x71, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.
// enable_ints();

/*
*	Function: rtc_open()
*	Description:funciton to open the RTC
*	input: none by checkpoint 2
*	output: returns 0 always
*	effects: Opens the rtc and initializes frequency to 2 HZ
*/
int32_t rtc_open(const uint8_t* filename){
    cli();
    outb(INDEX_REG_A,RTC_PORT);
    char prev = inb(CMOS_PORT);
    outb(INDEX_REG_A,RTC_PORT);
    outb((prev&0xF0)|0xF,CMOS_PORT);           //set the rate to 15, so that the frequency is 2Hz
    sti();
    return 0;
}
/*
 *	Function: rtc_close()
 *	Description: function to close the RTC.
 *	input: none by checkpoint2
 *	output: returns 0 always
 *	effects: closes the RTC
 */
int32_t rtc_close(int32_t fd){
    return 0;
}
/*
 *	Function: rtc_read()
 *	Description: read the contents within the RTC only after an interrupt has occured
 *	input: none by chckpoint2
 *	output: returns 0 upon success
 *	effects: Reads the RTC
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    if (curren_terminal_executing == 0){
        rtc_interupt_occur_t0 = 0;
    }
    else if ( curren_terminal_executing == 1){
        rtc_interupt_occur_t1 = 0;
    }
    else
    {
        rtc_interupt_occur_t2 = 0;
    }
    
    while(1){
        if (curren_terminal_executing == 0){
            if (rtc_interupt_occur_t0 == 1){           //wait until the next interupt by rct
                break;
            }
        }
        else if (curren_terminal_executing == 1){
            if (rtc_interupt_occur_t1 == 1){           //wait until the next interupt by rct
                break;
            }
        }
        else{
            if (rtc_interupt_occur_t2 == 1){           //wait until the next interupt by rct
                break;
            }
        }

    }
    return 0;
}

/*
 *	Function: rtc_write_help
 *	Description: Helper function of the rtc_write
 *	input: frequency = frequency for the RTC
 *	output: -1 = fail, 0 = success
 *	effects: its output will be set as a return value for rtc_write
 */

int32_t rtc_write_help(int32_t frequency){
    int low_bound = 2;
    int high_bound = 0x8000>>2;
    int f;
    int rate=15;

    if (frequency<low_bound || frequency>high_bound){       //if not the frequency can be accept, return failed
        return -1;
    }
    for (f = low_bound; f <= high_bound; f=f<<1){
        if (frequency == f){   
            cli();                 //judge whether it is multiple of two 
            outb(INDEX_REG_A,RTC_PORT);
            char prev = inb(CMOS_PORT);
            outb(INDEX_REG_A,RTC_PORT);
            outb((prev& 0xF0)|rate,CMOS_PORT);    //write rate to the rtc  
            sti();
            return 0;
        }
        rate--;
    }
    return -1;
}
/*
 *	Function: rtc_write()
 *	Description: the function used to change the rtc frequency
 *	input: the frequency we want
 *	output: 0 for success and -1 for fail
 *	effects:change the rtc frequency
 */


int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    int freq;
    /* Boundary check - ONLY 4 Bytes */	
	if (4 != nbytes || (int32_t)buf == NULL){ 
		return -1;  /* Fail - always need to write 4 bytes) */
    }
    else{ 
		freq = *((int32_t*)buf);
    }
    return rtc_write_help(freq);
}
