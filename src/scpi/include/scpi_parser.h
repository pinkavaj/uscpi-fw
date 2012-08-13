#ifndef __SCPI_PARSER_H__
#define __SCPI_PARSER_H__

#include "cmd_tree.h"

/* numeric suffix after keyword, internaly numbered from 0,
 * stored only for branches which supports more channels + one
 * used to process current branch */
extern uint8_t SCPI_num_suffixes[2];
extern uint8_t SCPI_num_suffixes_idx;

extern uint8_t SCPI_params_count;

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
/* comma is used to separate two values from one result set,
 * semicolon is used to separate result sets from two commands,
 * newline is used when some result is send to output */
#define _SCPI_NEED_COMMA() (SCPI_opts & (1<<3))
#define _SCPI_NEED_COMMA_reset() (SCPI_opts &= ~(1<<3))
#define _SCPI_NEED_COMMA_set() (SCPI_opts |= (1<<3))

#define _SCPI_NEED_SEMICOLON() (SCPI_opts & (1<<2))
#define _SCPI_NEED_SEMICOLON_reset() (SCPI_opts &= ~(1<<2))
#define _SCPI_NEED_SEMICOLON_set() (SCPI_opts |= (1<<2))

#define _SCPI_NEED_NEWLINE() (SCPI_opts & (1<<4))
#define _SCPI_NEED_NEWLINE_reset() (SCPI_opts &= ~(1<<4))
#define _SCPI_NEED_NEWLINE_set() (SCPI_opts |= (1<<4))
/* parse_pref - 1 when parsing character after escape ('\') in string, 
 * 	0 othervise */
#define _SCPI_PARSE_PARAM_PREF() (SCPI_opts & (1<<5))
#define _SCPI_PARSE_PARAM_PREF_set() (SCPI_opts |= (1<<5))
#define _SCPI_PARSE_PARAM_PREF_reset() (SCPI_opts &= ~(1<<5))

#endif

