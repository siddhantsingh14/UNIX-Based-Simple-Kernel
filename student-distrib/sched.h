#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "x86_desc.h"
#include "syscall.h"

#define FREQ        1193182
#define CHANNEL_0   0x40
#define MASK        0xFF
#define COMMAND_REG 0X43
#define PIT_IRQ     0X0
#define PIT_PORT    0x36
#define page_8KB 0x2000


extern void pit_init();
extern void pit_handler();
void scheduler();
void context_switch();

#endif
