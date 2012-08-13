#ifndef __SCPI_CONFIG_H__
#define __SCPI_CONFIG_H__

#include "../drivers/usart.h"

/* SCPI settings (IO) */

/* There are buffers for USART with extern declaration to share readed data
 * with other devices, if omitted SCPI module create its own buffer and thus
 * allocate memory, but this is unwanted, we have a little memory */
#define SCPI_in_len_t USART0_in_len_t
#define SCPI_IN_LEN USART0_IN_LEN
#define SCPI_in USART0_in
#define SCPI_in_len USART0_in_len
#define SCPI_printn(s, len) USART0_printn(s, len)
#define SCPI_print_P(s) USART0_print_P(s)
#define SCPI_putc(c) USART0_putc(c)

#endif

