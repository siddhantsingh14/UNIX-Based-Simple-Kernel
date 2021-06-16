#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "x86_desc.h"
//#include "lib.h"

#define bitmask_20 0xfffff000
#define bitmask_10 0xffc00000
#define kernel_start_addr 0x400000
#define PD_NUM_ENTRIES 1024
#define PT_NUM_ENTRIES 1024
#define SIZE_4KB 4096
#define SIZE_4MB 0x400000
#define SIZE_8MB 0x800000
#define SIZE_128MB 0x8000000
#define SIZE_132MB 0x8400000
#define USER_PT_LOCATION 0X20
#define cr4_page_size_enable 0x00000080
#define cr4_mixed_pages_enable 0x00000010
#define cr0_page_enable 0x80000000
#define VIDEO_MEMORY 0xB8000
#define TERMINAL_SIZE 0x1000
#define VIDEO_MEMORY_2 0xB9000
#define TERMINAL_MEMORY 0xB7000
#define VIDEO_MEMORY_3 0xB8



/***************** declaring functions ***************/
void init_paging ();

/***************** declaring struct *****************/
typedef struct pd_entries {
      uint32_t entries[PD_NUM_ENTRIES];
} pd_entries_t;


#endif
