#include <string.h>
#include <util/delay.h>
#include <util/delay_basic.h>

#include "config.h"
#include "iodef.h"
#include "spi_dev.h"


#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_SS _BV(PB4)
#define SPI_MOSI _BV(PB5)
#define SPI_MISO _BV(PB6)
#define SPI_SCK _BV(PB7)
#define SPI_MASK (SPI_MISO | SPI_SCK | SPI_MOSI | SPI_SS)


void SPI_init(void) {
//	SPSR &= ~SPI2X;
	PORT_MODIFY(SPI_DDR, SPI_MASK, 
			(DDR_OUT(SPI_SS | SPI_MOSI | SPI_SCK) | 
			 DDR_IN(SPI_MISO)));
//	PORT_MODIFY()
	SPCR = _BV(SPE) | _BV(MSTR);
	// set pull-up on input
	PORT_MODIFY(SPI_PORT, SPI_MISO, 0);
}

uint8_t SPI_transfer8b(uint8_t out) {
	SPDR = out;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

