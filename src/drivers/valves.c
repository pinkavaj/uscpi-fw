
#include "drivers/valves.h"
#include "drivers/valves_raw.h"

typedef uint8_t (*valve_get_t)(void);
typedef void (*valve_set_t)(void);

typedef struct {
	valve_set_t close, open;
	valve_get_t state;
} valve_t;

static const valve_t valves[VALVE_CHANNELS] = {
	{
		.close = valve_raw_C0_close,
		.open = valve_raw_C0_open,
		.state = valve_raw_C0_state,
	},
	{
		.close = valve_raw_C1_close,
		.open = valve_raw_C1_open,
		.state = valve_raw_C1_state,
	},
	{
		.close = valve_raw_C2_close,
		.open = valve_raw_C2_open,
		.state = valve_raw_C2_state,
	},
};

void valves_init(void)
{
	valves_raw_init();
}

void valve_close(uint8_t valve)
{
	valves[valve].close();
}

void valve_open(uint8_t valve)
{
	valves[valve].open();
}

uint8_t valve_state(uint8_t valve)
{
	return valves[valve].state();
}

