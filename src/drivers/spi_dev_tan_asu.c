#include "config.h"

#include <string.h>
#include <util/delay.h>
#include <util/delay_basic.h>

#include "drivers/spi.h"
//#include "drivers/ad974.h"
//#include "drivers/mcp4922.h"
//#include "drivers/sipo595.h"
#include "drivers/spi_dev.h"

/* number of currently selected SPI device, see spi.h */
static uint8_t SPI_dev_current;

void SPI_dev_select(uint8_t dev_num)
{
	if (dev_num == SPI_dev_current)
		return;
/*	if (SPI_dev_current != -1) {
		spi_devs[SPI_dev_current].select(0);
	}*/
	SPI_dev_current = dev_num;
/*	switch(SPI_dev_current)
	{
		case SPI_DEV_MAX1068_0:
		case SPI_DEV_MAX1068_1:
			max1068_select();
			break;
		case SPI_DEV_TLV5610_0:
			tlv5610_select();
			break;
		case SPI_DEV_NONE:
		default:
			return;
	}*/
}

uint16_t SPI_dev_AD_get_sample(uint8_t channel)
{
	channel = channel;
	switch(SPI_dev_current)
	{
/*		case SPI_DEV_MAX1068_0:
			return max1068_0_get_sample(channel);
		case SPI_DEV_MAX1068_1:
			return max1068_1_get_sample(channel);
		case SPI_DEV_NONE:*/
		default:
			return -1;
	}
}

void SPI_dev_DA_set_output(uint8_t channel, uint16_t output)
{
	channel = channel;
	output = output;
/*	switch(SPI_dev_current)
	{
		case SPI_DEV_TLV5610_0:
			tlv5610_0_write_channel(channel, output);
			return;
		case SPI_DEV_NONE:
		default:
			return;
	}*/
}

uint8_t SPI_dev_DA_width(void)
{
/*        switch(SPI_dev_current)
        {
                case SPI_DEV_TLV5610_0:
                        return tlv5610_width();
                default:
                        break;
        }*/
        return 0;
}

void SPI_dev_init(void) {
	SPI_dev_current = SPI_DEV_NONE;
	SPI_init();
/*	max1068_io_init();
	tlv5610_io_init();*/
}
