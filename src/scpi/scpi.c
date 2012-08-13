
#include <ctype.h>
#include <string.h>

#include "config.h"
#include "cmd_tools.h"
#include "lib/iobuf.h"
#include "scpi.h"
#include "scpi_parser.h"
#include "scpi/error.c"

/* If c is command separator returns c, 0 othervise */
#define SCPI_iscmdend(c) (c == '\n' || c == ';')
static SCPI_parse_t SCPI_parse_keyword(char c);
static SCPI_parse_t SCPI_parse_keyword_num(char c);
static SCPI_parse_t SCPI_parse_keyword_question(char c);
static SCPI_parse_t SCPI_parse_keyword_sep(char c);
static SCPI_parse_t SCPI_parse_param_start(char c);
static SCPI_parse_t SCPI_parse_param_str(char c);
static SCPI_parse_t SCPI_parse_param_str_end(char c);
static SCPI_parse_t SCPI_parse_param_nostr(char c);
static SCPI_parse_t SCPI_parse_param_end(char c);
static void SCPI_parser_reset_(void) ;

uint8_t SCPI_num_suffixes[2];
uint8_t SCPI_num_suffixes_idx;

/* Command for currently (last) selected branch item */
static const SCPI_cmd_t *SCPI_cmd;
/* current branch table where to find recieved keyword */
static const SCPI_branch_item_t *SCPI_branch;

/* Current parser, changes during different stages of parsing input */
static SCPI_parse_t (*_SCPI_parser)(char);

/* Get SCPI keyword [a-zA-Z] or skip to next parser */
static SCPI_parse_t SCPI_parse_keyword(char c)
{
	if (isalpha(c)) {
		if (SCPI_in_len > 12)
		{
			SCPI_err_set(&SCPI_err_112);
			return SCPI_parse_err;
		}
		c = toupper(c);
		SCPI_in[SCPI_in_len - 1] = c;
		return SCPI_parse_more;
	}
        if (SCPI_num_suffixes_idx >= sizeof(SCPI_num_suffixes)) {
                SCPI_err_set(&SCPI_err_130);
                return SCPI_parse_err;
        }
        SCPI_num_suffixes[SCPI_num_suffixes_idx] = 0;
	if (isdigit(c)) {
		_SCPI_parser = SCPI_parse_keyword_num;
		return SCPI_parse_keyword_num(c);
	}
	return SCPI_parse_keyword_question(c);
}

/* Parse numeric suffix after keyword */
static SCPI_parse_t SCPI_parse_keyword_num(char c)
{
	uint8_t num_suffix;
        
        num_suffix= SCPI_num_suffixes[SCPI_num_suffixes_idx];
	if (isdigit(c)) {
		if (num_suffix >= (UINT8_MAX / 10)) {
			SCPI_err_set(&SCPI_err_114);
			return SCPI_parse_err;
		}
                num_suffix = num_suffix * 10 + (c - '0');
		SCPI_num_suffixes[SCPI_num_suffixes_idx] = num_suffix;
		return SCPI_parse_drop_last;
	}
	/* Keyword numeric suffix is number in range <1,N> */
	if (num_suffix == 0) {
		SCPI_err_set(&SCPI_err_114);
		return SCPI_parse_err;
	}
	/* Internal numbering is from 0 */
	SCPI_num_suffixes[SCPI_num_suffixes_idx] = num_suffix - 1;
	return SCPI_parse_keyword_question(c);
}

/* Just check whatever is keyword question (c == '?') */
static SCPI_parse_t SCPI_parse_keyword_question(char c)
{
	_SCPI_parser = SCPI_parse_keyword_sep;
	if (c == '?') {
		_SCPI_CMD_IS_QUEST_set();
		if (_SCPI_PREV_RESULT())
			putc(';');
		else
			_SCPI_PREV_RESULT_set();
		return SCPI_parse_drop_last;
	}
	return SCPI_parse_keyword_sep(c);
}

/* We have whole keyword, try find it in table and handle it (eg. call 
 * function or select new keyword table */
