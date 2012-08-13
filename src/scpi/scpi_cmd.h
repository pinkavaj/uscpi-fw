#ifndef __SCPI_CMD_H__
#define __SCPI_CMD_H__

#include "scpi.h"

/* SCPI Operation Status */
#define SCPI_OPER_CALI	(1<<0)
#define SCPI_OPER_SETT	(1<<1)
#define SCPI_OPER_RANG	(1<<2)
#define SCPI_OPER_SWE	(1<<3)
#define SCPI_OPER_MEAS	(1<<4)
#define SCPI_OPER_TRIG	(1<<5)
#define SCPI_OPER_ARM	(1<<6)
#define SCPI_OPER_CORR	(1<<7)
#define SCPI_OPER_USR1	(1<<8)
#define SCPI_OPER_USR2	(1<<9)
#define SCPI_OPER_USR3	(1<<10)
#define SCPI_OPER_USR4	(1<<11)
#define SCPI_OPER_USR5	(1<<12)
#define SCPI_OPER_INST	(1<<13)
#define SCPI_OPER_PROG	(1<<14)
#define SCPI_OPER_trans_to1 0xffff
#define SCPI_OPER_trans_to0 0x0
uint16_t SCPI_OPER_cond(void);
void SCPI_OPER_cond_set(uint16_t val);
void SCPI_OPER_cond_reset(uint16_t val);
uint16_t SCPI_OPER_enab(void);
void SCPI_OPER_enab_update(void);
void SCPI_OPER_enab_set_(uint16_t val);
uint16_t SCPI_OPER_even_get(void);
/* Questionable Status */
#define SCPI_QUES_VOLT	(1<<0)
#define SCPI_QUES_CURR	(1<<1)
#define SCPI_QUES_TIME	(1<<2)
#define SCPI_QUES_POW	(1<<3)
#define SCPI_QUES_TEMP	(1<<4)
#define SCPI_QUES_FREQ	(1<<5)
#define SCPI_QUES_PHAS	(1<<6)
#define SCPI_QUES_MOD	(1<<7)
#define SCPI_QUES_CALI	(1<<8)
#define SCPI_QUES_USR1	(1<<9)
#define SCPI_QUES_USR2	(1<<10)
#define SCPI_QUES_USR3	(1<<11)
#define SCPI_QUES_USR4	(1<<12)
#define SCPI_QUES_INST	(1<<13)
#define SCPI_QUES_CMDW	(1<<14)
uint16_t SCPI_QUES_cond;
uint16_t SCPI_QUES_enab;
uint16_t SCPI_QUES_even;
#define SCPI_QUES_trans_to1 0xffff
#define SCPI_QUES_trans_to0 0x0
void SCPI_QUES_cond_set(uint16_t val);
void SCPI_QUES_cond_reset(uint16_t val);
void SCPI_QUES_enab_update(void);
uint16_t SCPI_QUES_even_get(void);
/* SCPI Standard Event Status Register/Enable */
#define SCPI_SESR_OPC	(1<<0)
#define SCPI_SESR_REQC	(1<<1)
#define SCPI_SESR_QERR	(1<<2)
#define SCPI_SESR_DDERR	(1<<3)
#define SCPI_SESR_EERR	(1<<4)
#define SCPI_SESR_CERR	(1<<5)
#define SCPI_SESR_UREQ	(1<<6)
#define SCPI_SESR_POWON	(1<<7)
uint8_t SCPI_SESR_enab(void);
uint8_t SCPI_SESR_get(void);
void SCPI_SESR_set(uint8_t val);
void SCPI_SESR_set_(uint8_t);
void SCPI_SESR_enab_update(void);
/* SCPI Status Byte */
#define SCPI_STB_USER1	(1<<0)
#define SCPI_STB_USER2	(1<<1)
#define SCPI_STB_EEQ	(1<<2)
#define SCPI_STB_QUES	(1<<3)
#define SCPI_STB_MAV	(1<<4)
#define SCPI_STB_SESR	(1<<5)
#define SCPI_STB_RQS	(1<<6)
#define SCPI_STB_OPER	(1<<7)
uint8_t SCPI_SRE(void);
uint8_t SCPI_STB(void);
void SCPI_STB_set(uint8_t);
void SCPI_STB_reset(uint8_t);
void SCPI_SRE_update(void);
void SCPI_SRE_set_(uint8_t val);

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

SCPI_parse_t SCPI_IC_test_adc(char c);
SCPI_parse_t SCPI_IC_test_div(char c);
SCPI_parse_t SCPI_IC_test_mul(char c);
SCPI_parse_t SCPI_IC_test_num(char c);
SCPI_parse_t SCPI_IC_test_time(char c);

/* SCPI Tools */
SCPI_parse_t SCPI_in_uint8(uint8_t *x);
SCPI_parse_t SCPI_in_uint16(uint16_t *x);
SCPI_parse_t SCPI_in_uint32(uint32_t *x);
SCPI_parse_t SCPI_cmd_err_108(void);

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
