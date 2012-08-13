#ifndef __PIC16_H__
#define __PIC16_H__

#include <inttypes.h>
#include "lib/math_cust.h"

typedef struct {
	FP_16_16_t output;
	int32_t gain_lin_e_old;
} pic16_data_t;

typedef struct {
	FP_0_16_t gain_lin;
	FP_0_16_t gain_int;
} pic16_param_t;

/*****************************************************************************/
void pic16(pic16_data_t *data, int16_t e, const pic16_param_t *param_E);
/*void pic_limit16(pic16_data_t *data, int16_t output);*/

#endif

