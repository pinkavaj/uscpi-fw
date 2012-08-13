#ifndef __THERMOMETER_PT_H__
#define __THERMOMETER_PT_H__

#include <inttypes.h>
#include "lib/math_cust.h"

// Temperature with precision 1/20 °C
typedef uint16_t temp_1_20_t;

FP_2_14_t Pt_TtoR(temp_1_20_t T);
temp_1_20_t Pt_RtoT(FP_2_14_t R);

#endif
