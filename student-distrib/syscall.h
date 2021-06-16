#ifndef SYSCALL_H
#define SYSCALL_H

#include "filesys.h"
#include "paging.h"
#include "keyboard.h"
#include "lib.h"
#include "rtc.h"

#define KEYBOARD_BUF_SIZE 128
#define USER_ADD 0x08048000
#define KSTACK_BASE_ADDR  0x800000               // translates to 8 MB
#define USER_STACK_SIZE   0x2000                 // translates to 8 kB 

typedef struct terminal_t{
    int8_t processes[4];
    int32_t active_process;
    int screen_x;
    int screen_y;
    uint8_t keyboard_buf[KEYBOARD_BUF_SIZE];
    //int keyboard_buffer_top;
    uint8_t terminal_flag;
    volatile int buffer_enable;
} terminal_t;

volatile terminal_t terminals[3]; //3: max number of terminals 
volatile int32_t running_terminal;
volatile int32_t next_term;
volatile int32_t active_terminal;


void page_switch(int32_t pross_id);
void display_switch(uint32_t term_id);
void process_init();
void terminal_active(uint32_t term_id);
pcb_t* get_pcb_two ();

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

//helper functions
void tables_init();
pcb_t * get_pcb();
int parse(uint8_t *cmds, const uint8_t* command, dentry_t *cmds_dentry, int32_t *cmds_inode, int *parse_flag);
void create_paging(int process_num);
void init_pcbstate();

int32_t ret_active_proc();
int32_t ret_active_term();
#endif /* SYSCALL_H */
