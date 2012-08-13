#ifndef __AD974_H__
#define __AD974_H__

#include <inttypes.h>


void ad974_io_init(void);
uint16_t ad974_get_sample(uint8_t channel);
void ad974_get_samples(uint8_t channel, uint32_t *val, uint8_t count);
uint8_t ad974_spi_cfg(void);

#endif
