#include "config.h"
#include "spi.h"
#include "timer.h"
#include "usart.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>

/* ** Interrupt vector ** */
/*
 ANA_COMP_vect
 EE_RDY_vect
 INT0_vect
 INT1_vect
 INT2_vect
 PCINT0_vect
 PCINT1_vect
 SPI_STC_vect
 SPM_RDY_vect
 */

/* handle all interupts in interrupt vector which does not have definet 
 * function */
/* BADISR_vect */

void main(void)
{
	cli();
//	SPI_init();
//	TIMER1_init();
	USART0_init();

//	TIMER1_start();
	
	sei();
	do {	
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
	} while(1);
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
