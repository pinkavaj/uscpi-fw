#include <inttypes.h>
#include <avr/eeprom.h>
#include "drivers/spi_dev.h"
#include "drivers/timer.h"
#include "lib/math_cust.h"
#include "lib/thermometer_pt.h"
#include "scpi.h"
#include "stat_report.h"
#include "temp.h"

/* Module is constructed to use this number of channels, regardless of amout
 * real channels used. */
#define TEMP_CHANNELS_INTERNAL 8
/* Minimal (bottom) DA output value (even in shortcut or so!) */
#define TEMP_OUTP_DA_MIN 3072

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

/* TODO: stavy vstupu (ok, zkrat, .. ?) */
/* temp_status_IU_stabilizing   - recovering from one of invalid states */
/* FIXME: rozdělit stavy regulačního automatu T a výstupu měření */
typedef enum {
        temp_status_IU_valid = 0,
        temp_status_IU_shortcut = 1,
        temp_status_IU_disconnected = 2,
        temp_status_IU_invalid = 4,
        temp_status_IU_stabilizing = 8,
} temp_status_IU_t;

/* Temperature channel working data */
/* R            - R from last measurement
 * T_dest       - destination T for slope
 * T_slope      - slope of ramp to reach T_dest in units: °C*20/(sec*4)
 * T_want       - requested value of regulation */
typedef struct {
        uint16_t dwel;
        temp_mode_t mode;
        pic16_data_t pic;
        FP_2_14_t R;
        temp_status_IU_t status;
        temp_1_20_t T_dest;
        uint8_t T_slope;
        temp_1_20_t T_want;
} temp_data_t;

/* Temperature channel constants */
/* R_slope      - R = U_code/I_code*R_slope 
 * R_0          - R of sensor at 0°C */
typedef struct {
        pic16_param_t pic;
	temp_correct_IU_t correct_IU;
        temp_dev_AD_t dev_AD;
        temp_dev_DA_t dev_DA;
        FP_16_16_t R_slope;
        FP_16_16_t R_0;
} temp_conf_t;

temp_data_t temp_data[TEMP_CHANNELS];
static const temp_conf_t EEMEM temp_conf_E[TEMP_CHANNELS] = {
        {
                .R_0 = 0x0c0000,
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
                        .gain_lin = 20000,
                        .gain_int = 2000,
                },
        },
/*        {
                .R_0 = 0x0a0000,
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
                        .gain_lin = 20000,
                        .gain_int = 2000,
                },
        },*/
};

temp_1_20_t temp_slop2public(uint8_t slop)
{
        return (slop * 4);
}

uint8_t temp_slop2internal(temp_1_20_t T)
{
        return (T + 2) / 4;
}

