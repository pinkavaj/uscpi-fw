
#include <ctype.h>
#include <string.h>

#include "scpi.h"
#include "scpi_cmd.h"
#include "scpi_tree.h"


/* 
 * get - command has query form (keyword followed by '?')
 * get_has_params - if 1 get (questionalble form) may have parameters
 * 	0 - questionable form does not accept parameters
 * set - command has set form
 * set_params_atonce - call function when all parameters parsing is finished
 * set_params_min - minimal amount of parameters required for function
 * set_params_opt - number of optional parameters, total maximal amout of
 * 	parameter allowed if set_params_min + set_params_opt
 * instruments - count of instruments  - 1, sufix is number
 * 	in range <1,N> and for internal storage are mapped to range <0,N-1>
 * parser - function to set as parser if keyword found or NULL for default
 * */
struct SCPI_cmd_struct {
	const uint8_t get_P;
	const uint8_t get_has_params_P;
	const uint8_t set_P;
	const uint8_t set_params_atonce_P;
	const uint8_t set_params_min_P;
	const uint8_t set_params_opt_P;
	uint8_t *num_suffix_P;
	SCPI_parse_t(*parser_P)(char c);
};
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

/*
 * short4b - if 1 short form of keyword is 4B lenght, if 0 it is 3B length
 * len - lenght of keyword
 * keyword - SCPI keyword, UPPERCASE long form of keyword
 * */
struct SCPI_key_struct {
	const uint8_t len_P;
	const uint8_t len_short_P;
	const char const *keyword_P;
};
#define SHORT3B (3)
#define SHORT4B (4)

/*
 * key - SCPI keyword for this item
 * func - SCPI function for this keyword or NULL
 * branch - sub-branch for this keyword or NULL
 * */
struct SCPI_branch_item_struct {
	const SCPI_key_t const *key_P;
	const SCPI_cmd_t const *cmd_P;
	const SCPI_branch_item_t const *branch_P;
};
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

/* SCPI Common Commands, need be filled to 4B with \0 */
static const char SCPI_kw_cls_P[] PROGMEM = "CLS";
static const char SCPI_kw_ese_P[] PROGMEM = "ESE";
static const char SCPI_kw_esr_P[] PROGMEM = "ESR";
static const char SCPI_kw_idn_P[] PROGMEM = "IDN";
static const char SCPI_kw_opc_P[] PROGMEM = "OPC";
static const char SCPI_kw_rst_P[] PROGMEM = "RST";
static const char SCPI_kw_sre_P[] PROGMEM = "SRE";
static const char SCPI_kw_stb_P[] PROGMEM = "STB";
static const char SCPI_kw_tst_P[] PROGMEM = "TST";
static const char SCPI_kw_wai_P[] PROGMEM = "WAI";
       
