#include <inttypes.h>
#include <avr/eeprom.h>
#include "drivers/spi_dev.h"
#include "drivers/timer.h"
#include "lib/math_cust.h"
#include "lib/temp.h"
#include "lib/thermometer_pt.h"

/* Module is constructed to use this number of channels, regardless of amout
 * real channels used. */
#define TEMP_CHANNELS_INTERNAL 8

/* temperature DAQ device to measure I and U */
typedef struct {
        uint8_t spi_dev_num;
        uint8_t Ichannel;
        uint8_t Uchannel;
} temp_dev_AD_t;

typedef struct {
        uint8_t spi_dev_num;
        uint8_t channel;
} temp_dev_DA_t;

 /* Correction for I and U  */
typedef struct {
        int8_t I_offs;
        int8_t U_offs;
} temp_correct_IU_t;

typedef enum {
        temp_status_IU_valid = 0,
        temp_status_IU_shortcut,
        temp_status_IU_disconnected,
        temp_status_IU_invalid,
} temp_status_IU_t;

/* Temperature channel working data */
typedef struct {
        pic16_data_t pic;
        FP_2_14_t R_want;
} temp_t;

/* Temperature channel constants */
typedef struct {
        pic16_param_t pic;
	temp_correct_IU_t correct_IU;
        temp_dev_AD_t dev_AD;
        temp_dev_DA_t dev_DA;
/* TODO: stavy vstupu (ok, zkrat, .. ?) */
        /* FIXME: FP num. format */
        uint32_t R_slope;
        uint32_t R_0;
} temp_conf_t;

