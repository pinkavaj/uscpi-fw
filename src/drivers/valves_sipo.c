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

    /* Close all ports */
	BIT_CLR(VALVE_SIPO1x_PORT, VALVE_SIPO1x_SCL);
	_delay_loop_1(1);
	BIT_SET(VALVE_SIPO1x_PORT, VALVE_SIPO1x_SCL);
}

// TODO: volat valves_sipo1_timeout jednou za 0.5 sec

/**
 * Make SIPO valves states transitions.
 *
 * Each valve use 2 voltages (Lo nad Hi). In order to open closed valve Hi
 * voltage must be applied, after stabilization of opened valve Lo voltage is
 * sufficient to keep it open. The transitions are:
 * 00 -> 10 (closed -> open)
 * 10 -> 11 (apply both Lo and Hi voltage, efectively does nothigh)
 * 11 -> 01 (sustain mode, apply only Lo voltage)
 **/
void valves_sipo1_timeout(void)
{
        uint8_t a = sipo_state & 0x55; // & 01 01 01 01
        uint8_t b = sipo_state & 0xaa; // & 10 10 10 10

        sipo_state = (sipo_state & ~(a << 1)) |  (b >> 1);
        valve_sipo1_send();
}

/**
 * Send wanted valve state into SIPO.
 **/
static void valve_sipo1_send(void)
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
	sipo_state &= ~(0x02 << 0);
	valve_sipo1_send();
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
	sipo_state &= ~(0x02 << 2);
	valve_sipo1_send();
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
	sipo_state &= ~(0x02 << 4);
	valve_sipo1_send();
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
	sipo_state &= ~(0x02 << 6);
	valve_sipo1_send();
}

uint8_t valve_sipo1_3_state(void)
{
	return sipo_state & (0x03 << 6);
}
