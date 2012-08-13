#include <util/delay_basic.h>

#include "drivers/iodef.h"
#include "spi.h"

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
	SPCR = _BV(SPE) | _BV(MSTR);
	// set pull-up on input
	PORT_MODIFY(SPI_PORT, (SPI_MISO | SPI_MOSI), 0);
}

void SPI_dummy_clk(void)
{
	uint8_t spcr = SPCR;
	/* Set SCK to inverted value of polarisation */
	if ((spcr & SPI_POL) == SPI_POL_LOW)
		PORT_MODIFY(SPI_PORT, SPI_SCK, SPI_SCK);
	else
		PORT_MODIFY(SPI_PORT, SPI_SCK, 0);
	SPCR = 0;
	/* TODO: delay should conform current SPI slock */
	_delay_loop_1(1);
	SPCR = spcr;
}

uint8_t SPI_transfer8b(uint8_t val)
{
	SPDR = val;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

