#ifndef _SCPI_CMD_H_
#define _SCPI_CMD_H_

//#include <avr/pgmspace.h>
#include <inttypes.h>
#include "key.h"
#include "scpi.h"

#define _SCPI_branch_END_ { .key_P = NULL, .cmd_P = NULL, .branch_P = NULL, }

/* 
 * get - command has query form (keyword followed by '?')
 * get_has_params - if 1 get (questionalble form) may have parameters
 * 	0 - questionable form does not accept parameters
 * set - command has set form
 * set_params_atonce - call function when all parameters parsing is finished
 * set_params_min - minimal amount of parameters required for function
 * set_params_opt - number of optional parameters, total maximal amout of
 * 	parameter allowed if set_params_min + set_params_opt
 * instruments - count of instruments  - 1, sufix is number
 * 	in range <1,N> and for internal storage are mapped to range <0,N-1>
 * parser - function to set as parser if keyword found or NULL for default
 * */
typedef struct SCPI_cmd_struct {
	const uint8_t get_P;
	const uint8_t get_has_params_P;
	const uint8_t set_P;
	const uint8_t set_params_atonce_P;
	const uint8_t set_params_min_P;
	const uint8_t set_params_opt_P;
	uint8_t num_suffix_max_P;
	SCPI_parse_t(*parser_P)(char c);
} SCPI_cmd_t;

/*
 * key - SCPI keyword for this item
 * func - SCPI function for this keyword or NULL
 * branch - sub-branch for this keyword or NULL
 * */
struct SCPI_branch_item_struct;
typedef struct SCPI_branch_item_struct SCPI_branch_item_t;
struct SCPI_branch_item_struct {
	const SCPI_key_t const *key_P;
	const SCPI_cmd_t const *cmd_P;
	const SCPI_branch_item_t const *branch_P;
};

#endif

// :set tabstop=8 softtabstop=8 shiftwidth=8 noexpandtab
