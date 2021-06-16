#include "syscall.h"

#define FAILURE -1
#define KB_8_MASK 0xFFFFE000
#define page8MB 0x800000
#define page8KB 0x2000
#define ONEKB   1024
#define KERNEL_DS_ 0x0018
#define FAILURE -1
#define SUCCESS 0
#define NOT_USED 0
#define USED 1

#define RTC_FILETYPE 0
#define DIR_FILETYPE 1
#define REG_FILETYPE 2

uint32_t pcb_state[6] = {0, 0, 0, 0, 0, 0}; //maximum of 6 concurrent tasks at once
int8_t terminal_shell[3] = {-1, -1, -1}; 
int isshell;
static uint32_t vidmap_pt[ONEKB] __attribute__((aligned (SIZE_4KB)));

int32_t display_terminal = 0;

int32_t active_proc = -1;

int32_t temp_fd;

//jump tables
file_operations_t files_table;
file_operations_t directory_table;
file_operations_t rtc_table;
file_operations_t stdin_table;
file_operations_t stdout_table;

/*
 * tables_init
 * DESCRIPTION: initializes jump tables to appropriate functions
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALS: none
 */
void tables_init()
{
    //functions initializes jump tables
    files_table.open = (void *)file_open;
    files_table.close = (void *)file_close;
    files_table.read = (void *)file_read;
    files_table.write = (void *)file_write;

    directory_table.open = (void *)directory_open;
    directory_table.close = (void *)directory_close;
    directory_table.read = (void *)directory_read;
    directory_table.write = (void *)directory_write;

    rtc_table.open = (void *)open_rtc;
    rtc_table.close = (void *)close_rtc;
    rtc_table.read = (void *)read_rtc;
    rtc_table.write = (void *)write_rtc;

    stdin_table.open = (void *)write_failure;
    stdin_table.close = (void *)write_failure;
    stdin_table.read = (void *)read_terminal;
    stdin_table.write = (void *)write_failure;

    stdout_table.open = (void *)read_failure;
    stdout_table.close = (void *)read_failure;
    stdout_table.read = (void *)read_failure;
    stdout_table.write = (void *)write_terminal;
}


int32_t ret_active_proc(){
    return active_proc;
}
int32_t ret_active_term(){
    return active_terminal;
}
/*
 * read
 * DESCRIPTION: reads data from keyboard, file, rtc or directory
 * INPUTS: fd -- index into open files for a task
 *         buf -- what to read
 *         nbytes -- how much to read
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t read(int32_t fd, void *buf, int32_t nbytes)
{
    //if file descriptor is out of bounds or buf is null, return failure
    //0: min fd num and 7: max fd num
    sti();
    pcb_t *curr_pcb = get_pcb();

    if (fd < 0 || fd > 7 || buf == NULL || fd == 1) {
        return FAILURE;
    }
    else if(curr_pcb->fds[fd].flags == 0) {
        return FAILURE;
    }

    return curr_pcb->fds[fd].fops_ptr->read(fd, buf, nbytes);
}

/*
 * write
 * DESCRIPTION: writes data to terminal or device (RTC)
 * INPUTS: fd -- index into open files for a task
 *         buf -- what to write
 *         nbytes -- how much to write
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t write(int32_t fd, const void *buf, int32_t nbytes)
{
    pcb_t *curr_pcb = get_pcb(); // get PCB for current process

    //if file descriptor is out of bounds or buf is null, return failure
    if (fd < 0 || fd > 7 || buf == NULL) { //0: min fd num and 7: max fd num
        return FAILURE;
    }
    else if(curr_pcb->fds[fd].flags == 0) {
        return FAILURE;
    }
    temp_fd = fd;
    return curr_pcb->fds[fd].fops_ptr->write(fd, buf, nbytes);
}

/*
 * open
 * DESCRIPTION: provides access to the file system
 * INPUTS: filename -- which file to open
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: none
 */
