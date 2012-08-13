#ifndef __USART_H__
#define __USART_H__

#include <avr/pgmspace.h>

/* avr-libc constant to compute multiplier for given baud rate */
#define USART0_BAUD 115200

/* types to define index/lenght of data in input/output buffer */
typedef uint8_t USART0_in_len_t;
/* Input and output buffers, they are shared betwen several modules to 
 * conserve memory, but different modules use internaly different names to 
 * acces them, so below is defined series of #define(s) to rename them. */
#define USART0_IN_LEN 128
extern char USART0_in[USART0_IN_LEN];
extern volatile USART0_in_len_t USART0_in_len;

void USART0_init(void);
void USART0_in_process(void);
void USART0_start_sending(void);

#endif

