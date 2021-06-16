/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"

/*#include "errors.h"*/

/* Enable masks for each of the 8 irqs */
#define MASK_IRQ0			0x01
#define MASK_IRQ1			0x02
#define MASK_IRQ2			0x04
#define MASK_IRQ3			0x08
#define MASK_IRQ4			0x10
#define MASK_IRQ5			0x20
#define MASK_IRQ6			0x40
#define MASK_IRQ7			0x80

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Common Interrupt masks */
#define SLAVE_MASK			0xFB
#define MASK_ALL			0xFF

/* Range of the slave irqs */
#define SLAVE_START			8
#define SLAVE_END			15

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/*
 * void i8259_init(uint32_t irq_num)
 * DESCRIPTION: enables, unmasks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */

/* Initialize the 8259 PIC */
void i8259_init(void) { 

	/* Update the current master mask */
	master_mask = MASK_ALL;

	/* Update the current slave mask */
	slave_mask = MASK_ALL;

	/* Initialize the master interrupt controller */
	outb(ICW1, MASTER_8259_PORT);				/* Start the init sequence */
	outb(ICW2_MASTER, MASTER_8259_PORT + 1);	/* Vector base */
	outb(ICW3_MASTER, MASTER_8259_PORT + 1);	/* Edge triggered, Cascade (slave) on IRQ2 */
	outb(ICW4, MASTER_8259_PORT + 1);			/* Select 8086 mode */

	/* Inititalize the slave interrupt controller */
	outb(ICW1, SLAVE_8259_PORT);				/* Start the init sequence */
	outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);		/* Vector base */
	outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);		/* Edge triggered, Cascade (slave) on IRQ2 */
	outb(ICW4, SLAVE_8259_PORT + 1);			/* Select 8086 mode */

	/*
	 * Normally we would unmask the interrupts here but each device initializer will do that
	 * so we wont do it here. We do still need to enable the Slave on the Master IRQ2 though.
	 */
	outb(MASK_ALL, MASTER_8259_PORT + 1);
	outb(MASK_ALL, SLAVE_8259_PORT + 1);
	enable_irq(2);

}

/*
 * void enable_irq(uint32_t irq_num)
 * DESCRIPTION: enables, unmasks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	if(irq_num>15 || irq_num<0)	return;
	//slave bound is from 8->15 (1000->1111)
	//master bound is from 0->7 (0000->0111)
	if(irq_num & SLAVE_START){	//checking if the irq is slave, (checking if the 4th bit is set)
		master_mask = master_mask & (~(1<<(2)));
		outb(master_mask,MASTER_8259_PORT+1);

		outb(slave_mask & (~(1)<<(irq_num-8)), SLAVE_8259_PORT+1);	//
		slave_mask &= ~(1<<(irq_num-8));
	}
	else{	//for master
		master_mask = master_mask & (~(1<<(irq_num)));
		outb(master_mask, MASTER_8259_PORT+1);	//
	}

}


/*
 * void disable_irq(uint32_t irq_num)
 * DESCRIPTION: enables, unmasks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	if(irq_num>15 || irq_num<0)	return;

	//slave bound is from 8->15 (1000->1111)
	//master bound is from 0->7 (0000->0111)
	if(irq_num & SLAVE_START){	//checking if the irq is slave, (checking if the 4th bit is set)
		outb(inb(SLAVE_8259_PORT+1) | (1<<(irq_num-8)), SLAVE_8259_PORT+1);
	}
	else{	//for master
		outb(inb(MASTER_8259_PORT+1) | (1<<(irq_num)), MASTER_8259_PORT+1);	//
	}

}


/*
 * void send_eoi(uint32_t irq_num
 * DESCRIPTION: enables, unmasks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {

	if(irq_num & SLAVE_START){	//checking if the irq is slave, (checking if the 4th bit is set)
		outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
		outb(EOI | CASCADE, MASTER_8259_PORT);	//
	}
	else{	//for master
		outb(EOI | (irq_num), MASTER_8259_PORT);
	}

}

