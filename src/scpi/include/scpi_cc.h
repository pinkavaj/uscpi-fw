#ifndef __SCPI_CC_H__
#define __SCPI_CC_H__

#include "scpi.h"

SCPI_parse_t SCPI_CC_cls(char c);

/* Standard Event Status Enable */
/* Standard Event Status Enable Query  */
SCPI_parse_t SCPI_CC_ese(char c);

/* Event Status Register Query */
SCPI_parse_t SCPI_CC_esr(char c);

/* Identification Query */
SCPI_parse_t SCPI_CC_idn(char c);

/* Operation Complete Command  */
/* Operation Complete Query */
SCPI_parse_t SCPI_CC_opc(char c);

/* Reset */
SCPI_parse_t SCPI_CC_rst(char c);

/* Service Request Enable */
/* Service Request Enable Query  */
SCPI_parse_t SCPI_CC_sre(char c);

/* Read Status Byte Query */
SCPI_parse_t SCPI_CC_stb(char c);

/* Self-Test Query  */
SCPI_parse_t SCPI_CC_tst(char c);

/* Wait-to-Continue - implementation is empty when operation executed 
 * synchronusly */
SCPI_parse_t SCPI_CC_wai(char c);

#endif

