/* Define IO functions used by SCPI */

#include "drivers/usart.h"
#include "scpi_io.h"

void SCPI_printn(const char *str, uint8_t len)
{
	USART0_printn(str, len);
}

void SCPI_print_P(PGM_P str_P)
{
	USART0_print_P(str_P);
}

void SCPI_putc(char c)
{
	USART0_putc(c);
}

