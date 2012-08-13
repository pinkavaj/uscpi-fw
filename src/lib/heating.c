
#include <inttypes.h>
#include <avr/eeprom.h>
#include "lib/heating.h"
#include "lib/pic16.h"
#include "drivers/spi_dev.h"
#include "drivers/timer.h"

typedef struct {
	uint16_t Icode;
	uint16_t Ucode;
} daq_data_t;

typedef struct {
        pic16_data_t pic;
} heating_channel_t;

typedef struct {
        uint8_t spi_dev_num;
        uint8_t Ichannel;
        uint8_t Uchannel;
        /* FIXME: tohle může přijít i jinam */
        uint16_t Icode_offs;
        uint16_t Ucode_offs;
} daq_param_t;

/* FIXME: */
typedef struct {
        uint16_t Imin;
        uint16_t Imax;
        uint16_t Umin;
        uint16_t Umax;
} daq_limits_t;

/* TODO: stavy vstupu (ok, zkrat, .. ?) */
/* TODO: uint32_t R_slope */

typedef struct {
        daq_param_t daq;
        pic16_param_t pic;
} heating_channel_param_t;

heating_channel_t channels[2];
static const heating_channel_param_t EEMEM channel_params_E[2] = {
        {
                .daq = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 0,
                        .Uchannel = 1,
                },
                .pic = {
                        .gain_lin = 1,
                        .gain_int = 1,
                },
        },
        {
                .daq = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 2,
                        .Uchannel = 3,
                },
                .pic = {
                        .gain_lin = 1,
                        .gain_int = 1,
                },
        },
};

static daq_data_t heating_daq(uint8_t channel)
{
	daq_data_t daq_data;

	uint32_t Icode = 0;
	uint32_t Ucode = 0;

        const heating_channel_param_t *params_E = &channel_params_E[channel];
	uint8_t Ichannel = eeprom_read_byte(&params_E->daq.Ichannel);
	uint8_t Uchannel = eeprom_read_byte(&params_E->daq.Uchannel);
        uint8_t spi_dev_num = eeprom_read_byte(&params_E->daq.spi_dev_num);
	
        SPI_dev_select(spi_dev_num);
#define SAMPLES 8
#define DAQ_READS 16
	uint16_t i =  DAQ_READS;
	TIMER1_delay_rel_reset();
	do {
		Icode += SPI_dev_AD_get_sample(Ichannel);
		Ucode += SPI_dev_AD_get_sample(Uchannel);
		TIMER1_delay_rel(TIMER1_TICKS_PER_PLC / DAQ_READS);
	} while (--i);

	daq_data.Icode = Icode / (SAMPLES * DAQ_READS);
	daq_data.Ucode = Ucode / (SAMPLES * DAQ_READS);
#undef DAQ_READS
#undef SAMPLES

	return daq_data;
}

void heating_daq_test(uint8_t channel, uint16_t *Icode, uint16_t *Ucode)
{
        daq_data_t daq_data;

        TIMER1_jiff_alarm(1);
        daq_data = heating_daq(channel);
        *Icode = daq_data.Icode;
        *Ucode = daq_data.Ucode;
}

daq_data_t heating_check_daq_data(daq_data_t daq_data)
{
        return daq_data;
}

void heating(void)
{
        daq_data_t daq_data;

        daq_data = heating_daq(0);
        daq_data = heating_check_daq_data(daq_data);
}

