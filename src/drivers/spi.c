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

void SPI_set_mode(uint8_t mode)
{
	SPCR = mode;
}

uint8_t SPI_transfer8b(uint8_t val)
{
	SPDR = val;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

uint16_t SPI_transfer16b(uint16_t val)
{
	/* This is due to optimalization. Operation above SPDR is marked as 
	 * volatile and thus default optimalization have some problems. */
	union {
		uint16_t val16;
		uint8_t val8[2];
	} val_u;

	/* This actualy do nothigh,  */
	val_u.val16 = val;

	/* val8[1] is in the same place in registers as high byte of val16 ... */
	SPDR = val_u.val8[1];
	loop_until_bit_is_set(SPSR, SPIF);
	val_u.val8[1] = SPDR;

	SPDR = val_u.val8[0];
	loop_until_bit_is_set(SPSR, SPIF);
	val_u.val8[0] = SPDR;

	return val_u.val16;
}

