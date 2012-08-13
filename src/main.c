#include "config.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <ctype.h>
#include <string.h>
#include <util/atomic.h>
#include <util/setbaud.h>

#include "scpi.h"

#define CHAR_RESET '\x03'
#define PRINT_DONE 0
#define USART0_OUT_LEN 128

/* Input state, used to handle input events when error occurs
 * need_newline - drop all chars until new newline
 * reset_parser - stop parsing current line and reset parser */
#define CHAR_ERR_ESC '\x00'
#define CHAR_ERR_FRAMING ((char)_BV(4))
#define CHAR_ERR_OVERFLOW ((char)_BV(3))
#define CHAR_ERR_PARITY ((char)_BV(2))

/* Timer 1 */
/* 2**14 3**3 5**2 */
#define TIM1_PERIOD_PER_SEC 32 /* timer 1 periods per second */
#define TIM1_PERIOD 43200 /* 31.25ms @ f0/8 */
#define TIM1_PERIOD_DAQ 1728 /* 1.25ms @ f0/8 */
volatile uint32_t time_sec = 0;	// MCU s counter
static volatile uint8_t loop_counter = 0;	// 0-31 (4x 8x 31.25 ms = 1000 ms)
static volatile uint8_t timer1A_spinlock = 0;
static volatile uint8_t timer1B_spinlock = 0;
//uint8_t
#define SPIN_LOCK(sl) do { while(!sl) ;; sl = 0; } while(0)

/* Input buffer for asynchronous data recieving */
char USART0_in[USART0_IN_LEN];
/* USART0 input buffer state */
//static in_state_t USART0_in_state = in_state_reset_parser;
/* lenght of currently processed part of buffer, processing procedure may
 * modify all data in range <0, USART0_in_len -1>, but no touch others */
volatile USART0_in_len_t USART0_in_len = 0;
/* index to first free position to store newly recieved char */
static volatile USART0_in_len_t USART0_in_len_ = 0;
/* Output buffer for asynchronous sending */
static char USART0_out[USART0_OUT_LEN];
/* position where put new character to buffer */
static volatile uint8_t USART0_out_len = 0;
/* position from where get character to send */
static volatile uint8_t USART0_out_len_ = 0;

void USART0_in_process(void);

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
 TIMER0_COMP_vect
 TIMER0_OVF_vect
 TIMER1_CAPT_vect
 TIMER1_COMPA_vect
*/

/* Called in 1.25ms interval */
ISR(TIMER1_COMPB_vect)
{
	timer1B_spinlock = 1;
}

/* Called in 31.25ms */
ISR(TIMER1_OVF_vect)
{
	timer1A_spinlock = 1;
	loop_counter++;
	if (loop_counter == TIM1_PERIOD_PER_SEC - 1) {
		loop_counter = 0;
		time_sec++;
	}
}
/*
 TIMER2_COMP_vect
 TIMER2_OVF_vect
 TIMER3_CAPT_vect
 TIMER3_COMPA_vect
 TIMER3_COMPB_vect
 TIMER3_OVF_vect
 */
ISR(USART0_RXC_vect)
{
	uint8_t c;
	uint8_t uerr;

	/* Get USART error and then coresponding char */
	uerr = UCSR0A & (_BV(FE0) | _BV(DOR0) | _BV(UPE0));
	c = UDR0;
	if (c == CHAR_RESET && !uerr)
		__asm__(" jmp __vectors ");
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
			if (uerr & _BV(DOR0) || USART0_in_len_ >= (USART0_IN_LEN - 2))
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
		if (!c) {
			if (USART0_in_len_ >= (USART0_IN_LEN - 2)) {
				USART0_in[USART0_in_len_++] = CHAR_ERR_OVERFLOW;
				break;
			}
			USART0_in[USART0_in_len_++] = c;
		}
	} while(0);
	/* Anounce there is new data to proceed */
	/* FIXME: bug, not called when eror during buffer filling */
	USART0_in_process();
}

/* USART0_TXC_vect */

/* Send data from output buffer, activated by int. enabling */
ISR(USART0_UDRE_vect)
{
	do {
		UDR0 = USART0_out[USART0_out_len_++];
		/* No more data to send, stop sending, reset buffer */
		if (USART0_out_len == USART0_out_len_) {
			UCSR0B &= ~_BV(UDRIE0);
			USART0_out_len = 0;
			USART0_out_len_ = 0;
			return;
		}
		/* until we may put something to send buffer */
	} while(UCSR0A & _BV(UDRE0));
}

/* USART1_RXC_vect
   USART1_TXC_vect
   USART1_UDRE_vect
   */

/* handle all interupts in interrupt vector which does not have definet 
 * function */
/* BADISR_vect */

static void TIMER1_init(void)
{
	/* OC1A, OC1B disconnected, no force output compare */
	/* fast PWM, TOP = ICR1 */
	TCCR1A = _BV(WGM11) | _BV(WGM10);
	/* fast PWM, TOP = ICR1 */
	TCCR1B = _BV(WGM12) | _BV(WGM13);
	TCNT1 = 0;
	/* Timer period settings */
	OCR1A = TIM1_PERIOD;
	OCR1B = TIM1_PERIOD_DAQ;
	/* enable interrupts for time overflow, compare match A and B */
	TIMSK = _BV(TOIE1) | _BV(OCIE1B);
}

