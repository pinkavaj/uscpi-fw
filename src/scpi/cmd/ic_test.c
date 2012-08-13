#include "ic_test.h"

#ifdef SCPI_TEST

#include <ctype.h>
#include <string.h>

#include "cmd.h"
#include "cmd_tools.h"
#include "scpi_parser.h"
#include "lib/extlib.h"
#include "lib/iobuf.h"

#include "drivers/spi_dev.h"
#include "drivers/timer.h"
#include "lib/math_cust.h"
#include "lib/thermometer_pt.h"
#include "temp.h"

#if (DEVICE_NAME == DEVICE_IMPA444_ASU)
#define SPI_DEV_AD_0 SPI_DEV_AD974_0
#define SPI_DEV_DA_0 SPI_DEV_MCP4922_0
#elif (DEVICE_NAME == DEVICE_MSA_HCU814)
#define SPI_DEV_AD_0 SPI_DEV_MAX1068_0
#define SPI_DEV_AD_1 SPI_DEV_MAX1068_1
#define SPI_DEV_DA_0 SPI_DEV_TLV5610_0
#endif

/*
static SCPI_parse_t SCPI_IC_test_adc(void)
{
	if (_SCPI_CMD_IS_QUEST()) {
		SPI_dev_select(SPI_DEV_AD_0);

		for (uint8_t channel = 0; ; channel++) {
			uint16_t sample;

			sample = SPI_dev_AD_get_sample(channel);
			SCPI_print_uint16(sample);
			sample = SPI_dev_AD_get_sample(channel);
			SCPI_print_uint16(sample);
			if (channel == 3)
				break;
		}
	} else {
                uint16_t val;

		SPI_dev_select(SPI_DEV_DA_0);

		SCPI_in_uint16(&val);
		SPI_dev_DA_set_output(0, val);

		SCPI_in_uint16(&val);
		SPI_dev_DA_set_output(1, val);
	} 
	return SCPI_parse_end;
}*/

static SCPI_parse_t SCPI_IC_test_func_div(void)
{
	uint64_t nom;
	uint32_t num;
	uint32_t *p = (void *)&nom;
        uint32_t val;

	SCPI_in_uint32(p+1);
	SCPI_in_uint32(p);
	SCPI_in_uint32(&num);

	val = math_div_64_32_r32(nom, num);
	SCPI_print_uint32(val);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_func_mul(void)
{
	uint32_t val1, val2;
	volatile uint64_t val;
        uint32_t *p;

	SCPI_in_uint32(&val1);
	SCPI_in_uint32(&val2);

	val = math_mul_32_32_r64(val1, val2);
	p = (void*)&val;
	SCPI_print_uint32(*(p+1));
	SCPI_print_uint32(val);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_func_dec(void)
{
        FP_16_16_t x;
        SCPI_parse_t ret;

	ret = SCPI_in_FP_16_16(&x);
        if (ret == SCPI_parse_err)
                return ret;

	SCPI_print_FP_16_16(x);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_func_int(void)
{
        uint32_t x;
        SCPI_parse_t ret;

	ret = SCPI_in_uint32(&x);
        if (ret == SCPI_parse_err)
                return ret;

	SCPI_print_uint32(x);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_port(void)
{
        uint8_t port;
        SCPI_parse_t ret;

		ret = SCPI_in_uint8(&port);
        if (ret == SCPI_parse_err)
                return ret;

		if (_SCPI_CMD_IS_QUEST()) {
			uint16_t val;
			if (port == 0) {
				val = PORTA;
			} else if (port == 1) {
				val = PORTB;
			} else if (port == 2) {
				val = PORTC;
			} else if (port == 3) {
				val = PORTD;
			} else {
				val = 0xffff;
			}

			SCPI_print_uint16(val);
        	return SCPI_parse_end;
		}

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_temp(void)
{
        uint16_t val;

        if (SCPI_params_count != 1)
                return SCPI_parse_err;

        SCPI_in_uint16(&val);
        val = Pt_RtoT(val);
        SCPI_print_uint16(val);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_temp_res(void)
{
        uint16_t val;

        if (SCPI_params_count != 1)
                return SCPI_parse_err;

        SCPI_in_uint16(&val);
        val = Pt_TtoR(val);
        SCPI_print_uint16(val);

        return SCPI_parse_end;
}

static SCPI_parse_t SCPI_IC_test_time(void)
{
	SCPI_print_uint16(time_sec);

        return SCPI_parse_end;
}

/*static SCPI_parse_t SCPI_IC_test_heat(void)
{
        for(uint8_t channel = 0; channel < TEMP_CHANNELS; channel++)
        {
		temp_data_IU_t temp_data_IU;

                temp_data_IU = temp_meas_IU(channel);
                SCPI_print_uint16(temp_data_IU.I);
                SCPI_print_uint16(temp_data_IU.U);
        }
        return SCPI_parse_end;
}*/

/*static const SCPI_cmd_t SCPI_cmd_test_adc_P PROGMEM = {
        .get_P = 1,
        .set_P = 1,
        .set_params_min_P = 2,
        .parser_P = SCPI_IC_test_adc,
};*/

static const SCPI_cmd_t SCPI_cmd_test_func_dec_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_func_dec,
};

static const SCPI_cmd_t SCPI_cmd_test_func_div_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_func_div,
};

static const SCPI_cmd_t SCPI_cmd_test_func_int_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_func_int,
};

static const SCPI_cmd_t SCPI_cmd_test_func_mul_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_func_mul,
};

/*static const SCPI_cmd_t SCPI_cmd_test_heat_P PROGMEM = {
        .get_P = 1,
        .parser_P = SCPI_IC_test_heat,
};*/

static const SCPI_cmd_t SCPI_cmd_test_port_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_port,
};

static const SCPI_cmd_t SCPI_cmd_test_temp_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_temp,
};

