/*****************************************************************************
 *     ***  PIC - PI Controller  ***
 *
 * Copyright (c) 2010 Lukas Kucera <lukas.kucera@vscht.cz>, 
 * 	Jiri Pinkava <jiri.pinkava@vscht.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *****************************************************************************/

#include <avr/eeprom.h>

#include "lib/pic16.h"
#include "lib/math_cust.h"

/*****************************************************************************/
void pic16(pic16_data_t *data, int16_t e, const pic16_param_t *param_E)
{
	int32_t gain_term32, int_term32, gain_term_old;
	register uint32_t output32;
	register int8_t outputHi = 0;
	pic16_param_t param;
	
	/* implement equation: 
	 * output += gain_term32 - data->gain_term_old + int_term32 */
	param.gain_lin = eeprom_read_word(&param_E->gain_lin);
	/* FIXME: result is int32_t */
	gain_term32 = e * param.gain_lin;
	output32 = data->output;
	asm (
		"sbrc	%D0, 7	\n"	/* check for sign */
		"com	%A1	\n" 	/* if negative, extend with 0xff */
		"mov	__tmp_reg__, __zero_reg__	\n"
		"sbrc	%D2, 7		\n"	/* check for sign */
		"com	__tmp_reg__	\n" 	/* if negative extend with 0xff */
		"add	%A0, %A2	\n"	/* output32 += gain_term32 */
		"adc	%B0, %B2	\n"
		"adc	%C0, %C2	\n"
		"adc	%D0, %D2	\n"
		"adc	%A1, __tmp_reg__	\n"	/* outputHi += Carry */
		: "+r" (output32), "+r" (outputHi)
		: "r" (gain_term32)
	    );

	param.gain_int = eeprom_read_word(&param_E->gain_int);
	/* FIXME: result is int32_t */
	int_term32 = e * param.gain_int;
	asm (
		"mov	__tmp_reg__, __zero_reg__	\n"
		"sbrc	%D2, 7		\n"	/* check for sign */
		"com	__tmp_reg__	\n" 	/* if negative extend with 0xff */
		"add	%A0, %A2	\n"	/* output32 += int_term32 */
		"adc	%B0, %B2	\n"
		"adc	%C0, %C2	\n"
		"adc	%D0, %D2	\n"
		"adc	%A1, __tmp_reg__	\n"	/* outputHi += Carry */
		: "+r" (output32), "+r" (outputHi)
		: "r" (int_term32)
	    );

	gain_term_old = data->gain_lin_e_old;
	asm (
		"mov	__tmp_reg__, __zero_reg__	\n"
		"sbrc	%D2, 7		\n"	/* check for sign */
		"com	__tmp_reg__	\n" 	/* if negative extend with 0xff */
		"sub	%A0, %A2	\n"	/* output32 -= gain_term_old */
		"sbc	%B0, %B2	\n"
		"sbc	%C0, %C2	\n"
		"sbc	%D0, %D2	\n"
		"sbc	%A1, __tmp_reg__	\n"	/* outputHi -= Carry */
		: "+r" (output32), "+r" (outputHi)
		: "r" (gain_term_old)
	    );

	if (outputHi != 0) {
		if (outputHi < 0)
			output32 = 0;
		else
			output32 = UINT32_MAX;

		gain_term32 = 0;
	}
	
	data->gain_lin_e_old = gain_term32;
	data->output = output32;
}


