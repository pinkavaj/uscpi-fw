#include "config.h"
#include "drivers/usart.h"

#include "scpi.h"
#include "lib/iobuf.h"

#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>

#define CHAR_RESET '\x03'

#define USART0_OUT_LEN 128

/* Input buffer for asynchronous data recieving */
char USART0_in[USART0_IN_LEN];
/* USART0 input buffer state */
//static in_state_t USART0_in_state = in_state_reset_parser;
/* lenght of currently processed part of buffer, processing procedure may
 * modify all data in range <0, USART0_in_len -1>, but no touch others */
volatile USART0_in_len_t USART0_in_len = 0;
/* index to first free position to store newly recieved char */
volatile USART0_in_len_t USART0_in_len_ = 0;

ISR(USART0_RXC_vect)
{
	uint8_t c;
	uint8_t uerr;

	/* Get USART error and then coresponding char */
	uerr = UCSR0A & (_BV(FE0) | _BV(DOR0) | _BV(UPE0));
	c = UDR0;
	if (c == CHAR_RESET && !uerr) {
		__asm__(" jmp 0x0 ");
	}
	do {
		/* buffer totaly full */
		if (USART0_in_len_ >= (USART0_IN_LEN - 1))
			break;
		/* handle low level errors on USART */
		if (uerr) {
			USART0_in[USART0_in_len_++] = CHAR_ERR_ESC;
			c = 0;
			if (uerr & _BV(UPE0))
				c |= CHAR_ERR_PARITY;
			if (uerr & _BV(FE0))
				c |= CHAR_ERR_FRAMING;
			if (uerr & _BV(DOR0) || USART0_in_len_ == 
					(USART0_IN_LEN - 2))
				c |= CHAR_ERR_OVERFLOW;
			
			USART0_in[USART0_in_len_++] = c;
			break;
		}
		/* input buffer full */
		if (USART0_in_len_ >= (USART0_IN_LEN - 2)) {
			USART0_in[USART0_in_len_++] = CHAR_ERR_ESC;
			USART0_in[USART0_in_len_++] = CHAR_ERR_OVERFLOW;
			break;
		}
		USART0_in[USART0_in_len_++] = c;
		if (c == CHAR_ERR_ESC) {
			if (USART0_in_len_ >= (USART0_IN_LEN - 2)) {
				USART0_in[USART0_in_len_++] = CHAR_ERR_OVERFLOW;
				break;
			}
			USART0_in[USART0_in_len_++] = c;
		}
	} while(0);
}

/* USART0_TXC_vect */

/* Send data from output buffer, enable interrupt UDRIE to start */
ISR(USART0_UDRE_vect)
{
	UDR0 = popc();
	/* No more data to send, stop sending */
	if (stdout_isempty()) {
		UCSR0B &= ~_BV(UDRIE0);
	}
}

/* USART1_RXC_vect
   USART1_TXC_vect
   USART1_UDRE_vect
   */

void USART0_init(void)
{
#define BAUD USART0_BAUD
#include <util/setbaud.h>
	/* disable recieving - flush buffer */
	UCSR0B = 0;
	SCPI_parser_reset();
	/* set baud rate, X freq is defined in Makefile */
        UBRR0L = UBRRL_VALUE;
        UBRR0H = UBRRH_VALUE;
#if USE_2X
        UCSR0A = _BV(U2X0);
#else
        UCSR0A = 0;
#endif
	/* Async, no parity, 1stop bit, 8bit */
	UCSR0C = _BV(URSEL0) | _BV(UCSZ00) | _BV(UCSZ01);
	/* disable pull-up on RxD */
	PORTD &= ~(1<<PD0);
	/* enable Rx and Recieve Complete Interrupt */
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}

/* Allow interrup to start sending */
void USART0_start_sending(void)
{
	UCSR0B |= _BV(UDRIE0);
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
