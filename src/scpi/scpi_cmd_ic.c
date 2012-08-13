#include <ctype.h>
#include <string.h>

#include "scpi_cmd.h"


/* Value of OPERationas Status Register */
static SCPI_parse_t SCPI_IC_stat_oper_cond(char UNUSED(c))
{
	SCPI_out_uint32(SCPI_OPER_cond);
	return SCPI_parse_end;
}

/* Value of SCPI OPERation ENABled register */
static SCPI_parse_t SCPI_IC_stat_oper_enab(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_out_uint32(SCPI_OPER_enab);
		return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	SCPI_parse_t ret = SCPI_in_uint16(&SCPI_OPER_enab);
	SCPI_OPER_enab_update();
	return ret;
}

/* Value of SCPI OPERation Event register */
static SCPI_parse_t SCPI_IC_stat_oper_even(char UNUSED(c))
{
	SCPI_out_uint32(SCPI_OPER_even_get());
	return SCPI_parse_end;
}

/* Reset all status enable registers (and others) to default state */
static SCPI_parse_t SCPI_IC_stat_pres(char UNUSED(c))
{
	SCPI_QUES_enab = 0;
	SCPI_OPER_enab = 0;
	SCPI_QUES_enab_update();
	SCPI_OPER_enab_update();
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_stat_ques_cond(char UNUSED(c))
{
	SCPI_out_uint32(SCPI_QUES_cond);
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_stat_ques_enab(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_out_uint32(SCPI_QUES_enab);
		return SCPI_parse_end;
	}
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	SCPI_parse_t ret = SCPI_in_uint16(&SCPI_QUES_enab);
	SCPI_QUES_enab_update();
	return ret;
}

static SCPI_parse_t SCPI_IC_stat_ques_even(char UNUSED(c))
{
	SCPI_out_uint32(SCPI_QUES_even_get());
	return SCPI_parse_end;
}

#include "../drivers/spi_dev.h"
#include "../drivers/ad974.h"
static SCPI_parse_t SCPI_IC_test_adc(char UNUSED(c))
{
	uint16_t val;

	SPI_dev_select(SPI_DEV_AD974_0);
	val = ad974_get_sample(0);
	SCPI_out_uint32(val);
	return SCPI_parse_end;
}

#include "../lib/math.h"
static SCPI_parse_t SCPI_IC_test_div(char UNUSED(c))
{
	static uint64_t nom;
	static uint32_t num;

	if (_SCPI_CMD_IS_QUEST()) {
		uint32_t val = math_div_64_32_r32(nom, num);
		SCPI_out_uint32(val);
	} else {
		uint32_t *p = (void *)&nom;
		SCPI_in_uint32(p+1);
		SCPI_in_uint32(p);
		SCPI_in_uint32(&num);
	}
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_mul(char UNUSED(c))
{
	static uint32_t val1, val2;

	if (_SCPI_CMD_IS_QUEST()) {
		volatile uint64_t val;

		val = math_mul_32_32_r64(val1, val2);
		uint32_t *p = (void*)&val;
		SCPI_out_uint32(*(p+1));
		SCPI_putc(',');
		SCPI_out_uint32(val);
	} else {
		SCPI_in_uint32(&val1);
		SCPI_in_uint32(&val2);
	}
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_num(char UNUSED(c))
{
	static uint32_t val;

	if (_SCPI_CMD_IS_QUEST())
		SCPI_out_uint32(val);
	else
		SCPI_in_uint32(&val);
	return SCPI_parse_end;
}

/* Get (next) error from error queue */
static SCPI_parse_t SCPI_IC_syst_err_next(char UNUSED(c))
{
	const SCPI_err_t *e;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		if (SCPI_err_count == 0) 
			e = &SCPI_err_0;
		else {
			e = SCPI_err[0];
			memmove(SCPI_err, SCPI_err + 1, 
					(SCPI_ERR_MAX - 1) * sizeof(void *));
			/* Error queue overflow indicated by 
			 * SCPI_err_count > SCPI_ERR_MAX */
			if (--SCPI_err_count == SCPI_ERR_MAX) 
				SCPI_err[SCPI_ERR_MAX - 1] = &SCPI_err_350;
			if (!SCPI_err_count)
				SCPI_STB_reset(SCPI_STB_EEQ);
		}
	}
	SCPI_print_P((const char*)pgm_read_word(&e->str_P));
	return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_syst_vers(char UNUSED(c))
{
	SCPI_print_P(SCPI_version_P);
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
