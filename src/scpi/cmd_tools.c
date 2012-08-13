#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_tools.h"

uint8_t SCPI_params_count;
uint8_t SCPI_param_in_buf_idx;

/* Wraper around SCPI_atoi, parse and return integer */
SCPI_parse_t SCPI_in_uint8(uint8_t *x)
{
	uint32_t val;
	SCPI_parse_t ret;

	ret = SCPI_in_uint32(&val);
	if (val > 0xff)
                return SCPI_err_set_(&SCPI_err_222);
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
                return SCPI_err_set_(&SCPI_err_222);
	*x = val;
	return ret;
}

/* Wraper around SCPI_atoi, parse and return integer */
SCPI_parse_t SCPI_in_uint32(uint32_t *x)
{
	char *ret;

	*x = strtoul(SCPI_in + SCPI_param_in_buf_idx, &ret, 0);
	SCPI_param_in_buf_idx += strlen(SCPI_in + SCPI_param_in_buf_idx) + 1;
	if (*ret != 0) {
                return SCPI_err_set_(&SCPI_err_121);
        }
	return SCPI_parse_end;
}

/* TODO: ... */
SCPI_parse_t SCPI_in_FP_16_16(uint32_t *x)
{
        x = x;

        return SCPI_parse_err;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
