/*****************************************************************************
 *     ***  TIM1  16-bit Timer1 A & B  ***
 *
 * Copyright (c) 2010 Lukas Kucera <lukas.kucera@vscht.cz>, 
 * 	Jiri Pinkava <jiri.pinkava@vscht.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *****************************************************************************/

#include "drivers/timer.h"

#include <avr/interrupt.h>

#define TIMER1_CLK_DISABLE	(0<<CS10)
#define TIMER1_CLK_f_1		(1<<CS10)
#define TIMER1_CLK_f_8		(2<<CS10)
#define TIMER1_CLK_f_64		(3<<CS10)
#define TIMER1_CLK_f_256	(4<<CS10)
#define TIMER1_CLK_f_1024	(5<<CS10)
#define TIMER1_CLK_EXT_FALL	(6<<CS10)
#define TIMER1_CLK_EXT_RAISE	(7<<CS10)
#define TIMER1_CLK_MASK		(7<<CS10)

#define SPIN_LOCK(sl) do { while(!sl) ;; sl = 0; } while(0)

volatile uint8_t period_counter;	// 0-31 (4x 8x 31.25 ms = 1000 ms)
volatile uint32_t time_sec;
volatile uint8_t timer1_period_sl;
volatile uint8_t timer1B_spinlock;

/*****************************************************************************/
void TIMER1_mode_CTC_A(void)
{
	TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
	TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
}

/*****************************************************************************/
void TIMER1_init(void)
{
	TCNT1 = 0;
	OCR1A = TIMER1_TICKS_PER_PERIOD;
	TIMER1_mode_CTC_A();
	TIMSK = _BV(OCIE1A);
//	TIMSK = _BV(TOIE1);

	time_sec = 0;
	period_counter = 0;

	timer1_period_sl = 0;
	timer1B_spinlock = 0;
}

/*****************************************************************************/
void TIMER1_start(void)
{
	TCCR1B = (TCCR1B & ~TIMER1_CLK_MASK) | TIMER1_CLK_f_8;
}

/*****************************************************************************/
void TIMER1_jiff_alarm(uint8_t jiffer_idx)
{
	OCR1B = (uint16_t)TIMER1_TICKS_PER_JIFFER * jiffer_idx;
	TIFR |= _BV(OCF1B);
	TIMSK |= _BV(OCIE1B);
	SPIN_LOCK(timer1B_spinlock);
	TIMSK &= ~_BV(OCIE1B);
}

/*****************************************************************************/
void TIMER1_delay_rel(uint16_t ticks)
{
	ticks += OCR1B;
        if (ticks > TIMER1_TICKS_PER_PERIOD ||
                        ticks < OCR1B)
                ticks -= TIMER1_TICKS_PER_PERIOD;
	OCR1B = ticks;
	TIFR |= _BV(OCF1B);
	TIMSK |= _BV(OCIE1B);
	timer1B_spinlock = 0;
	SPIN_LOCK(timer1B_spinlock);
}

/*****************************************************************************/
void TIMER1_delay_rel_reset(void)
{
	OCR1B = TCNT1;
}

uint8_t TIMER1_new_period(void)
{
        if (timer1_period_sl) {
                timer1_period_sl = 0;
                return 1;
        }
        return 0;
}

/*****************************************************************************/
/* Called in 1.25ms interval */
ISR(TIMER1_COMPB_vect)
{
	timer1B_spinlock = 1;
}

/*****************************************************************************/
/* Called in 31.25ms */
ISR(TIMER1_COMPA_vect)
//ISR(TIMER1_OVF_vect)
{
	timer1_period_sl = 1;
	period_counter++;
	if (period_counter == TIMER1_PERIODS_PER_SEC) {
		period_counter = 0;
		time_sec++;
	}
}

