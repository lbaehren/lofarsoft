#include "fitsio.h"

#define FITS_format                  7
#define MEMORY_format                99
#define MaxOutputNameLength 10000
#define MaxNrApplicationFilenames 1025

typedef struct 
{
  FILE *fptr, *fptr_hdr;
  fitsfile *fits_fptr;        /* FITS file pointer, defined in fitsio.h */  
  int format, opened_flag, enable_write_flag, dd_mode, NrBits;
  char filename[1000];
  long NrPulses;
  long NrBins, NrPols, nrFreqChan;
  float Period, SampTime, freq_cent, bw, ra, dec, dm, channelbw;   /* Negative period means not folded */
  float *scales, *offsets, *weights, *data;
  long double mjd;
  char psrname[20], observatory[100], institute[100], instrument[100], scanid[1000];
  long long datastart;      /* undefined in FITS */
  int nrpapoints;
  float *data_pa, *data_dpa;
  int longitudes_defined;
  float *data_long;
  int bins_defined;
  long int *data_bin;
  float RMSI, RMSL, RMSV;
  /* Following for FITS files */
  int fd_type;        /* +1 is LIN:A=X,B=Y, +2 is CIRC:A=L,B=R (I)  (or -1,-2 to change handiness) 0=undefined*/
  float fd_sang, fd_xyph;   /* in degrees */ 
  int poltype;        /* 0 = undefined 1=STOKES, 2=AABBCRCI */
}datafile_definition;

typedef struct {
  char progname[100], genusage[100];
  int switch_headerlist, switch_header, switch_ext, switch_verbose;
  int switch_output, switch_output2, verbose;
  char *extension;
  char outputname[MaxOutputNameLength], pgplotdevice[MaxOutputNameLength];
}patrickSoftApplication;

