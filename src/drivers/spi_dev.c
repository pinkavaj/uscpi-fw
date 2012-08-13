#include "config.h"

#if (DEVICE_NAME == DEVICE_IMPA444_ASU)
#include "spi_dev_impa444_asu.c"
#elif (DEVICE_NAME == DEVICE_MSA_HCU814)
#include "spi_dev_msa_hcu814.c"
#elif (DEVICE_NAME == DEVICE_TAN_ASU)
#include "spi_dev_tan_asu.h"
#endif

