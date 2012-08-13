#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "scpi_cmd.h"

/* Set bit(s) in Operation Status Register */
static void SCPI_OPER_cond_set(uint16_t val)
{
	SCPI_OPER_cond |= val;
	val &= SCPI_OPER_trans_to1;
	SCPI_OPER_even |= val;
	if (SCPI_OPER_even & SCPI_OPER_enab)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Reset bit(s) in Operation Status Register */
static void SCPI_OPER_cond_reset(uint16_t val)
{
	SCPI_OPER_cond &= ~val;
	val &= SCPI_OPER_trans_to0;
	SCPI_OPER_even |= val;
	if (SCPI_OPER_even & SCPI_OPER_enab)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Update sumary register after OPERation enabled register changed */
static void SCPI_OPER_enab_update(void)
{
	if (SCPI_OPER_even & SCPI_OPER_enab)
		SCPI_STB_set(SCPI_STB_OPER);
	else
		SCPI_STB_reset(SCPI_STB_OPER);
}

/* Get value from OPERation event register */
static uint16_t SCPI_OPER_even_get(void)
{
	uint16_t x = SCPI_OPER_even & SCPI_OPER_enab;
	SCPI_OPER_even = 0;
	SCPI_STB_reset(SCPI_STB_OPER);
	return x;
}

/* Set bit(s) in Operation Status Register */
static void SCPI_QUES_cond_set(uint16_t val)
{
	SCPI_QUES_cond |= val;
	val &= SCPI_QUES_trans_to1;
	SCPI_QUES_even |= val;
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
}

/* Reset bit(s) in Operation Status Register */
static void SCPI_QUES_cond_reset(uint16_t val)
{
	SCPI_QUES_cond &= ~val;
	val &= SCPI_QUES_trans_to0;
	SCPI_QUES_even |= val;
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
}

/* Update sumary register after QUEStionable enable register changed */
static void SCPI_QUES_enab_update(void)
{
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
	else
		SCPI_STB_reset(SCPI_STB_QUES);
}

/* Get value of QUEStionable event register */
static uint16_t SCPI_QUES_even_get(void)
{
	uint16_t x = SCPI_QUES_even & SCPI_QUES_enab;
	SCPI_QUES_even = 0;
	SCPI_STB_reset(SCPI_STB_QUES);
	return x;
}

/* Get value of Standard Event Status Register */
static uint8_t SCPI_SESR_get(void)
{
	uint8_t x = SCPI_SESR & SCPI_SESR_enab;
	SCPI_SESR = 0;
	SCPI_STB_reset(SCPI_STB_SESR);
	return x;
}

/* Set event bit in Standard Event Status Register */
static void SCPI_SESR_set(uint8_t val)
{
	SCPI_SESR |= val;
	if (SCPI_SESR & SCPI_SESR_enab)
		SCPI_STB_set(SCPI_STB_SESR);
}

/* Update sumary register when Stantard Event Status Enable Register changed */
static void SCPI_SESR_enab_update(void)
{
	if (SCPI_SESR & SCPI_SESR_enab)
		SCPI_STB_set(SCPI_STB_SESR);
	else 
		SCPI_STB_reset(SCPI_STB_SESR);
}

/* Shortcut to exit with error 108 - too much parameters */
static SCPI_parse_t SCPI_cmd_err_108(void)
{
	SCPI_err_set(&SCPI_err_108);
	return SCPI_parse_err;
}

/* Wraper around SCPI_atoi, parse and return integer */
static SCPI_parse_t SCPI_in_uint8(uint8_t *x)
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
static SCPI_parse_t SCPI_in_uint16(uint16_t *x)
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
static SCPI_parse_t SCPI_in_uint32(uint32_t *x)
{
	char *ret;

	*x = strtoul(SCPI_in, &ret, 0);
	if (*ret != 0)
		return SCPI_parse_err;
	return SCPI_parse_drop_str;
}

/* Print unisgned int to output */
static SCPI_parse_t SCPI_out_uint32f(uint32_t x, uint8_t digits)
{
	// maximal amount of digits for uint32 is 10
	char buf[10];
	char *c = buf + sizeof(buf);
	do {
		*--c = (x % (uint8_t)10) + '0';
		x /= (uint8_t)10;
		if (digits)
			digits--;
	} while (x);
	
	while(digits) {
		*--c = '0';
		digits--;
	}
	SCPI_printn(c, sizeof(buf) - (c - buf));
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_out_uint32(uint32_t x)
{
	return SCPI_out_uint32f(x, 0);
}

/* Print int to output */
/*static void SCPI_out_i(int i)
{
	if (i < 0) {
		SCPI_putc('-');
		i = -i;
	}
	SCPI_out_ui((unsigned int)i);
}*/

/* Print 1 as result value */
static SCPI_parse_t SCPI_out_1(void)
{
	SCPI_putc('1');
	return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
