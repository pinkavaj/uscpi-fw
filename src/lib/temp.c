
#include <inttypes.h>
#include <avr/eeprom.h>
#include "lib/temp.h"
#include "lib/pic16.h"
#include "drivers/spi_dev.h"
#include "drivers/timer.h"

typedef struct {
	uint16_t I;
	uint16_t U;
} temp_daq_data_t;

typedef struct {
        uint8_t spi_dev_num;
        uint8_t Ichannel;
        uint8_t Uchannel;
} temp_daq_dev_t;

/* FIXME: */
typedef struct {
        uint16_t Imin;
        uint16_t Imax;
        uint16_t Umin;
        uint16_t Umax;
        /* FIXME: tohle může přijít i jinam */
        int8_t I_offs;
        int8_t U_offs;
} daq_limits_t;

typedef enum {
        daq_status_valid = 0,
        daq_status_shortcut = 1,
        daq_status_invalid = 2,
} daq_status_t;

typedef struct {
        pic16_data_t pic;
} temp_channel_t;

typedef struct {
        temp_daq_dev_t daq;
        pic16_param_t pic;
/* TODO: stavy vstupu (ok, zkrat, .. ?) */
        uint32_t R_slope;
} temp_channel_param_t;

temp_channel_t channels[2];
static const temp_channel_param_t EEMEM channel_params_E[2] = {
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

static temp_daq_data_t temp_meas_IU(uint8_t channel)
{
	temp_daq_data_t daq_data;

	int32_t I = 0;
	int32_t U = 0;

        const temp_channel_param_t *params_E = &channel_params_E[channel];
	uint8_t Ichannel = eeprom_read_byte(&params_E->daq.Ichannel);
	uint8_t Uchannel = eeprom_read_byte(&params_E->daq.Uchannel);
        uint8_t spi_dev_num = eeprom_read_byte(&params_E->daq.spi_dev_num);
	
        SPI_dev_select(spi_dev_num);
#define SAMPLES 8
#define DAQ_READS 16
	uint8_t i = DAQ_READS;
	TIMER1_delay_rel_reset();
	do {
		I += SPI_dev_AD_get_sample(Ichannel);
		U += SPI_dev_AD_get_sample(Uchannel);
		TIMER1_delay_rel(TIMER1_TICKS_PER_PLC / DAQ_READS);
	} while (--i);

	daq_data.I = I / (SAMPLES * DAQ_READS);
	daq_data.U = U / (SAMPLES * DAQ_READS);
#undef DAQ_READS
#undef SAMPLES

	return daq_data;
}

void temp_meas_IU_test(uint8_t channel, uint16_t *I, uint16_t *U)
{
        temp_daq_data_t daq_data;

        TIMER1_jiff_alarm(1);
        daq_data = temp_meas_IU(channel);
        *I = daq_data.I;
        *U = daq_data.U;
}

daq_status_t temp_meas_IU_status(temp_daq_data_t daq_data)
{
        daq_status_t status;
        uint16_t Imin, Imax, Umin;

        if (daq_data.I > Imax) {
                if (daq_data.U < Umin) {
                        status = daq_status_shortcut;
                }
                else
                        status =daq_status_invalid;

        } else if (daq_data.I < Imin) {
        }

        return status;
}

void temp(void)
{
        temp_daq_data_t daq_data;

        daq_data = temp_meas_IU(0);
        temp_meas_IU_status(daq_data);
}

