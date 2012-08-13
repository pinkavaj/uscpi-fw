
#include <ctype.h>
#include <string.h>

#include "config.h"
#include "scpi.h"
#include "scpi_io.h"
#include "scpi_cmd.h"
#include "scpi_err.c"
#include "scpi_tree.h"


/* If c is command separator returns c, 0 othervise */
#define SCPI_iscmdend(c) (c == '\n' || c == ';')
static SCPI_parse_t SCPI_parse_keyword(char c);
static SCPI_parse_t SCPI_parse_keyword_num(char c);
static SCPI_parse_t SCPI_parse_keyword_question(char c);
static SCPI_parse_t SCPI_parse_keyword_sep(char c);
static SCPI_parse_t SCPI_parse_params(char c);
static void SCPI_parser_reset_(void) ;

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
/* Parsing parameter is finished, looking for parameter separator and/or 
 * command end */
#define _SCPI_PARSE_PARAM_SEP() (SCPI_opts & (1<<3))
#define _SCPI_PARSE_PARAM_SEP_set() (SCPI_opts |= (1<<3))
#define _SCPI_PARSE_PARAM_SEP_reset() (SCPI_opts &= ~(1<<3))
/* parse_pref - set to 1 when parsing prefix (#X) in integers or chracter
 * 		after escape ('\') in string, 0 othervise */
#define _SCPI_PARSE_PARAM_PREF() (SCPI_opts & (1<<4))
#define _SCPI_PARSE_PARAM_PREF_set() (SCPI_opts |= (1<<4))
#define _SCPI_PARSE_PARAM_PREF_reset() (SCPI_opts &= ~(1<<4))

/* Ancillyary macros to create string from something and join it together */
#define STR(s) STR_(s)
#define STR_(s) #s

/* num_suffix - numeric suffix after keyword (-1) internaly numbered from 0 */
#define NUM_SUFFIX_MAX 15
static union {
	uint8_t current;
} num_suffix ;

/* Command for currently (last) selected branch item */
static const SCPI_cmd_t *SCPI_cmd;
/* current branch table where to find recieved keyword */
static const SCPI_branch_item_t *SCPI_branch;

/* used to identify type of parameter */
#define SCPI_PARAMS_MAX 16
typedef uint8_t SCPI_param_type_t;
static uint8_t SCPI_params_count;
static SCPI_param_type_t SCPI_param_types[SCPI_PARAMS_MAX];
/* Identify type of parameter:
 * 2B, Identify basic data type, Character, numeric and expression */
#define SCPI_PAR_TYPE	(3<<0)
#define SCPI_PAR_NONE	(0<<0)
#define SCPI_PAR_CHAR	(1<<0)
#define SCPI_PAR_NUM	(2<<0)
#define SCPI_PAR_EXPR	(3<<0)
/* 1B, Parameter separator type: 0 = ' ', 1 = ',' */
#define SCPI_PAR_END		(1<<7)
/* 1B, Flags for character data, it might be alpha [a-zA-Z] or string ("...") */
#define SCPI_PAR_CHAR_TYPE	(SCPI_PAR_TYPE | (1<<2))
#define SCPI_PAR_CHAR_ALPHA	(SCPI_PAR_CHAR | (0<<2))
#define SCPI_PAR_CHAR_STR	(SCPI_PAR_CHAR | (1<<2))
/* 1B, Sign for number, no mather whatever float or integer */
#define SCPI_PAR_NUM_SIG	(SCPI_PAR_TYPE | (1<<2))
#define SCPI_PAR_NUM_SIG_NO	(SCPI_PAR_NUM | (0<<2))
#define SCPI_PAR_NUM_SIG_MINUS	(SCPI_PAR_NUM | (1<<2))
/* 1B, Basic number type, floating point number or integer */
#define SCPI_PAR_NUM_TYPE	(SCPI_PAR_TYPE | (1<<3))
#define SCPI_PAR_NUM_INT	(SCPI_PAR_NUM | (0<<3))
#define SCPI_PAR_NUM_FLOAT	(SCPI_PAR_NUM | (1<<3))
/* 2B, Integer type, decimal, octal, hexadecimal or binary */
#define SCPI_PAR_NUM_INT_TYPE	(SCPI_PAR_NUM_TYPE | (3<<4))
#define SCPI_PAR_NUM_INT_DEC	(SCPI_PAR_NUM_INT | (0<<4))
#define SCPI_PAR_NUM_INT_HEX	(SCPI_PAR_NUM_INT | (1<<4))
#define SCPI_PAR_NUM_INT_OCT	(SCPI_PAR_NUM_INT | (2<<4))
#define SCPI_PAR_NUM_INT_BIN	(SCPI_PAR_NUM_INT | (3<<4))
/* 1B, Floating point, 0 = no decimal separator present ('.'), 1 = present */
//#define SCPI_PAR_NUM_FLOAT_NOPT	(0<<4)
#define SCPI_PAR_NUM_FLOAT_PT		(1<<4)
/* 1B, Floating point, 0 = no exponent present, 1 = exponent present */
//#define SCPI_PAR_NUM_FLOAT_NOEXP	(0<<5)
#define SCPI_PAR_NUM_FLOAT_EXP	(1<<5)
/* 1B, Floating point exponent, 0 = positive, 1= negative */
//#define SCPI_PAR_NUM_FLOAT_EXP_NOSIG	(0<<6)
#define SCPI_PAR_NUM_FLOAT_EXP_MINUS	(1<<6)

