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
#include "lib/temp.h"

/* TODO: přemístit sem definici klíče */

SCPI_parse_t SCPI_IC_test_adc(char UNUSED(c))
{
	if (_SCPI_CMD_IS_QUEST()) {
		SPI_dev_select(SPI_DEV_AD974_0);

		for (uint8_t channel = 0; ; channel++) {
			uint16_t sample;

			sample = SPI_dev_AD_get_sample(channel);
			print_uint32(sample);
			putc(',');
			sample = SPI_dev_AD_get_sample(channel);
			print_uint32(sample);
			if (channel == 3)
				break;
			putc(',');
		}
	} else {
		uint16_t val;

		SPI_dev_select(SPI_DEV_MCP4922_0);

		SCPI_in_uint16(&val);
		SPI_dev_DA_set_output(0, val);

		SCPI_in_uint16(&val);
		SPI_dev_DA_set_output(1, val);
	} 
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_div(char UNUSED(c))
{
	static uint64_t nom;
	static uint32_t num;

	if (_SCPI_CMD_IS_QUEST()) {
		uint32_t val = math_div_64_32_r32(nom, num);
		print_uint32(val);
	} else {
		uint32_t *p = (void *)&nom;
		SCPI_in_uint32(p+1);
		SCPI_in_uint32(p);
		SCPI_in_uint32(&num);
	}
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_mul(char UNUSED(c))
{
	static uint32_t val1, val2;

	if (_SCPI_CMD_IS_QUEST()) {
		volatile uint64_t val;

		val = math_mul_32_32_r64(val1, val2);
		uint32_t *p = (void*)&val;
		print_uint32(*(p+1));
		putc(',');
		print_uint32(val);
	} else {
		SCPI_in_uint32(&val1);
		SCPI_in_uint32(&val2);
	}
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_num(char UNUSED(c))
{
	static uint32_t val;

	if (_SCPI_CMD_IS_QUEST())
		print_uint32(val);
	else
		return SCPI_in_uint32(&val);
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_temp(char UNUSED(c))
{
        uint16_t val;

        if (SCPI_params_count != 1)
                return SCPI_parse_err;

        SCPI_in_uint16(&val);
        val = Pt_RtoT(val);

        print_uint32(val);
        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_temp_res(char UNUSED(c))
{
        uint16_t val;

        if (SCPI_params_count != 1)
                return SCPI_parse_err;

        SCPI_in_uint16(&val);
        val = Pt_TtoR(val);

        print_uint32(val);
        return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_time(char UNUSED(c))
{
	print_uint32(time_sec);
	return SCPI_parse_end;
}

SCPI_parse_t SCPI_IC_test_heat(char UNUSED(c))
{
        for(uint8_t channel = 0; channel < 1; channel++)
        {
		temp_daq_data_t temp_daq_data;

                temp_daq_data = temp_meas_IU_raw(channel);
                print_uint32(temp_daq_data.I);
                putc(',');
                print_uint32(temp_daq_data.U);
        }
        return SCPI_parse_end;
}

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
