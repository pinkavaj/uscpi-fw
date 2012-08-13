#ifndef __SCPI_IC_LIST_H__
#define __SCPI_IC_LIST_H__

#include <avr/pgmspace.h>
#include "cmd.h"
#include "scpi.h"

extern const SCPI_branch_item_t SCPI_bt_list_P[] PROGMEM;
extern const SCPI_cmd_t SCPI_cmd_list_P PROGMEM;

void list_init(void);

#endif
/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
