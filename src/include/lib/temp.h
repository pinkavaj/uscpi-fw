#ifndef __HEATING_H__
#define __HEATING_H__

typedef struct {
	uint16_t I;
	uint16_t U;
} temp_daq_data_t;

temp_daq_data_t temp_meas_IU_raw(uint8_t channel);

#endif