static const SCPI_cmd_t SCPI_cmd_test_temp_res_P PROGMEM = {
        .get_P = 1,
        .get_has_params_P = 1,
        .parser_P = SCPI_IC_test_temp_res,
};

static const SCPI_cmd_t SCPI_cmd_test_time_P PROGMEM = {
        .get_P = 1,
        .parser_P = SCPI_IC_test_time,
};

#define _SCPI_BRANCH_(k, c, b) { .key_P = &k, .cmd_P = c, .branch_P = b, }
static const SCPI_branch_item_t SCPI_bt_test_temp_P[] PROGMEM = {
	_SCPI_BRANCH_(key_res_P, &SCPI_cmd_test_temp_res_P, NULL),
	_SCPI_branch_END_,
};

static const SCPI_branch_item_t SCPI_bt_test_func_P[] PROGMEM = {
	_SCPI_BRANCH_(key_dec_P, &SCPI_cmd_test_func_dec_P, NULL),
	_SCPI_BRANCH_(key_div_P, &SCPI_cmd_test_func_div_P, NULL),
	_SCPI_BRANCH_(key_int_P, &SCPI_cmd_test_func_int_P, NULL),
	_SCPI_BRANCH_(key_mul_P, &SCPI_cmd_test_func_mul_P, NULL),
};

const SCPI_branch_item_t SCPI_bt_test_P[] PROGMEM = {
//	_SCPI_BRANCH_(key_adc_P, &SCPI_cmd_test_adc_P, NULL),
	_SCPI_BRANCH_(key_func_P, NULL, SCPI_bt_test_func_P),
//	_SCPI_BRANCH_(key_heat_P, &SCPI_cmd_test_heat_P, NULL),
	_SCPI_BRANCH_(key_port_P, &SCPI_cmd_test_port_P, NULL),
	_SCPI_BRANCH_(key_temp_P, &SCPI_cmd_test_temp_P, SCPI_bt_test_temp_P),
	_SCPI_BRANCH_(key_time_P, &SCPI_cmd_test_time_P, NULL),
	_SCPI_branch_END_,
};

#endif
// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
