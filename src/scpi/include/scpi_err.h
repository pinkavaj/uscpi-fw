#ifndef __SCPI_ERR_H__
#define __SCPI_ERR_H__

typedef struct SCPI_err_struct {
	const char *str_P;
	uint8_t SES_P;
} SCPI_err_t;

extern const SCPI_err_t SCPI_err_0; 
extern const SCPI_err_t SCPI_err_100;
extern const SCPI_err_t SCPI_err_102;
extern const SCPI_err_t SCPI_err_103;
extern const SCPI_err_t SCPI_err_104;
extern const SCPI_err_t SCPI_err_108;
extern const SCPI_err_t SCPI_err_109;
extern const SCPI_err_t SCPI_err_110;
extern const SCPI_err_t SCPI_err_112;
extern const SCPI_err_t SCPI_err_113;
extern const SCPI_err_t SCPI_err_114;
extern const SCPI_err_t SCPI_err_171;
extern const SCPI_err_t SCPI_err_220;
extern const SCPI_err_t SCPI_err_222;
extern const SCPI_err_t SCPI_err_223;
extern const SCPI_err_t SCPI_err_321;
extern const SCPI_err_t SCPI_err_361;
extern const SCPI_err_t SCPI_err_362;
extern const SCPI_err_t SCPI_err_363;

void SCPI_err_queue_reset(void);
void SCPI_err_set(const SCPI_err_t *e);

#endif

