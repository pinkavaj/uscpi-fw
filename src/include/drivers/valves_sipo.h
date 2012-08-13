#ifndef __VALVES_SIPO_H__
#define __VALVES_SIPO_H__

/** Driver for valves attached trought SIPO register. */

#include <inttypes.h>

void valves_sipo_init(void);
void valves_sipo1_timeout(void);

void valve_sipo1_0_close(void);
void valve_sipo1_0_open(void);
uint8_t valve_sipo1_0_state(void);

void valve_sipo1_1_close(void);
void valve_sipo1_1_open(void);
uint8_t valve_sipo1_1_state(void);

void valve_sipo1_2_close(void);
void valve_sipo1_2_open(void);
uint8_t valve_sipo1_2_state(void);

void valve_sipo1_3_close(void);
void valve_sipo1_3_open(void);
uint8_t valve_sipo1_3_state(void);

#endif
