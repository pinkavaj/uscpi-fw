#ifndef __IODEF_H__
#define __IODEF_H__

/* Tools */
#define BIT_CLR(port, bit) (port &= ~bit)
#define BIT_SET(port, bit) (port |= bit)
#define PORT_MODIFY(port, mask, set) (port = (port & ~mask) | (set & mask))
#define DDR_IN(b) (0)
#define DDR_OUT(b) (b)

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

/* MCP4922 */
#define MCP4922_PORT PORTA
#define MCP4922_DDR DDRA
#define MCP4922_PIN PINA

#define MCP4922_CS _BV(PA6)
#define MCP4922_LD _BV(PA7)
#define MCP4922_MASK (MCP4922_CS | MCP4922_LD)

#endif

