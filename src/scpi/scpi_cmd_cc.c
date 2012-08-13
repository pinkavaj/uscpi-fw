#include <avr/version.h>
#include <ctype.h>
#include <string.h>

#include "scpi_cmd.h"


/* Small hack to jup to reset vector */
SCPI_parse_t __vectors(void);

/* Clear Status */
static SCPI_parse_t SCPI_CC_cls(char UNUSED(c))
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		SCPI_err_count = 0;
		SCPI_STB_reset(SCPI_STB_EEQ);
		SCPI_SESR_get();
		SCPI_OPER_even_get();
		SCPI_QUES_even_get();
	}
	return SCPI_parse_end;
}

/* Standard Event Status Enable */
/* Standard Event Status Enable Query  */
static SCPI_parse_t SCPI_CC_ese(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_out_uint32(SCPI_SESR_enab);
		return SCPI_parse_end;
	} 
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	SCPI_parse_t ret = SCPI_in_uint8(&SCPI_SESR);
	SCPI_SESR_enab_update();
	return ret;
}

/* Event Status Register Query */
static SCPI_parse_t SCPI_CC_esr(char UNUSED(c))
{
	SCPI_out_uint32(SCPI_SESR_get());
	return SCPI_parse_end;
}

/* Identification Query */
static SCPI_parse_t SCPI_CC_idn(char UNUSED(c))
{
	static const char IDN_P[] PROGMEM = 
		STR(INFO_COMPANY) ","
		STR(INFO_MODEL) "," 
		STR(INFO_AUTHOR) ",uSCPIfw-" STR(VER_MAJ) "." 
		STR(VER_MINOR) "." STR(VER_REV) "." STR(VER_BUILD)
		"-avr_libc-" __AVR_LIBC_VERSION_STRING__ ;

	SCPI_print_P(IDN_P);
	return SCPI_parse_end;
}

/* Operation Complete Command  */
/* Operation Complete Query */
static SCPI_parse_t SCPI_CC_opc(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_out_1();
		return SCPI_parse_end;
	}
	SCPI_SESR_set(SCPI_SESR_OPC);
	return SCPI_parse_end;
}

/* Reset */
static SCPI_parse_t SCPI_CC_rst(char UNUSED(c))
{
	return __vectors();
}

/* Service Request Enable */
/* Service Request Enable Query  */
static SCPI_parse_t SCPI_CC_sre(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_out_uint32(SCPI_SRE_);
		return SCPI_parse_end;
	}
	SCPI_parse_t ret = SCPI_in_uint8(&SCPI_SRE_);
	SCPI_SRE_update();
	return ret;
}

/* Read Status Byte Query */
static SCPI_parse_t SCPI_CC_stb(char UNUSED(c))
{
	uint8_t x = SCPI_STB & SCPI_SRE_;
	if (x)
		x |= SCPI_STB_RQS;
	SCPI_out_uint32(x);
	return SCPI_parse_end;
}

/* Self-Test Query  */
static SCPI_parse_t SCPI_CC_tst(char UNUSED(c))
{
	SCPI_out_1();
	return SCPI_parse_end;
}

/* Wait-to-Continue - implementation is empty when operation executed 
 * synchronusly */
static SCPI_parse_t SCPI_CC_wai(char UNUSED(c))
{
	return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
