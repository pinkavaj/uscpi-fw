#ifndef __PIC_H__
#define __PIC_H__

#include <inttypes.h>

typedef struct {
	uint16_t output;
	int16_t gain_lin_e_old;
} pic16_data_t;

typedef struct {
	/* FP 0.16 */
	uint16_t gain_lin;
	uint16_t gain_int;
} pic16_param_t;

/*****************************************************************************/
void pic16(pic16_data_t *data, int16_t e, const pic16_param_t *param_E);
/*void pic_limit16(pic16_data_t *data, int16_t output);*/

#endif

