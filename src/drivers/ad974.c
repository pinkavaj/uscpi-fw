#include <string.h>
#include <util/delay.h>
#include <util/delay_basic.h>

#include "config.h"
#include "iodef.h"
#include "spi.h"
#include "ad974.h"


void ad974_io_init(void)
{
	// DAQ port init
	PORT_MODIFY(AD974_DDR, AD974_MASK, \
			(AD974_RC | AD974_WR | AD974_CS | AD974_A1 | AD974_A0));
	PORT_MODIFY(AD974_PORT, AD974_MASK, (AD974_RC | AD974_WR | AD974_CS));
}

uint8_t ad974_spi_cfg()
{
	return SPI_CLOCK_1_4 |
		SPI_PHASE_TRAIL |
		SPI_POL_LOW |
		SPI_DORD_MSB |
		SPI_MODE_MASTER |
		SPI_ENABLE;
}

static void ad974_set_channel(uint8_t channel)
{
	/* set channel number, 10ns */
	PORT_MODIFY(AD974_PORT, (AD974_A0 | AD974_A1), 
			(((channel & 1) * AD974_A0) |
			(((channel & 2) >> 1) * AD974_A1)));
	/* write channel, 50 ns */
	PORT_MODIFY(AD974_PORT, AD974_WR, 0);
	_delay_loop_1(1);
	PORT_MODIFY(AD974_PORT, AD974_WR, AD974_WR);
}

static uint16_t ad974_get_sample_(void)
{
	uint16_t val;

	PORT_MODIFY(AD974_PORT, AD974_CS, 0);
	/* start conversion, 10ns CS->RC */
	PORT_MODIFY(AD974_PORT, AD974_RC, 0);
	/* wait until conversion complete 4us + few ns to set-up signals */
	_delay_us(5);
//	loop_until_bit_is_set(AD974_PORT, AD974_BUSY);
	/* data acquisition 1us + few ns to set up signals */
	PORT_MODIFY(AD974_PORT, AD974_RC, AD974_RC);
	ad974_set_channel(0);
	_delay_us(1.5);
	// read result
	val = SPI_transfer8b(0) << 8;
	val |= SPI_transfer8b(0);
	PORT_MODIFY(AD974_PORT, AD974_CS, AD974_CS);

	return val;
}

uint16_t ad974_get_sample(uint8_t channel)
{
	ad974_set_channel(channel);
	return ad974_get_sample_();
}

void ad974_get_samples(uint8_t channel, uint32_t *val, uint8_t count)
{
	// dummy read
	ad974_set_channel(channel);
	ad974_get_sample_();
	while(count--)
		*val += ad974_get_sample_();
}

