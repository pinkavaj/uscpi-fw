#ifndef __SCPI_PARSER_H__
#define __SCPI_PARSER_H__

/* FIXME: přejmenovat na SCPI_bits */
uint8_t SCPI_opts;
/* command is question (ends with '?') */
#define _SCPI_CMD_IS_QUEST() (SCPI_opts & (1<<0))
#define _SCPI_CMD_IS_QUEST_set() (SCPI_opts |= (1<<0))
#define _SCPI_CMD_IS_QUEST_reset() (SCPI_opts &= ~(1<<0))
/* This command is Common Command (start with '*'), not Instrument command */
#define _SCPI_CMD_IS_CC() (SCPI_opts & (1<<1))
#define _SCPI_CMD_IS_CC_set() (SCPI_opts |= (1<<1))
#define _SCPI_CMD_IS_CC_reset() (SCPI_opts &= ~(1<<1))
/* Tere was question before, some result is already writen and thus it is 
 * necesary write separator before write new result and also write newline
 * when command is finished */
#define _SCPI_PREV_RESULT() (SCPI_opts & (1<<2))
#define _SCPI_PREV_RESULT_set() (SCPI_opts |= (1<<2))
#define _SCPI_PREV_RESULT_reset() (SCPI_opts &= ~(1<<2))
/* parse_pref - 1 when parsing character after escape ('\') in string, 
 * 	0 othervise */
#define _SCPI_PARSE_PARAM_PREF() (SCPI_opts & (1<<4))
#define _SCPI_PARSE_PARAM_PREF_set() (SCPI_opts |= (1<<4))
#define _SCPI_PARSE_PARAM_PREF_reset() (SCPI_opts &= ~(1<<4))

#endif

