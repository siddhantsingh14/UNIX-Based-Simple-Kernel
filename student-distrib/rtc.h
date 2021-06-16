#ifndef _RTC_H
#define _RTC_H


//#include "lib.h"
#include "i8259.h"
//#include "x86_desc.h"
#include "idt.h"
/* Referenced https://wiki.osdev.org/RTC */

#define REG_A  0x8A
#define REG_B  0x8B
#define COMMAND_PORT 0x70
#define RTC_PORT 0x71

//checkpoint1 functions
void init_rtc();
void rtc_handler();
 
//rtc driver functions
int32_t open_rtc(const uint8_t *filename);
int32_t close_rtc(int32_t fd);
int32_t read_rtc(int32_t fd, void *buffer, int32_t num);
int32_t write_rtc(int32_t fd, const void *buffer, int32_t num);
int8_t set_freq_rtc(int32_t curr_freq);
int8_t get_freq();
#endif
