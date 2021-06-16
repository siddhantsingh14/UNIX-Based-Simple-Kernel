#include "sched.h"

/*
 * pit_init()
 * DESCRIPTION: initializes pit 
 * INPUTS: 0    
 * OUTPUTS: 0
 * SIDE EFFECTS: pit is ready to use
*/
void pit_init() {
    int32_t freq = FREQ / 50;
    outb(PIT_PORT, COMMAND_REG);
    outb(freq & MASK, CHANNEL_0);
    outb(freq >> 8, CHANNEL_0);
    enable_irq(PIT_IRQ);
}

/*
 * pit_handler()
 * DESCRIPTION: calls scheduler alg
 * INPUTS: 0    
 * OUTPUTS: 0
 * SIDE EFFECTS: scheduling invoked
*/
void pit_handler() {
    send_eoi(PIT_IRQ);
    if(terminals[0].terminal_flag == 0 || terminals[1].terminal_flag == 0 || terminals[2].terminal_flag == 0){
        scheduler();
    }
    //return;
}

/*
 * scheduler()
 * DESCRIPTION: implements round robin scheduling
 * INPUTS: none 
 * OUTPUTS: none
 * SIDE EFFECTS: processes switch every 20 ms
*/
void scheduler() {
    next_term = (running_terminal + 1) % 3;
    //1: next terminal, 3: total terminals 
    if(next_term != running_terminal && terminals[next_term].terminal_flag == 0){
        context_switch();
        return;
    }
    else {
        //save terminal state
        terminals[active_terminal].screen_x = (int)get_screenx();
        terminals[active_terminal].screen_y = (int)get_screeny();
        //12: bit shift 12 to make it 32 bit 
        //1: increment by 1, next terminal
        memcpy((uint32_t*)(VIDEO_MEMORY + ((active_terminal + 1) << 12)), (uint32_t*)(VIDEO_MEMORY), TERMINAL_SIZE);

        pcb_t *prev_pcb = get_pcb();

        active_terminal = next_term;
        clear();
        //restorig terminals state and updating new cursor position
        set_screenx((uint8_t)terminals[active_terminal].screen_x);
        set_screeny((uint8_t)terminals[active_terminal].screen_y);
        place_cursor((uint8_t)terminals[active_terminal].screen_x, (uint8_t)terminals[active_terminal].screen_y);
        //12: bit shift 12 to make it 32 bit
        //1: increment by 1, next terminal
        memcpy((uint32_t*)(VIDEO_MEMORY), (uint32_t*)(VIDEO_MEMORY + ((active_terminal + 1) << 12)), TERMINAL_SIZE);

        //save old process esp/ebp
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            : "=r"(prev_pcb->ksp), "=r"(prev_pcb->kbp)
        );
        execute((uint8_t*)"shell");
    }

}

  /* context switch 
    * switches the process number pcb and stores and restores 
    *          esp and ebp values 
    * Inputs: none     
    * Outputs: none
    */
void context_switch() {
    pcb_t *curr_pcb = get_pcb();
    running_terminal = next_term;
    pcb_t *next_pcb = get_pcb();

    page_switch(next_pcb->pid);

    tss.ss0 = KERNEL_DS; //kernal ds 
    tss.esp0 = SIZE_8MB - (next_pcb->pid * page_8KB) - 4; //decimal num 4

    asm volatile(
            "movl %%esp, %%ebx;"
            "movl %%ebp, %%ecx;"
            : "=r"(curr_pcb->ksp), "=r"(curr_pcb->kbp)
        );
    asm volatile(
            "movl %%ebx, %%esp;"
            "movl %%ecx, %%ebp;"
            : "=r"(next_pcb->ksp), "=r"(next_pcb->kbp)
        );

    return;
}
