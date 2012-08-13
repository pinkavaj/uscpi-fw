#ifndef __SCPI_IC_H__
#define __SCPI_IC_H__

#include "scpi.h"
#include "ic_cont.h"
#include "ic_meas.h"
#include "ic_sens_list.h"
#include "ic_sour_temp.h"
#ifdef SCPI_TEST
#include "ic_test.h"
#endif

extern uint8_t SCPI_params_count;

/* SCPI Instrument Commands */
SCPI_parse_t SCPI_IC_stat_oper_cond(void);
SCPI_parse_t SCPI_IC_stat_oper_enab(void);
SCPI_parse_t SCPI_IC_stat_oper_even(void);
SCPI_parse_t SCPI_IC_stat_pres(void);
SCPI_parse_t SCPI_IC_stat_ques_cond(void);
SCPI_parse_t SCPI_IC_stat_ques_enab(void);
SCPI_parse_t SCPI_IC_stat_ques_even(void);
SCPI_parse_t SCPI_IC_syst_err_next(void);
SCPI_parse_t SCPI_IC_syst_vers(void);

#endif
/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
