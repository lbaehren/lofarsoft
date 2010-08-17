#ifndef __GMRT_corr_dada_h
#define __GMRT_corr_dada_h

#include "dada_hdu.h"
#include "ascii_header.h"

#ifdef __cplusplus
extern "C" {
#endif

int gmrt_corr_get_header(dada_hdu_t * hdu);
int64_t gmrt_corr_dada_ipcio_read(dada_hdu_t * hdu, char * address, uint64_t size);

#ifdef __cplusplus
}
#endif

#endif