int32_t open(const uint8_t *filename){
// make a new dentry and populate it
    dentry_t dentry_to_open;
    if(filename == NULL || *filename == '\0'){
             return FAILURE;
    }
    if (read_dentry_by_name(filename, &dentry_to_open) == FAILURE)
        return FAILURE; // if file cannot be opened, return FAILURE

    pcb_t *curr_pcb = get_pcb(); // get PCB address for current process

    int i;
    // check whether or not the possible processes are used or not
    //start at 2
    //end at 7 (last process)
    for (i = 2; i <= 7; i++) {
    if (curr_pcb->fds[i].flags == NOT_USED)
        {
            break;
        }
    }

    if (i == 8)
        return FAILURE;

    int32_t file_type = dentry_to_open.filetype;

    if(curr_pcb->fds[i].flags != USED){

        switch (file_type) {
            case RTC_FILETYPE:
                curr_pcb->fds[i].inode = NULL;
                curr_pcb->fds[i].flags = 1; //set
                curr_pcb->fds[i].fops_ptr = &rtc_table;
                break;
            case DIR_FILETYPE:
                curr_pcb->fds[i].inode = NULL;
                curr_pcb->fds[i].flags = 1; //set
                curr_pcb->fds[i].fops_ptr = &directory_table;
                break;
            case REG_FILETYPE:
                curr_pcb->fds[i].inode = dentry_to_open.inode_num;
                curr_pcb->fds[i].flags = 1;
                curr_pcb->fds[i].fops_ptr = &files_table;
                curr_pcb->fds[i].file_pos = 0;
                break;
            default:
                return FAILURE;
        }

        if (curr_pcb->fds[i].fops_ptr->open(filename) == -1)
            return FAILURE;

    }   
    // curr_pcb->fds[fd].flags = USED;
    return i; //returns process i
}


/*
 * close
 * DESCRIPTION: closes the specific fd and makes it available for return from later calls to open
 * INPUTS: fd - which file to close
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: file is closed for later calls to open
 */
int32_t close(int32_t fd)
{
    // if file is out of bounds, return failure
    //0 is min, 7 is max process number
    //get pcb pointer for current process
    pcb_t *curr_pcb = get_pcb();

    if (fd < 0 || fd > 7 || fd == 1 || fd == 0) {
        return FAILURE;
    }

    else if (curr_pcb->fds[fd].flags != 0){
        curr_pcb->fds[fd].flags = NOT_USED;
        curr_pcb->fds[fd].file_pos = 0;
        curr_pcb->fds[fd].inode= 0;
        curr_pcb->fds[fd].fops_ptr->close(fd);
        return SUCCESS;
    }
    return FAILURE;
}

/* halt
 * terminates the process being run by execute
 * Inputs: status - tells us if process is active
 *         
 *
 * Outputs: 0 for pass
 */
int32_t halt(uint8_t status) {
    pcb_t* curr_pcb = get_pcb();

    uint32_t curr_status = (uint32_t)status;
    int first_shell = 0; //flag to check if its the first shell

    if(pcb_state[curr_pcb->pid] == 0){
        return FAILURE;
    }

    if(terminals[active_terminal].processes[0] == curr_pcb->pid) {
        first_shell = 1;
    }

    int i;
    for(i = 0; i < 4; i++) { //4: processes 
    //checks if process was being executed if so make available now
        if(terminals[active_terminal].processes[i] == curr_pcb->pid){
            terminals[active_terminal].processes[i] = -1;
            break;
        }
    }

    //are we exiting a shell
    if(terminal_shell[active_terminal] == curr_pcb->pid){
        terminal_shell[active_terminal] = curr_pcb->parent->pid;
    }

    // for loop sets flags to 1 for all processes then closes
    for(i = 0; i < 8; i++) {
        if(curr_pcb->fds[i].flags == 1) {
            close(i);
        }
    }

    // //trying to launch multi terminal
    if(first_shell == 1){
        execute((uint8_t*)"shell");
    }
    pcb_state[curr_pcb->pid] = 0;

    //do paging according t parent
    page_switch(curr_pcb->parent->pid);

    //modify tss
    tss.esp0 = SIZE_8MB - (curr_pcb->parent->pid * page8KB) - 4; //number 4

    /* halt assembly linkage
    * switches to parent process and halts the terminal
    * Inputs: parent base pointer, parent stack pointer, status of the current pcb 
    *         
    * Outputs: none
    */
    asm volatile(
        "movl %2, %%eax;"
        "movl %0, %%ebp;"
        "leave;"
        "ret;"
        :
        : "r" (curr_pcb->parent->kbp), "r" (curr_pcb->parent->ksp), "r" (curr_status)
        : "eax", "ebp", "esp"
    );

    return (uint32_t)status;
}

