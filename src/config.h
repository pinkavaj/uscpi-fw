#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

/* for debuging only */
#include <avr/io.h>

/* Select device and aparatus */
//#define DEVICE_NAME IMPA444_ASU
#define DEVICE_NAME MSA_HCU814

/* Configuration for IMPA444_ASU */
#if DEVICE_NAME == IMPA444_ASU
#define INFO_MODEL ASU-2H15-4L14-6V

/* Configuration for MSA_HCU814 */
#elif DEVICE_NAME == MSA_HCU814
#define INFO_MODEL HCU-814

#endif

/* Common */
#define F_CPU 11059200

#define VER_MAJ 0
#define VER_MINOR 0
#define VER_REV 0
#define VER_BUILD 799M
#define INFO_AUTHOR Lukas-Kucera_Jiri-Pinkava
#define INFO_COMPANY VSCHT
#ifndef INFO_MODEL
#define INFO_MODEL M162board-dev00
#endif

/* If defined SCPI is build including tests */
#define SCPI_TEST

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
