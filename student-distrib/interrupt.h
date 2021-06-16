#ifndef HANDLERS_H_
#define HANDLERS_H_

#ifndef ASM

#include "keyboard.h"
#include "rtc.h"
#include "sched.h"

/* Handlers for Intel */
extern void handle_SIG_DE(void);  			/* ISR for SIG_DE */
extern void handle_SIG_DB(void);  			/* ISR for SIG_DB */
extern void handle_SIG_NMI(void); 			/* ISR for SIG_NMI*/
extern void handle_SIG_BP(void);  			/* ISR for SIG_BP */
extern void handle_SIG_OF(void);  			/* ISR for SIG_OF */
extern void handle_SIG_BR(void);  			/* ISR for SIG_BR */
extern void handle_SIG_UD(void);  			/* ISR for SIG_UD */
extern void handle_SIG_NM(void);  			/* ISR for SIG_NM */
extern void handle_SIG_DF(void);  			/* ISR for SIG_DF */
extern void handle_SIG_SO(void);  			/* ISR for SIG_SO */
extern void handle_SIG_TS(void);  			/* ISR for SIG_TS */
extern void handle_SIG_NP(void);  			/* ISR for SIG_NP */
extern void handle_SIG_SS(void);  			/* ISR for SIG_SS */
extern void handle_SIG_GP(void);  			/* ISR for SIG_GP */
extern void handle_SIG_PF(void);  			/* ISR for SIG_PF */
extern void handle_SIG_MF(void);  			/* ISR for SIG_MF */
extern void handle_SIG_AC(void);  			/* ISR for SIG_AC */
extern void handle_SIG_MC(void);  			/* ISR for SIG_MC */
extern void handle_SIG_XF(void);  			/* ISR for SIG_XF */

/* Device ISRs */
extern void handle_SIG_RTC(void);			/* ISR for RTC 	  */
extern void handle_SIG_PIT(void);			/* ISR for PIT 	  */
extern void handle_SIG_KBD(void);			/* ISR for KBD	  */

/* System Call interrupt */
extern void handle_SIG_SYSCALL(void); 		/* ISR for SIG_SYSCALL*/

/* Add a handler for undefined interrupts just in case */
extern void handle_SIG_IDK(void);

#endif /* ASM */
#endif /* HANDLERS_H_ */