static SCPI_parse_t SCPI_parse_keyword_sep(char c)
{
        uint8_t num_suffix;
	uint8_t num_suffix_max;
	uint8_t kw_len;
	const SCPI_branch_item_t *branch;

        kw_len = SCPI_in_len - 1;
	SCPI_in[kw_len] = '\0';
	if (_SCPI_CMD_IS_CC()) {
		branch = SCPI_CC_ROOT;
	}
	else {
		branch = SCPI_branch;
	}
	/* find keywork in keyword table */
	do {
		uint8_t len;
		SCPI_key_t *key;

		key = (SCPI_key_t*)pgm_read_word(&branch->key_P);
		if (key == NULL) {
			SCPI_err_set(&SCPI_err_113);
			return SCPI_parse_err;
		}
		/* if lenght of current keyword is equal to lenght of long or 
		 * short form of keyword */
		len = pgm_read_byte(&key->len_short_P);
                if (len != kw_len)
                        len = kw_len + 1;
		if(!strncmp_P(SCPI_in, key->keyword_P, len))
			break;
		branch++;
	} while(1);
	SCPI_cmd = (SCPI_cmd_t*)pgm_read_word(&branch->cmd_P);

        /* Check numeric suffix */
        num_suffix = SCPI_num_suffixes[SCPI_num_suffixes_idx];
        if (SCPI_cmd != NULL) {
                num_suffix_max = pgm_read_word(&SCPI_cmd->num_suffix_max_P);
                if (num_suffix_max > 0) {
                        SCPI_num_suffixes_idx++;
                }
        } else
                num_suffix_max = 0;

        if (num_suffix > num_suffix_max) {
	        SCPI_err_set(&SCPI_err_114);
	        return SCPI_parse_err;
        }

	/* try setup parsing of another keyword, if found in tree */
	if (c == ':') {
		SCPI_branch = 
			(SCPI_branch_item_t*)pgm_read_word(&branch->branch_P);
		if (SCPI_branch == NULL) {
			SCPI_err_set(&SCPI_err_102);
			return SCPI_parse_err;
		}
		_SCPI_parser = SCPI_parse_keyword;
		return SCPI_parse_drop_all;
	}
	/* This keyword does not have associated fucntion */
	if (SCPI_cmd == NULL) {
		SCPI_err_set(&SCPI_err_103);
		return SCPI_parse_err;
	}
	/* Check whatever this type of command is suported (question/set) */
	if ((_SCPI_CMD_IS_QUEST() && !pgm_read_byte(&SCPI_cmd->get_P)) ||
		(!_SCPI_CMD_IS_QUEST() && !pgm_read_byte(&SCPI_cmd->set_P))) {
			SCPI_err_set(&SCPI_err_102);
			return SCPI_parse_err;
		}
	/* parameters separator must follow */
	if (SCPI_iscmdend(c)) {
		_SCPI_parser = (SCPI_parse_t (*)(char))
			pgm_read_word(&SCPI_cmd->parser_P);
		return _SCPI_parser(c);
	}
	_SCPI_parser = SCPI_parse_param_start;
	if (isspace(c))
		return SCPI_parse_drop_all;

	SCPI_err_set(&SCPI_err_113);
	return SCPI_parse_err;
}

/* Normalize parameters recieved on input, convert tehm to zero terminated 
 * string, and fill parameter types into SCPI_param_types list */
static SCPI_parse_t SCPI_parse_param_start(char c)
{
	if (SCPI_iscmdend(c))
		return SCPI_parse_param_end(c);

	if(isspace(c))
		return SCPI_parse_drop_last;

	if (c == '"')
		_SCPI_parser = SCPI_parse_param_str;
	else
		_SCPI_parser = SCPI_parse_param_nostr;
	return _SCPI_parser(c);
}

static SCPI_parse_t SCPI_parse_param_str(char c)
{
	/* TODO */
	if (!_SCPI_PARSE_PARAM_PREF()) {
		if (c == '"') {
			_SCPI_parser = SCPI_parse_param_str_end;
			return SCPI_parse_more;
		}
		if (c == '\\')
			_SCPI_PARSE_PARAM_PREF_set();
	} else
		_SCPI_PARSE_PARAM_PREF_reset();

	return SCPI_parse_more;
}

