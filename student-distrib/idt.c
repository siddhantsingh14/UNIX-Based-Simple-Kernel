
#include "idt.h"


/* init_idt()
 * 	DESCRIPTION: Initializes all 256 elements in the IDT with
 * 				 the proper IDT entry.
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: Initializes the IDT and loads the IDTR
 */
void init_idt(void) {
	int i;	

	/* Load all of the possible vectors with an unknown interrupt */
	for (i = 0; i < NUM_VEC; i++) {
		set_trap_gate(i, (uint32_t) handle_SIG_IDK);
	}

	/* Set the exception handlers for all of the Intel defined vectors */
	set_intr_gate(SIG_DE, (uint32_t) handle_SIG_DE);
	set_intr_gate(SIG_DB, (uint32_t) handle_SIG_DB);
	set_intr_gate(SIG_NMI, (uint32_t) handle_SIG_NMI);
	set_intr_gate(SIG_BP, (uint32_t) handle_SIG_BP);
	set_intr_gate(SIG_OF, (uint32_t) handle_SIG_OF);
	set_intr_gate(SIG_BR, (uint32_t) handle_SIG_BR);
	set_intr_gate(SIG_UD, (uint32_t) handle_SIG_UD);
	set_intr_gate(SIG_NM, (uint32_t) handle_SIG_NM);
	set_intr_gate(SIG_DF, (uint32_t) handle_SIG_DF);
	set_intr_gate(SIG_SO, (uint32_t) handle_SIG_SO);
	set_intr_gate(SIG_TS, (uint32_t) handle_SIG_TS);
	set_intr_gate(SIG_NP, (uint32_t) handle_SIG_NP);
	set_intr_gate(SIG_SS, (uint32_t) handle_SIG_SS);
	set_intr_gate(SIG_GP, (uint32_t) handle_SIG_GP);
	set_intr_gate(SIG_PF, (uint32_t) handle_SIG_PF);
	set_intr_gate(SIG_MF, (uint32_t) handle_SIG_MF);
	set_intr_gate(SIG_AC, (uint32_t) handle_SIG_AC);
	set_intr_gate(SIG_MC, (uint32_t) handle_SIG_MC);
	set_intr_gate(SIG_XF, (uint32_t) handle_SIG_XF);

	set_intr_gate(SIG_KBD, (uint32_t) handle_SIG_KBD);
	set_intr_gate(SIG_RTC, (uint32_t) handle_SIG_RTC);
	set_intr_gate(SIG_PIT, (uint32_t) handle_SIG_PIT);

	set_system_gate(SIG_SYSCALL, (uint32_t) handle_SIG_SYSCALL);

}

/* set_intr_gate
 * 	DESCRIPTION: Inserts an interrupt gate into the nth IDT entry. Segment Selector
 *              inside the gate is set to the kernel code's Segment Selector. The 
 *              Offset field is set to addr, which is the address of the interrupt
 *              handler. The DPL field is set to 0, Kernel Privilege Level.
 * 	INPUTS: n - index into the IDT
 * 			addr - Address of the interrupt handler
 * 	OUTPUTS: None.
 * 	RETURN VALUE: None.
 * 	SIDE EFFECTS: None.
 */
void set_intr_gate(uint8_t n, uint32_t addr) {
	/* Set the interrupt as present. */
	idt[n].present = 0x1;

	/* All hardware interrupts will have a kernel privilege level */
	idt[n].dpl = KERNEL_PL;

	/* Set the reserved bits */
	idt[n].reserved0 = 0x0;
	idt[n].reserved1 = 0x1;
	idt[n].reserved2 = 0x1;
	idt[n].reserved4 = 0x00;	/* 5-bit Undefined/Don't Care */

	/*
	 * This bit determines whether it is a Trap Gate or an Interrupt Gate.
	 * All exceptions call Trap Gates and all hardware interrupts are
	 * Interrupt Gates. Peep x86_desc.S to see why we did it this way
	 */
	idt[n].reserved3 = 0x0;

	/* Set the size. D will be always be 32 bits */
	idt[n].size = 0x1;

	/* Set the segment. This will always be the Kernel's CS */
	idt[n].seg_selector = KERNEL_CS;

	/* Set the Interrupt handler */
	SET_IDT_ENTRY(idt[n], addr);
}

/* set_trap_gate
 * 	DESCRIPTION: Same as set_system_gate except DPL set to 0
 * 	INPUTS: n - index into the IDT
 * 			addr - the address of the Interrupt handler
 * 	OUTPUTS: None.
 * 	RETURN VALUE: None.
 * 	SIDE EFFECTS: None.
 */
