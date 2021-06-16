#ifndef KEYBOARD_H
#define KEYBOARD_H

//Keyboard ports
#define KEYBOARD_ENCODER		0x60
#define KEYBOARD_CONTROLLER 	0x64


//Values for special keys (obtained from OSDev)
#define LEFT_SHIFT_PRESS	    0x2A
#define RIGHT_SHIFT_PRESS	    0x36
#define LEFT_SHIFT_RELEASE	    0xAA
#define RIGHT_SHIFT_RELEASE	    0xB6
#define	CAPS_PRESS		        0x3A
#define LEFT_CONTROL_PRESS		0x1D
#define LEFT_CONTROL_RELEASE	0x9D
#define L_PRESS			        0x26
#define BACKSPACE_PRESS		    0x8
#define KEYBOARDSIZE            127
#define ENTER_PRESS             0x1C
#define ENTER_RELEASE           0x9C
#define LEFT_ALT_PRESS          0x38
#define RIGHT_ALT_PRESS         0xE0
#define LEFT_ALT_RELEASE        0xB8
#define RIGHT_ALT_RELEASE       0xE0

#define F1_scancode             0x3B
#define F2_scancode             0x3C
#define F3_scancode             0x3D

/*
 * init_keyboard()
 * DESCRIPTION: initialise the keyboard 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
void init_keyboard();

/*
 * keyboard_handler()
 * DESCRIPTION: initialise the keyboard 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
void keyboard_handler();

/*
 * key_status()
 * DESCRIPTION: checks for special characters being typed
 *
 * INPUTS: key_pressed	
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly
 */
void key_status(unsigned char key_pressed);

/*
 * cntl_l_check()
 * DESCRIPTION: checks for cntl+l
 *
 * INPUTS: scan code of char pressed	
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
int cntl_l_check(unsigned char data);

/*
 * key_noprint()
 * DESCRIPTION: checks for backspace and enter
 *
 * INPUTS: char pressed	
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
void key_noprint(unsigned char key_pressed);


/*
 * open_terminal()
 * DESCRIPTION: opens the terminal
 * INPUTS: file	
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
int32_t open_terminal(const uint8_t *filename);

/*
 * read_terminal()
 * DESCRIPTION: reads from the terminal
 * INPUTS: file, buffer and the size
 * OUTPUTS: none
 * SIDE EFFECTS: reads the file
 */
int32_t read_terminal(int32_t fd, unsigned char *buf, int32_t num);

/*
 * write_terminal()
 * DESCRIPTION: writes to the terminal
 * INPUTS: file, buffer and the size
 * OUTPUTS: none
 * SIDE EFFECTS: write on the file
 */
int32_t write_terminal(int32_t fd, const unsigned char *buf, int32_t num);


int32_t close_terminal(int32_t fd);

int32_t read_failure (int32_t fd, unsigned char *buf, int32_t num);

int32_t write_failure (int32_t fd, const unsigned char *buf, int32_t num);


#endif
