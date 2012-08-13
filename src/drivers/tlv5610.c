/*****************************************************************************
 *     ***   Digital-to-Analog Converter TLV5610  ***
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

#define TLV5610_DATA_MASK	0x0fff
#define TLV5610_ADDRESS_MASK	0xf000

/* ukazka definic, tyto features nepouzivame */

#define TLV5610_ADDRESS_CTRL0	0x8000
#define TLV5610_ADDRESS_CTRL1	0x9000
#define TLV5610_ADDRESS_PRESET	0xA000
#define TLV5610_ADDRESS_RESERVED	0xB000
#define TLV5610_ADDRESS_DACAB	0xC000
#define TLV5610_ADDRESS_DACCD	0xD000
#define TLV5610_ADDRESS_DACEF	0xE000
#define TLV5610_ADDRESS_DACGH	0xF000

#define TLV5610_CTRL0_PD 4
#define TLV5610_CTRL0_PD_DOWN (1<<TLV5610_CTRL0_PD)
#define TLV5610_CTRL0_PD_NORM (0<<TLV5610_CTRL0_PD)

#define TLV5610_CTRL0_DO 3
#define TLV5610_CTRL0_DO_EN (1<<TLV5610_CTRL0_DO)
#define TLV5610_CTRL0_DO_DIS (0<<TLV5610_CTRL0_DO)

#define TLV5610_CTRL0_IM 0
#define TLV5610_CTRL0_IM_STRAIGHT (1<<TLV5610_CTRL0_IM)
#define TLV5610_CTRL0_IM_COMPLEMENT (0<<TLV5610_CTRL0_IM)

#define TLV5610_CTRL1_SAB 0
#define TLV5610_CTRL1_SAB_FAST (1<<TLV5610_CTRL1_SAB)
#define TLV5610_CTRL1_SAB_SLOW (0<<TLV5610_CTRL1_SAB)

#define TLV5610_CTRL1_PAB 4
#define TLV5610_CTRL1_PAB_DOWN (1<<TLV5610_CTRL1_PAB)
#define TLV5610_CTRL1_PAB_NORM (0<<TLV5610_CTRL1_PAB)

/*****************************************************************************/
void tlv5610_io_init(void)  // vlastni DAC neni treba inicializovat
{
	PORT_MODIFY(TLV5610_DDR, TLV5610_MASK,
			 	DDR_OUT(TLV5610_CS | TLV5610_LD));
	PORT_MODIFY(TLV5610_PORT, TLV5610_MASK, (TLV5610_CS | TLV5610_LD));
}

/*****************************************************************************/
void tlv5610_select(void)
{
        /* FIXME: select/deselect -> nastavení CS */
	SPI_set_mode(tlv5610_spi_mode());
}

/*****************************************************************************/
uint8_t tlv5610_spi_mode(void)
{ //FIXME zkontrolovat SPI
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
/* test channel */
	// nepouziji masku, ale vypocet s kontrolou
	if (ch >= 0 && ch =< 7) {
		val = val | ((uint16_t)ch << 12);
		tlv5610_write(val);
	}
}

