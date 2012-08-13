#ifndef __MATH_CUST_H__
#define __MATH_CUST_H__

#include <inttypes.h>

/* Various fixed point data types */
typedef uint32_t FP_16_16_t;
typedef uint16_t FP_0_16_t;
typedef uint16_t FP_2_14_t;

extern uint32_t math_div_64_32_r32 (uint64_t, uint32_t);
extern uint64_t math_mul_32_32_r64 (uint32_t, uint32_t);

#endif

