#include "lib/list.h"

list_t list[TEMP_CHANNELS][LIST_MAX];
uint8_t list_coun[TEMP_CHANNELS];
uint8_t list_temp_poin[TEMP_CHANNELS];

void list_init(void)
{
        for(uint8_t channel = TEMP_CHANNELS; channel; )
                list_coun[--channel] = 1;
}

