#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

/* for debuging only */
#include <avr/io.h>

#define DEVICE_IMPA444_ASU 1
#define DEVICE_MSA_HCU814 2
#define DEVICE_TAN_ASU 3

/* Select device and aparatus */
//#define DEVICE_NAME DEVICE_IMPA444_ASU
//#define DEVICE_NAME DEVICE_MSA_HCU814
#define DEVICE_NAME DEVICE_TAN_ASU

/* If defined SCPI is build including tests */
#define SCPI_TEST

/* Configuration for IMPA444_ASU */
#if (DEVICE_NAME == DEVICE_IMPA444_ASU)
#define INFO_MODEL ASU-2H15-4L14-6V

/* Configuration for MSA_HCU814 */
#elif (DEVICE_NAME == DEVICE_MSA_HCU814)
#define INFO_MODEL HCU-8H14

/* Configuration for TAN_ASU */
#elif (DEVICE_NAME == DEVICE_TAN_ASU)
#define INFO_MODEL TAN-ASU

#else
#error "No target device selected."
#endif

/* Common */
#define F_CPU 11059200

#define VER_MAJ 0
#define VER_MINOR 0
#define VER_REV 0
#define VER_BUILD 811M
#define INFO_AUTHOR Lukas-Kucera_Jiri-Pinkava
#define INFO_COMPANY VSCHT
#ifndef INFO_MODEL
#define INFO_MODEL M162board-dev00
#endif

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
