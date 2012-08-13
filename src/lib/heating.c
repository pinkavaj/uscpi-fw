
#include <inttypes.h>
#include "lib/heating.h"
#include "drivers/spi_dev.h"
#include "drivers/timer.h"

typedef struct {
	uint16_t Icode;
	uint16_t Ucode;
} heating_daq_t;

heating_daq_t heating_DAQ(uint8_t channel)
{
	heating_daq_t daq_data;

	daq_data = daq_data;
	channel = channel;
	uint32_t Icode = 0;
	uint32_t Ucode = 0;

	uint8_t chA = channel * 2;
	uint8_t chB = chA + 1;
	
	SPI_dev_select(SPI_DEV_AD974_0);
#define SAMPLES 8
#define DAQ_READS 16
	uint16_t i =  DAQ_READS;
	TIMER1_delay_rel_reset();
	do {
		Icode += SPI_dev_AD_get_sample(chA);
		Ucode += SPI_dev_AD_get_sample(chB);
		TIMER1_delay_rel(TIMER1_TICKS_PER_PLC / DAQ_READS);
	} while (--i);

	daq_data.Icode = Icode / (SAMPLES * DAQ_READS);
	daq_data.Ucode = Ucode / (SAMPLES * DAQ_READS);
#undef DAQ_READS
#undef SAMPLES

	return daq_data;
}

