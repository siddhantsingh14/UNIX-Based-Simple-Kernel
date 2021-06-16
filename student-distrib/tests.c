#include "tests.h"
#include "x86_desc.h"
#include "filesys.h"
#include "syscall.h"
//#include "lib.h"


#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");


static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Divide by zero check
 * 
 * Checks divide by zero instruction
 * Inputs: None
 * Outputs: None
 * Files: idt.c
 */
void dividebyzero(){
	TEST_HEADER;
	int temp;
	int one, two;
	one = 1;
	two = 0;
	temp = one/two;
}

/* Null pointer test for paging
 * 
 * Checks for page fault
 * Inputs: None
 * Outputs: None
 * Files: idt.c
 */
int null_ptr_test() {
	TEST_HEADER;
	int y;
	int result = FAIL; // passing is indicated with a page fault
	int * x = NULL;
	y = x[0];
	return result;
}


// add more tests here

/* Checkpoint 2 tests */

/* RTC test 
 * 
 * Prints 1 with different frequencies 
 * Inputs: None
 * Outputs: None
 * Files: rtc.c
 */
int rtc_full_test(){
	//int buffer[1];
	//buffer[0] = 2;
	int32_t num = 2;
	open_rtc(NULL);
	clear();
	while(num <= 1024){
		write_rtc(NULL, NULL, num);
		int i;
		for(i = 0; i < 20; i++){
			printf("1");
			read_rtc(NULL, NULL, 0);
		}
		num = num*2;
		//buffer[0] = 2*buffer[0];
		printf("\n");
	}
	close_rtc(NULL);
	return 1;
}

/* Terminal driver test
 * 
 * Checks if keyboard inputs are echoed by read and write terminal
 * Inputs: None
 * Outputs: None
 * Files: keyboard.c
 */
int terminal_driver_test()
{
	TEST_HEADER;
	clear();
	int32_t read_retval, write_retval;
	open_terminal(NULL);
	unsigned char buf[128];
	memset(buf, 0, 128);
	printf("Hi, what's your name? ");
	read_retval = read_terminal(NULL, buf, 128);
	printf("Hello, ");
	write_retval = write_terminal(NULL, buf, 128);
	close_terminal(NULL);
	return PASS;
}

/* Terminal Test
 * 
 * Checks if keyboard inputs are echoed by read and write terminal in a loop
 * Inputs: None
 * Outputs: None
 * Files: idt.c
 */
int terminal_test(){
	clear();
	unsigned char buf[128];
	memset(buf, 0, 128);
	while(1){
		printf("enter something ");
		read_terminal(NULL, buf, 128);
		printf("you entered ");
		write_terminal(NULL, buf, 128);
	}
	return 0;
}

/* terminal_read_write_test
 * checks functionality of terminal_read and terminal_write
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
 int ls_test() {
	TEST_HEADER;
	int result = PASS;

	int j; int x;
	uint8_t buf[32];
	memset(buf, 0, 32);
	uint8_t * string = (uint8_t*)".";
	directory_open(string);
	printf("File system contents: \n");
	for (j = 0; j < 15; j++) {
		directory_read(2, buf, 32);
		for (x = 0; x < 32; ++x)
			putc(buf[x]);	
		putc('\n');
		memset(buf, 0, 32);
	}
	directory_close(2);
	
	 return result;
 }

 /* file_read_test_
  * checks functionality of file_read
  * Inputs: None
  * Outputs: PASS/FAIL
  * Side Effects: None
  */
int file_read_test() {
	TEST_HEADER;
	int result = PASS;

	uint8_t* filename = (uint8_t*)"shell";
	uint8_t buf;
	dentry_t dentry;
	file_open(filename);
	printf("Opened file\n");
	//file_read(filename, buf, 187);
	//write_terminal(NULL, buf, 128);
	read_dentry_by_name(filename, &dentry);
	int index = 0;
	while(read_data(dentry.inode_num, index, &buf, 1) != 0){
		index++;
		putc(buf);
	}
	//write_terminal(NULL, buf, 187);
	printf("\n");
	file_close(2);
	printf("Closed file\n");
	return result;
}

/* Checkpoint 3 tests */

/* sys_write_test
 * check sys write works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int sys_write_test() {
	TEST_HEADER;

	//uint8_t * string = (uint8_t*)"hi there friends 892";
	void* buf[5];
	buf[0] = "h";
	buf[1] = "e";
	buf[2] = "l";
	buf[3] = "l";
	buf[4] = "o";
	//file_open(string);
	printf("starting open");
	open_terminal(NULL);
	printf("its opening");
	int i = write(1, buf, 20);
	printf("its writing");
	if(i == 20) return PASS;
	else return FAIL;
}

/* sys_read_terminal_test
 * checks if sys_read in terminal works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int sys_read_terminal_test() {
	TEST_HEADER;

	uint8_t buf[128];
	int j;
	open_terminal(NULL);
	printf("READING: \n");
	int i = read(0, buf, 128);
	printf("bytes read: %d\necho: ", i);
	for (j = 0; j < i; ++j) {
		putc(buf[j]);
	}
	return PASS;
}

/* sys_read_test
 * checks if sys_read works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int sys_read_test() {
	TEST_HEADER;

	uint8_t * string = (uint8_t*)"frame0.txt";
	uint8_t buf[187];
	open(string);
	int j = read(2, buf, 100);
	int i;
	for (i = 0; i < 100; ++i) {
		putc(buf[i]);
	}
	read(2, buf, 87);
	for (i = 0; i < 87; ++i) {
		putc(buf[i]);
	}
	if(j == 100) return PASS;
	else return FAIL;
}

/* open_syscall
 * checks if sys_read works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int open_syscall() {
	//printf("Attempting to open frame0.txt \n");
	uint8_t * string = (uint8_t*)"frame0.txt";
	open(string);
	//printf("Open in Syscall successful \n");
	return PASS;
}

/* sys_execute_test
 * checks if sys_execute in terminal works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int syscall_execute_test(){
	char* command = "testprint";
	execute ((uint8_t*)command);
	printf("execute worked");
	return PASS;
}

// /* sys_execute_test
//  * checks if sys_execute in terminal works
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  */
// int sys_execute_test() {
// 	TEST_HEADER;
// 	uint8_t * string = (uint8_t*)"cat";
// 	int32_t success = execute(string);
// 	printf("%d\n", success);
// 	string = (uint8_t*)"frame0.txt";
// 	success = execute(string);
// 	printf("%d\n", success);
// 	return PASS;
// }

// /* sys_execute_shell_test
//  * checks if shell in terminal works using sys execute
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: None
//  */
// int sys_execute_shell_test() {
// 	TEST_HEADER;
// 	uint8_t * string = (uint8_t*)"shell";
//     int32_t success;
//     while (1) {
//         success = execute(string);
//     }
// 	if (success == 0)	return PASS;
// 	else return FAIL;
// }
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here

	//idt_test();
	//dividebyzero();
	//null_ptr_test();
	file_read_test();
	//terminal_driver_test();
	//terminal_test();
	//rtc_full_test();
	//sys_write_test();
	//sys_read_terminal_test();
	TEST_OUTPUT("Open syscall test", open_syscall());
	TEST_OUTPUT("Syscall Execute Test", syscall_execute_test());
	//sys_execute_test();
	//sys_read_test();
	//TEST_OUTPUT("file_read_test", file_read_test());
	//TEST_OUTPUT("directory_read_test", ls_test());
}
