#ifndef __IOBUF_H__
#define __IOBUF_H__

#include <avr/pgmspace.h>
#include <inttypes.h>
#include "lib/math_cust.h"

typedef struct {
	volatile uint8_t start;
	volatile uint8_t end;
	char buf[255];
} iobuf255_t;

//#ifdef stdout_buf
extern iobuf255_t stdout_buf;

void stdout_buf_reset(void);
void stdout_flush(void);
uint8_t stdout_isempty(void);
char popc(void);
void putc(char c);
void print_FP_16_16(FP_16_16_t x);
void printn(const char *str, uint8_t len);
void print_P(PGM_P str_P);
void print_int32(int32_t x);
void print_int32f(int32_t x, uint8_t digits);
void print_uint32(uint32_t x);
void print_uint32f(uint32_t x, uint8_t digits);
//#endif

#endif

