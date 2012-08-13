#ifndef __MCP4922_H__
#define __MCP4922_H__

#include <inttypes.h>

void mcp4922_io_init(void);
void mcp4922_write_channel(uint8_t ch, uint16_t val);
uint8_t mcp4922_spi_mode(void);

#endif

