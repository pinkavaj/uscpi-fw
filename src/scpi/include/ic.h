#ifndef __SCPI_IC_H__
#define __SCPI_IC_H__

#include "scpi.h"
#include "ic_sour_temp.h"

extern uint8_t SCPI_params_count;

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
SCPI_parse_t SCPI_IC_test_temp(char c);
SCPI_parse_t SCPI_IC_test_temp_res(char c);
SCPI_parse_t SCPI_IC_test_time(char c);
SCPI_parse_t SCPI_IC_test_heat(char c);
#endif

#endif
/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