/* execute
 * loads and executes a new program, hands off processor to new program, until termination
 * Inputs: command - space seperated sequence of words, first word is the file name of program to be executed & so on
 *         
 * Outputs: 0 for pass, -1 for fail
 */
int32_t execute(const uint8_t *command)
{
    tables_init();
    //step 1: variables needed for parsing
    uint8_t cmds[32]; //32 is the maximum size of the command name
    dentry_t cmds_dentry;
    int32_t cmds_inode;
    uint8_t process_num = 0; //set to inactive
    uint8_t cur_process_num;
    uint8_t arg_dat[128]; //128 characters

    //step 2: variables needed for executable check
    uint8_t exe_data[40]; //the metadata bytes
    void *entry_address = 0;
    int switch_user;

    //step 1 parsing
    int i = 0;
    for(i = 0; i < 128; i++){ //128 characters
        arg_dat[i] = 0;
    }

    //clearing the cmds array of any garbage values
    for (i = 0; i < 32; i++){
        //32 is the maximum size of the command
        cmds[i] = 0;
    }

    if (command == NULL)
        return FAILURE;

    //32 is max size of command
    //check for invalid commands
    for (i = 0; i < 32; i++) {

        if (command[i] == '\0')
            break;
        if (command[i] == ' ' || command[i] == '\n')
            break; //invalid commands are skipped
        cmds[i] = command[i];
    }
    cmds[i] = '\0';

    if (i == 0) { //if the command was an enter, then return 0
        return FAILURE;
    }

    if(command[strlen((int8_t*)cmds)] == ' '){
            //grab the arguments
            for(i = 0; command[i + strlen((int8_t*)cmds) + 1] != '\n' && command[i + strlen((int8_t*)cmds) + 1] != '\0'; i++){
                  arg_dat[i] = command[i + strlen((int8_t*)cmds) + 1 ]; //1
            }
    arg_dat[i] = '\0';
    }

    if (!strncmp((int8_t*)cmds, (int8_t*) "quit", 4)){
        (void) halt(0);
    }

    //Searching the file, getting dentry first and then the inode number
    if (read_dentry_by_name(cmds, &cmds_dentry) == FAILURE)
    {
        return FAILURE;
    }

    //ensure what we're executing is a file
    if ((cmds_dentry.filetype) != 2) //if not 2 then return -1
    {
        return FAILURE;
    }

    cmds_inode = cmds_dentry.inode_num;

    //step 2 executable check
    uint8_t x_magic[4] = {0x7F, 0x45, 0x4C, 0x46}; //list of all the magic numbers
    //4: size f array
    read_data(cmds_inode, 0, exe_data, 40); //grabbing the metadata - first 40 bytes
    //Check the 4 magic numbers
    for (i = 0; i < 4; i++)
    {
        if (exe_data[i] != x_magic[i])
        {
            return FAILURE;
        }
    }

    //grab the entry address of the program
    entry_address = (void *)((exe_data[27] << 24) | (exe_data[26] << 16) | (exe_data[25] << 8) | (exe_data[24]));
    //27, 26, 25, 24: array elements and 24, 16, 8: bit shifts -- to get entry address MSB's and LSB's appropriately

    //step 3: paging

    if(pcb_state[0] == 1 && pcb_state[1] == 1 && pcb_state[2] == 1 && pcb_state[3] == 1 && pcb_state[4] == 1 && pcb_state[5] == 1){
        return FAILURE;
    }

    int j;
    for(j = 0; j < 6; j++){
        //6 processes at a time so set pcb state
        if(pcb_state[j] == 0){
            pcb_state[j] = 1;
            process_num = j;
            break;
        }
    }

    //set pid for processes
    pcb_t *task_pcb = (pcb_t *)(page8MB - ((process_num + 1)*page8KB));
    cur_process_num = process_num;
    task_pcb->pid = cur_process_num;

    //check child processes for terminal
    if(terminals[active_terminal].terminal_flag == 1){
        if(active_terminal == 0) {
            terminal_shell[0] = 0;
        }
        //updating process number based on active terminal
        running_terminal = active_terminal;
        terminals[active_terminal].processes[0] = cur_process_num;
        terminals[active_terminal].terminal_flag = 0;
    }

    else {
        //4 processes
        for(i = 0; i < 4; i++) {
            //if process is available then set 
                if(terminals[active_terminal].processes[i] == -1) {
                    terminals[active_terminal].processes[i] = cur_process_num;
                    break;
                }
            }
        }

    //processes 0 is first shell
    task_pcb->parent->pid = terminals[active_terminal].processes[0];
  
    strcpy((int8_t*)task_pcb->args, (const int8_t*)arg_dat);

    //create_paging(cur_process_num);
    page_switch(cur_process_num);

    //step 4 - user level loader
    /*
        0x8000000- 128MB
        0x48000- 4mb+8kb
        1023*4096- maximum length of file
    */
    read_data(cmds_dentry.inode_num, 0, (uint8_t *)(SIZE_128MB + 0x48000), 1023 * 4096);

    //step 5 - creating pcb

    //std in
     task_pcb->fds[0].flags = 1;
     task_pcb->fds[0].fops_ptr = &stdin_table;
     task_pcb->fds[0].file_pos = 0;

    //std out
     task_pcb->fds[1].flags = 1;
     task_pcb->fds[1].fops_ptr = &stdout_table;
     task_pcb->fds[1].file_pos = 0;

    int temp;
    //2-7 leftover flags to set
    for (temp = 2; temp <= 7; temp++) {
         task_pcb->fds[temp].flags = NOT_USED;
    }

         task_pcb->parent = get_pcb();
        //inputting parent EBP
        asm volatile("                \n\
            MOVL %%EBP, %0            \n\
            "
                    : "=r"(task_pcb->parent->kbp));

        //inputting parent ESP
        asm volatile("                \n\
            MOVL %%ESP, %0            \n\
            "
                    : "=r"(task_pcb->parent->ksp));

         task_pcb->parent->ss0 = tss.ss0;
         task_pcb->parent->esp0 = tss.esp0;


    switch_user = (SIZE_128MB + SIZE_4MB - 4); //decimal num 4
    tss.ss0 = KERNEL_DS_;
    tss.esp0 = SIZE_8MB - (cur_process_num * page8KB) - 4; //decimal num 4

    /* context switch assembly linkage
    * lowers the privilege level
    * Inputs: entry address of the program
    *         pushed user cs, user ds values manually
    *         
    * Outputs: none
    */

    asm volatile(
        "pushl  $0x2B;"       // user ds
        "pushl  $0x083FFFFC;" // esp
        "pushfl;"             // eflag
        "popl   %%eax;"
        "orl    $0x200, %%eax;" // re-enable interrupts
        "pushl  %%eax;"
        "pushl  $0x23;" //user_cs
        "movl   %0, %%eax;"
        "pushl  %%eax;" //eip
        "sti;"
        "iret;"
        :
        : "r"(entry_address)
        : "%eax");

    return SUCCESS;
}
/* get_pcb
 * gets current value of pcb ptr
 * Inputs: none
 *         
 *
 * Outputs: curr_pcb ptr
 */
