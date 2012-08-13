
#include "drivers/iodef.h"
#include "drivers/valves_raw.h"

void valves_raw_init(void)
{
	PORT_MODIFY(VALVE_RAW0x_PORT, VALVE_RAW0x_MASK, 0);
	PORT_MODIFY(VALVE_RAW0x_DDR, VALVE_RAW0x_MASK,
			DDR_OUT(VALVE_RAW0x_MASK));
}

void valve_raw_C0_close(void)
{
	BIT_CLR(VALVE_RAW0x_PORT, VALVE_RAW00_PIN);
}

void valve_raw_C0_open(void)
{
	BIT_SET(VALVE_RAW0x_PORT, VALVE_RAW00_PIN);
}

uint8_t valve_raw_C0_state(void)
{
	return VALVE_RAW0x_PORT & VALVE_RAW00_PIN;
}

void valve_raw_C1_close(void)
{
	BIT_CLR(VALVE_RAW0x_PORT, VALVE_RAW01_PIN);
}

void valve_raw_C1_open(void)
{
	BIT_SET(VALVE_RAW0x_PORT, VALVE_RAW01_PIN);
}

uint8_t valve_raw_C1_state(void)
{
	return VALVE_RAW0x_PORT & VALVE_RAW01_PIN;
}

void valve_raw_C2_close(void)
{
	BIT_CLR(VALVE_RAW0x_PORT, VALVE_RAW02_PIN);
}

void valve_raw_C2_open(void)
{
	BIT_SET(VALVE_RAW0x_PORT, VALVE_RAW02_PIN);
}

uint8_t valve_raw_C2_state(void)
{
	return VALVE_RAW0x_PORT & VALVE_RAW02_PIN;
}

