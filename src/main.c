#include "config.h"
#include "drivers/spi_dev.h"
#include "drivers/timer.h"
#include "drivers/usart.h"
#include "lib/iobuf.h"
#include "lib/temp.h"
#include "scpi.h"

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
	SPI_dev_init();
	TIMER1_init();
	USART0_init();
        temp_init();
	stdout_buf_reset();

	TIMER1_start();
	
	sei();
	do {	
                temp_loop();
	        SCPI_loop();
	} while(1);
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
