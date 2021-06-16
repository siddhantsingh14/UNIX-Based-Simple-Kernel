#include "lib.h"
#include "keyboard.h"
#include "i8259.h"
#include "syscall.h"

//when a key is pressed without caps NO shift
char lower[256] = { '\0','\0','1','2','3','4','5','6','7','8','9','0','-','=','\b',' ', 'q','w','e','r','t','y','u','i','o','p','[',']','\n','\0','a','s', 'd','f','g','h','j','k','l',';','\'','`','\0','\\','z','x','c','v', 'b','n','m',',','.','/','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

//when a key is pressed with caps AND shift
char uppershift[256] = { '\0','\0','!','@','#','$','%','^','&','*','(',')','_','+','\b',' ', 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n','\0','A','S', 'D','F','G','H','J','K','L',':','"','~','\0','|','Z','X','C','V', 'B','N','M','<','>','?','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

//when a key is pressed with caps NO shift
char caps[256] = { '\0','\0','1','2','3','4','5','6','7','8','9','0','-','=','\b',' ', 'Q','W','E','R','T','Y','U','I','O','P','[',']','\n','\0','A','S', 'D','F','G','H','J','K','L',';','\'','`','\0','\\','Z','X','C','V', 'B','N','M',',','.','/','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

//when a key is pressed without caps AND shift
char lowershift[256] = { '\0','\0','!','@','#','$','%','^','&','*','(',')','_','+','\b',' ', 'q','w','e','r','t','y','u','i','o','p','{','}','\n','\0','a','s', 'd','f','g','h','j','k','l',':','"','~','\0','|','z','x','c','v', 'b','n','m','<','>','?','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

//scancode arrays from http://www.cs.umd.edu/~hollings/cs412/s98/project/proj1/scancode
char ascii_code[58][2] =       /* Array containing ascii codes for
                   appropriate scan codes */
     {
       {   0,0   } ,
       {   0,0   } ,
       { '1','!' } ,
       { '2','@' } ,
       { '3','#' } ,
       { '4','$' } ,
       { '5','%' } ,
       { '6','^' } ,
       { '7','&' } ,
       { '8','*' } ,
       { '9','(' } ,
       { '0',')' } ,
       { '-','_' } ,
       { '=','+' } ,
       {   8,8   } ,
       {   0,0   } ,
       { 'q','Q' } ,
       { 'w','W' } ,
       { 'e','E' } ,
       { 'r','R' } ,
       { 't','T' } ,
       { 'y','Y' } ,
       { 'u','U' } ,
       { 'i','I' } ,
       { 'o','O' } ,
       { 'p','P' } ,
       { '[','{' } ,
       { ']','}' } ,
       {  10,10  } ,
       {   0,0   } ,
       { 'a','A' } ,
       { 's','S' } ,
       { 'd','D' } ,
       { 'f','F' } ,
       { 'g','G' } ,
       { 'h','H' } ,
       { 'j','J' } ,
       { 'k','K' } ,
       { 'l','L' } ,
       { ';',':' } ,
       {  39,34  } ,
       { '`','~' } ,
       {   0,0   } ,
       { '\\','|'} ,
       { 'z','Z' } ,
       { 'x','X' } ,
       { 'c','C' } ,
       { 'v','V' } ,
       { 'b','B' } ,
       { 'n','N' } ,
       { 'm','M' } ,
       { ',','<' } ,
       { '.','>' } ,
       { '/','?' } ,
       {   0,0   } ,
       {   0,0   } ,
       {   0,0   } ,
       { ' ',' ' } ,
   };



int flag=0;     //this flag keeps in check whether a special key has been pressed and whether it needs to continue with the rest of the handler
char *pointer;
int case_status;    //this variable keeps track of the status of the letters to check the case
int control_status; //this variable keeps track of the status of the control button
int alt_status;     //this variable keeps track of the status of the alt button
int caps_enable;    //using this flag to see if CAPS has been enabled or not
int is_alphabet;    //checking if the press is an alphabet
int enter_read[3] = {0,0,0};

int number_printed_line;

unsigned char keyboard_buf[KEYBOARDSIZE + 1]; //the plus one is for extra newline char

static int index[3] = {0,0,0};

volatile int32_t disp_term_k;


//the switch between the upper case and lower case will be made inside the fn ()

/*
 * init_keyboard()
 * DESCRIPTION: initialise the keyboard 
 *
 * INPUTS: irq_num  
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */

/* Initialize the keyboard */
void init_keyboard() {
    //int i;
    pointer = &lower[0];    //keeping the assumption that caps and shift have not been pressed yet
    case_status=0;      //lower case at init
    control_status=0;   //lower case to begin with
    caps_enable=0;  //disabling the caps
    is_alphabet=0;
    int i=0;
    for(i=0;i<3;i++) {
        memset(terminals[i].keyboard_buf, 0, 128);   //look at what to do with this
    }
    number_printed_line=0;
    enable_irq(1);  //enable irq1
}


/*
 * keyboard_handler()
 * DESCRIPTION: initialise the keyboard 
 *
 * INPUTS: irq_num  
 * OUTPUTS: none
 * SIDE EFFECTS: enables a specified IRQ
 */
/* Keyboard Interrupt */
void keyboard_handler() {
    /* Mask interrupts */
    //cli();
    disp_term_k = ret_active_term();
    //display_switch(disp_term_k);
    is_alphabet = 0;
    enter_read[disp_term_k] = 0;
    int ret=0;
    unsigned char data, print;
    
    data = inb(KEYBOARD_ENCODER);   //obtaining scancode

    key_status(data);
    
    ret = cntl_l_check(data);
    print = pointer[data];  //converting scan code to text using the array
    key_noprint(print); //for special keys that are not supposed to be printed
    
    
    print = pointer[data];

    if(control_status==1 && (print== 'l' || print== 'L')){
        send_eoi(1);    //for irq1
        return;
    }
    if(alt_status==1){ 
        send_eoi(1);    //for irq1 for the keyboard
        //check for terminal 1: alt + fn1, set display terminal tracker and call for function to switch terminal
        if(data==F1_scancode){
            disp_term_k = 0;
            //enter_read[0] = 1;
            display_switch(0);
        }
        //check for terminal 2: alt + fn2, set display terminal tracker and call for function to switch terminal
        if(data==F2_scancode){
            disp_term_k = 1;
            //enter_read[1] = 1;
            display_switch(1);
        }
        //check for terminal 3: alt + fn3, set display terminal tracker and call for function to switch terminal
        if(data==F3_scancode){
            disp_term_k = 2;
            //enter_read[2] = 1;
            display_switch(2);
        }
        return;
    }
    else if(print!='\0' && print!='\b'){
     terminals[disp_term_k].keyboard_buf[index[disp_term_k]] = print;
     index[disp_term_k]++; //keeping a seperate tab of tab of index for each terminal
     number_printed_line++;
     putc((uint8_t)print);
    }
    send_eoi(1);    //for irq1
    return;

    /* Unmask interrupts */
    //sti();
    
}

/*
 * key_status()
 * DESCRIPTION: checks for special characters being typed
 *
 * INPUTS: key_pressed  
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly
 */
void key_status(unsigned char key_pressed){
    //unsigned char print;
    if(key_pressed==CAPS_PRESS){
        if(caps_enable==1){ //caps was already enabled before this was pressed, so change to lower case
            case_status=0;
            caps_enable=0;  //disabling caps
            pointer = &lower[0];
            flag=1;
        }
        else{
            caps_enable=1;  //enalbing caps
            case_status= 1; //if shift is pressed, then change the case=1 to denote upper case
            pointer= &caps[0];
            flag=1;
        }
    }
    //taking case of shift press
    if(key_pressed==LEFT_SHIFT_PRESS || key_pressed==RIGHT_SHIFT_PRESS){
        if(caps_enable==1){ //if caps is enabled and shift is pressed, then lowercase should be printed
            case_status=0;
            pointer = &lowershift[0];
            flag=1;
        }
        else{   //else go ahead and print the upper case
            case_status= 1; //if shift is pressed, then change the case=1 to denote upper case
            pointer= &uppershift[0];
            flag=1;
        }
    }

    //taking care of shift release
    if(key_pressed==LEFT_SHIFT_RELEASE || key_pressed==RIGHT_SHIFT_RELEASE ){
        if(caps_enable==1){ //if caps was already enabled, then go ahead and print upper case after release
            case_status= 1; //if shift is pressed, then change the case=1 to denote upper case
            pointer= &caps[0];
            flag=1;
        }
        else{   //else go ahead and print the lower case
            case_status=0;
            pointer = &lower[0];
            flag=1;
        }
    }
    if(key_pressed==LEFT_CONTROL_PRESS){
        control_status=1;   //control is enabled
        flag=1;
    }
    else if(key_pressed==LEFT_CONTROL_RELEASE){
        control_status=0;   //control is disabled
        flag=1;
    }
    if(key_pressed==LEFT_ALT_PRESS || key_pressed==RIGHT_ALT_PRESS){
        alt_status=1;   //alt is enabled
        flag=1; 
    }
    else if(key_pressed==LEFT_ALT_RELEASE || key_pressed==RIGHT_ALT_RELEASE ){
        alt_status=0;   //alt is disabled
        flag=1;
    }
    if(!flag){
        is_alphabet=1;  //no special key, it is a printable character otherwise
    }   
}

/*
 * cntl_l_check()
 * DESCRIPTION: checks for cntl+l
 *
 * INPUTS: scan code of char pressed    
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
int cntl_l_check(unsigned char data){
    char curr_key;
    int ret=0;
    curr_key = ascii_code[data][0]; //shift is not detected yet, so treat it as lower case
    if(control_status){
        if(curr_key== 'l' || curr_key== 'L'){   //case for cntl_l is satisfied, will clear the screen but not the buffer
            
            clear();
            ret = 1;
        }
    }
    return ret;
}

/*
 * key_noprint()
 * DESCRIPTION: checks for backspace and enter
 *
 * INPUTS: char pressed 
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
void key_noprint(unsigned char key_pressed){
    if (key_pressed == '\b'){   //if a backspace is recognised
        if(index != 0 && number_printed_line!=0)    {
            terminals[disp_term_k].keyboard_buf[index[disp_term_k]-1] = 0;
            index[disp_term_k]--;    //moving the indice of the buffer back to its prev position
            number_printed_line--;
            putc('\b'); //implementing it on the screen
            }       
        send_eoi(1);    //for irq1
        return;
    }
    else if (key_pressed == '\n'){  //space is detected
        terminals[disp_term_k].keyboard_buf[index[disp_term_k]] = '\n'; //setting a new line character onto the buffer
        if(terminals[disp_term_k].buffer_enable){
            enter_read[disp_term_k] = 1;
        }
        index[disp_term_k]++;
        number_printed_line=0;
        send_eoi(1);    //for irq1
        return;
    }
}

/*
 * open_terminal()
 * DESCRIPTION: opens the terminal
 * INPUTS: file 
 * OUTPUTS: none
 * SIDE EFFECTS: sets the flags accordingly and clears the screen
 */
int32_t open_terminal(const uint8_t *filename){
    return 0;
}


/*
 * read_terminal()
 * DESCRIPTION: reads from the terminal
 * INPUTS: file, buffer and the size
 * OUTPUTS: none
 * SIDE EFFECTS: reads the file
 */
int32_t read_terminal(int32_t fd, unsigned char *buf, int32_t num){
    int i;
    if(buf == NULL) return -1;
    terminals[disp_term_k].buffer_enable = 1;  //enabling the buffer to read
    //sti();
    //check enter flag for the current terminal 
    while(enter_read[disp_term_k] == 0);
    for(i = 0; i < num && i < KEYBOARDSIZE ; i++) {
        ((unsigned char *)buf)[i] = terminals[disp_term_k].keyboard_buf[i];
        if(terminals[disp_term_k].keyboard_buf[i] == '\n'){    //if a new line chracter is read, then stop reading
            break;
        }
    }
    memset(terminals[disp_term_k].keyboard_buf, 0, 128);   //resetting the buffer after an enter is detected
    index[disp_term_k] = 0;
    enter_read[disp_term_k] = 0;
    terminals[disp_term_k].buffer_enable = 0;
    return ++i;
}


/*
 * write_terminal()
 * DESCRIPTION: writes to the terminal
 * INPUTS: file, buffer and the size
 * OUTPUTS: none
 * SIDE EFFECTS: write on the file
 */
int32_t write_terminal(int32_t fd, const unsigned char *buf, int32_t num){
    int i;
    for(i = 0; i < num; i++) {
            putc(buf[i]);   //putting all the printable characters onto the screen
    }
    return i;
}


/*
 * close_terminal()
 * DESCRIPTION: closes the terminal
 * INPUTS: file
 * OUTPUTS: none
 * SIDE EFFECTS: closes the terminal
 */
int32_t close_terminal(int32_t fd){
    return -1; //-1 is failure
}

/*
 * read_failure
 * DESCRIPTION: returns -1 for file_ops jumptable 
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
int32_t read_failure (int32_t fd, unsigned char *buf, int32_t num){
    return -1; //-1 is failure
}

/*
 * write_failure
 * DESCRIPTION: returns -1 for file_ops jumptable 
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none
 */
int32_t write_failure (int32_t fd, const unsigned char *buf, int32_t num) {
    return -1; //-1 is failure
}
