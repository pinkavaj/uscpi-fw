#ifndef __VALVE_RAW_H__
#define __VALVE_RAW_H__

/** Driver for valves attached directly to GPIO port. */

#include <inttypes.h>


void valves_raw_init(void);

void valve_raw_C0_close(void);
void valve_raw_C0_open(void);
uint8_t valve_raw_C0_state(void);
void valve_raw_C1_close(void);
void valve_raw_C1_open(void);
uint8_t valve_raw_C1_state(void);
void valve_raw_C2_close(void);
void valve_raw_C2_open(void);
uint8_t valve_raw_C2_state(void);

#endif
