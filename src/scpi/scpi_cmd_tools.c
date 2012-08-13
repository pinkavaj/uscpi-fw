#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "scpi_cmd.h"

uint16_t SCPI_OPER_cond_ = 0;
uint16_t SCPI_OPER_enab_ = 0;
uint16_t SCPI_OPER_even = 0;

uint8_t SCPI_SESR_ = 0;
uint8_t SCPI_SESR_enab_ = 0;

uint8_t SCPI_STB_ = 0;
uint8_t SCPI_SRE_ = 0;


uint8_t SCPI_STB(void)
{
	return SCPI_STB_;
}

void SCPI_STB_set(uint8_t x)
{ 
	SCPI_STB_ |= x & ~SCPI_STB_RQS;
}

void SCPI_STB_reset(uint8_t x)
{ 
	SCPI_STB_ &= ~x; 
}

uint8_t SCPI_SRE(void)
{
	return SCPI_SRE_;
}

void SCPI_SRE_set_(uint8_t val)
{
	SCPI_SRE_ = val;
}

void SCPI_SRE_update(void)
{ 
	SCPI_SRE_ &= ~SCPI_STB_RQS; 
}

uint16_t SCPI_OPER_cond(void)
{
	return SCPI_OPER_cond_;
}

uint16_t SCPI_OPER_enab(void)
{
	return SCPI_OPER_enab_;
}

void SCPI_OPER_enab_set_(uint16_t val)
{
	SCPI_OPER_enab_ = val;
}

/* Set bit(s) in Operation Status Register */
void SCPI_OPER_cond_set(uint16_t val)
{
	SCPI_OPER_cond_ |= val;
	val &= SCPI_OPER_trans_to1;
	SCPI_OPER_even |= val;
	if (SCPI_OPER_even & SCPI_OPER_enab_)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Reset bit(s) in Operation Status Register */
void SCPI_OPER_cond_reset(uint16_t val)
{
	SCPI_OPER_cond_ &= ~val;
	val &= SCPI_OPER_trans_to0;
	SCPI_OPER_even |= val;
	if (SCPI_OPER_even & SCPI_OPER_enab_)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Update sumary register after OPERation enabled register changed */
void SCPI_OPER_enab_update(void)
{
	if (SCPI_OPER_even & SCPI_OPER_enab_)
		SCPI_STB_set(SCPI_STB_OPER);
	else
		SCPI_STB_reset(SCPI_STB_OPER);
}

/* Get value from OPERation event register */
uint16_t SCPI_OPER_even_get(void)
{
	uint16_t x = SCPI_OPER_even & SCPI_OPER_enab_;
	SCPI_OPER_even = 0;
	SCPI_STB_reset(SCPI_STB_OPER);
	return x;
}

/* Set bit(s) in Operation Status Register */
void SCPI_QUES_cond_set(uint16_t val)
{
	SCPI_QUES_cond |= val;
	val &= SCPI_QUES_trans_to1;
	SCPI_QUES_even |= val;
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
}

/* Reset bit(s) in Operation Status Register */
void SCPI_QUES_cond_reset(uint16_t val)
{
	SCPI_QUES_cond &= ~val;
	val &= SCPI_QUES_trans_to0;
	SCPI_QUES_even |= val;
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
}

/* Update sumary register after QUEStionable enable register changed */
void SCPI_QUES_enab_update(void)
{
	if (SCPI_QUES_even & SCPI_QUES_enab)
		SCPI_STB_set(SCPI_STB_QUES);
	else
		SCPI_STB_reset(SCPI_STB_QUES);
}

/* Get value of QUEStionable event register */
uint16_t SCPI_QUES_even_get(void)
{
	uint16_t x = SCPI_QUES_even & SCPI_QUES_enab;
	SCPI_QUES_even = 0;
	SCPI_STB_reset(SCPI_STB_QUES);
	return x;
}

void SCPI_SESR_set_(uint8_t val)
{
	SCPI_SESR_ = val;
}

/* Get value of Standard Event Status Register */
uint8_t SCPI_SESR_get(void)
{
	uint8_t x = SCPI_SESR_ & SCPI_SESR_enab_;
	SCPI_SESR_ = 0;
	SCPI_STB_reset(SCPI_STB_SESR);
	return x;
}

/* Set event bit in Standard Event Status Register */
void SCPI_SESR_set(uint8_t val)
{
	SCPI_SESR_ |= val;
	if (SCPI_SESR_ & SCPI_SESR_enab_)
		SCPI_STB_set(SCPI_STB_SESR);
}

uint8_t SCPI_SESR_enab(void)
{
	return SCPI_SESR_enab_;
}

/* Update sumary register when Stantard Event Status Enable Register changed */
void SCPI_SESR_enab_update(void)
{
	if (SCPI_SESR_ & SCPI_SESR_enab_)
		SCPI_STB_set(SCPI_STB_SESR);
	else 
		SCPI_STB_reset(SCPI_STB_SESR);
}

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
