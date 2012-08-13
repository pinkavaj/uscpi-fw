#ifndef __USART_H__
#define __USART_H__

#include <avr/pgmspace.h>

/* avr-libc constant to compute multiplier for given baud rate */
#define USART0_BAUD 115200

/* Input state, used to handle input events when error occurs,
 * drop all input until newline found, reset SCPI parser
 * - USART errors
 * CHAR_ERR_LINE - SCPI parser error accured*/
#define CHAR_ERR_ESC '\x00'
#define CHAR_ERR_FRAMING ((char)_BV(4))
#define CHAR_ERR_OVERFLOW ((char)_BV(3))
#define CHAR_ERR_PARITY ((char)_BV(2))
#define CHAR_ERR_LINE ((char)_BV(1))

/* types to define index/lenght of data in input/output buffer */
typedef uint8_t USART0_in_len_t;
/* Input and output buffers, they are shared betwen several modules to 
 * conserve memory, but different modules use internaly different names to 
 * acces them, so below is defined series of #define(s) to rename them. */
#define USART0_IN_LEN 128
extern char USART0_in[USART0_IN_LEN];
extern volatile USART0_in_len_t USART0_in_len;
extern volatile USART0_in_len_t USART0_in_len_;

void USART0_init(void);
void USART0_start_sending(void);

#endif

