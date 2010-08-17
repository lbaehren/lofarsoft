
#define _PROFILE
//#define _DEBUG

#include "gmrt_delay_fns.h"


#ifndef __gmrt_cuda_h
#define __gmrt_cuda_h

int gmrt_corr_cuda_create_events(gmrt_corr_profile_t * p);

int gmrt_corr_cuda_alloc(gmrt_corr_data_t * d, gmrt_corr_params_t p);

float gmrt_corr_cuda_run(gmrt_corr_data_t * d, gmrt_corr_params_t * p, gmrt_corr_profile_t * prof,
                         SourceParType *source, CorrType * corr);

int gmrt_corr_cuda_dealloc(gmrt_corr_data_t * d, gmrt_corr_params_t p);

#endif
