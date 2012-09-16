#include <ctype.h>
#include <string.h>

#include "cmd_tree.h"
#include "cc.h"
#include "ic.h"
#include "scpi.h"
#include "key.h"

/* SCPI command tree item parameters */
#define YES 1
#define NO_ 0
#define GET(g, p) .get_P = g, .get_has_params_P = p
#define PARAMS_N 0
#define PARAMS_Y 1
#define SET(s, par) .set_P = s, par
#define PARAMS(min, opt) .set_params_min_P = min, .set_params_opt_P = opt
#define OPT_0 0
#define OPT_1 1
#define OPT_2 2
#define OPT_INF 0xff

/* SCPI command short form lenght */
#define SHORT3B (3)
#define SHORT4B (4)

/* SCPI Common Commands */
#define _SCPI_CMD_(c, g, s) \
	static const SCPI_cmd_t SCPI_cmd_ ## c ## _P PROGMEM = \
	{ .parser_P = SCPI_CC_ ## c, g, s }
_SCPI_CMD_(cls, GET(NO_, PARAMS_N), SET(YES, PARAMS(0, OPT_0)));
_SCPI_CMD_(ese, GET(YES, PARAMS_N), SET(YES, PARAMS(1, OPT_0)));
_SCPI_CMD_(esr, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(idn, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(opc, GET(YES, PARAMS_N), SET(YES, PARAMS(0, OPT_0)));
_SCPI_CMD_(rst, GET(NO_, PARAMS_N), SET(YES, PARAMS_N));
_SCPI_CMD_(sre, GET(YES, PARAMS_N), SET(YES, PARAMS(1, OPT_0)));
_SCPI_CMD_(stb, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(trg, GET(NO_, PARAMS_N), SET(YES, PARAMS_N));
_SCPI_CMD_(tst, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(wai, GET(NO_, PARAMS_N), SET(YES, PARAMS(0, OPT_0)));

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
	_SCPI_BRANCH_(key_trg_P, &SCPI_cmd_trg_P, NULL),
	_SCPI_BRANCH_(key_tst_P, &SCPI_cmd_tst_P, NULL),
	_SCPI_BRANCH_(key_wai_P, &SCPI_cmd_wai_P, NULL),
	_SCPI_branch_END_,
};

#undef _SCPI_CMD_
#define _SCPI_CMD_(p, n, g, s) \
	static const SCPI_cmd_t SCPI_cmd_ ## p ## _P PROGMEM = \
	{.parser_P = SCPI_IC_ ## p, g, s, .num_suffix_max_P = n}
/* SCPI Instrument Commands */
_SCPI_CMD_(stat_oper_cond, 0, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_oper_enab, 0, GET(YES, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0)));
_SCPI_CMD_(stat_oper_even, 0, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_pres, 0,      GET(NO_, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0))); 
_SCPI_CMD_(stat_ques_cond, 0, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(stat_ques_enab, 0, GET(YES, PARAMS_N), SET(YES, 
			PARAMS(1, OPT_0)));
_SCPI_CMD_(stat_ques_even, 0, GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(syst_err_next, 0,  GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(syst_vers, 0,      GET(YES, PARAMS_N), SET(NO_, PARAMS_N));

/* SCPI Instrument Command tree tables */
static const SCPI_branch_item_t SCPI_bt_sens_P[] PROGMEM = {
	_SCPI_BRANCH_(key_list_P, &SCPI_cmd_sens_list_P, SCPI_bt_sens_list_P),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_sour_P[] PROGMEM = {
	_SCPI_BRANCH_(key_temp_P, &SCPI_cmd_sour_temp_P, SCPI_bt_sour_temp_P),
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

static const SCPI_branch_item_t SCPI_bt_syst_err_P[] PROGMEM = {
	_SCPI_BRANCH_(key_next_P, &SCPI_cmd_syst_err_next_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_syst_P[] PROGMEM = {
	_SCPI_BRANCH_(key_err_P,  &SCPI_cmd_syst_err_next_P, 
			SCPI_bt_syst_err_P),
	_SCPI_BRANCH_(key_vers_P, &SCPI_cmd_syst_vers_P, NULL),
	_SCPI_branch_END_,
};

const SCPI_branch_item_t SCPI_bt_ROOT[] PROGMEM = {
	_SCPI_BRANCH_(key_cont_P, NULL, SCPI_bt_cont_P),
	_SCPI_BRANCH_(key_meas_P, NULL, SCPI_bt_meas_P),
	_SCPI_BRANCH_(key_sens_P, NULL, SCPI_bt_sens_P),
	_SCPI_BRANCH_(key_sour_P, NULL, SCPI_bt_sour_P),
	_SCPI_BRANCH_(key_stat_P, NULL, SCPI_bt_stat_P),
	_SCPI_BRANCH_(key_syst_P, NULL, SCPI_bt_syst_P),
#ifdef SCPI_TEST
	_SCPI_BRANCH_(key_test_P, NULL, SCPI_bt_test_P),
#endif
	_SCPI_branch_END_,
};

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
