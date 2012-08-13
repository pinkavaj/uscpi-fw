#include <ctype.h>
#include <string.h>

#include "scpi_cmd.h"


#define SCPI_atoi_SIGNED	(1<<0)
#define SCPI_atoi_UNSIGNED	(1<<1)
#define SCPI_atoi_1B	(1<<2)
#define SCPI_atoi_2B	(2<<2)
#define SCPI_atoi_4B	(4<<2)
#define SCPI_atoi_NB	(7<<2)

static SCPI_parse_t SCPI_atoi(void *x, uint8_t flags);


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

/* Drop parameter from input, move to next one */
static SCPI_parse_t SCPI_in_drop_param(char *end)
{
	if (--SCPI_params_count) {
		/* skip \0 at end of parameter */
		end++;
		memmove(SCPI_in, end, (SCPI_in_len - (SCPI_in - end) + 1)*sizeof(char));
		memmove(SCPI_param_types, SCPI_param_types + 1, 
				SCPI_params_count*sizeof(SCPI_param_type_t));
	}
	return SCPI_parse_end;
}

/* Wraper around SCPI_atoi, parse and return integer */
static SCPI_parse_t SCPI_in_uint8(uint8_t *x)
{
	return SCPI_atoi(x, SCPI_atoi_UNSIGNED | SCPI_atoi_1B);
}

/* Wraper around SCPI_atoi, parse and return integer */
static SCPI_parse_t SCPI_in_uint16(uint16_t *x)
{
	return SCPI_atoi(x, SCPI_atoi_UNSIGNED | SCPI_atoi_2B);
}

/* Parse integer value, if parsed sucessfully, set value of *x, othervise
 * set error code and exit with SCPI_parse_err, flags specify data type 
 * and lenght, see top of this file */
static SCPI_parse_t SCPI_atoi(void *x, uint8_t flags)
{
	char *c = SCPI_in;
	int8_t sig_neg;
	uint32_t y = 0;
	SCPI_param_type_t pt = SCPI_param_types[0];

	sig_neg = pt & SCPI_PAR_NUM_SIG & ~SCPI_PAR_NUM;
	/* Check parameter type */
	if ((pt & SCPI_PAR_NUM_TYPE) != SCPI_PAR_NUM_INT ||
			(sig_neg && (flags & SCPI_atoi_UNSIGNED)))
	{
		SCPI_err_set(&SCPI_err_104);
		return SCPI_parse_err;
	}
	if ((pt & SCPI_PAR_NUM_INT_TYPE) == SCPI_PAR_NUM_INT_DEC) {
		do {
			if (y > (((uint32_t)0xffffffff - 11)/10))
				break;
			y = y * 10;
			y += (*c - '0');
		} while(*++c);
	} else {
		SCPI_err_set(&SCPI_err_1);
		return SCPI_parse_err;
	}
	uint32_t k = 0xffffffff;

	if (flags & SCPI_atoi_SIGNED)
		k = k >> 1;
	if (flags & SCPI_atoi_1B)
		k = k >> 24;
	else if (flags & SCPI_atoi_2B)
		k = k >> 16;
	if (*c || (y & k) != y) {
		if (sig_neg)
			k++;
		if (*c || k != y) {
			SCPI_err_set(&SCPI_err_222);
			return SCPI_parse_err;
		}
	}
	if (sig_neg)
		y = (~y) & 0x7fffffff;
	if (flags & SCPI_atoi_1B)
		*(uint8_t*)x = y;
	else if (flags & SCPI_atoi_2B)
		*(uint16_t*)x = y;
	else if (flags & SCPI_atoi_4B)
		*(uint32_t*)x = y;

	return SCPI_in_drop_param(c);
}

/* Print unisgned int to output */
static SCPI_parse_t SCPI_out_uint(unsigned int i)
{
	char buf[5];
	char *c = buf + 5;
	do {
		*--c = (i % 10) + '0';
		i /= 10;
	} while (i);
	SCPI_printn(c, 5 - (c - buf));
	return SCPI_parse_end;
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
