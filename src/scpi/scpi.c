
#include <ctype.h>
#include <string.h>

#include "config.h"
#include "cmd_tools.h"
#include "lib/iobuf.h"
#include "list.h"
#include "scpi.h"
#include "temp.h"
#include "scpi_parser.h"
#include "scpi/error.c"

const char SCPI_version_P[] PROGMEM = "1999.0";

/* If c is command separator returns c, 0 othervise */
#define SCPI_iscmdend(c) (c == '\n' || c == ';')
static SCPI_parse_t SCPI_parse_keyword(void);
static SCPI_parse_t SCPI_parse_keyword_num(void);
static SCPI_parse_t SCPI_parse_keyword_question(void);
static SCPI_parse_t SCPI_parse_keyword_sep(void);
static SCPI_parse_t SCPI_parse_param_start(void);
static SCPI_parse_t SCPI_parse_param_str(void);
static SCPI_parse_t SCPI_parse_param_str_end(void);
static SCPI_parse_t SCPI_parse_param_nostr(void);
static SCPI_parse_t SCPI_parse_params_end(void);
static void SCPI_parser_reset_(void) ;

uint8_t SCPI_num_suffixes[2];
uint8_t SCPI_num_suffixes_idx;

/* Last character from input buffer, used only by local parser */
static char last_char;

/* Command for currently (last) selected branch item */
static const SCPI_cmd_t *SCPI_cmd;
/* current branch table where to find recieved keyword */
static const SCPI_branch_item_t *SCPI_branch;

/* Current parser, changes during different stages of parsing input */
static SCPI_parse_t (*_SCPI_parser)(void);

/* Get SCPI keyword [a-zA-Z] or skip to next parser */
static SCPI_parse_t SCPI_parse_keyword(void)
{
	if (isalpha(last_char)) {
		if (SCPI_in_len > 12)
			return SCPI_err_set_(&SCPI_err_112);
		last_char = toupper(last_char);
		SCPI_in[SCPI_in_len - 1] = last_char;
		return SCPI_parse_more;
	}
        if (SCPI_num_suffixes_idx >= sizeof(SCPI_num_suffixes))
                return SCPI_err_set_(&SCPI_err_130);
        SCPI_num_suffixes[SCPI_num_suffixes_idx] = 0;
	if (isdigit(last_char)) {
		_SCPI_parser = SCPI_parse_keyword_num;
		return SCPI_parse_keyword_num();
	}
	return SCPI_parse_keyword_question();
}

/* Parse numeric suffix after keyword */
static SCPI_parse_t SCPI_parse_keyword_num(void)
{
	uint8_t num_suffix;

        num_suffix = SCPI_num_suffixes[SCPI_num_suffixes_idx];
	if (isdigit(last_char)) {
		if (num_suffix >= (UINT8_MAX / 10))
			return SCPI_err_set_(&SCPI_err_114);
                num_suffix = num_suffix * 10 + (last_char - '0');
		SCPI_num_suffixes[SCPI_num_suffixes_idx] = num_suffix;
		return SCPI_parse_drop_last;
	}
	/* Keyword numeric suffix is number in range <1,N> */
	if (num_suffix == 0)
		return SCPI_err_set_(&SCPI_err_114);
	/* Internal numbering is from 0 */
	SCPI_num_suffixes[SCPI_num_suffixes_idx] = num_suffix - 1;
	return SCPI_parse_keyword_question();
}

/* Just check whatever is keyword question (c == '?') */
static SCPI_parse_t SCPI_parse_keyword_question(void)
{
	_SCPI_parser = SCPI_parse_keyword_sep;
        if (_SCPI_NEED_COMMA()) {
                _SCPI_NEED_COMMA_reset();
                _SCPI_NEED_SEMICOLON_set();
        }
	if (last_char == '?') {
		_SCPI_CMD_IS_QUEST_set();
		return SCPI_parse_drop_last;
	}
	return SCPI_parse_keyword_sep();
}

/* We have whole keyword, try find it in table and handle it (eg. call 
 * function or select new keyword table */
