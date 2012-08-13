#include <ctype.h>
#include <string.h>

#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "cmd.h"
#include "scpi/error.h"

/* TODO: přemístit sem definici klíče */

SCPI_parse_t SCPI_IC_syst_err_next(char UNUSED(c))
{
	const SCPI_err_t *e;

	e = SCPI_err_pop();
	print_P((const char*)pgm_read_word(&e->str_P));
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_syst_vers(char UNUSED(c))
{
	print_P(SCPI_version_P);
	return SCPI_parse_end;
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
