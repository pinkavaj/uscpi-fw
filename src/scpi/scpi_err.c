#include <ctype.h>
#include <string.h>

#include "config.h"
#include "stat_report.h"

#define _SCPI_ERR_(n, esr, t) \
	static const char SCPI_err ## n ## s_P[] PROGMEM = t; \
	const SCPI_err_t SCPI_err_ ## n PROGMEM = \
		{ .str_P = SCPI_err ## n ## s_P, .SES_P = esr }
_SCPI_ERR_(1, SCPI_SESR_CERR,    "1,\"Not implemented\"");
_SCPI_ERR_(0, 0,                 "0,\"OK\"");
_SCPI_ERR_(100, SCPI_SESR_CERR, "-100,\"Command error\"");
_SCPI_ERR_(101, SCPI_SESR_CERR, "-101,\"Invalid character\"");
_SCPI_ERR_(102, SCPI_SESR_CERR, "-102,\"Syntax error\"");
_SCPI_ERR_(103, SCPI_SESR_CERR, "-103,\"Invalid separator\"");
_SCPI_ERR_(104, SCPI_SESR_CERR, "-104,\"Data type error\"");
_SCPI_ERR_(105, SCPI_SESR_CERR, "-105,\"GET not allowed\"");
_SCPI_ERR_(108, SCPI_SESR_CERR, "-108,\"Parameter not allowed\"");
_SCPI_ERR_(109, SCPI_SESR_CERR, "-109,\"Missing parameter\"");
_SCPI_ERR_(110, SCPI_SESR_CERR, "-110,\"Command header error\"");
_SCPI_ERR_(111, SCPI_SESR_CERR, "-111,\"Header separator error\"");
_SCPI_ERR_(112, SCPI_SESR_CERR, "-112,\"Program mnemonic too long\"");
_SCPI_ERR_(113, SCPI_SESR_CERR, "-113,\"Undefined header\"");
_SCPI_ERR_(114, SCPI_SESR_CERR, "-114,\"Header suffix out of range\"");
_SCPI_ERR_(115, SCPI_SESR_CERR, "-115,\"Unexpected number of parameters\"");
_SCPI_ERR_(120, SCPI_SESR_CERR, "-120,\"Numeric data error\"");
_SCPI_ERR_(121, SCPI_SESR_CERR, "-121,\"Invalid character in number\"");
_SCPI_ERR_(123, SCPI_SESR_CERR, "-123,\"Exponent too large\"");
_SCPI_ERR_(124, SCPI_SESR_CERR, "-124,\"Too many digits\"");
_SCPI_ERR_(128, SCPI_SESR_CERR, "-128,\"Numeric data not allowed\"");
_SCPI_ERR_(130, SCPI_SESR_CERR, "-130,\"Suffix error\"");
_SCPI_ERR_(131, SCPI_SESR_CERR, "-131,\"Invalid suffix\"");
_SCPI_ERR_(134, SCPI_SESR_CERR, "-134,\"Suffix too long\"");
_SCPI_ERR_(138, SCPI_SESR_CERR, "-138,\"Suffix not allowed\"");
_SCPI_ERR_(140, SCPI_SESR_CERR, "-140,\"Character data error\"");
_SCPI_ERR_(141, SCPI_SESR_CERR, "-141,\"Invalid character data\"");
_SCPI_ERR_(144, SCPI_SESR_CERR, "-144,\"Character data too long\"");
_SCPI_ERR_(148, SCPI_SESR_CERR, "-148,\"Character data not allowed\"");
_SCPI_ERR_(150, SCPI_SESR_CERR, "-150,\"String data error\"");
_SCPI_ERR_(151, SCPI_SESR_CERR, "-151,\"Invalid string data\"");
_SCPI_ERR_(158, SCPI_SESR_CERR, "-158,\"String data not allowed\"");
_SCPI_ERR_(160, SCPI_SESR_CERR, "-160,\"Block data error\"");
_SCPI_ERR_(161, SCPI_SESR_CERR, "-161,\"Invalid block data\"");
_SCPI_ERR_(168, SCPI_SESR_CERR, "-168,\"Block data not allowed\"");
_SCPI_ERR_(170, SCPI_SESR_CERR, "-170,\"Expression error\"");
_SCPI_ERR_(171, SCPI_SESR_CERR, "-171,\"Invalid expression\"");
_SCPI_ERR_(178, SCPI_SESR_CERR, "-178,\"Expression data not allowed\"");
_SCPI_ERR_(180, SCPI_SESR_CERR, "-180,\"Macro error\"");
_SCPI_ERR_(181, SCPI_SESR_CERR, "-181,\"Invalid outside macro definition\"");
_SCPI_ERR_(183, SCPI_SESR_CERR, "-183,\"Invalid inside macro definition\"");
_SCPI_ERR_(184, SCPI_SESR_CERR, "-184,\"Macro parameter error\"");
_SCPI_ERR_(200, SCPI_SESR_EERR, "-200,\"Execution error\"");
_SCPI_ERR_(201, SCPI_SESR_EERR, "-201,\"Invalid while in local\"");
_SCPI_ERR_(202, SCPI_SESR_EERR, "-202,\"Settings lost due to rtl\"");
_SCPI_ERR_(203, SCPI_SESR_EERR, "-203,\"Command protected\"");
_SCPI_ERR_(210, SCPI_SESR_EERR, "-210,\"Trigger error\"");
_SCPI_ERR_(211, SCPI_SESR_EERR, "-211,\"Trigger ignored\"");
_SCPI_ERR_(212, SCPI_SESR_EERR, "-212,\"Arm ignored\"");
_SCPI_ERR_(213, SCPI_SESR_EERR, "-213,\"Init ignored\"");
_SCPI_ERR_(214, SCPI_SESR_EERR, "-214,\"Trigger deadlock\"");
_SCPI_ERR_(215, SCPI_SESR_EERR, "-215,\"Arm deadlock\"");
_SCPI_ERR_(220, SCPI_SESR_EERR, "-220,\"Parameter error\"");
_SCPI_ERR_(221, SCPI_SESR_EERR, "-221,\"Settings conflict\"");
_SCPI_ERR_(222, SCPI_SESR_EERR, "-222,\"Data out of range\"");
_SCPI_ERR_(223, SCPI_SESR_EERR, "-223,\"Too much data\"");
_SCPI_ERR_(224, SCPI_SESR_EERR, "-224,\"Illegal parameter value\"");
_SCPI_ERR_(225, SCPI_SESR_EERR, "-225,\"Out of memory.\"");
_SCPI_ERR_(226, SCPI_SESR_EERR, "-226,\"Lists not same length.\"");
_SCPI_ERR_(230, SCPI_SESR_EERR, "-230,\"Data corrupt or stale\"");
_SCPI_ERR_(231, SCPI_SESR_EERR, "-231,\"Data questionable\"");
_SCPI_ERR_(232, SCPI_SESR_EERR, "-232,\"Invalid format\"");
_SCPI_ERR_(233, SCPI_SESR_EERR, "-233,\"Invalid version\"");
_SCPI_ERR_(240, SCPI_SESR_EERR, "-240,\"Hardware error\"");
_SCPI_ERR_(241, SCPI_SESR_EERR, "-241,\"Hardware missing\"");
_SCPI_ERR_(250, SCPI_SESR_EERR, "-250,\"Mass storage error\"");
_SCPI_ERR_(251, SCPI_SESR_EERR, "-251,\"Missing mass storage\"");
_SCPI_ERR_(252, SCPI_SESR_EERR, "-252,\"Missing media\"");
_SCPI_ERR_(253, SCPI_SESR_EERR, "-253,\"Corrupt media\"");
_SCPI_ERR_(254, SCPI_SESR_EERR, "-254,\"Media full\"");
_SCPI_ERR_(255, SCPI_SESR_EERR, "-255,\"Directory full\"");
_SCPI_ERR_(256, SCPI_SESR_EERR, "-256,\"File name not found\"");
_SCPI_ERR_(257, SCPI_SESR_EERR, "-257,\"File name error\"");
_SCPI_ERR_(258, SCPI_SESR_EERR, "-258,\"Media protected\"");
_SCPI_ERR_(260, SCPI_SESR_EERR, "-260,\"Expression error\"");
_SCPI_ERR_(261, SCPI_SESR_EERR, "-261,\"Math error in expression\"");
_SCPI_ERR_(270, SCPI_SESR_EERR, "-270,\"Macro error\"");
_SCPI_ERR_(271, SCPI_SESR_EERR, "-271,\"Macro syntax error\"");
_SCPI_ERR_(272, SCPI_SESR_EERR, "-272,\"Macro execution error\"");
_SCPI_ERR_(273, SCPI_SESR_EERR, "-273,\"Illegal macro label\"");
_SCPI_ERR_(274, SCPI_SESR_EERR, "-274,\"Macro parameter error\"");
_SCPI_ERR_(275, SCPI_SESR_EERR, "-275,\"Macro definition too long\"");
_SCPI_ERR_(276, SCPI_SESR_EERR, "-276,\"Macro recursion error\"");
_SCPI_ERR_(277, SCPI_SESR_EERR, "-277,\"Macro redefinition not allowed\"");
_SCPI_ERR_(278, SCPI_SESR_EERR, "-278,\"Macro header not found\"");
_SCPI_ERR_(280, SCPI_SESR_EERR, "-280,\"Program error\"");
_SCPI_ERR_(281, SCPI_SESR_EERR, "-281,\"Cannot create program\"");
_SCPI_ERR_(282, SCPI_SESR_EERR, "-282,\"Illegal program name\"");
_SCPI_ERR_(283, SCPI_SESR_EERR, "-283,\"Illegal variable name\"");
_SCPI_ERR_(284, SCPI_SESR_EERR, "-284,\"Program currently running\"");
_SCPI_ERR_(285, SCPI_SESR_EERR, "-285,\"Program syntax error\"");
_SCPI_ERR_(286, SCPI_SESR_EERR, "-286,\"Program runtime error\"");
_SCPI_ERR_(290, SCPI_SESR_EERR, "-290,\"Memory use error\"");
_SCPI_ERR_(291, SCPI_SESR_EERR, "-291,\"Out of memory\"");
_SCPI_ERR_(292, SCPI_SESR_EERR, "-292,\"Referenced name does not exist\"");
_SCPI_ERR_(293, SCPI_SESR_EERR, "-293,\"Referenced name already exists\"");
_SCPI_ERR_(300, SCPI_SESR_DDERR, "-300,\"Device-specific error\"");
_SCPI_ERR_(310, SCPI_SESR_DDERR, "-310,\"System error\"");
_SCPI_ERR_(311, SCPI_SESR_DDERR, "-311,\"Memory error\"");
_SCPI_ERR_(312, SCPI_SESR_DDERR, "-312,\"PUD memory lost\"");
_SCPI_ERR_(313, SCPI_SESR_DDERR, "-313,\"Calibration memory lost\"");
_SCPI_ERR_(314, SCPI_SESR_DDERR, "-314,\"Save/recall memory lost\"");
_SCPI_ERR_(315, SCPI_SESR_DDERR, "-315,\"Configuration memory lost\"");
_SCPI_ERR_(320, SCPI_SESR_DDERR, "-320,\"Storage fault\"");
_SCPI_ERR_(321, SCPI_SESR_DDERR, "-321,\"Out of memory\"");
_SCPI_ERR_(330, SCPI_SESR_DDERR, "-330,\"Self-test failed\"");
_SCPI_ERR_(340, SCPI_SESR_DDERR, "-340,\"Calibration failed\"");
_SCPI_ERR_(350, SCPI_SESR_DDERR, "-350,\"Queue overflow\"");
_SCPI_ERR_(360, SCPI_SESR_DDERR, "-360,\"Communication error\"");
_SCPI_ERR_(361, SCPI_SESR_DDERR, "-361,\"Parity error in program message\"");
_SCPI_ERR_(362, SCPI_SESR_DDERR, "-362,\"Framing error in program message\"");
_SCPI_ERR_(363, SCPI_SESR_DDERR, "-363,\"Input buffer overrun\"");
_SCPI_ERR_(365, SCPI_SESR_DDERR, "-365,\"Time out error\"");
_SCPI_ERR_(400, SCPI_SESR_QERR, "-400,\"Query error\"");
_SCPI_ERR_(410, SCPI_SESR_QERR, "-410,\"Query INTERRUPTED\"");
_SCPI_ERR_(420, SCPI_SESR_QERR, "-420,\"Query UNTERMINATED\"");
_SCPI_ERR_(430, SCPI_SESR_QERR, "-430,\"Query DEADLOCKED\"");
_SCPI_ERR_(440, SCPI_SESR_QERR, "-440,\"Query UNTERMINATED after indefinite response\"");
_SCPI_ERR_(500, SCPI_SESR_QERR, "-500,\"Power on\"");
_SCPI_ERR_(600, SCPI_SESR_QERR, "-600,\"User request\"");
_SCPI_ERR_(700, SCPI_SESR_QERR, "-700,\"Request control\"");
_SCPI_ERR_(800, SCPI_SESR_QERR, "-800,\"Operation complete\"");

/* buffer for SCPI errors */
#define SCPI_ERR_MAX 4
static const SCPI_err_t *SCPI_err[SCPI_ERR_MAX];
static uint8_t SCPI_err_count = 0;

void SCPI_err_set(const SCPI_err_t *e)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (SCPI_err_count < SCPI_ERR_MAX) {
			SCPI_err[SCPI_err_count++] = e;
			SCPI_SESR_set(pgm_read_byte(&e->SES_P));
			SCPI_STB_set(SCPI_STB_EEQ);
		} else if (SCPI_err_count == SCPI_ERR_MAX) {
			SCPI_err_count++;
			SCPI_SESR_set(pgm_read_byte(&SCPI_err_350.SES_P));
		}
	}
}

const SCPI_err_t* SCPI_err_pop(void)
{
	const SCPI_err_t *e;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		if (SCPI_err_count == 0) 
			e = &SCPI_err_0;
		else {
			e = SCPI_err[0];
			memmove(SCPI_err, SCPI_err + 1,
					(SCPI_ERR_MAX - 1) * sizeof(void *));
			/* Error queue overflow indicated by 
			 * SCPI_err_count > SCPI_ERR_MAX */
			if (--SCPI_err_count == SCPI_ERR_MAX) 
				SCPI_err[SCPI_ERR_MAX - 1] = &SCPI_err_350;
			if (!SCPI_err_count)
				SCPI_STB_reset(SCPI_STB_EEQ);
		}
	}

	return e;
}

void SCPI_err_queue_reset(void)
{
	SCPI_err_count = 0;
	SCPI_STB_reset(SCPI_STB_EEQ);
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
