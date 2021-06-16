#ifndef IDT_H_
#define IDT_H_

//#include "lib.h"
#include "x86_desc.h"
#include "interrupt.h"

/*
 * SOURCE: https://www.oreilly.com/library/view/understanding-the-linux/0596002130/ch04s04.html
 */
/* Privilege Levels */
#define USER_PL		0x3
#define KERNEL_PL	0x0

#define NUM_VEC_EXCP	20
/* The following defines are the vector numbers Intel defined */
#define SIG_DE 		0		/* Division Error 			*/
#define SIG_DB		1		/* Debug Exception 			*/
#define SIG_NMI 	2		/* Non-maskable interrupt 	*/
#define SIG_BP		3		/* Breakpoint exception		*/
#define SIG_OF		4		/* Overflow exception		*/
#define SIG_BR		5		/* Bound Range exceeded		*/
#define SIG_UD		6		/* Invalid Opcode			*/
#define SIG_NM		7		/* Device not available 	*/
#define SIG_DF		8		/* Double Fault				*/
#define SIG_SO		9		/* Coprocessor segment overrun */
#define SIG_TS		10		/* Invalid TSS 				*/
#define SIG_NP		11		/* Segment not present		*/
#define SIG_SS		12		/* Stack Fault				*/
#define SIG_GP		13		/* General Protection 		*/
#define SIG_PF		14		/* Page-Fault				*/
/* 15 is intel reserved                                 */
#define SIG_MF		16		/* Floating-Point Error 	*/
#define SIG_AC		17		/* Alignment Check			*/
#define SIG_MC		18		/* Machine-Check			*/
#define SIG_XF		19		/* SIMD Floating point		*/
/* 20-31 are intel reserved... TODO: Do they need a handler? */

/* Device vector numbers */
#define SIG_RTC		40		/* RTC interrupt			*/
#define SIG_KBD 	33		/* Keyboard interrupt		*/
#define SIG_PIT     32      /* Pit Interrupt            */

/* System call vector number */
#define SIG_SYSCALL	0x80	/* System Call interrupt	*/

/*
 * Define unknown vector number to be a vector number that is unlikely to
 * be used
 */
#define SIG_IDK		0xFF

#ifndef ASM
/* Initializes the Interrupt Descriptor Table */
void init_idt(void);

/* Inserts an interrupt gate into the nth IDT entry */
void set_intr_gate(uint8_t n, uint32_t addr);

/* Inserts a Trap Gate into the nth IDT entry */
void set_trap_gate(uint8_t n, uint32_t addr);

/* Inserts a System Gate(trap gate) into the nth IDT entry */
void set_system_gate(uint8_t n, uint32_t addr);

/* handles all exceptions */
void handle_exception(unsigned int int_vector);

void handle_divide(unsigned int int_vector);

#endif
#endif
