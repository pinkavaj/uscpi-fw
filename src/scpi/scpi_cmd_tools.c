#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "scpi_cmd_tools.h"

/* Shortcut to exit with error 108 - too much parameters */
SCPI_parse_t SCPI_cmd_err_108(void)
{
	SCPI_err_set(&SCPI_err_108);
	return SCPI_parse_err;
}

/* Wraper around SCPI_atoi, parse and return integer */
SCPI_parse_t SCPI_in_uint8(uint8_t *x)
{
	uint32_t val;
	SCPI_parse_t ret;

	ret = SCPI_in_uint32(&val);
	if (val > 0xff)
		return SCPI_parse_err;
	*x = val;
	return ret;
}

/* Wraper around SCPI_atoi, parse and return integer */
SCPI_parse_t SCPI_in_uint16(uint16_t *x)
{
	uint32_t val;
	SCPI_parse_t ret;

	ret = SCPI_in_uint32(&val);
	if (val > 0xffff)
		return SCPI_parse_err;
	*x = val;
	return ret;
}

/* Wraper around SCPI_atoi, parse and return integer */
SCPI_parse_t SCPI_in_uint32(uint32_t *x)
{
	char *ret;

	*x = strtoul(SCPI_in + SCPI_param_in_buf_idx, &ret, 0);
	SCPI_param_in_buf_idx += strlen(SCPI_in + SCPI_param_in_buf_idx) + 1;
	if (*ret != 0)
		return SCPI_parse_err;
	return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
