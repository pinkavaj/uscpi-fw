
#include "drivers/valves.h"
#include "drivers/valves_raw.h"
#include "drivers/valves_sipo.h"

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
	{
		.close = valve_sipo1_0_close,
		.open = valve_sipo1_0_open,
		.state = valve_sipo1_0_state,
	},
	{
		.close = valve_sipo1_1_close,
		.open = valve_sipo1_1_open,
		.state = valve_sipo1_1_state,
	},
	{
		.close = valve_sipo1_2_close,
		.open = valve_sipo1_2_open,
		.state = valve_sipo1_2_state,
	},
	{
		.close = valve_sipo1_3_close,
		.open = valve_sipo1_3_open,
		.state = valve_sipo1_3_state,
	},
};

void valves_init(void)
{
	valves_raw_init();
	valves_sipo_init();
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

