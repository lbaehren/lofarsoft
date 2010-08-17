/* Program for offline processing -- Main front end program, reading the recorded data
   				  -- Written by Jayanta Roy (Last modified on 31 aug 2007)
   Integer Delay correction part is added on 04 oct 2007 --- Jayanta Roy
   Modified for Reading demultiplex splitted file ----- 11 Nov 2008 --- Jayanta Roy   
   IA and PA beam forming mode is added ---- 11 Nov 2008 by Jayanta Roy
   Dual pol is added ---- Jayanta Roy on 25 Dec 2008
   PA voltage stream recording is added ----- Jayanta Roy on 25 Dec 2008	 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <sys/time.h>
#include <time.h>
#include <sys/timeb.h>

#include "gmrt_delay_fns.h"
#include "gmrt_newcorr.h"

/*
 *  Calculate delays for the given antenna, source and timestamp
 */
int calculate_delays(SourceParType *source, CorrType * corr, struct timeval timestamp,
										 delay_vals_t * delays, fringe_vals_t * fringes )
{

	// imported from gmrt_newcorr.h
	float phase_ti[NUM_ACQ*NCHAN*FFTLEN/2];
	float dphase_t0[NUM_ACQ*NCHAN*FFTLEN/2];
	ModelParType mpar[MAX_SAMPS];

	FILE *fp_tstamp;
	int i, j, chan;
	char time_string[40], *time_ptr, str[100], *str_ptr, src_name[20];
	double mjd, ch_freq[FFTLEN/2];
	float src_ra, src_dec;
	double obs_freq, BW;
	double time_offset=0.0;
	int    int_time_offset=0;

	static int offset_delay[NCHAN*NUM_ACQ];
  static double delay_t0[NCHAN*NUM_ACQ],delay_ti[NCHAN*NUM_ACQ],dd_t0[NCHAN*NUM_ACQ];
  static double tm0;
  double tm1;
	float sign = 1.0;
	
	double time_ms = timestamp.tv_usec/1000000.000;
  struct tm * local_t = localtime(&timestamp.tv_sec);

#ifdef VERBOSE
	fprintf(stdout,"TIME is YYYY:MM:DD:HH:MIN:SEC = %d:%d:%d:%d:%d:%d.%d  \n",local_t->tm_year+1900,local_t->tm_mon+1,local_t->tm_mday,local_t->tm_hour,local_t->tm_min,local_t->tm_sec,(int)(time_ms*1000));
#endif

	for(chan=0;chan<FFTLEN/2;chan++)
	ch_freq[chan] = (BW/(FFTLEN/2))*chan + (sign*(BW/(FFTLEN/2))/2); // added half of spectral channel width to each channel, assuming USB.

	tm1 = local_t->tm_sec + local_t->tm_min*60 + local_t->tm_hour*3600 + time_ms;
	mjd = mjd_cal(local_t);
	corr->daspar.mjd_ref = mjd;
	calModelPar(tm1, corr, mpar, source, &(corr->daspar.antmask));
	tm0 = tm1;
	for(i=0;i<NCHAN*NUM_ANT;i++)
  {
		delay_t0[i] = mpar[i].delay;         // this is for array of structs
		dd_t0[i] = mpar[i].dd;
	}
  for(i=0;i<NCHAN*NUM_ANT;i++) 
	{
		delay_ti[i] = delay_t0[i] + (tm1-tm0)*dd_t0[i];
		delay_t0[i] = delay_ti[i];
		offset_delay[i] = (delay_ti[i])/SAMP_CLK; // DEALY wrt C02
		delay_ti[i] = delay_ti[i] - offset_delay[i]*SAMP_CLK;

		for(j=0;j<FFTLEN/2;j++)
		{
			phase_ti[i*FFTLEN/2+j] = (180.0/M_PI)*fmod((2.0*M_PI*(1-fmod((delay_t0[i]*(source->freq[0])*sign+delay_ti[i]*ch_freq[j]),1.0))),2.0*M_PI);
			dphase_t0[i*FFTLEN/2+j] = (180.0/M_PI)*fmod((2.0*M_PI*dd_t0[i]*(source->freq[0]*sign+ch_freq[j])),2.0*M_PI);
		}

		delays[i].delay_t0 = delay_t0[i];
		delays[i].dd_t0    = dd_t0[i];
	
		fringes[i].phase_ti   = phase_ti[i*FFTLEN/2];
		fringes[i].dphase_t0  = dphase_t0[i*FFTLEN/2];
		fringes[i].ddphase_t0 = (180.0/M_PI)*fmod(2.0*M_PI*(delay_ti[i]*ch_freq[1]),2.0*M_PI);

	}

	return 0;	
}


