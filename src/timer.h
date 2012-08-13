#ifndef __TIMER_H__
#define __TIMER_H__

#include <inttypes.h>

#define SPIN_LOCK(sl) do { while(!sl) ;; sl = 0; } while(0)

volatile uint32_t time_sec;	// MCU s counter
volatile uint8_t loop_counter;	// 0-31 (4x 8x 31.25 ms = 1000 ms)
volatile uint8_t timer1A_spinlock;
volatile uint8_t timer1B_spinlock;

void TIMER1_init(void);
void TIMER1_start(void);

#endif

