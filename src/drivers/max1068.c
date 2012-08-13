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
void max1068_io_init(void)
{
/*	PORT_MODIFY(MAX1068_PORT, 
	PORT_MODIFY(AD974_PORT, AD974_MASK, (AD974_RC | AD974_WR | AD974_CS));*/
}

void max1068_X_select(void)
{
        /* FIXME: select/deselect pro nastavení CS */
/*        SPI_set_mode();*/
}

uint16_t max1068_get_sample(uint8_t channel)
{
        /* FIXME: cele zkontrolovat a přidat  do Makefile */
        uint16_t result;

        BIT_CLR(MAX1068_PORT, MAX1068_0_CS);
        channel =<< 5;  // send address in form AAA00001
        channel |= 1;
        SPI_transfer8b(channel);
        _delay_us(8);    // wait for conversion
        result = SPI_transfer16b(0);
        BIT_SET(MAX1068_PORT, MAX1068_0_CS);

        return result;
}

/* FIXME: dodělat další elementární fukce + hlavička */

