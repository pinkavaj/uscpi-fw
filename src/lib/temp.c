
#include <inttypes.h>
#include <avr/eeprom.h>
#include "lib/temp.h"
#include "drivers/spi_dev.h"
#include "drivers/timer.h"

/* temperature DAQ device to measure I and U */
typedef struct {
        uint8_t spi_dev_num;
        uint8_t Ichannel;
        uint8_t Uchannel;
} temp_daq_dev_t;

/* FIXME: Limits */
typedef struct {
        uint16_t Imin;
        uint16_t Imax;
        uint16_t Umin;
        uint16_t Umax;
} daq_limits_t;

 /* Correction constants for DAQ I and U data */
typedef struct {
        int8_t I_offs;
        int8_t U_offs;
} temp_daq_data_correct_t;

/* FIXME: DAQ data status */
#define DAQ_STAT_I_OVER (1<<0)
#define DAQ_STAT_I_UNDER (2<<0)
#define DAQ_STAT_U_OVER (1<<3)
#define DAQ_STAT_U_UNDER (2<<3)
typedef enum {
        daq_status_valid = 0,
        daq_status_invalid1 = DAQ_STAT_I_OVER,
        daq_status_invalid2 = DAQ_STAT_I_UNDER,
        daq_status_invalid3 = DAQ_STAT_U_OVER,
        daq_status_invalid4 = DAQ_STAT_U_UNDER,
        daq_status_invalid5 = DAQ_STAT_I_OVER | DAQ_STAT_U_OVER,
        daq_status_shortcut = DAQ_STAT_I_OVER | DAQ_STAT_U_UNDER,
        daq_status_disconnected = DAQ_STAT_I_UNDER | DAQ_STAT_U_OVER,
        daq_status_invalid6 = DAQ_STAT_I_UNDER | DAQ_STAT_U_UNDER,
} daq_status_t;

/* Temperature channel working data */
typedef struct {
        pic16_data_t pic;
} temp_t;

/* Temperature channel constants */
typedef struct {
        temp_daq_dev_t daq;
        pic16_param_t pic;
	temp_daq_data_correct_t daq_data_correct;
/* TODO: stavy vstupu (ok, zkrat, .. ?) */
        uint32_t R_slope;
} temp_conf_t;

temp_t channels[2];
static const temp_conf_t EEMEM temp_conf_E[2] = {
        {
                .daq = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 0,
                        .Uchannel = 1,
                },
		.daq_data_correct = {
			.I_offs = 0,
			.U_offs = 0,
		},
                .pic = {
                        .gain_lin = 123,
                        .gain_int = 24,
                },
        },
        {
                .daq = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 2,
                        .Uchannel = 3,
                },
		.daq_data_correct = {
			.I_offs = 0,
			.U_offs = 0,
		},
                .pic = {
                        .gain_lin = 234,
                        .gain_int = 34,
                },
        },
};

pic16_param_t temp_pic_params_get(uint8_t channel)
{
        const pic16_param_t *pic_E;
        pic16_param_t pic_param;

        pic_E = &temp_conf_E[channel].pic;
        pic_param.gain_lin = eeprom_read_word(&pic_E->gain_lin);
        pic_param.gain_int = eeprom_read_word(&pic_E->gain_int);

        return pic_param;
}

void temp_pic_params_set(uint8_t channel, pic16_param_t pic_param)
{
        const pic16_param_t *pic_E;

        pic_E = &temp_conf_E[channel].pic;
        eeprom_write_word((uint16_t *)&pic_E->gain_lin, pic_param.gain_lin);
        eeprom_write_word((uint16_t *)&pic_E->gain_int, pic_param.gain_int);
}

static temp_daq_data_t temp_meas_IU(uint8_t channel)
{
	temp_daq_data_t daq_data;
	int32_t I = 0;
	int32_t U = 0;
	const temp_conf_t *conf_E;

	conf_E = &temp_conf_E[channel];
	uint8_t Ichannel = eeprom_read_byte(&conf_E->daq.Ichannel);
	uint8_t Uchannel = eeprom_read_byte(&conf_E->daq.Uchannel);
	uint8_t spi_dev_num = eeprom_read_byte(&conf_E->daq.spi_dev_num);
	
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

temp_daq_data_t temp_meas_IU_raw(uint8_t channel)
{
        TIMER1_jiff_alarm(1);
        return temp_meas_IU(channel);
}

/* Make correction above I nad U from acqusition */
temp_daq_data_t temp_meas_IU_correct(uint8_t channel, temp_daq_data_t daq_data)
{
	int8_t offs;
	const temp_conf_t *conf_E;

	conf_E = &temp_conf_E[channel];
	offs = eeprom_read_byte((uint8_t *)&conf_E->daq_data_correct.U_offs);
	daq_data.I -= offs;
	offs = eeprom_read_byte((uint8_t *)&conf_E->daq_data_correct.I_offs);
	daq_data.U -= offs;

	return daq_data;
}

daq_status_t temp_meas_IU_status(temp_daq_data_t daq_data)
{
	/* TODO: ... */
        daq_status_t status;
        uint16_t Imin, Imax, Umin;

        if (daq_data.I > Imax) {
                if (daq_data.U < Umin) {
                        status = daq_status_shortcut;
                }
                else
                        status = daq_status_invalid1;

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