static void TIMER1_start(void)
{
	/* clk source f/8 */
	TCCR1B |= _BV(CS11);
}

static void USART0_init(void)
{
	/* disable recieving/sending -> flush buffers */
	UCSR0B &= 0; /* ~(_BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0)); */
	/* set baud rate, X freq is defined in Makefile */
        UBRR0L = UBRRL_VALUE;
        UBRR0H = UBRRH_VALUE;
#if USE_2X
        UCSR0A |= _BV(U2X0);
#else
        UCSR0A &= ~(_BV(U2X0) | _BV(MPCM0) );
#endif
	/* Async, no parity, 1stop bit, 8bit */
	UCSR0C = _BV(URSEL0) | _BV(UCSZ00) | _BV(UCSZ01);
	/* enable Tx, Rx and Recieve Complete Interrupt */
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}

/* Print len bytes into output buffer */
void USART0_printn(const char *c, int len)
{
	if (!len)
		return;
	cli();
	/* Allow interrup to start sending */
	UCSR0B |= _BV(UDRIE0);
	if (USART0_out_len < USART0_OUT_LEN) {
USART0_printn_put:
		do {
			USART0_out[USART0_out_len++] = *(c++);
		} while (--len > 0 && USART0_out_len < USART0_OUT_LEN);
		if (len)
			goto USART0_printn_move;
		sei(); 
		return;
	} 
USART0_printn_move:
	if (USART0_out_len_ > 0) {
		memmove((char *)USART0_out, 
				(char *)USART0_out + USART0_out_len_,
				USART0_OUT_LEN - USART0_out_len_);
		USART0_out_len -= USART0_out_len_;
		USART0_out_len_ = 0;
		goto USART0_printn_put;
	}
	sei();
	sleep_mode();
	cli();
	goto USART0_printn_move;
}

/* Print string from Program memory into output buffer,
 * interrupts will be enabled by this rutine! */
void USART0_print_P(PGM_P c)
{
	cli();
	/* Allow interrup to start sending */
	UCSR0B |= _BV(UDRIE0);
	if (USART0_out_len < USART0_OUT_LEN) {
		uint8_t c_;
USART0_printn_put:
		do {
			c_ = pgm_read_byte(c++);
			if (!c_) {
				sei(); 
				return;
			}
			USART0_out[USART0_out_len++] = c_;
		} while (USART0_out_len < USART0_OUT_LEN);
	} 
	do {
		if (USART0_out_len_ > 0) {
			memmove((char *)USART0_out, 
					(char *)USART0_out + USART0_out_len_,
					USART0_OUT_LEN - USART0_out_len_);
			USART0_out_len -= USART0_out_len_;
			USART0_out_len_ = 0;
			goto USART0_printn_put;
		}
		sei();
		sleep_mode();
		cli();
	} while(1);
}

/* Start processing bytes in input buffer,
 * suppose tath interrupts are disabled, but can be enabled. */
void USART0_in_process(void)
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
		if (!c) {
			c = USART0_in[USART0_in_len++];
			if (c) {
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
			case SCPI_parse_end:
			case SCPI_parse_flush:
				memmove(USART0_in, USART0_in + USART0_in_len, 
						USART0_in_len_ - USART0_in_len);
				USART0_in_len_ -= USART0_in_len;
				USART0_in_len = 0;
				break;
			case SCPI_parse_flush_last:
				memmove(USART0_in + USART0_in_len - 1, 
						USART0_in + USART0_in_len, 
						USART0_in_len_ - USART0_in_len);
				USART0_in_len--;
				USART0_in_len_--;
				break;
			case SCPI_parse_cont:
				break;
		}
		if (c == '\n') {
			SCPI_parser_reset();
			char_err = 0;
		}
	}
	USART0_in_process_lock--;
}

/* Put character into output buffer */
void USART0_putc(char c)
{
	cli();
	if (USART0_out_len < USART0_OUT_LEN) {
USART0_putc_put:
		USART0_out[USART0_out_len++] = c;
		UCSR0B |= _BV(UDRIE0);
		sei(); 
		return;
	} 
USART0_putc_move:
	if (USART0_out_len_ > 0) {
		memmove((char *)USART0_out, 
				(char *)USART0_out + USART0_out_len_,
				USART0_OUT_LEN - USART0_out_len_);
		USART0_out_len -= USART0_out_len_;
		USART0_out_len_ = 0;
		goto USART0_putc_put;
	}
	sei();
	sleep_mode();
	cli();
	goto USART0_putc_move;
}

// Example of DAQ cycle
void x(void)
{
	SPIN_LOCK(timer1A_spinlock);
	// neco
	SPIN_LOCK(timer1B_spinlock);
	// DAQ 1
	SPIN_LOCK(timer1B_spinlock);
	// DAQ 2
	SPIN_LOCK(timer1B_spinlock);
	// neco
}

void main(void)
{
	TIMER1_init();
	USART0_init();
	TIMER1_start();
	
	sei();
	do {	
		x();
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
	} while(1);
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