#define _SCPI_key_(kw, s) static const SCPI_key_t SCPI_key_ ## kw ## _P \
	PROGMEM = { .keyword_P = SCPI_kw_ ## kw ## _P, .len_short_P = s, \
	.len_P = sizeof(SCPI_kw_ ## kw ## _P) - 1 }

_SCPI_key_(cls, SHORT3B);
_SCPI_key_(ese, SHORT3B);
_SCPI_key_(esr, SHORT3B);
_SCPI_key_(idn, SHORT3B);
_SCPI_key_(opc, SHORT3B);
_SCPI_key_(rst, SHORT3B);
_SCPI_key_(sre, SHORT3B);
_SCPI_key_(stb, SHORT3B);
_SCPI_key_(tst, SHORT3B);
_SCPI_key_(wai, SHORT3B);

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
/* Table of SCPI Common Commands */
const SCPI_branch_item_t SCPI_CC_ROOT[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_cls_P, &SCPI_cmd_cls_P, NULL),
	_SCPI_BRANCH_(SCPI_key_ese_P, &SCPI_cmd_ese_P, NULL),
	_SCPI_BRANCH_(SCPI_key_esr_P, &SCPI_cmd_esr_P, NULL),
	_SCPI_BRANCH_(SCPI_key_idn_P, &SCPI_cmd_idn_P, NULL),
	_SCPI_BRANCH_(SCPI_key_opc_P, &SCPI_cmd_opc_P, NULL),
	_SCPI_BRANCH_(SCPI_key_rst_P, &SCPI_cmd_rst_P, NULL),
	_SCPI_BRANCH_(SCPI_key_sre_P, &SCPI_cmd_sre_P, NULL),
	_SCPI_BRANCH_(SCPI_key_stb_P, &SCPI_cmd_stb_P, NULL),
	_SCPI_BRANCH_(SCPI_key_tst_P, &SCPI_cmd_tst_P, NULL),
	_SCPI_BRANCH_(SCPI_key_wai_P, &SCPI_cmd_wai_P, NULL),
	_SCPI_branch_END_,
};

/* SCPI Instrument Commands, list of keywords */
static const char SCPI_kw_cond_P[] PROGMEM = "CONDITION";
static const char SCPI_kw_enab_P[] PROGMEM = "ENABLE";
static const char SCPI_kw_err_P[]  PROGMEM = "ERROR";
static const char SCPI_kw_even_P[] PROGMEM = "EVENT";
static const char SCPI_kw_next_P[] PROGMEM = "NEXT";
static const char SCPI_kw_oper_P[] PROGMEM = "OPERATION";
static const char SCPI_kw_pres_P[] PROGMEM = "PRESET";
static const char SCPI_kw_ques_P[] PROGMEM = "QUESTIONABLE";
static const char SCPI_kw_stat_P[] PROGMEM = "STATUS";
static const char SCPI_kw_syst_P[] PROGMEM = "SYSTEM";
static const char SCPI_kw_vers_P[] PROGMEM = "VERSION";

static const char SCPI_kw_test_P[] PROGMEM = "TEST";
static const char SCPI_kw_adc_P[] PROGMEM = "ADC";
static const char SCPI_kw_div_P[] PROGMEM = "DIV";
static const char SCPI_kw_mul_P[] PROGMEM = "MUL";
       
_SCPI_key_(cond, SHORT4B);
_SCPI_key_(enab, SHORT4B);
_SCPI_key_(err, SHORT3B);
_SCPI_key_(even, SHORT4B);
_SCPI_key_(next, SHORT4B);
_SCPI_key_(oper, SHORT4B);
_SCPI_key_(pres, SHORT4B);
_SCPI_key_(ques, SHORT4B);
_SCPI_key_(stat, SHORT4B);
_SCPI_key_(syst, SHORT4B);
_SCPI_key_(vers, SHORT4B);

_SCPI_key_(test, SHORT4B);
_SCPI_key_(adc, SHORT3B);
_SCPI_key_(div, SHORT3B);
_SCPI_key_(mul, SHORT3B);

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

_SCPI_CMD_(test_adc,  GET(YES, PARAMS_N), SET(NO_, PARAMS_N));
_SCPI_CMD_(test_div,  GET(YES, PARAMS_N), SET(YES, PARAMS(3, OPT_0, ATONCE_Y)));
_SCPI_CMD_(test_mul,  GET(YES, PARAMS_N), SET(YES, PARAMS(2, OPT_0, ATONCE_Y)));

/* SCPI Instrument Command tree tables */
static const SCPI_branch_item_t SCPI_bt_syst_err_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_next_P, &SCPI_cmd_syst_err_next_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_oper_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_cond_P, &SCPI_cmd_stat_oper_cond_P, NULL),
	_SCPI_BRANCH_(SCPI_key_enab_P, &SCPI_cmd_stat_oper_enab_P, NULL),
	_SCPI_BRANCH_(SCPI_key_even_P, &SCPI_cmd_stat_oper_even_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_ques_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_cond_P, &SCPI_cmd_stat_ques_cond_P, NULL),
	_SCPI_BRANCH_(SCPI_key_enab_P, &SCPI_cmd_stat_ques_enab_P, NULL),
	_SCPI_BRANCH_(SCPI_key_even_P, &SCPI_cmd_stat_ques_even_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_stat_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_ques_P, &SCPI_cmd_stat_ques_even_P, 
			SCPI_bt_stat_ques_P),
	_SCPI_BRANCH_(SCPI_key_oper_P, &SCPI_cmd_stat_oper_even_P, 
			SCPI_bt_stat_oper_P),
	_SCPI_BRANCH_(SCPI_key_pres_P, &SCPI_cmd_stat_pres_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_syst_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_err_P,  &SCPI_cmd_syst_err_next_P, 
			SCPI_bt_syst_err_P),
	_SCPI_BRANCH_(SCPI_key_vers_P, &SCPI_cmd_syst_vers_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_test_P[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_adc_P, &SCPI_cmd_test_adc_P, NULL),
	_SCPI_BRANCH_(SCPI_key_div_P, &SCPI_cmd_test_div_P, NULL),
	_SCPI_BRANCH_(SCPI_key_mul_P, &SCPI_cmd_test_mul_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_ROOT[] PROGMEM = {
	_SCPI_BRANCH_(SCPI_key_stat_P, NULL, SCPI_bt_stat_P),
	_SCPI_BRANCH_(SCPI_key_syst_P, NULL, SCPI_bt_syst_P),

	_SCPI_BRANCH_(SCPI_key_test_P, NULL, SCPI_bt_test_P),
	_SCPI_branch_END_,
};

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
