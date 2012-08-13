#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "stat_report.h"
#include "scpi.h"

SCPI_status_reg_t SCPI_OPER;
SCPI_status_reg_t SCPI_QUES;

static struct {
	uint8_t status;
	uint8_t enabled;
} SCPI_SESR;

uint8_t SCPI_STB_enabled;

/* Return current value of status byte */
uint8_t SCPI_STB_get(void)
{
	uint8_t status = 0;

        if (SCPI_OPER.event & SCPI_OPER.enabled)
                status |= SCPI_STB_OPER;
        if (SCPI_QUES.event & SCPI_QUES.enabled)
                status |= SCPI_STB_QUES;
        if (SCPI_SESR.status & SCPI_SESR.enabled)
                status |= SCPI_STB_SESR;
        if (!SCPI_err_queue_empty())
                status |= SCPI_STB_EEQ;
        /* SCPI_STB_USER1 | SCPI_STB_USER2 | SCPI_STB_MAV */
       
        status &= SCPI_STB_enabled;
	if(status)
		status |= SCPI_STB_RQS;
	return status;
}

/* Set bit(s) in Operation Status Register */
void SCPI_OPER_cond_set(uint16_t val)
{
	SCPI_OPER.condition |= val;
	/* Fixme: má reagovat na změnu ne na pokus o změnu (ne na 1 -> 1 ...) */
	val &= SCPI_OPER.transition_up;
	SCPI_OPER.event |= val;
}

/* Reset bit(s) in Operation Status Register */
void SCPI_OPER_cond_reset(uint16_t val)
{
	SCPI_OPER.condition &= ~val;
	val &= SCPI_OPER.transition_down;
	SCPI_OPER.event |= val;
}

/* Get value from OPERation event register */
uint16_t SCPI_OPER_even_get(void)
{
	uint16_t val;
	
	val = SCPI_OPER.event & SCPI_OPER.enabled;
	SCPI_OPER.event = 0;
	return val;
}

/* Set bit(s) in Operation Status Register */
void SCPI_QUES_cond_set(uint16_t val)
{
	SCPI_QUES.condition |= val;
	val &= SCPI_QUES.transition_up;
	SCPI_QUES.event |= val;
}

void SCPI_QUES_cond_reset(uint16_t val)
{
	SCPI_QUES.condition &= ~val;
	val &= SCPI_QUES.transition_down;
	SCPI_QUES.event |= val;
}

/* Get value of QUEStionable event register */
uint16_t SCPI_QUES_even_get(void)
{
	uint16_t val;
	
	val = SCPI_QUES.event & SCPI_QUES.enabled;
	SCPI_QUES.event = 0;
	return val;
}

/* Get value of Standard Event Status Register */
uint8_t SCPI_SESR_get(void)
{
	uint8_t val;
	
	val = SCPI_SESR.status & SCPI_SESR.enabled;
	SCPI_SESR.status = 0;
	return val;
}

void SCPI_SESR_enab_set(uint8_t val)
{
	SCPI_SESR.enabled = val;
}

/* Set event bit in Standard Event Status Register */
void SCPI_SESR_set(uint8_t val)
{
	SCPI_SESR.status |= val;
}

uint8_t SCPI_SESR_enab_get(void)
{
	return SCPI_SESR.enabled;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
