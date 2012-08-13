#include <string.h>
#include <avr/sleep.h>
#include "lib/iobuf.h"
#include "drivers/usart.h"
#include "scpi.h"

iobuf255_t stdout_buf;
/* TODO: zoptimalizovat čas strávený se zakázaným přerušením */

/*****************************************************************************/
void stdout_buf_reset(void)
{
	stdout_buf.start = 0;
	stdout_buf.end = 0;
}

/*****************************************************************************/
char popc(void)
{
	if (stdout_buf.end != stdout_buf.start)
		return stdout_buf.buf[stdout_buf.start++];

	return 0;
}

/*****************************************************************************/
/* Wait until all data are send out, interrupts mus be enabled */
void stdout_flush(void)
{
	while (!stdout_isempty()) {
		sleep_mode();
	}
}

/*****************************************************************************/
/* Return true if end of buffer reached -> buffer is empty. */
uint8_t stdout_isempty(void)
{
	return stdout_buf.start == stdout_buf.end;
}

/*****************************************************************************/
uint8_t print_common(uint8_t len)
{
	uint8_t len_;

	if (stdout_buf.start > 0) {
		stdout_buf.end -= stdout_buf.start;
		memmove(stdout_buf.buf, stdout_buf.buf + stdout_buf.start,
				stdout_buf.end);
		stdout_buf.start = 0;
	}
	USART0_start_sending();
	len_ = sizeof(stdout_buf.buf) - stdout_buf.end;
	if (len_ < len) {
		SCPI_err_set(&SCPI_err_223);
		return len_;
	}
	return len;
}

/*****************************************************************************/
void print_FP_16_16(FP_16_16_t x)
{
        uint16_t fix, fract;

        fix = x / 65536;
        fract = x;

        print_uint32(fix);
        putc('.');
        x = (uint32_t)fract * 10000;
        x += 127;
        x /= 256;
        x *= 10;
        x += 127;
        x /= 256;
        print_uint32f(x, 5);
}

/*****************************************************************************/
/* Print len bytes into output buffer */
void printn(const char *str, uint8_t len)
{
	cli();
	len = print_common(len);
	memcpy(stdout_buf.buf + stdout_buf.end, str, len);
	stdout_buf.end += len;
	sei();
}

/*****************************************************************************/
void print_int32(int32_t x)
{
	print_int32f(x, 0);
}

/*****************************************************************************/
void print_int32f(int32_t x, uint8_t digits)
{
	if (x < 0)
	{
		putc('-');
		if (digits)
			digits--;
		x = -x;
	}
	print_uint32f(x, digits);
}

/*****************************************************************************/
void print_uint32(uint32_t x)
{
	print_uint32f(x, 0);
}

/*****************************************************************************/
/* print unisgned int to output */
void print_uint32f(uint32_t x, uint8_t digits)
{
	/* maximal number of digits for uint32 is 10 */
#define print_uint32_BUF_LEN 10
	char buf[print_uint32_BUF_LEN];
	uint8_t idx = print_uint32_BUF_LEN;

	do {
		buf[--idx] = (x % (uint8_t)10) + '0';
		x /= (uint8_t)10;
		if (digits)
			digits--;
	} while (x);
	
	while(digits) {
		buf[--idx] = '0';
		digits--;
	}
	printn(&buf[idx], print_uint32_BUF_LEN - idx);
#undef print_uint32_BUF_LEN
}

/*****************************************************************************/
/* Print string from Program memory into output buffer,
 * interrupts will be enabled by this rutine! */
void print_P(PGM_P str_P)
{
	uint8_t len;

	len = strlen_P(str_P);
	cli();
	len = print_common(len);
	memcpy_P(stdout_buf.buf + stdout_buf.end, str_P, len);
	stdout_buf.end += len;
	sei();
}

/*****************************************************************************/
/* Put character into output buffer */
void putc(char c)
{
	uint8_t len = 1;

	cli();
	if (print_common(len))
		stdout_buf.buf[stdout_buf.end++] = c;
	sei(); 
}

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
