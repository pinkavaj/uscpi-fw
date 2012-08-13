#ifndef __VALVES_H__
#define __VALVES_H__

#include <inttypes.h>

/** Number of valves. */
#define VALVE_CHANNELS (3)

void valves_init(void);

void valve_close(uint8_t valve);
void valve_open(uint8_t valve);
uint8_t valve_state(uint8_t valve);

#endif
