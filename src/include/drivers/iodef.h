#ifndef __IODEF_H__
#define __IODEF_H__

/* Tools */
#define BIT_CLR(port, bit) (port &= ~bit)
#define BIT_SET(port, bit) (port |= bit)
#define PORT_MODIFY(port, mask, set) (port = (port & ~mask) | (set & mask))
#define DDR_IN(b) (0)
#define DDR_OUT(b) (b)


#if DEVICE_NAME == IMPA444_ASU
/* AD974 */
#define AD974_PORT	PORTA
#define AD974_DDR	DDRA
#define AD974_PIN	PINA

#define	AD974_A0	_BV(PA0)
#define	AD974_A1	_BV(PA1)
#define	AD974_BUSY	_BV(PA2)
#define	AD974_CS	_BV(PA3)
#define	AD974_WR	_BV(PA4)
#define	AD974_RC	_BV(PA5)
#define AD974_MASK (AD974_A0 | AD974_A1 | AD974_BUSY | AD974_CS | AD974_WR | \
		AD974_RC)

/* TODO: zkontrolovat a dokončit volbu modulu */
/* dokončit definice pro druhé zařízení */
/* MCP4922 */
#define MCP4922_PORT PORTA
#define MCP4922_DDR DDRA
#define MCP4922_PIN PINA

#define MCP4922_CS _BV(PA6)
#define MCP4922_LD _BV(PA7)
#define MCP4922_MASK (MCP4922_CS | MCP4922_LD)

#elif DEVICE_NAME == MSA_HCU814

/* DAQ8H = DAQ subsystem for HCU814 
   2x MAX1068 + 1x TLV5610 */

/* MAX1068 8-ch 14-bit A/D converter */
#define MAX1068_PORT      PORTA
#define MAX1068_DDR       DDRA
#define MAX1068_PIN       PINA

#define MAX1068_0_CS   _BV(PA0) // out, 1
#define MAX1068_0_EOC  _BV(PA1) // in
#define MAX1068_1_CS   _BV(PA2) // out, 1
#define MAX1068_1_EOC  _BV(PA6) // in
#define MAX1068_MASK	(MAX1068_0_CS | MAX1068_0_EOC | \
			MAX1068_1_CS | MAX1068_1_EOC )

/* TLV5610 8-ch 12-bit D/A converter */
#define TLV5610_PORT      PORTA
#define TLV5610_DDR       DDRA
#define TLV5610_PIN       PINA

#define TLV5610_CS   _BV(PA3) // out, 1
#define TLV5610_CLR  _BV(PA4) // out, 1
#define TLV5610_LD   _BV(PA5) // out, 1
#define TLV5610_MASK	( TLV5610_CS | TLV5610_CLR | TLV5610_LD)

#endif

