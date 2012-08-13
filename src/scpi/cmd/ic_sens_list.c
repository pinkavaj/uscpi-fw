#include <ctype.h>

#include "cmd_tools.h"
#include "ic_sens_list.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"
#include "list.h"

/* Return channel <N> defined in CMD ( :TEMPerature<N> ) */
static uint8_t get_temp_channel(void)
{
#if TEMP_CHANNELS != 1
        if (SCPI_num_suffixes_idx != 1)
                SCPI_err_set(&SCPI_err_2);
#endif

        return SCPI_num_suffixes[0];
}

SCPI_parse_t SCPI_IC_sens_list_coun(void)
{
        uint8_t channel;
        uint8_t coun;
        SCPI_parse_t ret;

        channel = get_temp_channel();
        coun = list_coun[channel];
        if (_SCPI_CMD_IS_QUEST()) {
                if (coun == LIST_COUN_INF)
                        SCPI_print_P(key_inf_P.keyword_P);
                else
                        SCPI_print_uint16(coun);
                return SCPI_parse_end;
        }
        ret = SCPI_in_uint8(&list_coun[channel]);
        if (ret == SCPI_parse_err) {
                if(keycmp_P(SCPI_in + SCPI_param_in_buf_idx, &key_inf_P))
                        return ret;
                list_coun[channel] = LIST_COUN_INF;
        } else {
                if (list_coun[channel] == LIST_COUN_INF) {
                        list_coun[channel] = coun;
                        return SCPI_err_set_(&SCPI_err_222);
                }
        }

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sens_list_dwel(void)
{
        uint8_t channel;
        uint8_t poin = 0;
        SCPI_parse_t ret;

        channel = get_temp_channel();
        if (_SCPI_CMD_IS_QUEST()) {
                uint8_t poin_end;

                poin_end = list_temp_poin[channel];
                while(poin < poin_end)
                        SCPI_print_uint16(list[channel][poin++].dwel);

                return SCPI_parse_end;
        }
        do {
                ret = SCPI_in_uint16(&list[channel][poin++].dwel);
                if (ret == SCPI_parse_err)
                        return ret;
        } while (poin < SCPI_params_count);
        /* if specified only one value, is used for all items in list */
        if (poin == 1) {
                uint16_t dwel;

                dwel = list[channel][0].dwel;
                do {
                        list[channel][poin++].dwel = dwel;
                } while(poin < LIST_MAX);
        } else 
                list_temp_poin[channel] = poin;

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sens_list_slop(void)
{
        uint8_t channel;
        uint8_t poin = 0;
        SCPI_parse_t ret;
        temp_1_20_t slop;

        channel = get_temp_channel();
        if (_SCPI_CMD_IS_QUEST()) {
                uint8_t poin_end;

                poin_end = list_temp_poin[channel];
                while (poin < poin_end)
                        SCPI_print_temp_1_20(
                                        temp_slop2public(
                                                list[channel][poin++].slop));
                return SCPI_parse_end;
        }
        do {
                ret = SCPI_in_temp_1_20(&slop);
                if (ret == SCPI_parse_err)
                        return ret;
                list[channel][poin++].slop = temp_slop2internal(slop);
        } while (poin < SCPI_params_count);
        /* if specified only one value, is used for all items in list */
        if (poin == 1) {
                uint16_t slop;

                slop = list[channel][0].slop;
                do {
                        list[channel][poin++].slop = slop;
                } while(poin < LIST_MAX);
        } else 
                list_temp_poin[channel] = poin;

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sens_list_temp(void)
{
        uint8_t channel;
        uint8_t poin = 0;
        SCPI_parse_t ret;

        channel = get_temp_channel();
        if (_SCPI_CMD_IS_QUEST()) {
                uint8_t poin_end;

                poin_end = list_temp_poin[channel];
                while(poin < poin_end)
                        SCPI_print_temp_1_20(list[channel][poin++].T_spo);

                return SCPI_parse_end;
        }
        do {
                ret = SCPI_in_temp_1_20(&list[channel][poin++].T_spo);
                if (ret == SCPI_parse_err)
                        return ret;
        } while (poin < SCPI_params_count);
        list_temp_poin[channel] = poin;

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sens_list_temp_poin(void)
{
        uint8_t channel;

        channel = get_temp_channel();
        SCPI_print_uint16(list_temp_poin[channel]);

        return SCPI_parse_end;
}

const SCPI_cmd_t SCPI_cmd_sens_list_P PROGMEM = {
        .num_suffix_max_P = TEMP_CHANNELS - 1,
};

static const SCPI_cmd_t SCPI_cmd_sens_list_coun_P PROGMEM = {
        .parser_P = SCPI_IC_sens_list_coun, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
};

static const SCPI_cmd_t SCPI_cmd_sens_list_dwel_P PROGMEM = {
        .parser_P = SCPI_IC_sens_list_dwel, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_opt_P = LIST_MAX - 1,
};

static const SCPI_cmd_t SCPI_cmd_sens_list_slop_P PROGMEM = {
        .parser_P = SCPI_IC_sens_list_slop, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_opt_P = LIST_MAX - 1,
};

static const SCPI_cmd_t SCPI_cmd_sens_list_temp_P PROGMEM = {
        .parser_P = SCPI_IC_sens_list_temp, 
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 1,
        .set_params_opt_P = LIST_MAX - 1,
};

static const SCPI_cmd_t SCPI_cmd_sens_list_temp_poin_P PROGMEM = {
        .parser_P = SCPI_IC_sens_list_temp_poin, 
        .get_P = 1,
};

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
static const SCPI_branch_item_t SCPI_bt_sens_list_dwel_P[] PROGMEM = {
	_SCPI_BRANCH_(key_poin_P, &SCPI_cmd_sens_list_temp_poin_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_sens_list_slop_P[] PROGMEM = {
	_SCPI_BRANCH_(key_poin_P, &SCPI_cmd_sens_list_temp_poin_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_sens_list_temp_P[] PROGMEM = {
	_SCPI_BRANCH_(key_poin_P, &SCPI_cmd_sens_list_temp_poin_P, NULL),
	_SCPI_branch_END_,
};

const SCPI_branch_item_t SCPI_bt_sens_list_P[] PROGMEM = {
	_SCPI_BRANCH_(key_coun_P, &SCPI_cmd_sens_list_coun_P, NULL),
	_SCPI_BRANCH_(key_dwel_P, &SCPI_cmd_sens_list_dwel_P, 
                        SCPI_bt_sens_list_dwel_P),
	_SCPI_BRANCH_(key_slop_P, &SCPI_cmd_sens_list_slop_P, 
                        SCPI_bt_sens_list_slop_P),
	_SCPI_BRANCH_(key_temp_P, &SCPI_cmd_sens_list_temp_P, 
                        SCPI_bt_sens_list_temp_P),
	_SCPI_branch_END_,
};

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
