#ifndef __EXTLIB_H__
#define __EXTLIB_H__

#define ARRAY_SIZE(array) ((sizeof(array) / sizeof(array[0])))

#ifndef NULL
#define NULL 0
#endif

/* Ancillyary macros to create string from something and join it together */
#define STR(s) STR_(s)
#define STR_(s) #s

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

