#ifndef __CONFIG__
#define __CONFIG__

#include <stdint.h>
#include <avr/pgmspace.h>

/* for debuging only */
#include <avr/io.h>

#define VER_MAJ 0
#define VER_MINOR 0
#define VER_REV 0
#define VER_BUILD 456M
#define INFO_AUTHOR Lukas-Kucera_Jiri-Pinkava
#define INFO_COMPANY VSCHT
#ifndef INFO_MODEL
#define INFO_MODEL M162board-dev00
#endif

/* avr-libc constant to compute multiplier for given baud rate */
#define BAUD 115200

/* types to define index/lenght of data in input/output buffer */
typedef uint8_t USART0_in_len_t;
/* Input and output buffers, they are shared betwen several modules to 
 * conserve memory, but different modules use internaly different names to 
 * acces them, so below is defined series of #define(s) to rename them. */
#define USART0_IN_LEN 128
extern char USART0_in[USART0_IN_LEN];
extern volatile USART0_in_len_t USART0_in_len;

void USART0_printn(const char *c, int len);
void USART0_print_P(PGM_P c);
void USART0_putc(char c);

/* SCPI settings (IO) */

/* There are buffers for USART with extern declaration to share readed data
 * with other devices, if omitted SCPI module create its own buffer and thus
 * allocate memory, but this is unwanted, we have a little memory */
#define SCPI_in_len_t USART0_in_len_t
#define SCPI_IN_LEN USART0_IN_LEN
#define SCPI_in USART0_in
#define SCPI_in_len USART0_in_len
#define SCPI_printn(c, len) USART0_printn(c, len)
#define SCPI_print_P(c) USART0_print_P(c)
#define SCPI_putc(c) USART0_putc(c)

/* ancillyary macros */
#ifndef NULL
#define NULL 0
#endif

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
