/* Define IO functions used by SCPI */

#include "lib/iobuf.h"
#include "scpi_io.h"

void SCPI_printn(const char *str, uint8_t len)
{
	printn(str, len);
}

void SCPI_print_P(PGM_P str_P)
{
	print_P(str_P);
}

void SCPI_putc(char c)
{
	putc(c);
}