/* Current parser, changes during different stages of parsing input */
static SCPI_parse_t (*_SCPI_parser)(char);

#include "scpi_tree.c"
#include "scpi_cmd_cc.c"
#include "scpi_cmd_ic.c"
#include "scpi_cmd_tools.c"

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
	if (isdigit(c)) {
		_SCPI_parser = SCPI_parse_keyword_num;
		return SCPI_parse_keyword_num(c);
	}
	return SCPI_parse_keyword_question(c);
}

/* Parse numeric suffix after keyword */
static SCPI_parse_t SCPI_parse_keyword_num(char c)
{
	if (isdigit(c)) {
		uint8_t x = num_suffix.current * 10 + (c - '0');
		if (x > NUM_SUFFIX_MAX) {
			SCPI_err_set(&SCPI_err_114);
			return SCPI_parse_err;
		}
		num_suffix.current = x;
		return SCPI_parse_drop_last;
	}
	/* Keyword numeric suffix is number in range <1,N> */
	if (num_suffix.current == 0) {
		SCPI_err_set(&SCPI_err_114);
		return SCPI_parse_err;
	}
	/* Internal numbering is from 0 */
	num_suffix.current--;
	return SCPI_parse_keyword_question(c);
}

/* Just check whatever is keyword question (c == '?') */
static SCPI_parse_t SCPI_parse_keyword_question(char c)
{
	_SCPI_parser = SCPI_parse_keyword_sep;
	if (c == '?') {
		_SCPI_CMD_IS_QUEST_set();
		if (_SCPI_PREV_RESULT())
			SCPI_putc(';');
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
	uint8_t kw_len = SCPI_in_len - 1;
	SCPI_in[kw_len] = '\0';
	const SCPI_branch_item_t *branch;
	if (_SCPI_CMD_IS_CC()) {
		branch = SCPI_CC_ROOT;
	}
	else {
		branch = SCPI_branch;
	}
	/* find keywork in keyword table */
	do {
		uint8_t len, len_short;
		SCPI_key_t *key;
		key = (SCPI_key_t*)pgm_read_word(&branch->key_P);
		if (key == NULL) {
			SCPI_err_set(&SCPI_err_113);
			return SCPI_parse_err;
		}
		/* if lenght of current keyword is equal to lenght of long or 
		 * short form of keyword */
		len = pgm_read_byte(&key->len_P);
		len_short = pgm_read_byte(&key->len_short_P);
		if (len == kw_len || len_short == kw_len) {
			PGM_P keyword_P = (const char *)pgm_read_word(
					&key->keyword_P);
			if(!memcmp_P(SCPI_in, keyword_P, kw_len))
				break;
		}
		branch++;
	} while(1);
	SCPI_cmd = (SCPI_cmd_t*)pgm_read_word(&branch->cmd_P);
	/* try setup parsing of another keyword, if found in tree */
	if (num_suffix.current > 0) {
		/* Keyword numeric suffix is set (greather than 1)
		 * but not allowed */
		uint8_t *cmd_num_suffix = 
			(uint8_t*)pgm_read_word(&SCPI_cmd->num_suffix_P);
		if (SCPI_cmd == NULL || cmd_num_suffix == NULL) {
			SCPI_err_set(&SCPI_err_114);
			return SCPI_parse_err;
		}
		*cmd_num_suffix = num_suffix.current;
	}
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
	_SCPI_parser = SCPI_parse_params;
	if (isspace(c))
		return SCPI_parse_drop_all;

	SCPI_err_set(&SCPI_err_113);
	return SCPI_parse_err;
}

/* Normalize parameters recieved on input, convert tehm to zero terminated 
 * string, and fill parameter types into SCPI_param_types list */
static SCPI_parse_t SCPI_parse_params(char c)
{
	SCPI_param_type_t pt;

	if (SCPI_in_len == 1) {
		if (SCPI_iscmdend(c))
			goto SCPI_parse_params_eof;
		/* Drop whitespaces at start of parameter list */
		if(isspace(c))
			return SCPI_parse_drop_all;
	}

	pt = SCPI_param_types[SCPI_params_count];

	/* End of parameter reached, normalize param. separator (' ', ',') */
	if (_SCPI_PARSE_PARAM_SEP()) {
		if (isspace(c) && !SCPI_iscmdend(c))
			return SCPI_parse_drop_last;
		if (c == ',') {
			/* Parameter separator already present */
			if (pt & SCPI_PAR_END) {
				SCPI_err_set(&SCPI_err_220);
				return SCPI_parse_err;
			}
			SCPI_param_types[SCPI_params_count] |= 
					SCPI_PAR_END;
			return SCPI_parse_drop_last;
		}
		/* Start of new parameter or end of command found */
		if (SCPI_iscmdend(c)) {
			goto SCPI_parse_params_eof;
			SCPI_params_count++;
		}
		if (!(SCPI_param_types[SCPI_params_count] & SCPI_PAR_END)) {
			SCPI_err_set(&SCPI_err_103);
			return SCPI_parse_err;
		}
		SCPI_params_count++;
		/* new parameter starts, is there a room to store type? */
		if (SCPI_params_count >= SCPI_PARAMS_MAX) {
			SCPI_err_set(&SCPI_err_223);
			return SCPI_parse_err;
		}
		_SCPI_PARSE_PARAM_SEP_reset();
		pt = 0;
	}
	/* Try detect type of parameter */
	if (!pt) {
		if (isdigit(c))
			pt = SCPI_PAR_NUM_INT;
		else if (c == '#') {
			_SCPI_PARSE_PARAM_PREF_set();
			pt = SCPI_PAR_NUM_INT;
		} else if (c == '+') {
			SCPI_param_types[SCPI_params_count] = SCPI_PAR_NUM_INT;
			return SCPI_parse_drop_last;
		} else if (c == '-') 
			pt = SCPI_PAR_NUM_INT | SCPI_PAR_NUM_SIG_MINUS;
		else if (c == '.')
			pt = SCPI_PAR_NUM_FLOAT | SCPI_PAR_NUM_FLOAT_PT;
		else if (c == '"')
			pt = SCPI_PAR_CHAR_STR;
		else if (isalpha(c))
			pt = SCPI_PAR_CHAR_ALPHA;
		else if (SCPI_iscmdend(c))
			goto SCPI_parse_params_eof;
		else {
			SCPI_err_set(&SCPI_err_220);
			return SCPI_parse_err;
		}

		SCPI_param_types[SCPI_params_count] = pt;
		return SCPI_parse_more;
	}

	if ((pt & SCPI_PAR_NUM_TYPE) == SCPI_PAR_NUM_INT) {
		/* Parsing char after '#', prefix before number */
		if (_SCPI_PARSE_PARAM_PREF()) {
			if (c == 'B')
				pt |= SCPI_PAR_NUM_INT_BIN;
			else if (c == 'H')
				pt |= SCPI_PAR_NUM_INT_HEX;
			else if (c == 'Q')
				pt |= SCPI_PAR_NUM_INT_OCT;
			else {
				SCPI_err_set(&SCPI_err_102);
				return SCPI_parse_err;
			}
			_SCPI_PARSE_PARAM_PREF_reset();
			SCPI_param_types[SCPI_params_count] = pt;
			return SCPI_parse_more;
		}
		switch(pt & SCPI_PAR_NUM_INT_TYPE) {
			case SCPI_PAR_NUM_INT_HEX:
				if (c >= 'A' && c <= 'F')
					return SCPI_parse_more;
			default:
			case SCPI_PAR_NUM_INT_DEC:
				if (isdigit(c))
					return SCPI_parse_more;
				break;
			case SCPI_PAR_NUM_INT_OCT:
				if (c >= '0' && c <= '7')
					return SCPI_parse_more;
				break;
			case SCPI_PAR_NUM_INT_BIN:
				if (c == '0' || c == '1')
					return SCPI_parse_more;
				break;
		}
		/* Is floating point number ? */
		if ((c == '.' || c == 'E') && 
				(pt & SCPI_PAR_NUM_INT_TYPE) == 
				SCPI_PAR_NUM_INT_DEC) {
			pt = (pt & ~SCPI_PAR_NUM_TYPE) | SCPI_PAR_NUM_FLOAT;
			SCPI_param_types[SCPI_params_count] = pt;
		} else 
			/* TODO: kontrola osamoceného +- */
			goto SCPI_parse_params_end;
	}
	if ((pt & SCPI_PAR_NUM_TYPE) == SCPI_PAR_NUM_FLOAT) {
			return SCPI_parse_err;
	}
	if ((pt & SCPI_PAR_CHAR_TYPE) == SCPI_PAR_CHAR_STR) {
		if (_SCPI_PARSE_PARAM_PREF()) {
			_SCPI_PARSE_PARAM_PREF_reset();
			/* TODO: check for nonprintable characters */
			return SCPI_parse_more;
		}
		if (c == '\\') {
			_SCPI_PARSE_PARAM_PREF_set();
			return SCPI_parse_drop_last;
		}
		if (c == '"')
			goto SCPI_parse_params_end2;
		return SCPI_parse_more;
	}
	if ((pt & SCPI_PAR_CHAR_TYPE) == SCPI_PAR_CHAR_ALPHA) {
		if (isalpha(c))
			return SCPI_parse_more;
		goto SCPI_parse_params_end;
	}
	/* This will never happend */
	return SCPI_parse_err;

SCPI_parse_params_end:
	/* Check for parameter separator, otherwise some parameter 
	 * parsing failed */
	SCPI_in[SCPI_in_len - 1] = '\0';
	if (SCPI_iscmdend(c)) {
		SCPI_params_count++;
		goto SCPI_parse_params_eof;
	}
	if (c == ',' || isspace(c)) {
		if (c == ',') {
			pt |= SCPI_PAR_END;
			SCPI_param_types[SCPI_params_count] = pt;
		}
SCPI_parse_params_end2:
		_SCPI_PARSE_PARAM_SEP_set();
		return SCPI_parse_more;
	}
	SCPI_err_set(&SCPI_err_220);
	return SCPI_parse_err;

	/* Jumped when end of command found */
SCPI_parse_params_eof:
	/* TODO: check parameters, its count ant type */
//			SCPI_err_set(&SCPI_err_109); // Missing parameter
	/* After parameter separator must follow parameter */
	if (SCPI_params_count) 
		if (SCPI_param_types[SCPI_params_count - 1] & SCPI_PAR_END) {
			SCPI_err_set(&SCPI_err_220);
			return SCPI_parse_err;
		}
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
		SCPI_putc('\r');
		SCPI_putc('\n');
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
	num_suffix.current = 0;
	SCPI_cmd = NULL;
	_SCPI_CMD_IS_QUEST_reset();
	_SCPI_CMD_IS_CC_reset();
	_SCPI_PARSE_PARAM_SEP_reset();
	_SCPI_PARSE_PARAM_PREF_reset();
	SCPI_params_count = 0;
	SCPI_param_types[0] = 0;
	_SCPI_parser = NULL;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
