#include "rtc.h"
//int open = 0; 
int interrupt_rtc;
int freq_value;
/*
 * init_rtc()
 * DESCRIPTION: disables and masks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: disables the specified IRQ
 */
void init_rtc() {
   // cli();			    // disable interrupts	
    outb(REG_B, COMMAND_PORT);           // select register B, and disable NMI
    int8_t prev = inb(RTC_PORT);	    // read the current value of register B
    outb(REG_B, COMMAND_PORT);		    // set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    //sti();

    int8_t rate = 0xF; //set to 1111
    outb(RTC_PORT, REG_A);
    prev = inb(RTC_PORT);
    outb(COMMAND_PORT, REG_A);
    prev = prev & 0xF0; //mask last 4 bits 
    outb(prev | rate, RTC_PORT);

    enable_irq(8); //enables rtc irq - 8
}

/*
 * rtc_handler()
 * DESCRIPTION: disables and masks the specified IRQ 
 *
 * INPUTS: irq_num	
 * OUTPUTS: none
 * SIDE EFFECTS: disables the specified IRQ
 */
void rtc_handler() {
	//printf("RTC Interrupt \n");
	//disable_irq(8);
    //test_interrupts();
	outb(0x0C, COMMAND_PORT); //0x0C is reg C in hex code to make sure we can get another interrupt
	inb(RTC_PORT);
    interrupt_rtc = 1;
    send_eoi(8); //sends eoi 8 for rtc
	//enable_irq(8);
}

/*
 * open_rtc(const uint8_t *filename)
 * DESCRIPTION: Opens the RTC, sets the frequency to 2
 *
 * INPUTS: filename
 * OUTPUTS: 0 on success
 */
int32_t open_rtc(const uint8_t *filename){
    //calls to set frequency to 2 Hz
    outb(REG_A, COMMAND_PORT);
    unsigned char prev_ = inb(RTC_PORT);
    //2: frequenecy default used for cat rtc
    int8_t val_freq = set_freq_rtc(2);

    //sets 4 msb's of registerA to val_freq from the set_freq function call
    outb(REG_A, COMMAND_PORT);
    //mask 0xf0 for msb's
    outb(((0xF0&prev_) | val_freq) , RTC_PORT);
    return 0;
}

/*
 * read_rtc(int32_t fd, void *buffer, int32_t num)
 * DESCRIPTION: Checsk for an RTC interrupt, waits for it
 *
 * INPUTS: fd, buffer, num	
 * OUTPUTS: 0 on success
 * SIDE EFFECTS: stays in a while(1) loop till interrupt is detected
 */
int32_t read_rtc(int32_t fd, void *buffer, int32_t num){
    //while 1 loop till interrupt
    interrupt_rtc = 0;
    sti();
    while(1){
        //sti();
        //once interrupt is seen return
        if(interrupt_rtc == 1){
            cli();
            return 0;
        }
        //cli();
    }
    cli();
    return 0;
}

/*
 * set_freq_rtc(int32_t curr_freq)
 * DESCRIPTION: sets the RTC frequency 
 *
 * INPUTS: curr_freq: frequency to set it to	
 * OUTPUTS: 0 on success, -1 on invalid inputs
 * SIDE EFFECTS: sets the frequency to 2 by default
 */
int8_t set_freq_rtc(int32_t curr_freq){
    int8_t val_freq;
    //out of range returns -1
    if(curr_freq < 2 || curr_freq > 1024){
        return -1;
    }
    //switch case for valid powers of 2, return 16-log2val , used os dev for values 
    switch(curr_freq){
        case 1024: val_freq = 0x06; //all of these cases display powers of 2 in range, 16 -log2(1024) with 1024 Hz 
        break;
        case 512: val_freq = 0x07; //all of these hex values are calculated: 16 -log2(512) with 512 Hz
        break;
        case 256: val_freq = 0x08; //16 -log2(256) with 256 Hz
        break;
        case 128: val_freq = 0x09; //16 -log2(128) with 128 Hz
        break;
        case 64: val_freq = 0x0A; //16 -log2(64) with 64 Hz
        break;
        case 32: val_freq = 0x0B; //16 -log2(32) with 32 Hz
        break;
        case 16: val_freq = 0x0C; //16 -log2(16) with 16 Hz
        break;
        case 8: val_freq = 0x0D; //16 -log2(8) with 8 Hz
        break;
        case 4: val_freq = 0x0E; //16 -log2(4) with 4 Hz
        break;
        case 2: val_freq = 0x0F; //16 -log2(2) with 2 Hz
        break;
        default: return -1; //default is same as case 2 Hz
    }
    return val_freq;
}

/*
 * write_rtc(int32_t fd, const void *buffer, int32_t num)
 * DESCRIPTION: writes a new frequency  
 *
 * INPUTS: fd,
 *         buffer: has the frequency to set it to
*          num
 * OUTPUTS: 0 on success
 */
int32_t write_rtc(int32_t fd, const void *buffer, int32_t num){
    outb(REG_A, COMMAND_PORT);
    unsigned char prev_ = inb(RTC_PORT);
    int8_t val_freq = set_freq_rtc(*(int32_t*) buffer);
    freq_value = val_freq;

    //sets 4 msb's of registerA to val_freq from the set_freq function call
    outb(REG_A, COMMAND_PORT);
    //0xf0 mask for msb's
    outb(((0xF0&prev_) | val_freq) , RTC_PORT);
    return 0;
}

/*
 * int rtc_close(int32_t fd);
 * DESCRIPTION: Closes the RTC driver and sets frequency to 2
 *
 * INPUTS: fd
 * OUTPUTS: 0 on success
 * SIDE EFFECTS: none
 */
int32_t close_rtc(int32_t fd){
    //mask irq for pic - 8
    // disable_irq(8);
    return 0;
}

/*
 * int get_freq();
 * DESCRIPTION: Returns the current RTC frequency value
 *
 * INPUTS: none 
 * OUTPUTS: current frequency 
 * SIDE EFFECTS: none
 */
int8_t get_freq(){
    return freq_value;
}
