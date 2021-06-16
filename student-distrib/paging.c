#include "paging.h"

/* init_paging()
 *
 * Description: initialize the page directory & page table according to documentation (one for video memory and one for kernel)
 * Inputs: None
 * outputs: initializes the page directory and table
 */
void
init_paging () {
    //initialize page table
    int i;
    for (i = 0; i < PT_NUM_ENTRIES; i++) {
        //table at index 0 is initialized to not present
        if( i >= (VIDEO_MEMORY/SIZE_4KB) || i <= (VIDEO_MEMORY/SIZE_4KB) + 3) { //3: number of terminals 
        page_table[i].present = 1;
        page_table[i].global_page = 1;
        //12: bt shift for 20 MSBs 
        //allocate video memory for multiple terminals 
        page_table[i].bit_20_addr = (VIDEO_MEMORY>>12)+ (i - (VIDEO_MEMORY/SIZE_4KB));
        }
        else {
        page_table[i].present = 0;
        page_table[i].global_page = 0;
        page_table[i].bit_20_addr = i*(SIZE_4KB>>12);
        }
        page_table[i].read_write = 1;
        page_table[i].user_supervisor = 0;
        page_table[i].write_through = 0;
        page_table[i].cache_disabled = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].attribute_index = 0;
        page_table[i].avail = 0;
    }

    //initialize page directory for the 4mb for the 4kb pages, this is where we have virtual memory
    page_directory[0].directory_entry_4kb.present = 1;
    page_directory[0].directory_entry_4kb.read_write = 1;
    page_directory[0].directory_entry_4kb.user_supervisor = 0;
    page_directory[0].directory_entry_4kb.write_through = 0;
    page_directory[0].directory_entry_4kb.cache_disabled = 1;
    page_directory[0].directory_entry_4kb.accessed = 0;
    page_directory[0].directory_entry_4kb.reserved = 0;
    page_directory[0].directory_entry_4kb.page_size = 0;
    page_directory[0].directory_entry_4kb.global_page = 1;
    page_directory[0].directory_entry_4kb.avail = 0;
    page_directory[0].directory_entry_4kb.bit_20_addr = (uint32_t) page_table >> 12; //12 because we are only accessing the first 20 bits

    //initialize page directory for the 4mb kernel
    page_directory[1].directory_entry_4mb.present = 1;
    page_directory[1].directory_entry_4mb.read_write = 1;
    page_directory[1].directory_entry_4mb.user_supervisor = 0;
    page_directory[1].directory_entry_4mb.write_through = 0;
    page_directory[1].directory_entry_4mb.cache_disabled = 0;
    page_directory[1].directory_entry_4mb.accessed = 0;
    page_directory[1].directory_entry_4mb.dirty = 0;
    page_directory[1].directory_entry_4mb.page_size = 1;
    page_directory[1].directory_entry_4mb.global_page = 0;
    page_directory[1].directory_entry_4mb.avail = 0;
    page_directory[1].directory_entry_4mb.attribute_index = 0;
    page_directory[1].directory_entry_4mb.reserved = 0;
    page_directory[1].directory_entry_4mb.bit_10_addr = SIZE_4MB >> 22; //22: bit shift for 10 MSBs
    

    //rest of page directories aren't present in physical memory, so loop through from index 2 till 1024 and set bit 0 (present) as 0
    for(i = 2; i < PD_NUM_ENTRIES; i++) {
        //no need for if statement since starting at i = 2
        page_directory[i].directory_entry_4mb.present = 0;
        page_directory[i].directory_entry_4mb.read_write = 1;
        page_directory[i].directory_entry_4mb.user_supervisor = 0;
        page_directory[i].directory_entry_4mb.write_through = 0;
        page_directory[i].directory_entry_4mb.cache_disabled = 0;
        page_directory[i].directory_entry_4mb.accessed = 0;
        page_directory[i].directory_entry_4mb.dirty = 0;
        page_directory[i].directory_entry_4mb.page_size = 0;
        page_directory[i].directory_entry_4mb.global_page = 0;
        page_directory[i].directory_entry_4mb.avail = 0;
        page_directory[i].directory_entry_4mb.attribute_index = 0;
        page_directory[i].directory_entry_4mb.reserved = 0;
        page_directory[i].directory_entry_4mb.bit_10_addr = i*(SIZE_4MB>>22); //22: bit shift for 10 MSBs
    }

    //assembly function to set the control registers cr0, cr3, and cr4 that enable the appropriate bits for paging
    asm (
        "movl $page_directory, %%eax;       "
        "movl %%eax, %%cr3;                 "
        "movl %%cr4, %%eax;                 "
        "orl $0x00000010, %%eax;            "
        "movl %%eax, %%cr4;                 "
        "movl %%cr0, %%eax;                 "
        "orl $0x80000000, %%eax;            "
        "movl %%eax, %%cr0                  "
        : : : "eax", "cc" );

        return;
}
