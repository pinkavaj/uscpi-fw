#ifndef __SCPI__
#define __SCPI__

#include "config.h"

#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

/* FXIME: hack, rework to iobuf */
#include "drivers/usart.h"
#define SCPI_in USART0_in
#define SCPI_in_len USART0_in_len
#define SCPI_in_len_ USART0_in_len_

/*
 * SCPI_parse_more - continue recieving, need more data
 * SCPI_parse_drop_all - drop all data in buffer, continue recieving
 * SCPI_parse_drop_last - drop last char from buffer, continue recieving
 * SCPI_parse_drop_str - drop string at begin of buffer, continue recieving
 * SCPI_parse_end - end of command, call SPIC_parser_reset to start revieving
 * SCPI_parse_err - stop recieving, drop all until newline
 * */
typedef enum SCPI_parse_enum {
	SCPI_parse_more = 0,
	SCPI_parse_drop_all = 1,
	SCPI_parse_drop_last = 2,
	SCPI_parse_drop_str = 3,
	SCPI_parse_end = 4,
	SCPI_parse_err = 5,
} SCPI_parse_t;

/* error.h need SCPI_parse_t to be defined */
#include "scpi/error.h"

#define SCPI_isstart(c) ( c == ':' || c == '*' || isalpha(c))
SCPI_parse_t SCPI_parse(char c);
void SCPI_parser_reset(void);

static const char SCPI_version_P[] PROGMEM = "1999.0";

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