pcb_t *get_pcb()
{
    //0xffffe000 is mask
    uint32_t esp;
    asm volatile(
        "movl %%esp, %0;"
        : "=r" (esp)
    );
    return (pcb_t*)(esp & 0xffffe000);
}



/* create_paging
 * initializes paging for user page 
 * Inputs: process_num - which process from 0-8
 *         
 *
 * Outputs: none
 */
void create_paging(int process_num)
{
    //22: offset to access page directory address stored in 10 MSBs
    uint32_t program_addr = (uint32_t)((SIZE_8MB + process_num * SIZE_4MB) >> 22);
    //calculating index for new pd
    int user_idx = (int)(SIZE_128MB >> 22);

    //new page directory entries being set
    page_directory[user_idx].directory_entry_4mb.present = 1;
    page_directory[user_idx].directory_entry_4mb.read_write = 1;
    page_directory[user_idx].directory_entry_4mb.user_supervisor = 1;
    page_directory[user_idx].directory_entry_4mb.write_through = 1;
    page_directory[user_idx].directory_entry_4mb.cache_disabled = 1;
    page_directory[user_idx].directory_entry_4mb.accessed = 0;
    page_directory[user_idx].directory_entry_4mb.dirty = 0;
    page_directory[user_idx].directory_entry_4mb.page_size = 1;
    page_directory[user_idx].directory_entry_4mb.global_page = 0;
    page_directory[user_idx].directory_entry_4mb.avail = 0;
    page_directory[user_idx].directory_entry_4mb.attribute_index = 0;
    page_directory[user_idx].directory_entry_4mb.reserved = 0;
    page_directory[user_idx].directory_entry_4mb.bit_10_addr = program_addr;

    //flushing TLB via assembly linkage
    asm volatile("          \n\
        movl %%cr3, %%eax   \n\
        movl %%eax, %%cr3   \n\
        " ::
                     : "eax", "cc");

    return;
}

