#define ASM     1

#include "interrupt.h"
#include "idt.h"
//#include "i8259.h"
//#include "system_calls.h"
#include "x86_desc.h"
#include "types.h"
//#include "errors.h"

/* handle_SIG_DE
 * 	DESCRIPTION: ISR for SIG_DE signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_DE
handle_SIG_DE:
		pushal							# push all registers
		pushl $SIG_DE					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

/* handle_SIG_DB
 * 	DESCRIPTION: ISR for SIG_DB signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_DB
handle_SIG_DB:
		pushal							# push all registers
		pushl $SIG_DB					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

/* handle_SIG_NMI
 * 	DESCRIPTION: ISR for SIG_NMI signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_NMI
handle_SIG_NMI:
		pushal							# push all registers
		pushl $SIG_NMI					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_BP
 * 	DESCRIPTION: ISR for SIG_BP signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_BP
handle_SIG_BP:
		pushal							# push all registers
		pushl $SIG_BP					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_OF
 * 	DESCRIPTION: ISR for SIG_OF signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_OF
handle_SIG_OF:
		pushal							# push all registers
		pushl $SIG_OF					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_BR
 * 	DESCRIPTION: ISR for SIG_BR signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_BR
handle_SIG_BR:
		pushal							# push all registers
		pushl $SIG_BR					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_UD
 * 	DESCRIPTION: ISR for SIG_UD signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_UD
handle_SIG_UD:
		pushal							# push all registers
		pushl $SIG_UD					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_NM
 * 	DESCRIPTION: ISR for SIG_NM signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_NM
handle_SIG_NM:
		pushal							# push all registers
		pushl $SIG_NM					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_DF
 * 	DESCRIPTION: ISR for SIG_DF signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_DF
handle_SIG_DF:
		pushal							# push all registers
		pushl $SIG_DF   				# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_SO
 * 	DESCRIPTION: ISR for SIG_SO signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_SO
handle_SIG_SO:
		pushal							# push all registers
		pushl $SIG_SO					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_TS
 * 	DESCRIPTION: ISR for SIG_TS signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_TS
handle_SIG_TS:
		pushal							# push all registers
		pushl $SIG_TS					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_NP
 * 	DESCRIPTION: ISR for SIG_NP signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_NP
handle_SIG_NP:
		pushal							# push all registers
		pushl $SIG_NP					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_SS
 * 	DESCRIPTION: ISR for SIG_SS signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_SS
handle_SIG_SS:
		pushal							# push all registers
		pushl $SIG_SS					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_GP
 * 	DESCRIPTION: ISR for SIG_GP signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_GP
handle_SIG_GP:
		pushal							# Push registers
		pushfl							# Push flags
		pushl $SIG_GP					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popfl							# restore flags
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_PF
 * 	DESCRIPTION: ISR for SIG_PF signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_PF
handle_SIG_PF:
		pushal							# push all registers
		pushl $SIG_PF					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		popl %eax						# pop error code
		iret							# return from interrupt

 /* handle_SIG_MF
 * 	DESCRIPTION: ISR for SIG_MF signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_MF
handle_SIG_MF:
		pushal							# push all registers
		pushl $SIG_MF					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_AC
 * 	DESCRIPTION: ISR for SIG_AC signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_AC
handle_SIG_AC:
		pushal							# push all registers
		pushl $SIG_AC					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_MC
 * 	DESCRIPTION: ISR for SIG_MC signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_MC
handle_SIG_MC:
		pushal							# push all registers
		pushl $SIG_MC					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_XF
 * 	DESCRIPTION: ISR for SIG_XF signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_XF
handle_SIG_XF:
		pushal							# push all registers
		pushl $SIG_XF					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		iret							# return from interrupt

 /* handle_SIG_RTC
 * 	DESCRIPTION: ISR for SIG_RTC signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_RTC
handle_SIG_RTC:
		#pushfl							# Push flags
		pushal							# Push registers
		call rtc_handler				# Call rtc_handler()
		popal							# Restore registers
		#popfl							# Restore flags
		#sti
		iret							# Return from interrupt

 /* handle_SIG_KBD
 * 	DESCRIPTION: ISR for SIG_KBD signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_KBD
handle_SIG_KBD:
		cli
		pushal							# Push registers
		#pushfl							# Push flags
		call keyboard_handler			# call keyboard_handler()
		#popfl		 					# Restore flags
		popal							# Restore registers
		sti
		iret							# return from interrupt

 /* handle_SIG_PIT
 * 	DESCRIPTION: ISR for SIG_PIT signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_PIT
handle_SIG_PIT:
		cli
		pushal							# Push registers
		#pushfl							# Push flags
		call pit_handler				# call pit_handler()
		#popfl		 					# Restore flags
		popal							# Restore registers
		sti
		iret							# return from interrupt

 /* handle_SIG_IDK
 * 	DESCRIPTION: ISR for SIG_DIK signal
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_IDK
handle_SIG_IDK:
		pushfl							# save flags
		pushal							# push all registers
		pushl $SIG_IDK					# push the vector number
		call handle_exception			# call exception handler
		addl $4, %esp					# remove vector number
		popal							# restore registers
		popfl							# restore flags
		iret							# return from interrupt

 /* handle_SIG_SYSCALL
 * 	DESCRIPTION: ISR for SIG_SYSCALL signal. EAX holds the index of
 				 the system call.
 * 	INPUTS: None
 * 	OUTPUTS: None
 * 	RETURN VALUE: None
 * 	SIDE EFFECTS: None
 */
