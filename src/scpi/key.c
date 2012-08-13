#include "config.h"
#include "key.h"

#define SHORT3B 3
#define SHORT4B 4
#define _key_(kw, key, slen) \
        __extension__ const SCPI_key_t key_ ## kw ## _P PROGMEM = \
        { .len_short_P = slen, .keyword_P = key, }

/* SCPI Common Commands, list of keywords */
_key_(cls, "CLS", SHORT3B);
_key_(ese, "ESE", SHORT3B);
_key_(esr, "ESR", SHORT3B);
_key_(idn, "IDN", SHORT3B);
_key_(opc, "OPC", SHORT3B);
_key_(rst, "RST", SHORT3B);
_key_(sre, "SRE", SHORT3B);
_key_(stb, "STB", SHORT3B);
_key_(tst, "TST", SHORT3B);
_key_(wai, "WAI", SHORT3B);

/* SCPI Instrument Commands, list of keywords */
_key_(clos, "CLOSE",        SHORT4B);
_key_(cond, "CONDITION",    SHORT4B);
_key_(cont, "CONTROL",      SHORT4B);
_key_(coun, "COUNT",        SHORT4B);
_key_(der,  "DERIVATIVE",   SHORT3B);
_key_(dwel, "DWELL",        SHORT4B);
_key_(enab, "ENABLE",       SHORT4B);
_key_(err,  "ERROR",        SHORT3B);
_key_(even, "EVENT",        SHORT4B);
_key_(fix,  "FIXED",        SHORT3B);
_key_(func, "FUNCTION",     SHORT4B);
_key_(gain, "GAIN",         SHORT4B);
_key_(inf,  "INFINITY",     SHORT3B);
_key_(int,  "INTEGRAL",     SHORT3B);
_key_(lcon, "LCONSTANTS",   SHORT4B);
_key_(list, "LIST",         SHORT4B);
_key_(meas, "MEASURE",      SHORT4B);
_key_(mode, "MODE",         SHORT4B);
_key_(next, "NEXT",         SHORT4B);
_key_(open, "OPEN",         SHORT4B);
_key_(oper, "OPERATION",    SHORT4B);
_key_(poin, "POINTS",       SHORT4B);
_key_(port, "PORT",         SHORT4B);
_key_(pres, "PRESET",       SHORT4B);
_key_(prog, "PROGRAM",      SHORT4B);
_key_(ques, "QUESTIONABLE", SHORT4B);
_key_(res,  "RESISTANCE",   SHORT3B);
_key_(rtim, "RTIME",        SHORT4B);
_key_(seq,  "SEQUENCE",     SHORT3B);
_key_(sens, "SENSE",        SHORT4B);
_key_(slop, "SLOPE",        SHORT4B);
_key_(sour, "SOURCE",       SHORT4B);
_key_(spo,  "SPOINT",       SHORT3B);
_key_(stat, "STATUS",       SHORT4B);
_key_(syst, "SYSTEM",       SHORT4B);
_key_(temp, "TEMPERATURE",  SHORT4B);
_key_(test, "TEST",         SHORT4B);
_key_(trg,  "TRG",          SHORT3B);
_key_(valv, "VALVE",        SHORT4B);
_key_(vers, "VERSION",      SHORT4B);

#ifdef SCPI_TEST
_key_(adc, "ADC",  SHORT3B);
_key_(dec, "DEC",  SHORT3B);
_key_(div, "DIV",  SHORT3B);
_key_(heat,"HEAT", SHORT4B);
_key_(mul, "MUL",  SHORT3B);
_key_(time,"TIME", SHORT4B);
#endif

uint8_t keycmp_P(const char *s, prog_SCPI_key_t key)
{
        uint8_t len;
        
        len = pgm_read_byte(&key->len_short_P);
        if (!strncmp_P(s, key->keyword_P, len)) {
                if (s[len] == '\0')
                        return 0;
        }

        return strcmp_P(s, key->keyword_P);
}

