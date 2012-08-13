#ifndef __TEMP_H__
#define __TEMP_H__

#include "lib/pic16.h"

typedef struct {
	uint16_t I;
	uint16_t U;
} temp_daq_data_t;

temp_daq_data_t temp_meas_IU_raw(uint8_t channel);
pic16_param_t temp_pic_params_get(uint8_t channel);
void temp_pic_params_set(uint8_t channel, pic16_param_t pic_param);

#endif

