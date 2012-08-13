#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

/* for debuging only */
#include <avr/io.h>

#define F_CPU 11059200

#define VER_MAJ 0
#define VER_MINOR 0
#define VER_REV 0
#define VER_BUILD 797M
#define INFO_AUTHOR Lukas-Kucera_Jiri-Pinkava
#define INFO_COMPANY VSCHT
#ifndef INFO_MODEL
#define INFO_MODEL M162board-dev00
#endif

/* If defined SCPI is build including tests */
#define SCPI_TEST

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
