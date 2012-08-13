#ifndef __SCPI_STAT_REPORT_H__
#define __SCPI_STAT_REPORT_H__

#include <inttypes.h>

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
uint16_t SCPI_OPER_cond_get(void);
void SCPI_OPER_cond_set(uint16_t val);
void SCPI_OPER_cond_reset(uint16_t val);
uint16_t SCPI_OPER_enab_get(void);
void SCPI_OPER_enab_set(uint16_t val);
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
#define SCPI_QUES_trans_to1 0xffff
#define SCPI_QUES_trans_to0 0x0
uint16_t SCPI_QUES_enab_get(void);
void SCPI_QUES_enab_set(uint16_t enabled);
uint16_t SCPI_QUES_cond_get(void);
void SCPI_QUES_cond_set(uint16_t val);
void SCPI_QUES_cond_reset(uint16_t val);
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
uint8_t SCPI_SESR_get(void);
void SCPI_SESR_set(uint8_t val);
uint8_t SCPI_SESR_enab_get(void);
void SCPI_SESR_enab_set(uint8_t);
/* SCPI Status Byte */
#define SCPI_STB_USER1	(1<<0)
#define SCPI_STB_USER2	(1<<1)
#define SCPI_STB_EEQ	(1<<2)
#define SCPI_STB_QUES	(1<<3)
#define SCPI_STB_MAV	(1<<4)
#define SCPI_STB_SESR	(1<<5)
#define SCPI_STB_RQS	(1<<6)
#define SCPI_STB_OPER	(1<<7)
uint8_t SCPI_STB_get(void);
void SCPI_STB_reset(uint8_t val);
void SCPI_STB_set(uint8_t val);
uint8_t SCPI_SRE_get(void);
void SCPI_SRE_set(uint8_t val);

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
