#ifndef __TEMP_H__
#define __TEMP_H__

#include "lib/pic16.h"
#include "lib/thermometer_pt.h"

#define TEMP_CHANNELS 2

typedef struct {
	uint16_t I;
	uint16_t U;
} temp_data_IU_t;

void temp_init(void);
temp_1_20_t temp_get(uint8_t channel);
temp_data_IU_t temp_meas_IU(uint8_t channel);
pic16_param_t temp_pic_params_get(uint8_t channel);
void temp_pic_params_set(uint8_t channel, pic16_param_t pic_param);
void temp_loop(void);
temp_1_20_t temp_want_get(uint8_t channel);
void temp_want_set(uint8_t channel, temp_1_20_t T);

#endif

