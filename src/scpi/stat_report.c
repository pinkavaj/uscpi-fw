#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "stat_report.h"
#include "scpi.h"

SCPI_status_reg_t SCPI_OPER;
SCPI_status_reg_t SCPI_QUES;
SCPI_SESR_t SCPI_SESR;
uint8_t SCPI_STB_enabled;

/* Return current value of status byte */
uint8_t SCPI_STB_get(void)
{
	uint8_t status = 0;

        if (SCPI_OPER.event & SCPI_OPER.enabled)
                status |= SCPI_STB_OPER;
        if (SCPI_QUES.event & SCPI_QUES.enabled)
                status |= SCPI_STB_QUES;
        if (SCPI_SESR.event & SCPI_SESR.enabled)
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

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
