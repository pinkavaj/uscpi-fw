#include <ctype.h>
#include <string.h>

#include "cmd.h"
#include "cmd_tools.h"
#include "scpi_parser.h"
#include "stat_report.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"

/* TODO: přemístit sem definici klíče */

/* Value of OPERation Status Register */
SCPI_parse_t SCPI_IC_stat_oper_cond(void)
{
	SCPI_print_uint16(SCPI_OPER.condition);

        return SCPI_parse_end;
}

/* Value of SCPI OPERation ENABled register */
SCPI_parse_t SCPI_IC_stat_oper_enab(void)
{
        SCPI_parse_t ret;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_OPER.enabled);

                return SCPI_parse_end;
	}
	ret = SCPI_in_uint16(&SCPI_OPER.enabled);
        if (ret == SCPI_parse_err)
                return ret;

	return ret;
}

/* Value of SCPI OPERation Event register */
SCPI_parse_t SCPI_IC_stat_oper_even(void)
{
	SCPI_print_uint16(SCPI_OPER.event & SCPI_OPER.enabled);
        SCPI_OPER.event = 0;

        return SCPI_parse_end;
}

/* Reset all status enable registers (and others) to default state */
SCPI_parse_t SCPI_IC_stat_pres(void)
{
	SCPI_QUES.enabled = 0;
        SCPI_QUES.transition_down = 0;
        SCPI_QUES.transition_up = 0;
	SCPI_OPER.enabled = 0;
        SCPI_OPER.transition_down = SCPI_OPER_SWE;
        SCPI_OPER.transition_up = 0;
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_cond(void)
{
	SCPI_print_uint16(SCPI_QUES.condition);

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_enab(void)
{
        SCPI_parse_t ret;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_QUES.enabled);

                return SCPI_parse_end;
	}
	ret = SCPI_in_uint16(&SCPI_QUES.enabled);
        if (ret == SCPI_parse_err)
                return SCPI_parse_err;
	return ret;
}

SCPI_parse_t SCPI_IC_stat_ques_even(void)
{
	SCPI_print_uint16(SCPI_QUES.event & SCPI_QUES.enabled);
        SCPI_QUES.event = 0;

        return SCPI_parse_end;
}

/* FIXME: just to hide warning, remove as soon as posible */
void fake(void)
{
	SCPI_QUES_cond_set(0);
	SCPI_QUES_cond_reset(0);
}
			
// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
