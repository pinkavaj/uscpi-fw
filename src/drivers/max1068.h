#ifndef __MAX1068_H__
#define __MAX1068_H__

#include <inttypes.h>

void max1068_io_init(void);
uint16_t max1068_get_sample(uint8_t channel);
void max1038_select(void);
uint8_t max1068_spi_mode(void);

#endif
