#include <avr/version.h>

#include "config.h"
#include "cmd.h"
#include "cmd_tools.h"
#include "scpi_parser.h"
#include "stat_report.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"

/* TODO: přesunout sem strom */

/* Small hack to jup to reset vector */
SCPI_parse_t __vectors(void);

/* Clear Status */
SCPI_parse_t SCPI_CC_cls(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		SCPI_err_queue_reset();
		SCPI_SESR_get();
		SCPI_OPER_even_get();
		SCPI_QUES_even_get();
	}
	return SCPI_parse_end;
}

/* Standard Event Status Enable */
/* Standard Event Status Enable Query  */
SCPI_parse_t SCPI_CC_ese(void)
{
	if (_SCPI_CMD_IS_QUEST()) {
		print_uint32(SCPI_SESR_enab_get());
		return SCPI_parse_end;
	} 
	if (SCPI_params_count != 1)
		return SCPI_cmd_err_108();
	uint8_t val = 0;
	SCPI_parse_t ret = SCPI_in_uint8(&val);
	if (ret == SCPI_parse_err)
		return ret;
	SCPI_SESR_enab_set(val);
	return ret;
}

/* Event Status Register Query */
SCPI_parse_t SCPI_CC_esr(void)
{
	print_uint32(SCPI_SESR_get());
	return SCPI_parse_end;
}

/* Identification Query */
SCPI_parse_t SCPI_CC_idn(void)
{
	static const char IDN_P[] PROGMEM = 
		STR(INFO_COMPANY) ","
		STR(INFO_MODEL) "," 
		STR(INFO_AUTHOR) ",uSCPIfw-" STR(VER_MAJ) "." 
		STR(VER_MINOR) "." STR(VER_REV) "." STR(VER_BUILD)
		"-avr_libc-" __AVR_LIBC_VERSION_STRING__ ;

	print_P(IDN_P);
	return SCPI_parse_end;
}

/* Operation Complete Command  */
/* Operation Complete Query */
SCPI_parse_t SCPI_CC_opc(void)
{
	if (_SCPI_CMD_IS_QUEST()) {
		putc('1');
		return SCPI_parse_end;
	}
	SCPI_SESR_set(SCPI_SESR_OPC);
	return SCPI_parse_end;
}

/* Reset */
SCPI_parse_t SCPI_CC_rst(void)
{
	return __vectors();
}

/* Service Request Enable */
/* Service Request Enable Query  */
SCPI_parse_t SCPI_CC_sre(void)
{
	if (_SCPI_CMD_IS_QUEST()) {
		print_uint32(SCPI_SRE_get());
		return SCPI_parse_end;
	}
	uint8_t val = 0;
	SCPI_parse_t ret = SCPI_in_uint8(&val);
	if (ret == SCPI_parse_err)
		return ret;
	SCPI_SRE_set(val);
	return ret;
}

/* Read Status Byte Query */
SCPI_parse_t SCPI_CC_stb(void)
{
	print_uint32(SCPI_STB_get());
	return SCPI_parse_end;
}

/* Self-Test Query  */
SCPI_parse_t SCPI_CC_tst(void)
{
	putc('1');
	return SCPI_parse_end;
}

/* Wait-to-Continue - implementation is empty when operation executed 
 * synchronusly */
SCPI_parse_t SCPI_CC_wai(void)
{
	return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
