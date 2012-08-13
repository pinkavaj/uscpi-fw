#include "drivers/timer.h"

#include <avr/interrupt.h>

/* Timer 1 */
/* 2**14 3**3 5**2 */
#define TIM1_PERIOD_PER_SEC 32 /* timer 1 periods per second */
#define TIM1_PERIOD 43200 /* 31.25ms @ f0/8 */
#define TIM1_PERIOD_DAQ 1728 /* 1.25ms @ f0/8 */
volatile uint32_t time_sec;	// MCU s counter
volatile uint8_t loop_counter;	// 0-31 (4x 8x 31.25 ms = 1000 ms)
volatile uint8_t timer1A_spinlock;
volatile uint8_t timer1B_spinlock;

/*
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
	if (loop_counter == TIM1_PERIOD_PER_SEC) {
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

void TIMER1_init(void)
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

void TIMER1_start(void)
{
	/* clk source f/8 */
	TCCR1B |= _BV(CS11);
}

