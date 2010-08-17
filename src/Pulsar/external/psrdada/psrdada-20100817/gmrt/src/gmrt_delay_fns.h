
#include <inttypes.h>

#include "gmrt_newcorr.h"

#ifndef __gmrt_delay_fns_h
#define __gmrt_delay_fns_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct delay_vals
{
	double   delay_t0;
	double   dd_t0;
} delay_vals_t;

typedef struct fringe_vals
{
	double phase_ti;
	double dphase_t0;
	double ddphase_t0;
} fringe_vals_t;

int read_antenna_file(const char * antsys_hdr, AntennaParType *antenna);

int read_antenna_connectivity(const char * sampler_hdr_file, CorrType * corr);

int read_source_file(const char * source_hdr_file, SourceParType *source, 
										 double * BW, char * source_name);

int calculate_delays(SourceParType *source, CorrType * corr, struct timeval timestamp, 
										 delay_vals_t * delays, fringe_vals_t * fringes );

void print_vals(delay_vals_t * delays, fringe_vals_t * fringes);

#ifdef __cplusplus
}
#endif

#endif