/*
 * getargs
 * DESCRIPTION: reads program's command line arguments into a user-level buffer
 * INPUTS: none
 * OUTPUTS: buf - where to copy args to
 *          nbytes - how many bytes to copy
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: buf is populated
 */
 int32_t getargs (uint8_t* buf, int32_t nbytes) {
     // if there are no args or empty buffer, return FAILURE
     if (buf == NULL || nbytes <= 0)
        return FAILURE;

    // get current pcb to copy args
    pcb_t* curr_pcb = (pcb_t *)get_pcb();

    // if no args, return FAILURE
    if (curr_pcb->args == NULL)
        return FAILURE;

    // otherwise, get args from pcb
    int8_t* args_to_copy = (int8_t *)(curr_pcb->args);

    // if nbytes is not valid length, return FAILURE
    if (nbytes < strlen(args_to_copy) + 1)
        return FAILURE;

    // if all the checks pass, copy pcb's args into buf and return SUCCESS
    strcpy((void*)buf, (void*)args_to_copy);
    return SUCCESS;
 }

 /*
 * vidmap
 * DESCRIPTION: maps text-mode video memory into user space at a pre-set virtual address
 * INPUTS: where to map video mem to
 * OUTPUTS: none
 * RETURN VALS: FAILURE, SUCCESS
 * SIDE EFFECTS: text-mode video memory is mapped to user space
 */
 int32_t vidmap (uint8_t** screen_start) {
     //null check
    if(screen_start == NULL){
        return FAILURE;
    }

    //check for screen bounds
    if((int)screen_start < SIZE_128MB || (int)screen_start >= SIZE_128MB + SIZE_4MB){
        return FAILURE;
    }
    //creating 4kb page table
    //finding user index where 22 is bit shift to get index

    int user_idx_ = (int)(SIZE_132MB >> 22);
    page_directory[user_idx_].directory_entry_4kb.present = 1;
    page_directory[user_idx_].directory_entry_4kb.read_write = 1;
    page_directory[user_idx_].directory_entry_4kb.user_supervisor = 1;
    page_directory[user_idx_].directory_entry_4kb.write_through = 0;
    page_directory[user_idx_].directory_entry_4kb.cache_disabled = 0;
    page_directory[user_idx_].directory_entry_4kb.accessed = 0;
    page_directory[user_idx_].directory_entry_4kb.reserved = 0;
    page_directory[user_idx_].directory_entry_4kb.page_size = 0;
    page_directory[user_idx_].directory_entry_4kb.global_page = 0;
    page_directory[user_idx_].directory_entry_4kb.avail = 0;
    page_directory[user_idx_].directory_entry_4kb.bit_20_addr = ((uint32_t) vidmap_pt) >> 12; //12 bit shift to get the 20 MSB's

    page_table[user_idx_].present = 1;
    page_table[user_idx_].global_page = 0;
    page_table[user_idx_].read_write = 1;
    page_table[user_idx_].user_supervisor = 1;
    page_table[user_idx_].write_through = 0;
    page_table[user_idx_].cache_disabled = 0;
    page_table[user_idx_].accessed = 0;
    page_table[user_idx_].dirty = 0;
    page_table[user_idx_].attribute_index = 0;
    page_table[user_idx_].avail = 0;
    page_table[user_idx_].bit_20_addr = (VIDEO_MEMORY>>12); 

    //set video memory pointer address to new page table
    //0x03FF is bit mask
    vidmap_pt[((SIZE_132MB >> 12) & 0x03FF)]  = page_table[user_idx_].val;
    //12 bit shift
    //0x03ff is mask

    *screen_start = (uint8_t *) SIZE_132MB;
    return SUCCESS;
 }