static SCPI_parse_t SCPI_parse_param_str_end(char c)
{
	if (SCPI_iscmdend(c)) {
		SCPI_params_count++;
		return SCPI_parse_param_end(c);
	}

	if (isspace(c))
		return SCPI_parse_drop_last;

	if (c == ',') {
		SCPI_in[SCPI_in_len - 1] = 0;
		SCPI_params_count++;
		_SCPI_parser = SCPI_parse_param_start;
	}
	return SCPI_parse_err;
}

static SCPI_parse_t SCPI_parse_param_nostr(char c)
{
	if (SCPI_iscmdend(c)) {
		if (isspace(SCPI_in[SCPI_in_len - 2]))
			SCPI_in[SCPI_in_len - 2] = 0;
		SCPI_params_count++;
		return SCPI_parse_param_end(c);
	}

	if (isspace(c)) {
		if (isspace(SCPI_in[SCPI_in_len - 2])) {
			SCPI_in[SCPI_in_len - 2] = ' ';
			return SCPI_parse_drop_last;
		} else
			SCPI_in[SCPI_in_len - 1] = ' ';
	} else
	if (c == ',') {
		SCPI_in[SCPI_in_len - 1] = 0;
		SCPI_params_count++;
		_SCPI_parser = SCPI_parse_param_start;
	}

	return SCPI_parse_more;
}

static SCPI_parse_t SCPI_parse_param_end(char c)
{
	if (isspace(SCPI_in[SCPI_in_len - 1]))
		SCPI_in[SCPI_in_len - 1] = 0;
	_SCPI_parser = (SCPI_parse_t (*)(char))
		pgm_read_word(&SCPI_cmd->parser_P);
	return _SCPI_parser(c);
}

/* Clean ups when no more input(=output) for this comman will be proceeded */
static SCPI_parse_t SCPI_parse_end_(SCPI_parse_t ret)
{
	/* TODO: mark parser as reset neede */
	if (_SCPI_PREV_RESULT())
	{
		putc('\r');
		putc('\n');
	}
	return ret;
}

/* Detect type of SCPI message or call proper parsing function */
SCPI_parse_t SCPI_parse(char c)
{
	if (_SCPI_parser == NULL) {
		if (c == '\n')
			return SCPI_parse_end_(SCPI_parse_end);
		else if (isspace(c))
			return SCPI_parse_drop_all;

		_SCPI_parser = SCPI_parse_keyword;

		if (c == '*') {
			_SCPI_CMD_IS_CC_set();
			return SCPI_parse_drop_all;
		} else if (c == ':') {
			/* start parsing at root level */
			return SCPI_parse_drop_all;
		}
	}

	/* TODO: check whatever reset has been called */
	SCPI_parse_t ret = _SCPI_parser(c);
	/* End of command, but not of command sequence */
	if (ret == SCPI_parse_end) {
		if (c == ';') {
			SCPI_parser_reset_();
			return SCPI_parse_drop_all;
		}
		return SCPI_parse_end_(ret);
	}
	/* Command error, but there is end of line, so reset immediately */
	if (ret == SCPI_parse_err && c == '\n') {
		_SCPI_parser = NULL;
		return SCPI_parse_end_(SCPI_parse_end);
	}
	return ret;
}

/* Reset parser before new command sequence begins */
void SCPI_parser_reset(void) 
{
	SCPI_branch = SCPI_bt_ROOT;
	_SCPI_PREV_RESULT_reset();
	SCPI_parser_reset_();
}

/* Reset parser after end of command (when new command start */
static void SCPI_parser_reset_(void) 
{
	SCPI_cmd = NULL;
	_SCPI_CMD_IS_QUEST_reset();
	_SCPI_CMD_IS_CC_reset();
	_SCPI_PARSE_PARAM_PREF_reset();
        SCPI_num_suffixes_idx = 0;
	SCPI_params_count = 0;
	SCPI_param_in_buf_idx = 0;
	_SCPI_parser = NULL;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
