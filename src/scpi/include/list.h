#ifndef __LIST_H__
#define __LIST_H__

#include "temp.h"

#define LIST_COUN_INF 0
#define LIST_MAX 4

typedef struct {
        uint8_t slop;
        temp_1_20_t T_spo;
        uint16_t dwel;
} list_t;

extern list_t list[TEMP_CHANNELS][LIST_MAX];
/* How much times iterate trought list. */
extern uint8_t list_coun[TEMP_CHANNELS];
/* Number of points in list. */
extern uint8_t list_temp_poin[TEMP_CHANNELS];

void list_init(void);

#endif
