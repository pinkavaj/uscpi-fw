#include "config.h"

#include <string.h>
#include <util/delay.h>
#include <util/delay_basic.h>

#include "drivers/spi.h"
#include "drivers/ad974.h"
#include "drivers/mcp4922.h"
#include "drivers/spi_dev.h"

/* number of currently selected SPI device, see spi.h */
static uint8_t SPI_dev_current;

void SPI_dev_select(uint8_t dev_num)
{
/*	if (SPI_dev_current != -1) {
		spi_devs[SPI_dev_current].select(0);
	}*/
	SPI_dev_current = dev_num;
	switch(SPI_dev_current)
	{
		case SPI_DEV_AD974_0:
			SPCR = ad974_spi_mode();
//			spi_devs[dev_num].select(1);
			break;
		case SPI_DEV_MCP4922_0:
			SPCR = mcp4922_spi_mode();
//			spi_devs[dev_num].select(1);
			break;
		default:
		case SPI_DEV_NONE:
			return;
	}
}

void SPI_dev_init(void) {
	SPI_dev_current = SPI_DEV_NONE;
	SPI_init();
	ad974_io_init();
	mcp4922_io_init();
}


