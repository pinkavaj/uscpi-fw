#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <inttypes.h>

#define SPI_DEV_NONE -1

void SPI_init(void);
void SPI_select_dev(int8_t dev_num);
void SPI_transfer(void *recv, void *send, uint8_t count);
// void SPI_exit(void);

#endif
