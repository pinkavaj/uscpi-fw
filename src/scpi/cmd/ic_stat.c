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
	SCPI_print_uint16(SCPI_OPER_cond_get());

        return SCPI_parse_end;
}

/* Value of SCPI OPERation ENABled register */
SCPI_parse_t SCPI_IC_stat_oper_enab(void)
{
        SCPI_parse_t ret;
        uint16_t val;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_OPER_enab_get());

                return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_err_set_(&SCPI_err_108);
	ret = SCPI_in_uint16(&val);
        if (ret == SCPI_parse_err)
                return ret;
	SCPI_OPER_enab_set(val);

	return ret;
}

/* Value of SCPI OPERation Event register */
SCPI_parse_t SCPI_IC_stat_oper_even(void)
{
	SCPI_print_uint16(SCPI_OPER_even_get());

        return SCPI_parse_end;
}

/* Reset all status enable registers (and others) to default state */
SCPI_parse_t SCPI_IC_stat_pres(void)
{
	SCPI_QUES_enab_set(0);
	SCPI_OPER_enab_set(0);
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_cond(void)
{
	SCPI_print_uint16(SCPI_QUES_cond_get());

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_enab(void)
{
        SCPI_parse_t ret;
        uint16_t val;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_QUES_enab_get());

                return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_err_set_(&SCPI_err_108);
	ret = SCPI_in_uint16(&val);
        if (ret == SCPI_parse_err)
                return SCPI_parse_err;
	SCPI_QUES_enab_set(val);
	return ret;
}

SCPI_parse_t SCPI_IC_stat_ques_even(void)
{
	SCPI_print_uint16(SCPI_QUES_even_get());

        return SCPI_parse_end;
}

/* FIXME: just to hide warning, remove as soon as posible */
void fake(void)
{
	SCPI_OPER_cond_set(0);
	SCPI_OPER_cond_reset(0);
	SCPI_QUES_cond_set(0);
	SCPI_QUES_cond_reset(0);
}
			
// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
