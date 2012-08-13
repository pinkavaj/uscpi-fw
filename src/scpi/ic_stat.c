#include <ctype.h>
#include <string.h>

#include "scpi_cmd.h"
#include "scpi_cmd_tools.h"
#include "scpi_parser.h"
#include "status_reporting.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"

/* Value of OPERation Status Register */
SCPI_parse_t SCPI_IC_stat_oper_cond(char UNUSED(c))
{
	print_uint32(SCPI_OPER_cond_get());
	return SCPI_parse_end;
}

/* Value of SCPI OPERation ENABled register */
SCPI_parse_t SCPI_IC_stat_oper_enab(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		print_uint32(SCPI_OPER_enab_get());
		return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	uint16_t val;
	SCPI_parse_t ret = SCPI_in_uint16(&val);
	SCPI_OPER_enab_set(val);
	return ret;
}

/* Value of SCPI OPERation Event register */
SCPI_parse_t SCPI_IC_stat_oper_even(char UNUSED(c))
{
	print_uint32(SCPI_OPER_even_get());
	return SCPI_parse_end;
}

/* Reset all status enable registers (and others) to default state */
SCPI_parse_t SCPI_IC_stat_pres(char UNUSED(c))
{
	SCPI_QUES_enab_set(0);
	SCPI_OPER_enab_set(0);
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_cond(char UNUSED(c))
{
	print_uint32(SCPI_QUES_cond_get());
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_stat_ques_enab(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		print_uint32(SCPI_QUES_enab_get());
		return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	uint16_t val;
	SCPI_parse_t ret = SCPI_in_uint16(&val);
	SCPI_QUES_enab_set(val);
	return ret;
}

SCPI_parse_t SCPI_IC_stat_ques_even(char UNUSED(c))
{
	print_uint32(SCPI_QUES_even_get());
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