temp_t temp_data[TEMP_CHANNELS];
static const temp_conf_t EEMEM temp_conf_E[TEMP_CHANNELS] = {
        {
                .R_0 = 0,
                .R_slope = 0x0023A147,
		.correct_IU = {
			.I_offs = 0,
			.U_offs = 0,
		},
                .dev_AD = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 0,
                        .Uchannel = 1,
                },
                .dev_DA = {
                        .spi_dev_num = SPI_DEV_MCP4922_0,
                        .channel = 0,
                },
                .pic = {
                        .gain_lin = 123,
                        .gain_int = 24,
                },
        },
        {
                .R_0 = 0,
                .R_slope = 0x0023A147,
		.correct_IU = {
			.I_offs = 0,
			.U_offs = 0,
		},
                .dev_AD = {
                        .spi_dev_num = SPI_DEV_AD974_0,
                        .Ichannel = 2,
                        .Uchannel = 3,
                },
                .dev_DA = {
                        .spi_dev_num = SPI_DEV_MCP4922_0,
                        .channel = 1,
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

temp_data_IU_t temp_meas_IU(uint8_t channel)
{
	temp_data_IU_t data_IU;
	int32_t I = 0, U = 0;
	const temp_conf_t *conf_E;
	uint8_t spi_dev_num;
	uint8_t Ichannel, Uchannel;

	conf_E = &temp_conf_E[channel];
	spi_dev_num = eeprom_read_byte(&conf_E->dev_AD.spi_dev_num);
	Ichannel = eeprom_read_byte(&conf_E->dev_AD.Ichannel);
	Uchannel = eeprom_read_byte(&conf_E->dev_AD.Uchannel);
	
        SPI_dev_select(spi_dev_num);
#define DAQ_SAMPLES 8
#define DAQ_READS 16
	TIMER1_delay_rel_reset();
        for (uint8_t i = DAQ_READS; i; i--) {
                for (uint8_t x = DAQ_SAMPLES; x; x--)
                        I += SPI_dev_AD_get_sample(Ichannel);
                for (uint8_t x = DAQ_SAMPLES; x; x--)
                        U += SPI_dev_AD_get_sample(Uchannel);
		TIMER1_delay_rel(TIMER1_TICKS_PER_PLC / DAQ_READS);
	}

	data_IU.I = I / (DAQ_SAMPLES * DAQ_READS);
	data_IU.U = U / (DAQ_SAMPLES * DAQ_READS);

	return data_IU;
}

/* Make correction above I nad U from acqusition */
static temp_data_IU_t temp_correct_IU(temp_data_IU_t data_IU, uint8_t channel)
{
	int8_t offs;
	const temp_conf_t *conf_E;

	conf_E = &temp_conf_E[channel];
	offs = eeprom_read_byte((uint8_t *)&conf_E->correct_IU.U_offs);
	data_IU.I -= offs;
	offs = eeprom_read_byte((uint8_t *)&conf_E->correct_IU.I_offs);
	data_IU.U -= offs;

	return data_IU;
}

static void temp_shortcut(uint8_t channel)
{
        uint8_t spi_dev_num;
        uint8_t DAchannel;
	const temp_dev_DA_t *dev_DA_E;

	dev_DA_E = &temp_conf_E[channel].dev_DA;
	spi_dev_num = eeprom_read_byte(&dev_DA_E->spi_dev_num);
	DAchannel = eeprom_read_byte(&dev_DA_E->channel);
        
        SPI_dev_select(spi_dev_num);
        /* FIXME: value */
        SPI_dev_DA_set_output(DAchannel, 0);
}

static temp_status_IU_t temp_status_IU(temp_data_IU_t data_IU)
{
#define IMAX 2**15-128
/* FIXME: DAQ data status */
#define DAQ_STAT_I_OVER (1<<0)
#define DAQ_STAT_I_UNDER (2<<0)
#define DAQ_STAT_U_OVER (1<<3)
#define DAQ_STAT_U_UNDER (2<<3)
        temp_status_IU_t status;
/*        uint16_t Imin, Imax, Umax, Umin;

//        Imin = pgm_r

        if (data_IU.I > Imax) {
                if (data_IU.U < Umin) {
                        status = daq_status_shortcut;
                }
                else
                        status = daq_status_invalid;

        } else if (data_IU.I < Imin) {
        }*/
        data_IU = data_IU;
        //status = temp_status_IU_invalid;
        status = temp_status_IU_valid;

        return status;
}

static uint32_t temp_get_R(temp_data_IU_t data_IU, uint8_t channel)
{
        uint64_t tmp64;
        uint32_t R, R_slope;

        R_slope = eeprom_read_dword(&temp_conf_E[channel].R_slope);
        /* R = U * R_slope / I;
         * R_norm = R / R_0 */
        tmp64 = math_mul_32_32_r64(data_IU.U, R_slope);
        R = math_div_64_32_r32(tmp64, data_IU.I);

        return R;
}

#include "lib/iobuf.h"
static void temp_loop_(uint8_t channel)
{
        temp_data_IU_t data_IU;
        temp_status_IU_t status;

        putc('A'+channel);
        data_IU = temp_meas_IU(channel);
        status = temp_status_IU(data_IU);
        if (status == temp_status_IU_disconnected) {
                temp_shortcut(channel);
                return;
        }
        if (status == temp_status_IU_invalid) {
                temp_shortcut(channel);
                return;
        }
        if (status == temp_status_IU_shortcut) {
                temp_shortcut(channel);
                return;
        }
        data_IU = temp_correct_IU(data_IU, channel);
        temp_get_R(data_IU, channel);

        uint16_t e;

        pic16(&temp_data[channel].pic, e, &temp_conf_E[channel].pic);
        TIMER1_jiff_alarm(TIMER1_ALARM_SEND);
        /* TODO:  Regulujeme */
}

void temp_loop(void)
{
        static uint8_t channel;

        if (!TIMER1_new_period())
                return;

        if (channel < TEMP_CHANNELS)
                temp_loop_(channel);
        else {
                if (channel >= TEMP_CHANNELS_INTERNAL) {
                        channel = 0;
                        return;
                }
        }

        channel++;
}

