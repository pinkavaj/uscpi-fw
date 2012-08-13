#ifndef __SPI_DEV_H__
#define __SPI_DEV_H__

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <inttypes.h>

/* indexes for SPI devices, use continous numbering to get optimal C code */
#define SPI_DEV_AD974_0 0
#define SPI_DEV_MCP4922_0 1
/* this shoul have number last_num + 2 */
#define SPI_DEV_NONE 2

void SPI_dev_init(void);
void SPI_dev_select(uint8_t dev_num);
uint16_t SPI_dev_AD_get_sample(uint8_t channel);

#endif
