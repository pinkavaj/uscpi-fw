#ifndef __TIMER1_H__
#define __TIMER1_H__

#include <inttypes.h>

/* Timer 1 */
/* 2**14 3**3 5**2 */
#define TIMER1_TICKS_PER_JIFFER 1728 /* 1.25ms @ f0/8 */
#define TIMER1_TICKS_PER_PERIOD 43200 /* 31.25ms @ f0/8 */
#define TIMER1_TICKS_PER_PLC ((TIMER1_TICKS_PER_JIFFER * 16)) /* 20ms */
#define TIMER1_PERIODS_PER_SEC 32 /* timer 1 periods per second */

/* timeline timed by timer 1 */
#define TIM_OFFS_DAC 2
#define TIM_OFFS_20 20

extern volatile uint32_t time_sec;	// MCU s counter
extern volatile uint8_t period_counter;	// 0-31 (4x 8x 31.25 ms = 1000 ms)

void TIMER1_init(void);
void TIMER1_start(void);
void TIMER1_jiff_alarm(uint8_t jiffer_idx);
void TIMER1_delay_rel(uint16_t ticks);
void TIMER1_delay_rel_reset(void);

#endif

