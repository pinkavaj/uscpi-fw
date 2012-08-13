#include <ctype.h>
#include <string.h>

#include "scpi.h"
#include "scpi_cc.h"
#include "scpi_cmd.h"
#include "cmd_tree.h"

/* SCPI command tree item parameters */
#define YES 1
#define NO_ 0
#define GET(g, p) .get_P = g, .get_has_params_P = p
#define PARAMS_N 0
#define PARAMS_Y 1
#define SET(s, par) .set_P = s, par
#define PARAMS(min, opt, once) .set_params_min_P = min, \
		.set_params_opt_P = opt, .set_params_atonce_P = once
#define ATONCE_N 0
#define ATONCE_Y 1
#define OPT_0 0
#define OPT_1 1
#define OPT_2 2
#define OPT_INF 0xff

/* SCPI command short form lenght */
#define SHORT3B (3)
#define SHORT4B (4)

#define _SCPI_branch_END_ { .key_P = NULL, .cmd_P = NULL, .branch_P = NULL, }

/* SCPI Common Commands */
#define _SCPI_CMD_(c, g, s) \
	static const SCPI_cmd_t SCPI_cmd_ ## c ## _P PROGMEM = \
	{ .parser_P = SCPI_CC_ ## c, g, s}
_SCPI_CMD_(cls, GET(NO_, PARAMS_N), SET(YES, PARAMS(0, OPT_0, ATONCE_Y)));
_SCPI_CMD_(ese, GET(YES, PARAMS_N), SET(YES, PARAMS(1, OPT_0, ATONCE_Y)));
_SCPI_CMD_(esr, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(idn, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(opc, GET(YES, PARAMS_N), SET(YES, PARAMS(0, OPT_0, ATONCE_Y)));
_SCPI_CMD_(rst, GET(NO_, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(sre, GET(YES, PARAMS_N), SET(YES, PARAMS(1, OPT_0, ATONCE_Y)));
_SCPI_CMD_(stb, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(tst, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(wai, GET(NO_, PARAMS_N), SET(YES, PARAMS(0, OPT_0, ATONCE_Y)));

#define _key_(kw, key, slen) \
        __extension__ static const SCPI_key_t key_ ## kw ## _P PROGMEM = \
        { .len_short_P = slen, .keyword_P = key, }

_key_(cls, "CLS", SHORT3B);
_key_(ese, "ESE", SHORT3B);
_key_(esr, "ESR", SHORT3B);
_key_(idn, "IDN", SHORT3B);
_key_(opc, "OPC", SHORT3B);
_key_(rst, "RST", SHORT3B);
_key_(sre, "SRE", SHORT3B);
_key_(stb, "STB", SHORT3B);
_key_(tst, "TST", SHORT3B);
_key_(wai, "WAI", SHORT3B);

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
/* Table of SCPI Common Commands */
const SCPI_branch_item_t SCPI_CC_ROOT[] PROGMEM = {
	_SCPI_BRANCH_(key_cls_P, &SCPI_cmd_cls_P, NULL),
	_SCPI_BRANCH_(key_ese_P, &SCPI_cmd_ese_P, NULL),
	_SCPI_BRANCH_(key_esr_P, &SCPI_cmd_esr_P, NULL),
	_SCPI_BRANCH_(key_idn_P, &SCPI_cmd_idn_P, NULL),
	_SCPI_BRANCH_(key_opc_P, &SCPI_cmd_opc_P, NULL),
	_SCPI_BRANCH_(key_rst_P, &SCPI_cmd_rst_P, NULL),
	_SCPI_BRANCH_(key_sre_P, &SCPI_cmd_sre_P, NULL),
	_SCPI_BRANCH_(key_stb_P, &SCPI_cmd_stb_P, NULL),
	_SCPI_BRANCH_(key_tst_P, &SCPI_cmd_tst_P, NULL),
	_SCPI_BRANCH_(key_wai_P, &SCPI_cmd_wai_P, NULL),
	_SCPI_branch_END_,
};

/* SCPI Instrument Commands, list of keywords */
_key_(cond, "CONDITION",    SHORT4B);
_key_(enab, "ENABLE",       SHORT4B);
_key_(err,  "ERROR",        SHORT3B);
_key_(even, "EVENT",        SHORT4B);
_key_(next, "NEXT",         SHORT4B);
_key_(oper, "OPERATION",    SHORT4B);
_key_(pres, "PRESET",       SHORT4B);
_key_(ques, "QUESTIONABLE", SHORT4B);
_key_(res,  "RESISTANCE",   SHORT3B);
_key_(sour, "SOURCE",       SHORT4B);
_key_(stat, "STATUS",       SHORT4B);
_key_(syst, "SYSTEM",       SHORT4B);
_key_(temp, "TEMPERATURE",  SHORT4B);
_key_(test, "TEST",         SHORT4B);
_key_(vers, "VERSION",      SHORT4B);

_key_(adc, "ADC",  SHORT3B);
_key_(div, "DIV",  SHORT3B);
_key_(heat,"HEAT", SHORT4B);
_key_(mul, "MUL",  SHORT3B);
_key_(num, "NUM",  SHORT3B);
_key_(time,"TIME", SHORT4B);

#undef _SCPI_CMD_
#define _SCPI_CMD_(p, g, s) \
	static const SCPI_cmd_t SCPI_cmd_ ## p ## _P PROGMEM = \
	{.parser_P = SCPI_IC_ ## p, g, s}

/* SCPI Instrument Commands */
_SCPI_CMD_(stat_oper_cond, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_oper_enab, GET(YES, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0, ATONCE_Y)));
_SCPI_CMD_(stat_oper_even, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_pres,      GET(NO_, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0, ATONCE_Y))); 
_SCPI_CMD_(stat_ques_cond, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_ques_enab, GET(YES, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0, ATONCE_Y)));
_SCPI_CMD_(stat_ques_even, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(syst_err_next,  GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(syst_vers,      GET(YES, PARAMS_N), SET(NO_, PARAMS_N));

_SCPI_CMD_(test_adc,  GET(YES, PARAMS_N), SET(YES, PARAMS(2, OPT_0, ATONCE_Y)));
_SCPI_CMD_(test_div,  GET(YES, PARAMS_N), SET(YES, PARAMS(3, OPT_0, ATONCE_Y)));
_SCPI_CMD_(test_heat, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(test_mul,  GET(YES, PARAMS_N), SET(YES, PARAMS(2, OPT_0, ATONCE_Y)));
_SCPI_CMD_(test_num,  GET(YES, PARAMS_N), SET(YES, PARAMS(1, OPT_0, ATONCE_Y)));
_SCPI_CMD_(test_temp, GET(YES, PARAMS_Y), SET(NO_, PARAMS_N));
_SCPI_CMD_(test_temp_res, GET(YES, PARAMS_Y), SET(NO_, PARAMS_N));
_SCPI_CMD_(test_time, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));

/* SCPI Instrument Command tree tables */
static const SCPI_branch_item_t SCPI_bt_syst_err_P[] PROGMEM = {
	_SCPI_BRANCH_(key_next_P, &SCPI_cmd_syst_err_next_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_oper_P[] PROGMEM = {
	_SCPI_BRANCH_(key_cond_P, &SCPI_cmd_stat_oper_cond_P, NULL),
	_SCPI_BRANCH_(key_enab_P, &SCPI_cmd_stat_oper_enab_P, NULL),
	_SCPI_BRANCH_(key_even_P, &SCPI_cmd_stat_oper_even_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_ques_P[] PROGMEM = {
	_SCPI_BRANCH_(key_cond_P, &SCPI_cmd_stat_ques_cond_P, NULL),
	_SCPI_BRANCH_(key_enab_P, &SCPI_cmd_stat_ques_enab_P, NULL),
	_SCPI_BRANCH_(key_even_P, &SCPI_cmd_stat_ques_even_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_P[] PROGMEM = {
	_SCPI_BRANCH_(key_ques_P, &SCPI_cmd_stat_ques_even_P, 
			SCPI_bt_stat_ques_P),
	_SCPI_BRANCH_(key_oper_P, &SCPI_cmd_stat_oper_even_P, 
			SCPI_bt_stat_oper_P),
	_SCPI_BRANCH_(key_pres_P, &SCPI_cmd_stat_pres_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_syst_P[] PROGMEM = {
	_SCPI_BRANCH_(key_err_P,  &SCPI_cmd_syst_err_next_P, 
			SCPI_bt_syst_err_P),
	_SCPI_BRANCH_(key_vers_P, &SCPI_cmd_syst_vers_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_test_temp_P[] PROGMEM = {
	_SCPI_BRANCH_(key_res_P, &SCPI_cmd_test_temp_res_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_test_P[] PROGMEM = {
	_SCPI_BRANCH_(key_adc_P, &SCPI_cmd_test_adc_P, NULL),
	_SCPI_BRANCH_(key_div_P, &SCPI_cmd_test_div_P, NULL),
	_SCPI_BRANCH_(key_heat_P, &SCPI_cmd_test_heat_P, NULL),
	_SCPI_BRANCH_(key_mul_P, &SCPI_cmd_test_mul_P, NULL),
	_SCPI_BRANCH_(key_num_P, &SCPI_cmd_test_num_P, NULL),
	_SCPI_BRANCH_(key_temp_P, &SCPI_cmd_test_temp_P, SCPI_bt_test_temp_P),
	_SCPI_BRANCH_(key_time_P, &SCPI_cmd_test_time_P, NULL),
	_SCPI_branch_END_,
};

const SCPI_branch_item_t SCPI_bt_ROOT[] PROGMEM = {
	_SCPI_BRANCH_(key_stat_P, NULL, SCPI_bt_stat_P),
	_SCPI_BRANCH_(key_syst_P, NULL, SCPI_bt_syst_P),

	_SCPI_BRANCH_(key_test_P, NULL, SCPI_bt_test_P),
	_SCPI_branch_END_,
};

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
