#ifndef __TEMP_H__
#define __TEMP_H__

#include "lib/pic16.h"
#include "lib/thermometer_pt.h"

#define TEMP_CHANNELS 2

typedef struct {
	uint16_t I;
	uint16_t U;
} temp_data_IU_t;

typedef enum {
        temp_mode_fix = 0,
        temp_mode_list = 1,
        temp_mode_prog = 2,
} temp_mode_t;

void temp_init(void);
void temp_loop(void);

temp_1_20_t temp_get(uint8_t channel);
temp_data_IU_t temp_meas_IU(uint8_t channel);
temp_mode_t temp_mode_get(uint8_t channel);
void temp_mode_set(uint8_t channel, temp_mode_t mode);
pic16_param_t temp_pic_params_get(uint8_t channel);
void temp_pic_params_set(uint8_t channel, pic16_param_t pic_param);
temp_1_20_t temp_want_get(uint8_t channel);
void temp_want_set(uint8_t channel, temp_1_20_t T);

#endif

