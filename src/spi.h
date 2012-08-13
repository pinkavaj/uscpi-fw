#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <inttypes.h>

/* indexes for SPI devices, use continous numbering to get optimal C code */
#define SPI_DEV_NONE 0xff
#define SPI_DEV_AD974_0 0
#define SPI_DEV_MCP4922_0 1

void SPI_init(void);
void SPI_select_dev(uint8_t dev_num);
void SPI_transfer(void *recv, void *send, uint8_t count);

void ad974_set_channel(uint8_t channel);

#endif
