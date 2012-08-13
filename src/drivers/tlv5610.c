/*****************************************************************************
 *     ***  Analog-to-Digital Converter MAX1068  ***
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
#include "drivers/max1068.h"

/*****************************************************************************/
void tlv5610_io_init(void)
{
/* TODO:	PORT_MODIFY(TLV5610_DDR, TLV5610_MASK, 
			DDR_OUT(TLV5610_CS | TLV5610_LD));
	PORT_MODIFY(TLV5610_PORT, TLV5610_MASK, (TLV5610_CS | TLV5610_LD));*/
}

/*****************************************************************************/
void tlv5610_select(void)
{
        /* FIXME: select/deselect -> nastavení CS */
	SPI_set_mode(tlv5610_spi_mode());
}

/*****************************************************************************/
uint8_t tlv5610_spi_mode(void)
{
	return SPI_CLOCK_1_4 |
		SPI_PHASE_TRAIL |
		SPI_POL_LOW |
		SPI_DORD_MSB |
		SPI_MODE_MASTER |
		SPI_ENABLE;
}

/*****************************************************************************/
uint8_t tlv5610_width(void)
{
        return 12;
}

/*****************************************************************************/
static void tlv5610_write(uint16_t datastring)
{
	BIT_CLR(TLV5610_PORT, TLV5610_CS);
	/* send new value to latch, Hi Lo */ 
	SPI_transfer16b(datastring);
	/* send latch to output 40ns to setup, 100ns width */
	BIT_CLR(TLV5610_PORT, TLV5610_LD);
	_delay_loop_1(1);
	BIT_SET(TLV5610_PORT, TLV5610_LD);
	BIT_SET(TLV5610_PORT, TLV5610_CS);
}

/*****************************************************************************/
void tlv5610_write_channel(uint8_t ch, uint16_t val)
{
        /* FIXME: jak posilat data a kanal dovnitř? */
/*#define _TLV5610_CFG_ (TLV5610_GAIN_1 | TLV5610_OUTPUT_ON | TLV5610_BUF_DIS)
	if (ch == 0)
		val = (val & TLV5610_DATA_MASK) | TLV5610_CH_A | _TLV5610_CFG_;
	if (ch == 1)
		val = (val & TLV5610_DATA_MASK) | TLV5610_CH_B | _TLV5610_CFG_;
#undef _TLV5610_CFG_*/

	tlv5610_write(val);
}

