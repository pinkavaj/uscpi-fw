#include <avr/version.h>

#include "config.h"
#include "cmd.h"
#include "cmd_tools.h"
#include "scpi_parser.h"
#include "stat_report.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "temp.h"

/* TODO: přesunout sem strom */

/* Small hack to jup to reset vector */
SCPI_parse_t __vectors(void);

/* Clear Status */
SCPI_parse_t SCPI_CC_cls(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		SCPI_err_queue_reset();
		SCPI_SESR.event = 0;
		SCPI_OPER.event = 0;
		SCPI_QUES.event = 0;
	}
	return SCPI_parse_end;
}

/* Standard Event Status Enable */
/* Standard Event Status Enable Query  */
SCPI_parse_t SCPI_CC_ese(void)
{
        SCPI_parse_t ret;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_SESR.enabled);

                return SCPI_parse_end;
	} 
	ret = SCPI_in_uint8(&SCPI_SESR.enabled);
	return ret;
}

/* Event Status Register Query */
SCPI_parse_t SCPI_CC_esr(void)
{
	SCPI_print_uint16(SCPI_SESR.event & SCPI_SESR.enabled);
        SCPI_SESR.event = 0;

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

	SCPI_print_P(IDN_P);

        return SCPI_parse_end;
}

/* Operation Complete Command  */
/* Operation Complete Query */
SCPI_parse_t SCPI_CC_opc(void)
{
	if (_SCPI_CMD_IS_QUEST()) {
                SCPI_print_uint16(1);
		return SCPI_parse_end;
	}
	SCPI_SESR.event |= SCPI_SESR_OPC;
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
        SCPI_parse_t ret;

	if (_SCPI_CMD_IS_QUEST()) {
		SCPI_print_uint16(SCPI_STB_enabled);

                return SCPI_parse_end;
	}
	ret = SCPI_in_uint8(&SCPI_STB_enabled);
	if (ret == SCPI_parse_err)
		return ret;
        SCPI_STB_enabled &= ~SCPI_STB_RQS;
	return ret;
}

/* Read Status Byte Query */
SCPI_parse_t SCPI_CC_stb(void)
{
	SCPI_print_uint16(SCPI_STB_get());
        return SCPI_parse_end;
}

/* *TRG */
SCPI_parse_t SCPI_CC_trg(void)
{
        temp_trg();
        return SCPI_parse_end;
}

/* Self-Test Query  */
SCPI_parse_t SCPI_CC_tst(void)
{
        /* FIXME: use common printing */
        SCPI_print_uint16(1);
	return SCPI_parse_end;
}

/* Wait-to-Continue - implementation is empty when operation executed 
 * synchronusly */
SCPI_parse_t SCPI_CC_wai(void)
{
	return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
