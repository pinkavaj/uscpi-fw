#include <ctype.h>
#include <string.h>

#include "scpi_cmd.h"
#include "cmd_tools.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"

#include "lib/temp.h"

/* Common function to get/set PI regulator constants */
SCPI_parse_t temp_lcon(uint8_t gain)
{
        uint16_t val;
        pic16_param_t pic_param;
        SCPI_parse_t ret;
        int8_t channel;

        if (SCPI_num_suffixes_idx != 1) {
                SCPI_err_set(&SCPI_err_2);
                return SCPI_parse_err;
        }

        channel = SCPI_num_suffixes[0];
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
                return SCPI_parse_err;
        
        if (gain)
                pic_param.gain_lin = val;
        else
                pic_param.gain_int = val;
        temp_pic_params_set(channel, pic_param);

        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_sour_temp_lcon_gain(char UNUSED(c))
{
        return temp_lcon(1);
}

SCPI_parse_t SCPI_IC_sour_temp_lcon_int(char UNUSED(c))
{
        return temp_lcon(0);
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
