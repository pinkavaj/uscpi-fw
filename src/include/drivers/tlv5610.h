#ifndef __TLV5610_H__
#define __TLV5610_H__

#include <inttypes.h>

void tlv5610_io_init(void);
void tlv5610_select(void);
uint8_t tlv5610_spi_mode(void);
uint8_t tlv5610_width(void);
void tlv5610_0_write_channel(uint8_t ch, uint16_t val);

#endif

