#include <ctype.h>
#include <string.h>

#include "config.h"
#include "status_reporting.h"

/* SCPI errors */
/* SCPI_SES_QERR
 * SCPI_SES_EERR */

#define _SCPI_ERR_(n, esr, t) \
	static const char SCPI_err ## n ## s_P[] PROGMEM = t; \
	const SCPI_err_t SCPI_err_ ## n PROGMEM = \
		{ .str_P = SCPI_err ## n ## s_P, .SES_P = esr }
_SCPI_ERR_(1, SCPI_SESR_CERR,    "1,\"Not implemented\"");
_SCPI_ERR_(0, 0,                 "0,\"OK\"");
_SCPI_ERR_(100, SCPI_SESR_CERR,  "-100,\"Command error\"");
_SCPI_ERR_(102, SCPI_SESR_CERR,  "-102,\"Syntax error\"");
_SCPI_ERR_(103, SCPI_SESR_CERR,  "-103,\"Invalid separator\"");
_SCPI_ERR_(104, SCPI_SESR_CERR,  "-104,\"Data type error\"");
_SCPI_ERR_(108, SCPI_SESR_CERR,  "-108,\"Parameter not allowed\"");
_SCPI_ERR_(109, SCPI_SESR_CERR,  "-109,\"Missing parameter\"");
_SCPI_ERR_(110, SCPI_SESR_CERR,  "-110,\"Command header error\"");
_SCPI_ERR_(112, SCPI_SESR_CERR,  "-112,\"Program mnemonic too long\"");
_SCPI_ERR_(113, SCPI_SESR_CERR,  "-113,\"Undefined header\"");
_SCPI_ERR_(114, SCPI_SESR_CERR,  "-114,\"Header suffix out of range\"");
_SCPI_ERR_(171, SCPI_SESR_CERR,  "-171,\"Invalid expression\"");
_SCPI_ERR_(220, SCPI_SESR_EERR,  "-220,\"Parameter error\"");
_SCPI_ERR_(222, SCPI_SESR_EERR,  "-222,\"Data out of range\"");
_SCPI_ERR_(223, SCPI_SESR_EERR,  "-223,\"Too much data\"");
_SCPI_ERR_(321, SCPI_SESR_DDERR, "-321,\"Out of memory\"");
_SCPI_ERR_(350, SCPI_SESR_DDERR, "-350,\"Queue overflow\"");
_SCPI_ERR_(361, SCPI_SESR_DDERR, "-361,\"Parity error in program message\"");
_SCPI_ERR_(362, SCPI_SESR_DDERR, "-362,\"Framing error in program message\"");
_SCPI_ERR_(363, SCPI_SESR_DDERR, "-363,\"Input buffer overrun\"");

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