.global handle_SIG_SYSCALL
handle_SIG_SYSCALL:
													# BOOKKEEPING
		pushfl										# Push flags
		pushl %ebp									# Push ebp
		pushl %edi									# Push edi
		pushl %esi									# Push esi
		pushl %edx									# Push edx, arg 3
		pushl %ecx									# Push ecx, arg 2
		pushl %ebx									# Push ebx, arg 1
													# END BOOKKEEPING

													# CHECK FOR VALID SYSCALL NUM
		cmpl $0, %eax						# Check lower bound for syscall num
		jle syscall_error							# Return error
		cmpl $10, %eax						# Check upper bound for syscall num
		jg syscall_error							# Return error

													# CALL THE SYSTEM CALL
		call *sys_call_table(, %eax, 4)				# call exception handler
		jmp syscall_return							# Return from syscall

syscall_error:
													# PLACE ERROR IN RETURN VALUE
													# TODO: Use error defines
		movl $-1, %eax								# place -1 into eax

syscall_return:
													# BOOKEEPING
		popl %ebx									# Restore ebx
		popl %ecx									# Restore ecx
		popl %edx									# Restore edx
		popl %esi									# Restore esi
		popl %edi									# Restore edi
		popl %ebp									# Restore ebp
		popfl										# restore flags
													# END BOOKKEPING

													# RETURN FROM SYS CALL
		iret										# return from interrupt

/* Jump table for the system calls. Each system call has an index provided by eax. */
sys_call_table:
	.long 0x0, halt, execute, read, write, open, close, getargs, vidmap
	#, set_handler, sigreturn

/* portal_to_user_land
 * 	DESCRIPTION: Sets up the stack and returns control to the user space
 *				 starting at the entry point of the program to run.
 *	INPUTS: entry_addr - address of the start of the program
 *	OUTPUTS: None.
 *	RETURN VALUE: None.
 *	SIDE EFFECTS: None.
 *
 *  REFERENCE: https://c9x.me/x86/html/file_module_x86_id_145.html
 *
 */
 .global run_user_program
run_user_program:

													# SAVE PARAMETERS
 		movl 4(%esp), %eax							# Save instr_ptr into eax
 		movl 8(%esp), %ebx							# Save stack_ptr into ebx

													# EMULATE AN "int xxx" CALL
 		pushl $USER_DS								# Push the user's data segment
 		pushl %ebx									# Push stack pointer
 		pushfl										# Push flags
 		pushl $USER_CS								# Push the users' code segment
		pushl %eax									# Push the instr_ptr of the prgm

													# RETURN TO USER CODE
		iret										# Return to instr_ptr of prgm

/* return_from_user_program
 * 	DESCRIPTION: Returns control to the calling process of the program.
 *				 The syscall handler should have pushed eip onto the
 *				 the stack when it 'call'ed the system call function.
 *
 * 	INPUTS: par_ksp - kernel stack pointer of the parent
 *			par_kbp - kernel base pointer of the parent
 *	OUTPUTS: None.
 * 	RETURN VALUE: ret_val - return value of the called program
 * 	SIDE EFFECTS: None.
 */
 .global return_from_user_program
 return_from_user_program:

													# RESTORE REGISTERS TO PARENT STATUS
		popl %eax									# Dont need this return address
		popl %eax									# Place return value into eax
		popl %ebp									# Place parent's kbp into ebp
		popl %esp									# Place parent's ksp into esp


													# RETURN TO SYSCALL HANDLER
 		leave										# movl ebp, esp
 													# popl ebp
 		ret											# popl eip
