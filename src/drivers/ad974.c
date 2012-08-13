/*****************************************************************************
 *     ***  Analog-to-Digital Converter AD974  ***
 *
 * Copyright (c) 2010 Lukas Kucera <lukas.kucera@vscht.cz>,
 * 	Jiri Pinkava <jiri.pinkava@vscht.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *****************************************************************************/

#include "config.h"
#include <util/delay.h>
#include <util/delay_basic.h>

#include "drivers/iodef.h"
#include "drivers/spi.h"
#include "drivers/ad974.h"

/*****************************************************************************/
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

/*****************************************************************************/
//#pragma GCC diagnostic ignored "-Winit-self"
static uint16_t ad974_get_sample_(void)
{
	/* Is used as input variable, but does not mather about the content,
	 * just hackou out warning: 'val' is used uninitialized ... */
	uint16_t val = val;

	BIT_CLR(AD974_PORT, AD974_CS);
	/* start conversion, 10ns CS->RC */
	BIT_CLR(AD974_PORT, AD974_RC);
	/* Wait until conversion complete 4us + few x0 ns to set-up signals. */
	_delay_us(4.2);
//	loop_until_bit_is_set(AD974_PORT, AD974_BUSY);

	/* data acquisition 1us + few ns to set up signals */
	BIT_SET(AD974_PORT, AD974_RC);
	_delay_us(1.2);
	val = SPI_transfer16b(val);

	BIT_SET(AD974_PORT, AD974_CS);
	return val;
}

/*****************************************************************************/
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

/*****************************************************************************/
uint16_t ad974_get_sample(uint8_t channel)
{
	uint16_t val;

	ad974_set_channel(channel);
	val = ad974_get_sample_();
	if (val < 0x8000)
		return 0;
	return val - 0x8000;
}

/*****************************************************************************/
void ad974_select(void)
{
	SPI_set_mode(ad974_spi_mode());
	/* AD974 need one dummy read cycle. First DAQ result is moustly invalid
	 * because of internal comfusion. It generates at random synchronization
	 * pulse and thus use different timming/mode when sending data.  */
	ad974_get_sample_();
}

/*****************************************************************************/
uint8_t ad974_spi_mode()
{
	return SPI_CLOCK_1_4 |
		SPI_PHASE_TRAIL |
		SPI_POL_LOW |
		SPI_DORD_MSB |
		SPI_MODE_MASTER |
		SPI_ENABLE;
}

