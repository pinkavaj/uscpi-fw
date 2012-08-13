#include <ctype.h>

#include "cmd_tools.h"
#include "ic_cont.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "drivers/valves.h"

static uint8_t get_valv_channel(void)
{
#if VALVE_CHANNELS != 1
        if (SCPI_num_suffixes_idx != 1 && SCPI_num_suffixes_idx != 2)
                SCPI_err_set(&SCPI_err_2);
#endif

        return SCPI_num_suffixes[0];
}

static SCPI_parse_t SCPI_IC_cont_valv_clos(void)
{
        uint8_t channel;

        channel = get_valv_channel();
		valve_close(channel);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_cont_valv_open(void)
{
    uint8_t channel;

    channel = get_valv_channel();
	if (_SCPI_CMD_IS_QUEST()) {
		uint8_t state;

		state = valve_state(channel);
        SCPI_print_uint16(state);

        return SCPI_parse_end;
	}
	valve_open(channel);

    return SCPI_parse_end;
}

static const SCPI_cmd_t SCPI_cmd_cont_valv_clos_P PROGMEM = {
        .parser_P = SCPI_IC_cont_valv_clos,
        .set_P = 1,
	.num_suffix_max_P = VALVE_CHANNELS - 1,
};

static const SCPI_cmd_t SCPI_cmd_cont_valv_open_P PROGMEM = {
        .parser_P = SCPI_IC_cont_valv_open,
        .get_P = 1,
		.set_P = 1,
	.num_suffix_max_P = VALVE_CHANNELS - 1,
};

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
static const SCPI_branch_item_t SCPI_bt_cont_valv_P[] PROGMEM = {
	_SCPI_BRANCH_(key_clos_P, &SCPI_cmd_cont_valv_clos_P, NULL),
	_SCPI_BRANCH_(key_open_P, &SCPI_cmd_cont_valv_open_P, NULL),
	_SCPI_branch_END_,
};

const SCPI_branch_item_t SCPI_bt_cont_P[] PROGMEM = {
	_SCPI_BRANCH_(key_valv_P, &SCPI_cmd_cont_valv_open_P,
			SCPI_bt_cont_valv_P),
	_SCPI_branch_END_,
};

