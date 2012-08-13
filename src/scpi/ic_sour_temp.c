#include <ctype.h>
#include <string.h>

#include "cmd_tools.h"
#include "ic_sour_temp.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "lib/temp.h"

/* Return channel <N> defined in CMD ( :TEMPerature<N> ) */
static uint8_t get_temp_channel(void)
{
        if (SCPI_num_suffixes_idx != 1) {
                SCPI_err_set(&SCPI_err_2);
                return SCPI_parse_err;
        }

        return SCPI_num_suffixes[0];
}

SCPI_parse_t SCPI_IC_sour_temp_dwel(void)
{
        uint8_t channel;

        channel = get_temp_channel();
        SCPI_err_set(&SCPI_err_1);

        /* TODO: ... */
        return SCPI_parse_err;
}

/* Common function to get/set PI regulator constants */
static SCPI_parse_t temp_lcon(uint8_t gain)
{
        pic16_param_t pic_param;
        SCPI_parse_t ret;
        int8_t channel;
        uint16_t val;

        channel = get_temp_channel();
        pic_param = temp_pic_params_get(channel);
        if (_SCPI_CMD_IS_QUEST()) {
                if (gain)
                        print_uint32(pic_param.gain_lin);
                else
                        print_uint32(pic_param.gain_int);
                return SCPI_parse_end;
        }
       
        /* FIXME: should be FP in range <0, 1) */
        ret = SCPI_in_uint16(&val);
        if (ret == SCPI_parse_err)
                return ret;
        
        if (gain)
                pic_param.gain_lin = val;
        else
                pic_param.gain_int = val;
        temp_pic_params_set(channel, pic_param);

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sour_temp_lcon_gain(void)
{
        return temp_lcon(1);
}

SCPI_parse_t SCPI_IC_sour_temp_lcon_int(void)
{
        return temp_lcon(0);
}

SCPI_parse_t SCPI_IC_sour_temp_mode(void)
{
        uint8_t channel;

        channel = get_temp_channel();
        SCPI_err_set(&SCPI_err_1);

        /* TODO: ... */
        return SCPI_parse_err;
}

SCPI_parse_t SCPI_IC_sour_temp_rtim(void)
{
        uint8_t channel;

        channel = get_temp_channel();
        SCPI_err_set(&SCPI_err_1);

        /* TODO: ... */
        return SCPI_parse_err;
}

SCPI_parse_t SCPI_IC_sour_temp_spo(void)
{
        uint8_t channel;

        channel = get_temp_channel();
        SCPI_err_set(&SCPI_err_1);

        /* TODO: ... */
        return SCPI_parse_err;
}

static const SCPI_cmd_t SCPI_cmd_sour_temp_dwel_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_dwel, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sour_temp_lcon_gain_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_lcon_gain, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sour_temp_lcon_int_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_lcon_int, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sour_temp_mode_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_mode, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sour_temp_rtim_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_rtim, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sour_temp_spo_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_spo, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
};

const SCPI_cmd_t SCPI_cmd_sour_temp_P PROGMEM = {
        .parser_P = SCPI_IC_sour_temp_spo,
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_atonce_P = 1,
	.num_suffix_max_P = TEMP_CHANNELS - 1,
};

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
static const SCPI_branch_item_t SCPI_bt_sour_temp_lcon_P[] PROGMEM = {
	_SCPI_BRANCH_(key_gain_P, &SCPI_cmd_sour_temp_lcon_gain_P, NULL),
	_SCPI_BRANCH_(key_int_P, &SCPI_cmd_sour_temp_lcon_int_P, NULL),
	_SCPI_branch_END_,
};

const SCPI_branch_item_t SCPI_bt_sour_temp_P[] PROGMEM = {
	_SCPI_BRANCH_(key_dwel_P, &SCPI_cmd_sour_temp_dwel_P, NULL),
	_SCPI_BRANCH_(key_lcon_P, &SCPI_cmd_sour_temp_lcon_gain_P, 
                        SCPI_bt_sour_temp_lcon_P),
	_SCPI_BRANCH_(key_mode_P, &SCPI_cmd_sour_temp_mode_P, NULL),
	_SCPI_BRANCH_(key_rtim_P, &SCPI_cmd_sour_temp_rtim_P, NULL),
	_SCPI_BRANCH_(key_spo_P, &SCPI_cmd_sour_temp_spo_P, NULL),
	_SCPI_branch_END_,
};

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
