#ifndef _RTC_H
#define _RTC_H
#include "types.h"
#include "scheduling.h"


#define RTC_PORT                0x70
#define CMOS_PORT               0x71
#define INDEX_REG_A             0x8A
#define INDEX_REG_B             0x8B
#define INDEX_REG_C             0x8C

#define RTC_IRQ           8




extern void rtc_init();
extern void rtc_interrupt();
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void *buf , int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_write_help(int32_t frequencyc);



#endif