int read_antenna_file(const char * antsys_hdr_file, AntennaParType *antenna)
{

	FILE * fptr = fopen(antsys_hdr_file, "r");
	if (!fptr)
	{
		fprintf(stderr, "ERROR: could not open antsys header file [%s]\n", antsys_hdr_file);
		return -1;
	}

	get_antenna(fptr, antenna);
	fclose(fptr);
	return 0;
}

// Reading antenna connectivity 
int read_antenna_connectivity(const char * sampler_hdr_file, CorrType * corr)
{
	FILE * fptr = fopen(sampler_hdr_file,"r");
  if (!fptr)
  {
    fprintf(stderr, "ERROR: could not open sampler header file [%s]\n", sampler_hdr_file);
    return -1;
  }
  get_sampler(fptr, corr);
  fclose(fptr);
	return 0;
}

int read_source_file(const char * source_hdr_file, SourceParType *source, double * BW, char * source_name)
{

	char 	 str[100];
	char * str_ptr;

  FILE *fptr = fopen(source_hdr_file,"r");
	if (!fptr) 
	{
		fprintf(stderr, "ERROR: could not open source header file [%s]\n", source_hdr_file);
    return -1;
	}

  while(fgets(str,100,fptr))
  {
		str_ptr=str;
    while(*str_ptr == '*') str_ptr++;
    if (strncmp(str_ptr, "sou_name", 8) == 0) continue;
    if (*str_ptr == '#') continue;
    else 
		{
      sscanf(str_ptr,"%s", source_name);
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", &(source->ra_app));
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", &(source->dec_app));
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", &(source->freq[0]));
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", BW);
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", &(source->first_lo[0]));
      while(*str_ptr++ != ' ');
      while(isspace(*str_ptr)) str_ptr++;
      sscanf(str_ptr,"%lf", &(source->bb_lo[0]));
      break;
    }
  }
  fclose(fptr);

#ifdef VERBOSE
	fprintf(stdout,"SRC %s Source RA is %f DEC is %f \n",source_name,source->ra_app,source->dec_app);
  fprintf(stdout,"Observation RF freq is %lf, BW is %lf, LO1 is %lf and BB LO1 is %lf \n",
									source->freq[0],*BW, source->first_lo[0],source->bb_lo[0]);
#endif

	return 0;
}

void print_vals(delay_vals_t * delays, fringe_vals_t * fringes)
{
	unsigned i=0;

	fprintf(stdout, "=============================================\n");
	fprintf(stdout, "DELAYS\n");
	fprintf(stdout, "=============================================\n");
	for (i=0; i<NCHAN*NUM_ANT; i++)
	{
		fprintf(stdout, "%e %e\n",delays[i].delay_t0, delays[i].dd_t0);
	}

	fprintf(stdout, "=============================================\n");
	fprintf(stdout, "FRINGES\n");
	fprintf(stdout, "=============================================\n");
	for (i=0; i<NCHAN*NUM_ANT; i++)
	{
		fprintf(stdout, "%5.10lf %5.10lf %5.10lf \n",fringes[i].phase_ti, fringes[i].dphase_t0, fringes[i].ddphase_t0);
	}
}
