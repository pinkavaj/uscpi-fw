#include "config.h"
#include "drivers/usart.h"

// FIXME: remove
#include "scpi.h"
#include "lib/iobuf.h"

#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>

#define CHAR_RESET '\x03'

#define USART0_OUT_LEN 128
/* Input state, used to handle input events when error occurs,
 * drop all input until newline found, reset SCPI parser
 * - USART errors
 * CHAR_ERR_LINE - SCPI parser error accured*/
#define CHAR_ERR_ESC '\x00'
#define CHAR_ERR_FRAMING ((char)_BV(4))
#define CHAR_ERR_OVERFLOW ((char)_BV(3))
#define CHAR_ERR_PARITY ((char)_BV(2))
#define CHAR_ERR_LINE ((char)_BV(1))

#if FE0 != 4
#warning "FE0 != CHAR_ERR_FRAMING, this might lead to a bit larger code."
#endif
#if DOR0 != 3
#warning "DOR0 != CHAR_ERR_OVERFLOW, this might lead to a bit larger code."
#endif
#if UPE0 != 2
#warning "UPE0 != CHAR_ERR_PARITY, this might lead to a bit larger code."
#endif

/* Input buffer for asynchronous data recieving */
char USART0_in[USART0_IN_LEN];
/* USART0 input buffer state */
//static in_state_t USART0_in_state = in_state_reset_parser;
/* lenght of currently processed part of buffer, processing procedure may
 * modify all data in range <0, USART0_in_len -1>, but no touch others */
volatile USART0_in_len_t USART0_in_len = 0;
/* index to first free position to store newly recieved char */
static volatile USART0_in_len_t USART0_in_len_ = 0;

static void USART0_in_process(void);

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
	/* Anounce there is new data to proceed */
	/* FIXME: bug, not called when error during buffer filling */
	USART0_in_process();
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

/* Start processing bytes in input buffer,
 * suppose tath interrupts are disabled, but can be enabled. */
static void USART0_in_process(void)
{
	static volatile uint8_t USART0_in_process_lock = 0;
	static uint8_t char_err = 0;
	char c;
	SCPI_parse_t ret;

	/* parsing already in progress or no data to parse */
	if (USART0_in_process_lock)
		return;

	USART0_in_process_lock++;
	while (USART0_in_len < USART0_in_len_) {
		c = USART0_in[USART0_in_len++];
		if (c == CHAR_ERR_ESC) {
			c = USART0_in[USART0_in_len++];
			if (c != CHAR_ERR_ESC) {
				char_err = c;
				if (char_err & CHAR_ERR_PARITY)
					SCPI_err_set(&SCPI_err_361);
				if (char_err & CHAR_ERR_FRAMING)
					SCPI_err_set(&SCPI_err_362);
				if (char_err & CHAR_ERR_OVERFLOW)
					SCPI_err_set(&SCPI_err_363);
			}
		}

		sei();
		ret = char_err ? SCPI_parse_err : SCPI_parse(c);
		cli();
		switch (ret)
		{
			default:
			case SCPI_parse_err:
				char_err = CHAR_ERR_LINE;
			case SCPI_parse_end:
			case SCPI_parse_drop_all:
				USART0_in_len_ -= USART0_in_len;
				memmove(USART0_in, USART0_in + USART0_in_len, USART0_in_len_);
				USART0_in_len = 0;
				break;
			case SCPI_parse_drop_last:
				memmove(USART0_in + USART0_in_len - 1, 
						USART0_in + USART0_in_len, 
						USART0_in_len_ - USART0_in_len);
				USART0_in_len--;
				USART0_in_len_--;
				break;
			case SCPI_parse_drop_str:
				{
					uint8_t idx = 0;
					while (USART0_in[idx]) {
						if (idx == USART0_in_len)
							break;
						idx++;
						USART0_in_len_--;
					};
					
					if (USART0_in[idx] == 0 && idx < USART0_in_len) {
						idx++;
						USART0_in_len_--;
					}
					memmove(USART0_in, USART0_in + idx, USART0_in_len_);
				}

				break;
			case SCPI_parse_more:
				break;
		}
		if (c == '\n') {
			SCPI_parser_reset();
			char_err = 0;
		}
	}
	USART0_in_process_lock--;
}

/* Allow interrup to start sending */
void USART0_start_sending(void)
{
	UCSR0B |= _BV(UDRIE0);
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
