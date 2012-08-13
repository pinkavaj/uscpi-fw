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
        uint16_t condition_old;

        condition_old = SCPI_OPER.condition;
	SCPI_OPER.condition |= val;
        
        val = condition_old ^ SCPI_OPER.condition;
	SCPI_OPER.event |= val & SCPI_OPER.transition_up;
}

/* Reset bit(s) in Operation Status Register */
void SCPI_OPER_cond_reset(uint16_t val)
{
        uint16_t condition_old;

        condition_old = SCPI_OPER.condition;
	SCPI_OPER.condition &= ~val;

        val = condition_old ^ SCPI_OPER.condition;
	SCPI_OPER.event |= val & SCPI_OPER.transition_down;
}

/* Set bit(s) in Operation Status Register */
void SCPI_QUES_cond_set(uint16_t val)
{
        uint16_t condition_old;

        condition_old = SCPI_QUES.condition;
	SCPI_QUES.condition |= val;

        val = condition_old ^ SCPI_QUES.condition;
	SCPI_QUES.event |= val & SCPI_QUES.transition_up;
}

void SCPI_QUES_cond_reset(uint16_t val)
{
        uint16_t condition_old;

        condition_old = SCPI_QUES.condition;
	SCPI_QUES.condition &= ~val;

        val = condition_old ^ SCPI_QUES.condition;
	SCPI_QUES.event |= val & SCPI_QUES.transition_down;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
