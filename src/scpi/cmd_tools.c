#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_tools.h"
#include "scpi_parser.h"
#include "lib/math_cust.h"

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
/* TODO: použít přímou konverzi, když víme že *x není ovlivněno pokud dojdek chybě */
SCPI_parse_t SCPI_in_uint32(uint32_t *x)
{
	char *endptr;
        uint32_t val;

	val = strtoul(SCPI_in + SCPI_param_in_buf_idx, &endptr, 10);
	if (*endptr != 0)
                return SCPI_err_set_(&SCPI_err_121);
        *x = val;
	SCPI_param_in_buf_idx += strlen(SCPI_in + SCPI_param_in_buf_idx) + 1;
	return SCPI_parse_end;
}

#include "lib/iobuf.h"
SCPI_parse_t SCPI_in_FP_16_16(FP_16_16_t *x)
{
        char *endptr;
        union {
                struct {
                        uint16_t fract;
                        uint16_t num;
                } parts;
                FP_16_16_t FP;
        } N;
        uint32_t val;

        val = strtoul(SCPI_in + SCPI_param_in_buf_idx, &endptr, 10);
        if (val > 0xffff)
                return SCPI_err_set_(&SCPI_err_222);
        N.parts.num = val;

        if (*endptr == '.') {
		endptr++;
		if (*endptr != 0) {
        		uint8_t len;

        		len = strlen(endptr);
        		if (len > 5) {
                		len = 5;
                		endptr[5] = 0;
        		}
        		val = strtoul(endptr, &endptr, 10);
        		while (len < 5) {
                		val *= 10;
                		len++;
        		}

        		val *= 0x100;
        		val += 4;
        		val /= 10;
        		val *= 0x100;
        		val +=  4999;
        		val /= 10000;
        		N.parts.fract = val;
        	}
        }
        if (*endptr != 0)
                return SCPI_err_set_(&SCPI_err_121);

        *x = N.FP;
        SCPI_param_in_buf_idx += strlen(SCPI_in + SCPI_param_in_buf_idx) + 1;
        return SCPI_parse_end;
}

static void SCPI_print_common(void)
{
        if (_SCPI_NEED_COMMA())
                putc(',');
        if (_SCPI_NEED_SEMICOLON())
                putc(';');
        _SCPI_NEED_SEMICOLON_reset();
        _SCPI_NEED_COMMA_set();
        _SCPI_NEED_NEWLINE_set();
}

void SCPI_print_FP_16_16(FP_16_16_t val)
{
        SCPI_print_common();
        print_FP_16_16(val);
}

void SCPI_print_P(PGM_P s)
{
        SCPI_print_common();
        print_P(s);
}

void SCPI_print_temp_1_20(temp_1_20_t T)
{
        uint16_t T_fract;

        SCPI_print_common();

        T_fract = T % 20;
        T = T / 20;
        print_uint32(T);
        putc('.');
        print_uint32f(T_fract * 5, 2);
}

void SCPI_print_uint16(uint16_t val)
{
        SCPI_print_common();
        print_uint32(val);
}

void SCPI_print_uint32(uint32_t val)
{
        SCPI_print_common();
        print_uint32(val);
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
