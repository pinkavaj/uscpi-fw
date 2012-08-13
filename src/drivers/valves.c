
#include "drivers/valves.h"
#include "drivers/valves_raw.h"

typedef uint8_t (*valve_get_t)(void);
typedef void (*valve_set_t)(void);

typedef struct {
	valve_set_t close, open;
	valve_get_t status;
} valve_t;

static const valve_t valves[] = {
	{
		.close = valve_raw_C0_close,
		.open = valve_raw_C0_open,
		.status = valve_raw_C0_state,
	},
};

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
	return valves[valve].status();
}

