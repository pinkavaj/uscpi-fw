#ifndef __SCPI_CC_H__
#define __SCPI_CC_H__

#include "scpi.h"

SCPI_parse_t SCPI_CC_cls(void);

/* Standard Event Status Enable */
/* Standard Event Status Enable Query  */
SCPI_parse_t SCPI_CC_ese(void);

/* Event Status Register Query */
SCPI_parse_t SCPI_CC_esr(void);

/* Identification Query */
SCPI_parse_t SCPI_CC_idn(void);

/* Operation Complete Command  */
/* Operation Complete Query */
SCPI_parse_t SCPI_CC_opc(void);

/* Reset */
SCPI_parse_t SCPI_CC_rst(void);

/* Service Request Enable */
/* Service Request Enable Query  */
SCPI_parse_t SCPI_CC_sre(void);

/* Read Status Byte Query */
SCPI_parse_t SCPI_CC_stb(void);

/* Self-Test Query  */
SCPI_parse_t SCPI_CC_tst(void);

/* Wait-to-Continue - implementation is empty when operation executed 
 * synchronusly */
SCPI_parse_t SCPI_CC_wai(void);

#endif

