
#include <inttypes.h>
#include "lib/heating.h"

typedef struct {
	uint16_t Icode;
	uint16_t Ucode;
} heating_daq_t;

heating_daq_t heating_DAQ(uint8_t channel)
{
	heating_daq_t daq_data;

	daq_data = daq_data;
	channel = channel;
/*	uint32_t Icode = 0;
	uint32_t Ucode = 0;

	uint8_t chA = chan_H * 2;
	uint8_t chB = chA + 1;
	
#define SAMPLES 8
	TIMER1_rel_delay_reset();
	uint16_t i = DAQ_READS;
	do {
		ad974_get_samples(chA, &Icode, SAMPLES);
		ad974_get_samples(chB, &Ucode, SAMPLES);
		TIMER1_rel_delay_wait(TIMER1_TICKS_PER_JIFFER);
	} while (--i);

	daq.Icode = Icode / (SAMPLES * DAQ_READS);
	daq.Ucode = Ucode / (SAMPLES * DAQ_READS);
#undef SAMPLES*/

	return daq_data;
}

