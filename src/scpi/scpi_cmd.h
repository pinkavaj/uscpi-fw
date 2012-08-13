#ifndef __SCPI_CMD_H__
#define __SCPI_CMD_H__

#include "scpi.h"

/* SCPI Common Commands */
SCPI_parse_t SCPI_CC_cls(char c);
SCPI_parse_t SCPI_CC_ese(char c);
SCPI_parse_t SCPI_CC_esr(char c);
SCPI_parse_t SCPI_CC_idn(char c);
SCPI_parse_t SCPI_CC_opc(char c);
SCPI_parse_t SCPI_CC_rst(char c);
SCPI_parse_t SCPI_CC_sre(char c);
SCPI_parse_t SCPI_CC_stb(char c);
SCPI_parse_t SCPI_CC_tst(char c);
SCPI_parse_t SCPI_CC_wai(char c);

/* SCPI Instrument Commands */
SCPI_parse_t SCPI_IC_stat_oper_cond(char c);
SCPI_parse_t SCPI_IC_stat_oper_enab(char c);
SCPI_parse_t SCPI_IC_stat_oper_even(char c);
SCPI_parse_t SCPI_IC_stat_pres(char c);
SCPI_parse_t SCPI_IC_stat_ques_cond(char c);
SCPI_parse_t SCPI_IC_stat_ques_enab(char c);
SCPI_parse_t SCPI_IC_stat_ques_even(char c);
SCPI_parse_t SCPI_IC_syst_err_next(char c);
SCPI_parse_t SCPI_IC_syst_vers(char c);

#ifdef SCPI_TEST
SCPI_parse_t SCPI_IC_test_adc(char c);
SCPI_parse_t SCPI_IC_test_div(char c);
SCPI_parse_t SCPI_IC_test_mul(char c);
SCPI_parse_t SCPI_IC_test_num(char c);
SCPI_parse_t SCPI_IC_test_time(char c);
SCPI_parse_t SCPI_IC_test_heat(char c);
#endif

/* SCPI Tools */
SCPI_parse_t SCPI_in_uint8(uint8_t *x);
SCPI_parse_t SCPI_in_uint16(uint16_t *x);
SCPI_parse_t SCPI_in_uint32(uint32_t *x);
SCPI_parse_t SCPI_cmd_err_108(void);

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
