#include "drivers/iodef.h"
#include "drivers/spi.h"
#include "drivers/valves_sipo.h"
#include <util/delay_basic.h>

static uint8_t sipo_state;

void valves_sipo_init(void)
{
	PORT_MODIFY(VALVE_SIPO1x_PORT, VALVE_SIPO1x_MASK, 0);
	PORT_MODIFY(VALVE_SIPO1x_DDR, VALVE_SIPO1x_MASK,
			DDR_OUT(VALVE_SIPO1x_MASK));

	//sipo_state = 0;
}

static void valve_sipo1_send()
{
	SPI_transfer8b(sipo_state);
	BIT_SET(VALVE_SIPO1x_PORT, VALVE_SIPO1x_RCK);
	_delay_loop_1(1);
	BIT_CLR(VALVE_SIPO1x_PORT, VALVE_SIPO1x_RCK);
}

void valve_sipo1_0_close(void)
{
	sipo_state &= ~(0x03 << 0);
	valve_sipo1_send();
}

void valve_sipo1_0_open(void)
{
//	sipo_state &= ~(0x03 << 0);
//	valve_sipo1_send();
}

uint8_t valve_sipo1_0_state(void)
{
	return sipo_state & (0x03 << 0);
}

void valve_sipo1_1_close(void)
{
	sipo_state &= ~(0x03 << 2);
	valve_sipo1_send();
}

void valve_sipo1_1_open(void)
{
}

uint8_t valve_sipo1_1_state(void)
{
	return sipo_state & (0x03 << 2);
}

void valve_sipo1_2_close(void)
{
	sipo_state &= ~(0x03 << 4);
	valve_sipo1_send();
}

void valve_sipo1_2_open(void)
{
}

uint8_t valve_sipo1_2_state(void)
{
	return sipo_state & (0x03 << 4);
}

void valve_sipo1_3_close(void)
{
	sipo_state &= ~(0x03 << 6);
	valve_sipo1_send();
}

void valve_sipo1_3_open(void)
{
}

uint8_t valve_sipo1_3_state(void)
{
	return sipo_state & (0x03 << 6);
}
