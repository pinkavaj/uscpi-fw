#ifndef __pt_RT_table_H__
#define __pt_RT_table_H__

#include <inttypes.h>

typedef uint16_t fix_2_14;
typedef uint16_t fix_10_6;

fix_2_14 pt_R(fix_10_6 T);
fix_10_6 pt_T(fix_2_14 R);

#endif

