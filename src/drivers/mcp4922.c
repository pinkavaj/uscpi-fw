/*****************************************************************************/
/*        ****  Digital-to-Analog Converter MCP4922  ****
 */
#include <util/delay.h>
#include <util/delay_basic.h>

#include "iodef.h"
#include "spi.h"
#include "mcp4922.h"


#define MCP4922_OUTPUT 12
#define MCP4922_OUTPUT_ON (1<<MCP4922_OUTPUT)
#define MCP4922_OUTPUT_OFF (0<<MCP4922_OUTPUT)

#define MCP4922_BUF 14
#define MCP4922_BUF_DIS (0<<MCP4922_BUF)
#define MCP4922_BUF_EN (1<<MCP4922_BUF)

#define MCP4922_GAIN 13
#define MCP4922_GAIN_1 (1<<MCP4922_GAIN)
#define MCP4922_GAIN_2 (0<<MCP4922_GAIN)

#define MCP4922_CH 15
#define MCP4922_CH_A (0<<MCP4922_CH)
#define MCP4922_CH_B (1<<MCP4922_CH)


/*****************************************************************************/
void mcp4922_io_init(void)
{
	PORT_MODIFY(MCP4922_DDR, MCP4922_MASK, 
			DDR_OUT(MCP4922_CS | MCP4922_LD));
	PORT_MODIFY(MCP4922_PORT, MCP4922_MASK, (MCP4922_CS | MCP4922_LD));
}

uint8_t mcp4922_spi_cfg(void)
{
	return SPI_CLOCK_1_4 |
		SPI_PHASE_LEAD |
		SPI_POL_LOW |
		SPI_DORD_MSB |
		SPI_MODE_MASTER |
		SPI_ENABLE;
}

/*****************************************************************************/
static void mcp4922_send_(uint16_t datastring)
{
	PORT_MODIFY(MCP4922_PORT, MCP4922_CS, 0);
	/* send new value to latch, Hi Lo */ 
	SPI_transfer8b(datastring >> 8);
	SPI_transfer8b(datastring);
	/* send latch to output 40ns to setup, 100ns width */
	PORT_MODIFY(MCP4922_PORT, MCP4922_LD, 0);
	_delay_loop_1(1);
	PORT_MODIFY(MCP4922_PORT, MCP4922_LD, MCP4922_LD);
	PORT_MODIFY(MCP4922_PORT, MCP4922_CS, MCP4922_CS);
}

/*****************************************************************************/
void mcp4922_write_channel(uint8_t ch, uint16_t val)
{
#define _MCP4922_CFG_ (MCP4922_GAIN_1 | MCP4922_OUTPUT_ON | MCP4922_BUF_DIS)
	if (ch == 0)
		val |= MCP4922_CH_A | _MCP4922_CFG_;
	if (ch == 1)
		val |= MCP4922_CH_B | _MCP4922_CFG_;
#undef _MCP4922_CFG_

	mcp4922_send_(val);
}