static SCPI_parse_t SCPI_parse_keyword_sep(void)
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
		const SCPI_key_t *key;

		key = (const SCPI_key_t*)pgm_read_word(&branch->key_P);
		if (key == NULL)
			return SCPI_err_set_(&SCPI_err_113);
		if(!keycmp_P(SCPI_in, key))
			break;
		branch++;
	} while(1);
	SCPI_cmd = (SCPI_cmd_t*)pgm_read_word(&branch->cmd_P);

        /* Check numeric suffix */
        num_suffix = SCPI_num_suffixes[SCPI_num_suffixes_idx];
        if (SCPI_cmd != NULL) {
                num_suffix_max = pgm_read_byte(&SCPI_cmd->num_suffix_max_P);
                if (num_suffix_max > 0) {
                        SCPI_num_suffixes_idx++;
                }
        } else
                num_suffix_max = 0;

        if (num_suffix > num_suffix_max)
	        return SCPI_err_set_(&SCPI_err_114);

	/* try setup parsing of another keyword, if found in tree */
	if (last_char == ':') {
		SCPI_branch = 
			(SCPI_branch_item_t*)pgm_read_word(&branch->branch_P);
		if (SCPI_branch == NULL)
			return SCPI_err_set_(&SCPI_err_102);
		_SCPI_parser = SCPI_parse_keyword;
		return SCPI_parse_drop_all;
	}
	/* This keyword does not have associated fucntion */
	if (SCPI_cmd == NULL)
		return SCPI_err_set_(&SCPI_err_103);
	/* Check whatever this type of command is suported (question/set) */
	if ((_SCPI_CMD_IS_QUEST() && !pgm_read_byte(&SCPI_cmd->get_P)) ||
		(!_SCPI_CMD_IS_QUEST() && !pgm_read_byte(&SCPI_cmd->set_P)))
			return SCPI_err_set_(&SCPI_err_102);
	/* parameters separator must follow */
	if (SCPI_iscmdend(last_char))
		return SCPI_parse_params_end();

	_SCPI_parser = SCPI_parse_param_start;
	if (isspace(last_char))
		return SCPI_parse_drop_all;

	return SCPI_err_set_(&SCPI_err_113);
}

/* Normalize parameters recieved on input, convert tehm to zero terminated 
 * string, and fill parameter types into SCPI_param_types list */
static SCPI_parse_t SCPI_parse_param_start(void)
{
	if (SCPI_iscmdend(last_char))
		return SCPI_parse_params_end();

	if(isspace(last_char))
		return SCPI_parse_drop_last;

	if (last_char == '"')
		_SCPI_parser = SCPI_parse_param_str;
	else
		_SCPI_parser = SCPI_parse_param_nostr;
	return _SCPI_parser();
}

static SCPI_parse_t SCPI_parse_param_str(void)
{
	/* TODO */
	if (!_SCPI_PARSE_PARAM_PREF()) {
		if (last_char == '"') {
			_SCPI_parser = SCPI_parse_param_str_end;
			return SCPI_parse_more;
		}
		if (last_char == '\\')
			_SCPI_PARSE_PARAM_PREF_set();
	} else
		_SCPI_PARSE_PARAM_PREF_reset();

	return SCPI_parse_more;
}

static SCPI_parse_t SCPI_parse_param_str_end(void)
{
	if (SCPI_iscmdend(last_char)) {
		SCPI_params_count++;
		return SCPI_parse_params_end();
	}

	if (isspace(last_char))
		return SCPI_parse_drop_last;

	if (last_char == ',') {
		SCPI_in[SCPI_in_len - 1] = 0;
		SCPI_params_count++;
		_SCPI_parser = SCPI_parse_param_start;
	}
	return SCPI_parse_err;
}

static SCPI_parse_t SCPI_parse_param_nostr(void)
{
	if (SCPI_iscmdend(last_char)) {
		if (isspace(SCPI_in[SCPI_in_len - 2]))
			SCPI_in[SCPI_in_len - 2] = 0;
		SCPI_params_count++;
		return SCPI_parse_params_end();
	}

	if (isspace(last_char)) {
		if (isspace(SCPI_in[SCPI_in_len - 2])) {
			SCPI_in[SCPI_in_len - 2] = ' ';
			return SCPI_parse_drop_last;
		} else
			SCPI_in[SCPI_in_len - 1] = ' ';
	} else
	if (last_char == ',') {
		SCPI_in[SCPI_in_len - 1] = 0;
		SCPI_params_count++;
		_SCPI_parser = SCPI_parse_param_start;
	}

	return SCPI_parse_more;
}

