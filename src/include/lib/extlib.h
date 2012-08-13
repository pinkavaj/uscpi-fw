#ifndef __EXTLIB_H__
#define __EXTLIB_H__

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

