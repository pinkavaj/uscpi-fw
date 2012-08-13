#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "stat_report.h"

typedef struct {
	/* TODO: add transition filter support */
	uint16_t condition;
/*	uint16_t transition_up;
	uint16_t transition_down;*/
	uint16_t enabled;
	uint16_t event;
} SCPI_status_reg_t;

static SCPI_status_reg_t SCPI_OPER;
static SCPI_status_reg_t SCPI_QUES;

static struct {
	uint8_t status;
	uint8_t enabled;
} SCPI_SESR;

static struct {
	uint8_t status;
	uint8_t enabled;
} SCPI_status_byte;

/* Return current value of status byte */
uint8_t SCPI_STB_get(void)
{
	uint8_t status;
	
	status = SCPI_status_byte.status & SCPI_status_byte.enabled;
	if(status)
		status |= SCPI_STB_RQS;
	return status;
}

/* Set selected bits in status byte */
void SCPI_STB_set(uint8_t x)
{ 
	SCPI_status_byte.status |= x & ~SCPI_STB_RQS;
}

/* Clear selected bits in status byte */
void SCPI_STB_reset(uint8_t x)
{ 
	SCPI_status_byte.status &= ~x; 
}

/* Returns status register enabled, enabled bits in STB */
uint8_t SCPI_SRE_get(void)
{
	return SCPI_status_byte.enabled;
}

/* Set enabled bits in STB */
void SCPI_SRE_set(uint8_t val)
{
	SCPI_status_byte.enabled = val & ~SCPI_STB_RQS;
}

/* Return operational condition register */
uint16_t SCPI_OPER_cond_get(void)
{
	return SCPI_OPER.condition;
}

/* return operational condition enabled register */
uint16_t SCPI_OPER_enab_get(void)
{
	return SCPI_OPER.enabled;
}

/* Set new value for operational status enabled */
void SCPI_OPER_enab_set(uint16_t val)
{
	SCPI_OPER.enabled = val;
	if (SCPI_OPER.event & SCPI_OPER.enabled)
		SCPI_STB_set(SCPI_STB_OPER);
	else
		SCPI_STB_reset(SCPI_STB_OPER);
}

/* Set bit(s) in Operation Status Register */
void SCPI_OPER_cond_set(uint16_t val)
{
	SCPI_OPER.condition |= val;
	/* Fixme: má reagovat na změnu ne na pokus o změnu (ne na 1 -> 1 ...) */
	val &= SCPI_OPER_trans_to1;
	SCPI_OPER.event |= val;
	if (SCPI_OPER.event & SCPI_OPER.enabled)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Reset bit(s) in Operation Status Register */
void SCPI_OPER_cond_reset(uint16_t val)
{
	SCPI_OPER.condition &= ~val;
	val &= SCPI_OPER_trans_to0;
	SCPI_OPER.event |= val;
	if (SCPI_OPER.event & SCPI_OPER.enabled)
		SCPI_STB_set(SCPI_STB_OPER);
}

/* Get value from OPERation event register */
uint16_t SCPI_OPER_even_get(void)
{
	uint16_t val;
	
	val = SCPI_OPER.event & SCPI_OPER.enabled;
	SCPI_OPER.event = 0;
	SCPI_STB_reset(SCPI_STB_OPER);
	return val;
}

uint16_t SCPI_QUES_cond_get(void)
{
	return SCPI_QUES.condition;
}

/* Set bit(s) in Operation Status Register */
void SCPI_QUES_cond_set(uint16_t val)
{
	SCPI_QUES.condition |= val;
	val &= SCPI_QUES_trans_to1;
	SCPI_QUES.event |= val;
	if (SCPI_QUES.event & SCPI_QUES.enabled)
		SCPI_STB_set(SCPI_STB_QUES);
}

void SCPI_QUES_cond_reset(uint16_t val)
{
	SCPI_QUES.condition &= ~val;
	val &= SCPI_QUES_trans_to0;
	SCPI_QUES.event |= val;
	if (SCPI_QUES.event & SCPI_QUES.enabled)
		SCPI_STB_set(SCPI_STB_QUES);
}

uint16_t SCPI_QUES_enab_get(void)
{
	return SCPI_QUES.enabled;
}

/* update QUEStionable enable register changed */
void SCPI_QUES_enab_set(uint16_t enabled)
{
	SCPI_QUES.enabled = enabled;
	if (SCPI_QUES.event & SCPI_QUES.enabled)
		SCPI_STB_set(SCPI_STB_QUES);
	else
		SCPI_STB_reset(SCPI_STB_QUES);
}

/* Get value of QUEStionable event register */
uint16_t SCPI_QUES_even_get(void)
{
	uint16_t val;
	
	val = SCPI_QUES.event & SCPI_QUES.enabled;
	SCPI_QUES.event = 0;
	SCPI_STB_reset(SCPI_STB_QUES);
	return val;
}

/* Update sumary register when Stantard Event Status Enable Register changed */
void SCPI_SESR_enab_update(void)
{
	if (SCPI_SESR.status & SCPI_SESR.enabled)
		SCPI_STB_set(SCPI_STB_SESR);
	else 
		SCPI_STB_reset(SCPI_STB_SESR);
}

/* Get value of Standard Event Status Register */
uint8_t SCPI_SESR_get(void)
{
	uint8_t val;
	
	val = SCPI_SESR.status & SCPI_SESR.enabled;
	SCPI_SESR.status = 0;
	SCPI_STB_reset(SCPI_STB_SESR);
	return val;
}

void SCPI_SESR_enab_set(uint8_t val)
{
	SCPI_SESR.enabled = val;
	SCPI_SESR_enab_update();
}

/* Set event bit in Standard Event Status Register */
void SCPI_SESR_set(uint8_t val)
{
	SCPI_SESR.status |= val;
	if (SCPI_SESR.status & SCPI_SESR.enabled)
		SCPI_STB_set(SCPI_STB_SESR);
}

uint8_t SCPI_SESR_enab_get(void)
{
	return SCPI_SESR.enabled;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