void set_trap_gate(uint8_t n, uint32_t addr) {
	/*
	 * Set the interrupt as present. We will set them all
	 * as present because we have a generic signal handler
	 * that will tell us if we are missing a handler for
	 * something.
	 */
	idt[n].present = 0x1;

	/*
	 * All exceptions will have a kernel privilege level
	 */
	idt[n].dpl = KERNEL_PL;

	/* Set the reserved bits */
	idt[n].reserved0 = 0x0;
	idt[n].reserved1 = 0x1;
	idt[n].reserved2 = 0x1;
	idt[n].reserved4 = 0x00;	/* 5-bit Undefined/Don't Care */

	/*
	 * This bit determines whether it is a Trap Gate or an Interrupt Gate.
	 * All exceptions call Trap Gates and all hardware interrupts are
	 * Interrupt Gates. Peep x86_desc.S to see why we did it this way
	 */
	idt[n].reserved3 = 0x1;

	/* Set the size. D will be always be 32 bits */
	idt[n].size = 0x1;

	/* Set the segment. This will always be the Kernel's CS */
	idt[n].seg_selector = KERNEL_CS;

	/* Add the interrupt handler */
	SET_IDT_ENTRY(idt[n], addr);
}

/* set_system_gate
 * 	DESCRIPTION: Inserts an interrupt gate into the nth IDT entry. Segment Selector
 *              inside the gate is set to the kernel code's Segment Selector. The 
 *              Offset field is set to addr, which is the address of the interrupt
 *              handler. The DPL field is set to 3, User Privilege Level.
 * 	INPUTS: n - index into the IDT
 * 			addr - address of the interrupt handler
 * 	OUTPUTS: None.
 * 	RETURN VALUE: None.
 * 	SIDE EFFECTS: None.
 */
void set_system_gate(uint8_t n, uint32_t addr) {
	/* Set the interrupt as present. */
	idt[n].present = 0x1;

	/* All exceptions will have a user privilege level */
	idt[n].dpl = USER_PL;

	/* Set the reserved bits */
	idt[n].reserved0 = 0x0;
	idt[n].reserved1 = 0x1;
	idt[n].reserved2 = 0x1;
	idt[n].reserved4 = 0x00;	/* 5-bit Undefined/Don't Care */

	/*
	 * This bit determines whether it is a Trap Gate or an Interrupt Gate.
	 * All exceptions call Trap Gates and all hardware interrupts call
	 * Interrupt Gates.
	 */
	idt[n].reserved3 = 0x0;

	/* Set the size. D will be always be 32 bits */
	idt[n].size = 0x1;

	/* Set the segment. This will always be the Kernel's CS */
	idt[n].seg_selector = KERNEL_CS;

	/* Add the interrupt handler */
	SET_IDT_ENTRY(idt[n], addr);
}

/* handle_exception
 * 	DESCRIPTION: This function is called from all of the exception ISRs. Those
 * 				 ISRs push their interrupt vectors onto the stack, call this
 * 				 function, clean up the stack, and return with the iret instruction.
 * 				 For more info, look at interrupts.h/S files.
 * 	INPUTS: int_vector - the interrupt vector corresponding to the exception
 * 	OUTPUS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: Prints the exception message and kills the program that
 * 				  caused it.
 */
void handle_exception(unsigned int int_vector) {

	switch (int_vector) {
		case SIG_DE:  {printf("[EXCEPTION] Division Error\n"); }
		case SIG_DB:  printf("[EXCEPTION] Debug Exception\n"); break;
		case SIG_NMI: printf("[EXCEPTION] Non-maskable Interrupt\n"); break;
		case SIG_BP:  printf("[EXCEPTION] Breakpoint Exception\n"); break;
		case SIG_OF:  printf("[EXCEPTION] Overflow Exception\n"); break;
		case SIG_BR:  printf("[EXCEPTION] Bound Range Exceeded\n"); break;
		case SIG_UD:  printf("[EXCEPTION] Invalid Opcode\n"); break;
		case SIG_NM:  printf("[EXCEPTION] Device not available\n"); break;
		case SIG_DF:  printf("[EXCEPTION] Double Fault\n"); break;
		case SIG_SO:  printf("[EXCEPTION] Coprocessor Segment Overrun\n"); break;
		case SIG_TS:  printf("[EXCEPTION] Invalid TSS\n"); break;
		case SIG_NP:  printf("[EXCEPTION] Segment not present\n"); break;
		case SIG_SS:  printf("[EXCEPTION] Stack Fault Error\n"); break;
		case SIG_GP:  printf("[EXCEPTION] General Protection Error\n"); break;
		case SIG_PF:  {
			uint32_t return_val;
			asm volatile
			("movl %%cr2, %0"
				:"=r"(return_val)
				:
			);
			printf("Page fault occur at %#x\n", return_val);
			
			break;
		}
		case SIG_MF:  printf("[EXCEPTION] Floating Point Error\n"); break;
		case SIG_AC:  printf("[EXCEPTION] Alignment Check Error\n"); break;
		case SIG_MC:  printf("[EXCEPTION] Machine-Check Error\n"); break;
		case SIG_XF:  printf("[EXCEPTION] SIMD Floating point\n"); break;
		default: printf("[EXCEPTION] Unknown Interrupt %d\n", int_vector); break;
	}
	//halt(255);
	while(1);

}
