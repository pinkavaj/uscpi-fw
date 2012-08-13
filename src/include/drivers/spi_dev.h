#ifndef __SPI_DEV_H__
#define __SPI_DEV_H__

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <inttypes.h>

#define SPI_DEV_NONE 0xff

/* indexes for SPI devices, use continous numbering to get optimal C code */
#if (DEVICE_NAME == DEVICE_IMPA444_ASU)
#define SPI_DEV_AD974_0 0
#define SPI_DEV_MCP4922_0 1

#elif (DEVICE_NAME == DEVICE_MSA_HCU814)
#define SPI_DEV_MAX1068_0 0
#define SPI_DEV_MAX1068_1 1
#define SPI_DEV_TLV5610_0 2
#endif

void SPI_dev_init(void);
void SPI_dev_select(uint8_t dev_num);
uint16_t SPI_dev_AD_get_sample(uint8_t channel);
void SPI_dev_DA_set_output(uint8_t channel, uint16_t output);
uint8_t SPI_dev_DA_width(void);

#endif