static SCPI_parse_t SCPI_parse_params_end(void)
{
        uint8_t params_min, params_opt;

	if (isspace(SCPI_in[SCPI_in_len - 1]))
		SCPI_in[SCPI_in_len - 1] = 0;

        if (!_SCPI_CMD_IS_QUEST()) {
                params_min = pgm_read_byte(&SCPI_cmd->set_params_min_P);
                params_opt = pgm_read_byte(&SCPI_cmd->set_params_opt_P);
                if (SCPI_params_count < params_min)
                        return SCPI_err_set_(&SCPI_err_109);
                if (SCPI_params_count > (params_min+params_opt))
                        return SCPI_err_set_(&SCPI_err_108);
        }

	_SCPI_parser = (SCPI_parse_t (*)(void))
		pgm_read_word(&SCPI_cmd->parser_P);

	return _SCPI_parser();
}

//const char _nl[] PROGMEM = "\r\n";
/* Clean ups when no more input(=output) for this comman will be proceeded */
static SCPI_parse_t SCPI_parse_end_(SCPI_parse_t ret)
{

	if (_SCPI_NEED_NEWLINE())
	{
//                SCPI_print_P(_nl);
		putc('\r');
		putc('\n');
	}
	return ret;
}

/* Detect type of SCPI message or call proper parsing function */
static SCPI_parse_t SCPI_parse(char c)
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

        last_char = c;
	/* TODO: check whatever reset has been called */
	SCPI_parse_t ret = _SCPI_parser();
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

/* Start processing bytes in input buffer,
 * suppose tath interrupts are disabled, but can be enabled. */
void SCPI_loop(void)
{
	static uint8_t char_err = 0;
	char c;
	SCPI_parse_t ret;

        /* handle all time critical routines first */
        temp_loop();

	while (USART0_in_len < USART0_in_len_) {
		c = USART0_in[USART0_in_len++];
		if (c == CHAR_ERR_ESC) {
			c = USART0_in[USART0_in_len++];
			if (c != CHAR_ERR_ESC) {
				char_err = c;
				if (char_err & CHAR_ERR_PARITY)
					SCPI_err_set(&SCPI_err_361);
				if (char_err & CHAR_ERR_FRAMING)
					SCPI_err_set(&SCPI_err_362);
				if (char_err & CHAR_ERR_OVERFLOW)
					SCPI_err_set(&SCPI_err_363);
			}
		}

		ret = char_err ? SCPI_parse_err : SCPI_parse(c);
		cli();
		switch (ret)
		{
			default:
			case SCPI_parse_err:
				char_err = CHAR_ERR_LINE;
			case SCPI_parse_end:
			case SCPI_parse_drop_all:
				USART0_in_len_ -= USART0_in_len;
				memmove(USART0_in, USART0_in + USART0_in_len, USART0_in_len_);
				USART0_in_len = 0;
				break;
			case SCPI_parse_drop_last:
				memmove(USART0_in + USART0_in_len - 1, 
						USART0_in + USART0_in_len, 
						USART0_in_len_ - USART0_in_len);
				USART0_in_len--;
				USART0_in_len_--;
				break;
			case SCPI_parse_drop_str:
				{
					uint8_t idx = 0;
					while (USART0_in[idx]) {
						if (idx == USART0_in_len)
							break;
						idx++;
						USART0_in_len_--;
					};
					
					if (USART0_in[idx] == 0 && idx < USART0_in_len) {
						idx++;
						USART0_in_len_--;
					}
					memmove(USART0_in, USART0_in + idx, USART0_in_len_);
				}

				break;
			case SCPI_parse_more:
				break;
		}
		sei();
		if (c == '\n') {
			SCPI_parser_reset();
			char_err = 0;
		}
	}
}

/* Reset parser before new command sequence begins */
void SCPI_parser_reset(void) 
{
	SCPI_branch = SCPI_bt_ROOT;
	_SCPI_NEED_NEWLINE_reset();
        _SCPI_NEED_COMMA_reset();
        _SCPI_NEED_SEMICOLON_reset();
	SCPI_parser_reset_();
}

/* Reset parser after end of command (when new command starts) */
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

void SCPI_init(void)
{
        list_init();
        temp_init();
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