/**************                  MULTI TERMINALS BEGIN                  ***************/

 /*
 * process_init()
 * DESCRIPTION: intializes all descriptions and flags for terminals info array
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALS: none
 */
 void process_init() {
    int i, j;
    for(i = 0; i < 3; i++){ //3: num terminals
        terminals[i].terminal_flag = 1;
        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
        terminals[i].buffer_enable = 0;
        for(j = 0; j < 4; j++){ //4 processes
            terminals[i].processes[j] = -1;
        }
    }
    terminals[i].terminal_flag = 0;
    active_terminal = 0;
}

 /*
 * page_switch()
 * DESCRIPTION: switches pages for halt, multi-terminal, and scheduling
 *              adds page entry into the page directory at user space 
 * INPUTS: process id 
 * OUTPUTS: none
 * SIDE EFFECTS: new pages are created in memory 
 */
void page_switch(int32_t pross_id){

    // pcb_t* curr_pcb = get_pcb();
    uint32_t addr;
    //0: min number of processes, 8: maximum number of processes 
    if(pross_id < 0 || pross_id >= 8) {
        return;
    }

    addr = SIZE_8MB + (pross_id * SIZE_4MB);
    int user_idx = (int)(USER_ADD >> 22);

    //new page directory entries being set
    page_directory[user_idx].directory_entry_4mb.present = 1;
    page_directory[user_idx].directory_entry_4mb.read_write = 1;
    page_directory[user_idx].directory_entry_4mb.user_supervisor = 1;
    page_directory[user_idx].directory_entry_4mb.write_through = 0;
    page_directory[user_idx].directory_entry_4mb.cache_disabled = 0;
    page_directory[user_idx].directory_entry_4mb.accessed = 0;
    page_directory[user_idx].directory_entry_4mb.dirty = 0;
    page_directory[user_idx].directory_entry_4mb.page_size = 1;
    page_directory[user_idx].directory_entry_4mb.global_page = 0;
    page_directory[user_idx].directory_entry_4mb.avail = 0;
    page_directory[user_idx].directory_entry_4mb.attribute_index = 0;
    page_directory[user_idx].directory_entry_4mb.reserved = 0;
    page_directory[user_idx].directory_entry_4mb.bit_10_addr = addr >> 22; //3 terminals

    //flushing TLB via assembly linkage
    asm volatile("          \n\
        movl %%cr3, %%eax   \n\
        movl %%eax, %%cr3   \n\
        " ::
                     : "eax", "cc");

    return;
}


 /*
 * display_switch
 * DESCRIPTION: switches the terminal being displayed on current screen 
 * INPUTS: terminal id: the terminal to switch to 
 * OUTPUTS: none
 * RETURN VALS: none 
 * SIDE EFFECTS: executes a new shell instance 
 */
void display_switch(uint32_t term_id){
    //get current pcb
    pcb_t* curr_pcb = get_pcb();

    //assembly to store esp and ebp values 
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=r"(curr_pcb->ksp), "=r"(curr_pcb->kbp)
    );

    //save terminal x and y position on screen
    terminals[active_terminal].screen_x = (int)get_screenx();
    terminals[active_terminal].screen_y = (int)get_screeny();

    //store to displaying terminal from video memory
    //12: bit shift value to make it 32 bit to match vid mem
    //1: increment by 1, next terminal
    memcpy((uint32_t*)(VIDEO_MEMORY + ((active_terminal + 1) << 12)), (uint32_t*)(VIDEO_MEMORY), TERMINAL_SIZE);

    //update active terminal
    active_terminal = term_id;
    curr_pcb = get_pcb();

    //switch paging accordiing to new process 
    page_switch(curr_pcb->pid);

    //retsore to video memory on screen 
    //12: bit shift value to make it 32 bit to match vid mem
    //1: increment by 1, next terminal
    memcpy((uint32_t*)(VIDEO_MEMORY), (uint32_t*)(VIDEO_MEMORY + ((active_terminal + 1) << 12)), TERMINAL_SIZE);

    //set screen values and update cursor position
    set_screenx((uint8_t)terminals[active_terminal].screen_x);
    set_screeny((uint8_t)terminals[active_terminal].screen_y);

    uint8_t x = get_screenx();
    uint8_t y = get_screeny();

    place_cursor(x, y);

    if(terminals[active_terminal].terminal_flag){
        execute((uint8_t*)"shell");
    }
    // else {
    // tss.ss0 = 0x0018; //kernal ds 
    // tss.esp0 = SIZE_8MB - (curr_pcb->pid * page8KB) - 4; //decimal num 4

    // asm volatile(
    //         "movl %0, %%esp;"
    //         "movl %1, %%ebp;"
    //         : "=r"(curr_pcb->ksp), "=r"(curr_pcb->kbp)
    //     );
    // }
}
