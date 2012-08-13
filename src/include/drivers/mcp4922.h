#ifndef __MCP4922_H__
#define __MCP4922_H__

#include <inttypes.h>

void mcp4922_io_init(void);
void mcp4922_select(void);
uint8_t mcp4922_spi_mode(void);
uint8_t mcp4922_width(void);
void mcp4922_write_channel(uint8_t ch, uint16_t val);

#endif

