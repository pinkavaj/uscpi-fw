/*****************************************************************************
 *        ****  Analog-to-Digital Converter AD974  ****
 */

#include <util/delay.h>
#include <util/delay_basic.h>

#include "iodef.h"
#include "spi.h"
#include "ad974.h"

void ad974_io_init(void)
{
	PORT_MODIFY(AD974_DDR, AD974_MASK,
			(DDR_OUT(AD974_RC) | 
			 DDR_OUT(AD974_WR) | 
			 DDR_OUT(AD974_CS) | 
			 DDR_OUT(AD974_A1) |
			 DDR_OUT(AD974_A0) |
			 DDR_IN(AD974_BUSY)));
	PORT_MODIFY(AD974_PORT, AD974_MASK, (AD974_RC | AD974_WR | AD974_CS));
}

uint8_t ad974_spi_mode()
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
	BIT_CLR(AD974_PORT, AD974_WR);
	_delay_loop_1(1);
	BIT_SET(AD974_PORT, AD974_WR);
}

static uint16_t ad974_get_sample_(void)
{
	uint16_t val;

	BIT_CLR(AD974_PORT, AD974_CS);
	/* start conversion, 10ns CS->RC */
	BIT_CLR(AD974_PORT, AD974_RC);
	/* wait until conversion complete 4us + few ns to set-up signals */
	_delay_us(5);
//	loop_until_bit_is_set(AD974_PORT, AD974_BUSY);
	/* data acquisition 1us + few ns to set up signals */
	BIT_SET(AD974_PORT, AD974_RC);
	_delay_us(1.5);
	// read result
	val = SPI_transfer8b(0) << 8;
	val |= SPI_transfer8b(0);
	BIT_SET(AD974_PORT, AD974_CS);

	return val;
}

uint16_t ad974_get_sample(uint8_t channel)
{
	ad974_set_channel(channel);
	return ad974_get_sample_();
}

void ad974_get_samples(uint8_t channel, uint32_t *val, uint8_t count)
{
	ad974_set_channel(channel);
	/* dummy read in order to drop data from previous channel */
	ad974_get_sample_();

	while(count--)
		*val += ad974_get_sample_();
}

