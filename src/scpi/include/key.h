#ifndef _SCPI_KEY_H_
#define _SCPI_KEY_H_

#include <avr/pgmspace.h>
#include <inttypes.h>

/*
 * len_short - lenght of short form of command (3 or 4 B)
 * keyword - SCPI keyword, UPPERCASE long form of keyword
 * */
typedef struct SCPI_key_struct {
	const uint8_t len_short_P;
	const prog_char keyword_P[];
} SCPI_key_t;

extern const SCPI_key_t key_cls_P PROGMEM;
extern const SCPI_key_t key_ese_P PROGMEM;
extern const SCPI_key_t key_esr_P PROGMEM;
extern const SCPI_key_t key_idn_P PROGMEM;
extern const SCPI_key_t key_opc_P PROGMEM;
extern const SCPI_key_t key_rst_P PROGMEM;
extern const SCPI_key_t key_sre_P PROGMEM;
extern const SCPI_key_t key_stb_P PROGMEM;
extern const SCPI_key_t key_tst_P PROGMEM;
extern const SCPI_key_t key_wai_P PROGMEM;
 
extern const SCPI_key_t key_cond_P PROGMEM;
extern const SCPI_key_t key_dwel_P PROGMEM;
extern const SCPI_key_t key_enab_P PROGMEM;
extern const SCPI_key_t key_err_P PROGMEM;
extern const SCPI_key_t key_even_P PROGMEM;
extern const SCPI_key_t key_gain_P PROGMEM;
extern const SCPI_key_t key_int_P PROGMEM;
extern const SCPI_key_t key_lcon_P PROGMEM;
extern const SCPI_key_t key_meas_P PROGMEM;
extern const SCPI_key_t key_mode_P PROGMEM;
extern const SCPI_key_t key_next_P PROGMEM;
extern const SCPI_key_t key_oper_P PROGMEM;
extern const SCPI_key_t key_pres_P PROGMEM;
extern const SCPI_key_t key_ques_P PROGMEM;
extern const SCPI_key_t key_res_P PROGMEM;
extern const SCPI_key_t key_rtim_P PROGMEM;
extern const SCPI_key_t key_sour_P PROGMEM;
extern const SCPI_key_t key_spo_P PROGMEM;
extern const SCPI_key_t key_stat_P PROGMEM;
extern const SCPI_key_t key_syst_P PROGMEM;
extern const SCPI_key_t key_temp_P PROGMEM;
extern const SCPI_key_t key_test_P PROGMEM;
extern const SCPI_key_t key_vers_P PROGMEM;
 
extern const SCPI_key_t key_adc_P PROGMEM;
extern const SCPI_key_t key_div_P PROGMEM;
extern const SCPI_key_t key_heat_P PROGMEM;
extern const SCPI_key_t key_mul_P PROGMEM;
extern const SCPI_key_t key_num_P PROGMEM;
extern const SCPI_key_t key_time_P PROGMEM;

#endif

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab

