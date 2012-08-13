#ifndef __CONFIG__
#define __CONFIG__

#include <stdint.h>

/* for debuging only */
#include <avr/io.h>

#define VER_MAJ 0
#define VER_MINOR 0
#define VER_REV 0
#define VER_BUILD 503M
#define INFO_AUTHOR Lukas-Kucera_Jiri-Pinkava
#define INFO_COMPANY VSCHT
#ifndef INFO_MODEL
#define INFO_MODEL M162board-dev00
#endif

/* ancillyary macros */
#ifndef NULL
#define NULL 0
#endif

/* Allow marking function parameters as unused to quaiet warnings, this
 * also force that this parameter really cannot be used */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#endif

/* :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab */
