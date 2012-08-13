#ifndef __SCPI_CMD_TOOLS_H__
#define __SCPI_CMD_TOOLS_H__

#include "scpi.h"

/* SCPI Tools */
SCPI_parse_t SCPI_in_uint8(uint8_t *x);
SCPI_parse_t SCPI_in_uint16(uint16_t *x);
SCPI_parse_t SCPI_in_uint32(uint32_t *x);
SCPI_parse_t SCPI_cmd_err_108(void);

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
