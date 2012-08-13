#include <ctype.h>

#include "cmd_tools.h"
#include "ic_meas.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "lib/temp.h"

static uint8_t get_temp_channel(void)
{
        if (SCPI_num_suffixes_idx != 1)
                SCPI_err_set(&SCPI_err_2);

        return SCPI_num_suffixes[0];
}

static SCPI_parse_t SCPI_IC_meas_temp(void)
{
        uint8_t channel;
        temp_1_20_t T;

        channel = get_temp_channel();
        T = temp_get(channel);
        print_uint32(T / 20);
        putc('.');
        print_uint32f((T % 20) * 5, 2);

        return SCPI_parse_end;
}

static const SCPI_cmd_t SCPI_cmd_meas_temp_P PROGMEM = {
        .parser_P = SCPI_IC_meas_temp,
        .get_P = 1,
	.num_suffix_max_P = TEMP_CHANNELS - 1,
};

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
const SCPI_branch_item_t SCPI_bt_meas_P[] PROGMEM = {
	_SCPI_BRANCH_(key_temp_P, &SCPI_cmd_meas_temp_P, NULL),
	_SCPI_branch_END_,
};