static temp_data_IU_t temp_meas_IU(uint8_t channel)
{
	temp_data_IU_t data_IU;
	int32_t I = 0, U = 0;
	uint8_t Ichannel, Uchannel;
	uint8_t spi_dev_num;
	const temp_dev_AD_t *dev_AD_E;

	dev_AD_E = &temp_conf_E[channel].dev_AD;
	spi_dev_num = eeprom_read_byte(&dev_AD_E->spi_dev_num);
	Ichannel = eeprom_read_byte(&dev_AD_E->Ichannel);
	Uchannel = eeprom_read_byte(&dev_AD_E->Uchannel);
	
        SPI_dev_select(spi_dev_num);
#define DAQ_SAMPLES 4
#define DAQ_READS 64
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

/* Make correction for I nad U from acqusition */
static temp_data_IU_t temp_correct_IU(uint8_t channel, temp_data_IU_t data_IU)
{
	int8_t offs;
        const temp_correct_IU_t *correct_IU_E;

	correct_IU_E = &temp_conf_E[channel].correct_IU;
	offs = eeprom_read_byte((uint8_t *)&correct_IU_E->U_offs);
	data_IU.I -= offs;
	offs = eeprom_read_byte((uint8_t *)&correct_IU_E->I_offs);
	data_IU.U -= offs;

	return data_IU;
}

static void temp_output_DA(uint8_t channel, uint16_t output)
{
        uint8_t spi_dev_num;
        uint8_t channel_dev_DA;
        uint8_t DA_width;
	const temp_dev_DA_t *dev_DA_E;
        uint16_t round;

        /* At AD output must be allways voltage, in order to measure 
         * resistence, detect shortcut or disconnection. */
        if (output < TEMP_OUTP_DA_MIN) {
                output = TEMP_OUTP_DA_MIN;
                pic16_reset(&temp_data[channel].pic, output);
        }

	dev_DA_E = &temp_conf_E[channel].dev_DA;
	spi_dev_num = eeprom_read_byte(&dev_DA_E->spi_dev_num);
	channel_dev_DA = eeprom_read_byte(&dev_DA_E->channel);
        
        SPI_dev_select(spi_dev_num);
        DA_width = SPI_dev_DA_width();
        round = 0x8000 >> DA_width;
        /* Rounding */
        round += output;
        /* Not overflow */
        if(round >= output)
                output = round;
        output >>= 16 - DA_width;
        SPI_dev_DA_set_output(channel_dev_DA, output);
}

/* TODO: DAQ data status */
static temp_status_IU_t temp_status_IU(temp_data_IU_t data_IU)
{
#define IMAX ((0x7fff - 0x7f))
#define IMIN 2048
#define UMAX ((0x7fff - 0x7f))
#define UMIN 4096

#define I_OVER 1
#define I_UNDER 2
#define U_OVER 4
#define U_UNDER 8
        temp_status_IU_t status;
        status = temp_status_IU_valid;
        uint8_t status_ = 0;

        if (data_IU.I > IMAX)
                status_ = I_OVER;
        else if (data_IU.I < IMIN)
                status_ = I_UNDER;

        if (data_IU.U > UMAX)
                status_ |= U_OVER;
        else if (data_IU.U < UMIN)
                status_ |= U_UNDER;
       
        if (status_ == (I_UNDER))
                status = temp_status_IU_disconnected;
        else if (status_ == (I_UNDER | U_UNDER))
                status = temp_status_IU_shortcut;
        else if (status_)
                status = temp_status_IU_invalid;

        return status;
}

static FP_2_14_t temp_calc_R(uint8_t channel, temp_data_IU_t data_IU)
{
        uint64_t tmp64;
        uint32_t R, R_0, R_slope;

        /* R = U * R_slope / I;
         * R_norm = R / R_0; */
        R_0 = eeprom_read_dword(&temp_conf_E[channel].R_0);
        R_slope = eeprom_read_dword(&temp_conf_E[channel].R_slope);

        tmp64 = math_mul_32_32_r64(R_slope, data_IU.U);
        R = math_div_64_32_r32(tmp64, data_IU.I);
        /* conversion to FP_2_14_t is done by >> 2 */
        tmp64 = math_mul_32_32_r64(R, 0x10000 >> 2);
        R = math_div_64_32_r32(tmp64, R_0);
        if (R > 0xffff)
                R = 0xffff;
        return R;
}

static void temp_loop_(uint8_t channel)
{
        int32_t e;
        uint16_t output;
        FP_2_14_t R, R_want;
        temp_data_IU_t data_IU;
        temp_status_IU_t status_IU;

        TIMER1_jiff_alarm(TIMER1_ALARM_MEAS);
        data_IU = temp_meas_IU(channel);
        status_IU = temp_status_IU(data_IU);
        if (status_IU != temp_status_IU_valid) {
                temp_output_DA(channel, 0);
                temp_data[channel].status = status_IU;
                return;
        }

        data_IU = temp_correct_IU(channel, data_IU);
        R = temp_calc_R(channel, data_IU);

        /* TODO: check R - emp_data[channel].R < K */
        if (temp_data[channel].status == temp_status_IU_disconnected ||
                        temp_data[channel].status == temp_status_IU_invalid ||
                        temp_data[channel].status == temp_status_IU_shortcut) {
                temp_data[channel].status = temp_status_IU_stabilizing;
                return;
        } else if (temp_data[channel].status == temp_status_IU_stabilizing) {
                temp_data[channel].status = temp_status_IU_valid;
                temp_data[channel].T_want = Pt_RtoT(R);
        }

        if (SCPI_OPER.condition & SCPI_OPER_SWE) {
                if (temp_data[channel].mode == temp_mode_fix) {
                        temp_1_20_t T_dest, T_want, T_want_old;
                        uint8_t T_slope;

                        T_dest = temp_data[channel].T_dest;
                        T_want_old = T_want = temp_data[channel].T_want;
                        T_slope = temp_data[channel].T_slope;

                        if (T_dest < T_want) {
                                T_want -= T_slope;
                                /* if (numeric underflow || ...) */
                                if (T_want > T_want_old || T_want < T_dest)
                                        T_want = T_dest;
                        } else {
                                T_want += T_slope;
                                if (T_want < T_want_old || T_want > T_dest)
                                        T_want = T_dest;
                        }

                        temp_data[channel].T_want = T_want;

                        /* FIXME: stop sweping */
                        if (T_dest == T_want)
                                temp_data[channel].status = temp_status_IU_valid;

                } else if (temp_data[channel].mode == temp_mode_list) {
                        /* TODO: ... */
                } else if (temp_data[channel].mode == temp_mode_prog) {
                        /* TODO: ... */
                }
        }
        temp_data[channel].R = R;
        
        R_want = Pt_TtoR(temp_data[channel].T_want);
        e = (int32_t)R_want - R;
        if (e > INT16_MAX)
                e = INT16_MAX;
        if (e < INT16_MIN)
                e = INT16_MIN;

        pic16(&temp_data[channel].pic, e, &temp_conf_E[channel].pic);

        TIMER1_jiff_alarm(TIMER1_ALARM_SEND);
        output = temp_data[channel].pic.output >> 16;
        temp_output_DA(channel, output);
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

uint16_t temp_dwel_get(uint8_t channel)
{
        return temp_data[channel].dwel;
}

void temp_dwel_set(uint8_t channel, uint16_t dwel)
{
        temp_data[channel].dwel = dwel;
}

temp_1_20_t temp_get(uint8_t channel)
{
        temp_1_20_t T;

        T = Pt_RtoT(temp_data[channel].R);
        return T;
}

void temp_init(void)
{
        for(uint8_t channel = TEMP_CHANNELS; channel;)
        {
                channel--;
                temp_output_DA(channel, 0);
                temp_data[channel].T_slope = 1 * 20 / 4; /* 1°C/sec */
                temp_data[channel].dwel = 60;
        }
}

temp_mode_t temp_mode_get(uint8_t channel)
{
        return temp_data[channel].mode;
}

void temp_mode_set(uint8_t channel, temp_mode_t mode)
{
        temp_data[channel].mode = mode;
}

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

temp_1_20_t temp_slope_get(uint8_t channel)
{
        return temp_slop2public(temp_data[channel].T_slope);
}

void temp_slope_set(uint8_t channel, temp_1_20_t slope)
{
        temp_data[channel].T_slope = temp_slop2internal(slope);
}

void temp_trg(void)
{
        if (SCPI_OPER.condition & SCPI_OPER_SWE) {
                SCPI_err_set(&SCPI_err_210);
                return;
        }
        SCPI_OPER_cond_set(SCPI_OPER_SWE);
}

temp_1_20_t temp_want_get(uint8_t channel)
{
        return temp_data[channel].T_dest;
}

void temp_want_set(uint8_t channel, temp_1_20_t T)
{
        temp_data[channel].T_dest = T;
        if (temp_data[channel].status == temp_status_IU_valid) {
                temp_data[channel].T_want = Pt_RtoT(temp_data[channel].R);
        }
}

