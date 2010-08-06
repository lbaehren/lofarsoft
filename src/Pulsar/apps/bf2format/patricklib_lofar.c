#include <math.h>
#include "fitsio.h"

int internal_applicationFilenameList[MaxNrApplicationFilenames];
int internal_applicationNrfilenames;
int internal_applicationCurFilename;

void internalFITSscalePulse(float *pulse, long nrSamples, float *offset, float *scale, float maxvalue)
{
  long i;
  /* Determine scaling to cast data into ints */
  *offset = pulse[0];
  for(i = 0; i < nrSamples; i++) {
    /*    fprintf(stderr, "XXXXXX %f\n", pulse[i]); */
    if(pulse[i] < *offset)
      *offset = pulse[i];
  }
  *scale = (pulse[0]-*offset)/maxvalue;
  for(i = 0; i < nrSamples; i++) {
    if((pulse[i]-*offset)/(*scale) > maxvalue)
      *scale = (pulse[i]-*offset)/maxvalue;
  }
  if(*scale == 0.0)
    *scale = 1;
  /*  fprintf(stderr, "  internalFITSscalePulse: scale=%e offset=%e maxvalue=%f\n", *scale, *offset, maxvalue); */
}


int lookupSubintTable(datafile_definition datafile)
{
  int ncols, status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  long nrows;
  if(fits_movnam_hdu(datafile.fits_fptr, BINARY_TBL, "SUBINT", 0, &status)) {
    fprintf(stderr, "ERROR lookupSubintTable: Cannot move to subint HDU.\n");
    return 0;
  } 
  fits_get_num_rows(datafile.fits_fptr, &nrows, &status);
  fits_get_num_cols(datafile.fits_fptr, &ncols, &status);
  /*  printf("FITS table = %ld x %d\n", nrows, ncols); */
  if(ncols < 19) {
    fprintf(stderr, "ERROR lookupSubintTable: Table has not the correct size, was writePSRFITSHeader not called?\n");
    return 0;
  }
  return 1;
}

void printHeaderCommandlineOptions(FILE *printdevice)
{
  fprintf(printdevice, "Valid options for the -header option are:\n");
  fprintf(printdevice, "  bw           band width.\n");
  fprintf(printdevice, "  chbw         channel band width.\n");
  fprintf(printdevice, "  dt           sampling time.\n");
  fprintf(printdevice, "  fdtype       1=lin 2=circ, sign is handiness.\n");
  fprintf(printdevice, "  freq         centre frequency.\n");
  fprintf(printdevice, "  mjd          MJD of the observation.\n");
  fprintf(printdevice, "  name         Name of the pulsar.\n");
  fprintf(printdevice, "  nrbins       Nr of time-bins (use at own risk).\n");
  fprintf(printdevice, "  nrbits       Nr of bits (use at own risk).\n");
  fprintf(printdevice, "  nrchan       Nr of frequency channels (use at own risk).\n");
  fprintf(printdevice, "  nrpulses     Nr pulses in observation (use at own risk).\n");
  fprintf(printdevice, "  observatory  Change the observatory.\n");
  fprintf(printdevice, "  p0           Period.\n");
  fprintf(printdevice, "  poltype      Polarization type.\n");
  fprintf(printdevice, "  sang         angle of receiver.\n");
  fprintf(printdevice, "  scan         Scan ID of the observation.\n");
  fprintf(printdevice, "  xyph         other angle of receiver.\n");
}

/* Automatically done in readHeaderPSRData if verbose is set */
void printHeaderPSRData(datafile_definition datafile, int ident)
{
  int i;
  for(i = 0; i < ident; i++) printf(" ");
  printf("NrPulses=%ld NrBins=%ld NrPols=%ld nrFreqChan=%ld NrBits=%d\n",datafile.NrPulses,datafile.NrBins,datafile.NrPols, datafile.nrFreqChan,datafile.NrBits);
  for(i = 0; i < ident; i++) printf(" ");
  printf("Period=%f SampTime=%f  mjd=%.9lf\n",datafile.Period,datafile.SampTime,datafile.mjd);
  for(i = 0; i < ident; i++) printf(" ");
  printf("freq_cent=%f bw=%f channelbw=%f\n",datafile.freq_cent,datafile.bw, datafile.channelbw);
  for(i = 0; i < ident; i++) printf(" ");
  printf("Header length = %ld bytes\n", (long)datafile.datastart);
  for(i = 0; i < ident; i++) printf(" ");
  printf("Pulsar=%s  Ra=%f  Dec=%f  dm=%f\n", datafile.psrname, datafile.ra, datafile.dec, datafile.dm);
  for(i = 0; i < ident; i++) printf(" ");
  printf("FileName=%s  ScanID=%s\n", datafile.filename, datafile.scanid);
  for(i = 0; i < ident; i++) printf(" ");
  printf("observatory=%s  institute=%s  instrument=%s\n", datafile.observatory, datafile.institute, datafile.instrument);
  for(i = 0; i < ident; i++) printf(" ");
  printf("fd_type=%d  fd_sang=%f  fd_xyph=%f  poltype=%d\n", datafile.fd_type, datafile.fd_sang, datafile.fd_xyph, datafile.poltype);
  for(i = 0; i < ident; i++) printf(" ");
  printf("paswing=");
  if(datafile.nrpapoints > 0)
    printf("YES");
  else
    printf("NO");
  printf("  longitudes=");
  if(datafile.longitudes_defined)
    printf("YES");
  else
    printf("NO");
  printf("  bins=");
  if(datafile.bins_defined)
    printf("YES\n");
  else
    printf("NO\n");
}

/* This function is hacked. Close the files and free the memory if required. */
int closePSRData(datafile_definition *datafile)
{
  int status = 0;
  if(datafile->opened_flag) {
    if(datafile->format == FITS_format) {
      fits_close_file(datafile->fits_fptr, &status);
      if(status) 
	fits_report_error(stderr, status); /* print any error message */
      free(datafile->scales);
      free(datafile->offsets);
      free(datafile->weights);
    }else if(datafile->format != MEMORY_format){
      fclose(datafile->fptr);
    }
    if(datafile->format == MEMORY_format) {
      free(datafile->data);
      if(datafile->nrpapoints > 0) {
	free(datafile->data_pa);
	free(datafile->data_dpa);
      }
      if(datafile->longitudes_defined)
	free(datafile->data_long);
      if(datafile->bins_defined) 
	free(datafile->data_bin);
    }
    datafile->opened_flag = 0;
    return status;
  }else {
    return 0;
  }
}

/* This function is hacked Returns NULL if the last file has been
  processed. */
char *getNextFilenameFromList(patrickSoftApplication *application, char **argv)
{
  if(internal_applicationCurFilename == internal_applicationNrfilenames)
    return NULL;
  return argv[internal_applicationFilenameList[internal_applicationCurFilename++]];
}


int writePSRFITSHeader(datafile_definition *datafile, int verbose)
{
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  char dummy_txt[2000], dummy_txt2[2000], dummy_txt3[2000];
  int dummy_int;
  float dummy_float;
  double dummy_double;
  long i, j;

  /* Create primary header */

  if(fits_create_img(datafile->fits_fptr, 8, 0, NULL, &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create primary table.\n");
    return 0;
  }

  sprintf(dummy_txt, "4.1");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "HDRVER", dummy_txt, "Header version", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  sprintf(dummy_txt, "PSRFITS");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "FITSTYPE", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  if(fits_write_key(datafile->fits_fptr, TSTRING, "TELESCOP", datafile->observatory, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  sprintf(dummy_txt, "PSR");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "OBS_MODE", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  if(fits_write_key(datafile->fits_fptr, TSTRING, "SRC_NAME", datafile->psrname, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  if(fits_write_key(datafile->fits_fptr, TSTRING, "BACKEND", datafile->instrument, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  if(datafile->fd_type == 1 || datafile->fd_type == -1) {
    sprintf(dummy_txt, "LIN");
  }else if(datafile->fd_type == 2 || datafile->fd_type == -2) {
    sprintf(dummy_txt, "CIRC");
  }
  if(datafile->fd_type != 0) {
    if(fits_write_key(datafile->fits_fptr, TSTRING, "FD_POLN", dummy_txt, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
    if(datafile->fd_type > 0) {
      dummy_int = 1;
    }else {
      dummy_int = -1;
    }
    if(fits_write_key(datafile->fits_fptr, TINT, "FD_HAND", &dummy_int, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
  }else {
    fprintf(stderr, "WARNING writePSRFITSHeader: fd_type is not set.\n");
  }
  if(fits_write_key(datafile->fits_fptr, TFLOAT, "FD_SANG", &datafile->fd_sang, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  if(fits_write_key(datafile->fits_fptr, TFLOAT, "FD_XYPH", &datafile->fd_xyph, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }


  if(fits_write_key(datafile->fits_fptr, TFLOAT, "OBSFREQ", &datafile->freq_cent, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  dummy_int = datafile->mjd;
  if(fits_write_key(datafile->fits_fptr, TINT, "STT_IMJD", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  dummy_int = (datafile->mjd - (float)dummy_int)*(24.0*3600.0);
  if(fits_write_key(datafile->fits_fptr, TINT, "STT_SMJD", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }

  dummy_int = datafile->mjd;
  dummy_float = (datafile->mjd - (float)dummy_int)*(24.0*3600.0);
  dummy_int = dummy_float;
  dummy_float -= dummy_int;

  if(fits_write_key(datafile->fits_fptr, TFLOAT, "STT_OFFS", &dummy_float, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  

  /* Create some other unused tables */

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "HISTORY", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "BANDPASS", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "COHDISP", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "PSRPARAM", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "T2PREDICT", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }
  

  /* Create the subint header */

  sprintf(dummy_txt, "%ldE", datafile->nrFreqChan);
  sprintf(dummy_txt2, "%ldE", datafile->nrFreqChan*datafile->NrPols);
  if(datafile->dd_mode == 0 && datafile->Period > 0) {
    sprintf(dummy_txt3, "%ldI", datafile->nrFreqChan*datafile->NrPols*datafile->NrBins);
  }else {   /* search mode */
    sprintf(dummy_txt3, "%ldb", datafile->nrFreqChan*datafile->NrPols*datafile->NrBins);
  }
  /* Instead of writing out a model, simply write periods in table. Works for PSRCHIVE. */
  char *ttypes[] = {"INDEXVAL", "TSUBINT", "OFFS_SUB", "LST_SUB", "RA_SUB", "DEC_SUB", "GLON_SUB", "GLAT_SUB", "FD_ANG", "POS_ANG", "PAR_ANG", "TEL_AZ", "TEL_ZEN", "DAT_FREQ", "DAT_WTS", "DAT_OFFS", "DAT_SCL", "DATA", "PERIOD"};
  char *tform[] = {"1D", "1D", "1D", "1D", "1D", "1D", "1D", "1D", "1E", "1E", "1E", "1E", "1E", dummy_txt, dummy_txt, dummy_txt2, dummy_txt2, dummy_txt3, "1D"};
  char *tunit[] = {"", "s", "s", "s", "deg", "deg", "deg", "deg", "deg", "deg", "deg", "deg", "deg", "MHz", "", "", "", "Jy", "s"};
  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, datafile->NrPulses, 19, ttypes, tform, tunit, "SUBINT", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }
  sprintf(dummy_txt, "TIME");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "INT_TYPE", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  sprintf(dummy_txt, "SEC");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "INT_UNIT", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  if(datafile->poltype == 1) {
    sprintf(dummy_txt, "STOKES");
  }else if(datafile->poltype == 2) {
    sprintf(dummy_txt, "AABBCRCI");
  }
  if(datafile->poltype != 0) {
    if(fits_write_key(datafile->fits_fptr, TSTRING, "POL_TYPE", dummy_txt, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
  }else {
    fprintf(stderr, "WARNING writePSRFITSHeader: poltype is not set.\n");
  }
  dummy_int = datafile->NrPols;
  if(fits_write_key(datafile->fits_fptr, TINT, "NPOL", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  dummy_float = datafile->SampTime;
  if(fits_write_key(datafile->fits_fptr, TFLOAT, "TBIN", &dummy_float, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  if(datafile->dd_mode == 0 && datafile->Period > 0) {
    dummy_int = datafile->NrBins;
    if(fits_write_key(datafile->fits_fptr, TINT, "NBIN", &dummy_int, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
  }else {   /* In search mode */
    dummy_int = 1;
    if(fits_write_key(datafile->fits_fptr, TINT, "NBIN", &dummy_int, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
    dummy_int = datafile->NrBins;
    if(fits_write_key(datafile->fits_fptr, TINT, "NSBLK", &dummy_int, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
    if(datafile->NrBits <= 0 || datafile->NrBits > 8) {
      fprintf(stderr, "WARNING writePSRFITSHeader: Assume you want to write out 8 bit search mode data.\n");
      datafile->NrBits = 8;
    }
    dummy_int = datafile->NrBits;
    if(fits_write_key(datafile->fits_fptr, TINT, "NBITS", &dummy_int, "", &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
  }
  dummy_int = datafile->nrFreqChan;
  if(fits_write_key(datafile->fits_fptr, TINT, "NCHAN", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  dummy_float = datafile->channelbw;
  if(fits_write_key(datafile->fits_fptr, TFLOAT, "CHAN_BW", &dummy_float, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  dummy_float = datafile->dm;
  if(fits_write_key(datafile->fits_fptr, TFLOAT, "DM", &dummy_float, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  for(i = 0; i < datafile->NrPulses; i++) {
    /* Instead of writing out a model, simply write fixed period in header. Works for PSRCHIVE. */
    dummy_double = datafile->Period;
    if(fits_write_col(datafile->fits_fptr, TDOUBLE, 19, i+1, 1, 1, &dummy_double, &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
    /* Set the subintegration time equal to the pule perid, i.e. assume single pulses were recorded. */
    if(fits_write_col(datafile->fits_fptr, TDOUBLE, 2, i+1, 1, 1, &dummy_double, &status) != 0) {
      fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
      return 0;
    }
    for(j = 0; j < datafile->nrFreqChan; j++) {
      dummy_float = datafile->freq_cent - 0.5*(datafile->nrFreqChan-1.0)*datafile->channelbw + j*datafile->channelbw;
      if(fits_write_col(datafile->fits_fptr, TFLOAT, 14, i+1, 1+j, 1, &dummy_float, &status) != 0) {
	fprintf(stderr, "ERROR writeFITSpulse: Error writing frequency.\n");
	fits_report_error(stderr, status); /* print any error message */
	return 0;
      }
      dummy_float = 1;
      if(fits_write_col(datafile->fits_fptr, TFLOAT, 15, i+1, 1+j, 1, &dummy_float, &status) != 0) {
	fprintf(stderr, "ERROR writeFITSpulse: Error writing weights.\n");
	fits_report_error(stderr, status); /* print any error message */
	return 0;
      }
    }
  }

  /* DIG_STAT table  */

  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "DIG_STAT", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }
  dummy_int = datafile->NrPulses*datafile->NrPols*datafile->nrFreqChan;
  if(fits_write_key(datafile->fits_fptr, TINT, "NLEV", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  sprintf(dummy_txt, "9-bit");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "DIG_MODE", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
    }
  dummy_int = 2;
  if(fits_write_key(datafile->fits_fptr, TINT, "NDIGR", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  dummy_int = 1;
  if(fits_write_key(datafile->fits_fptr, TINT, "NCYCSUB", &dummy_int, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }
  sprintf(dummy_txt, "AUTO");
  if(fits_write_key(datafile->fits_fptr, TSTRING, "DIGLEV", dummy_txt, "", &status) != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot write keyword.\n");
    return 0;
  }


  /*  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, datafile->NrPulses*datafile->NrPols*datafile->nrFreqChan, 3, NULL, NULL, NULL, "DIG_CNTS", &status)  != 0) { */
  
  if(fits_create_tbl(datafile->fits_fptr, BINARY_TBL, 0, 0, NULL, NULL, NULL, "DIG_CNTS", &status)  != 0) {
    fprintf(stderr, "ERROR writePSRFITSHeader: Cannot create table.\n");
    return 0;
  }


  /*  fits_close_file(datafile->fits_fptr, &status);
      return 0;*/
  return 1;
}

/* Hacked. */
int openPSRData(datafile_definition *datafile, char *filename, int format, int enable_write, int read_in_memory, int verbose)
{
  int status = 0, iomode;   /* CFITSIO status value MUST be initialized to zero! */
  char open_mode[100], filename2[1000];


  strcpy(datafile->filename, filename);
  datafile->opened_flag = 0;
  /*  printf("Opening format %d\n", format); */
  datafile->format = format;
  datafile->enable_write_flag = enable_write;


  datafile->format = format;



  open_mode[0] = 0;
  if(enable_write) {
    strcat(open_mode, "w");
    if(verbose) printf("Opening file '%s' for writing\n", filename);
  }else {
    strcat(open_mode, "r");
    if(verbose) printf("Opening file '%s' for reading\n", filename);
  }


  if(format == FITS_format) {
    /* Table 6 should be the subintegrations */
    if(enable_write) {
      iomode = READWRITE;
      /* exlamation mark caused file to be overwritten. */
      sprintf(filename2, "!%s", filename);
      if (!fits_create_file(&(datafile->fits_fptr), filename2, &status)) {
	if(verbose) printf("  '%s' opened\n", filename);
	datafile->opened_flag = 1;
      }else {
	datafile->opened_flag = 0;
      }
    }else {
      strcpy(filename2, filename);
      iomode = READONLY;
      if (!fits_open_file(&(datafile->fits_fptr), filename2, iomode, &status)) {
	if(verbose) printf("  '%s' opened\n", filename);
	datafile->opened_flag = 1;
      }else {
	datafile->opened_flag = 0;
      }
    }
    if (status) fits_report_error(stderr, status); /* print any error message */
  }

  
  datafile->fptr_hdr = datafile->fptr;

  return datafile->opened_flag;
}

/* Copy the struct to another, except the file pointers. */
void copy_paramsPSRData(datafile_definition datafile_source, datafile_definition *datafile_dest) 
{
  datafile_dest->fptr = NULL;
  datafile_dest->fptr_hdr = NULL;
  datafile_dest->fits_fptr = NULL;
  datafile_dest->scales = NULL;
  datafile_dest->offsets = NULL;
  datafile_dest->weights = NULL;
  datafile_dest->format = datafile_source.format;
  /* Don't copy open status and write status */
  /*  datafile_dest->opened_flag = datafile_source.opened_flag; 
      datafile_dest->enable_write_flag = datafile_source.enable_write_flag; */
  datafile_dest->dd_mode = datafile_source.dd_mode;
  datafile_dest->NrPulses = datafile_source.NrPulses;
  datafile_dest->NrBins = datafile_source.NrBins;
  datafile_dest->NrBits = datafile_source.NrBits;
  datafile_dest->NrPols = datafile_source.NrPols;
  datafile_dest->nrFreqChan = datafile_source.nrFreqChan;
  datafile_dest->Period = datafile_source.Period;
  datafile_dest->SampTime = datafile_source.SampTime;
  datafile_dest->freq_cent = datafile_source.freq_cent;
  datafile_dest->bw = datafile_source.bw;
  datafile_dest->ra = datafile_source.ra;
  datafile_dest->dec = datafile_source.dec;
  datafile_dest->dm = datafile_source.dm;
  datafile_dest->channelbw = datafile_source.channelbw;
  datafile_dest->mjd = datafile_source.mjd;
  strcpy(datafile_dest->filename, datafile_source.filename);
  strcpy(datafile_dest->psrname, datafile_source.psrname);
  strcpy(datafile_dest->observatory, datafile_source.observatory);
  strcpy(datafile_dest->institute, datafile_source.institute);
  strcpy(datafile_dest->instrument, datafile_source.instrument);
  strcpy(datafile_dest->scanid, datafile_source.scanid);

  datafile_dest->nrpapoints = datafile_source.nrpapoints;
  datafile_dest->longitudes_defined = datafile_source.longitudes_defined;
  datafile_dest->bins_defined = datafile_source.longitudes_defined;
  datafile_dest->RMSI = datafile_source.RMSI;
  datafile_dest->RMSL = datafile_source.RMSL;
  datafile_dest->RMSV = datafile_source.RMSV;

  datafile_dest->fd_type = datafile_source.fd_type;
  datafile_dest->fd_sang = datafile_source.fd_sang;
  datafile_dest->fd_xyph = datafile_source.fd_xyph;
  datafile_dest->poltype = datafile_source.poltype;
}

/* Returns 0 if parse error */
int PSRDataHeader_parse_commandline(datafile_definition *psrdata, int argc, char **argv, int verbose)
{
  int i, j, bw_set, channelbw_set;
  char identifier[100], value[100];

  channelbw_set = 0;
  bw_set = 0;

  for(i = 1; i < argc - 1; i++) {
    if(strcmp(argv[i], "-header") == 0) {
      i++;
      j = sscanf(argv[i], "%s %s", identifier, value);
      if(j != 2) {
	fprintf(stderr, "ERROR PSRDataHeader_parse_commandline:  Cannot parse -header option.\n\n");
	printHeaderCommandlineOptions(stderr);
	return 0;
      }else {
	if(strcmp(identifier,"name") == 0) {
	  strcpy(psrdata->psrname,value);
	  if(verbose) printf("  hdr.psrname = %s\n", psrdata->psrname);
	}else if(strcmp(identifier,"freq") == 0) {
	  psrdata->freq_cent = atof(value);
	  if(verbose) printf("  hdr.freq_cent = %f MHz\n", psrdata->freq_cent);
	}else if(strcmp(identifier,"bw") == 0) {
	  psrdata->bw = atof(value);
	  bw_set = 1;
	  if(!channelbw_set)
	    psrdata->channelbw = psrdata->bw/(float)psrdata->nrFreqChan;
	  if(verbose) printf("  hdr.bw = %f MHz\n", psrdata->bw);
	  if(verbose) printf("  hdr.channelbw = %f MHz\n", psrdata->channelbw);
	}else if(strcmp(identifier,"chbw") == 0) {
	  psrdata->channelbw = atof(value);
	  channelbw_set = 1;
	  if(!bw_set)
	    psrdata->bw = (psrdata->channelbw)*(psrdata->nrFreqChan);
	  if(verbose) printf("  hdr.bw = %f MHz\n", psrdata->bw);
	  if(verbose) printf("  hdr.channelbw = %f MHz\n", psrdata->channelbw);
	}else if(strcmp(identifier,"p0") == 0 || strcmp(identifier,"P0") == 0) {
	  psrdata->Period = atof(value);
	  if(verbose) printf("  hdr.Period = %f s\n", psrdata->Period);
	}else if(strcmp(identifier,"dt") == 0) {
	  psrdata->SampTime = atof(value);
	  if(verbose) printf("  hdr.SampTime = %f s\n", psrdata->SampTime);
	}else if(strcmp(identifier,"mjd") == 0) {
	  psrdata->mjd = atof(value);
	  if(verbose) printf("  hdr.mjd = %f\n", psrdata->mjd);
	}else if(strcmp(identifier,"scan") == 0) {
	  strncpy(psrdata->scanid, value, 100);
	  if(verbose) printf("  hdr.ScanNum = %s\n", psrdata->scanid);
	}else if(strcmp(identifier,"observatory") == 0) {
	  strncpy(psrdata->observatory, value, 100);
	  if(verbose) printf("  hdr.observatory = %s\n", psrdata->observatory);
	}else if(strcmp(identifier,"nrpulses") == 0) {
	  psrdata->NrPulses = atol(value);
	  if(verbose) printf("  hdr.NrPulses = %ld\n", psrdata->NrPulses);
	}else if(strcmp(identifier,"nrbins") == 0) {
	  psrdata->NrBins = atol(value);
	  if(verbose) printf("  hdr.NrBins = %ld\n", psrdata->NrBins);
	}else if(strcmp(identifier,"nrbits") == 0) {
	  psrdata->NrBits = atoi(value);
	  if(verbose) printf("  hdr.NrBits = %d\n", psrdata->NrBits);
	}else if(strcmp(identifier,"nrchan") == 0) {
	  psrdata->nrFreqChan = atol(value);
	  if(verbose) printf("  hdr.nrFreqChan = %ld\n", psrdata->nrFreqChan);
	}else if(strcmp(identifier,"fdtype") == 0) {
	  psrdata->fd_type = atoi(value);
	  if(verbose) printf("  hdr.fd_type = %d\n", psrdata->fd_type);
	}else if(strcmp(identifier,"sang") == 0) {
	  psrdata->fd_sang = atof(value);
	  if(verbose) printf("  hdr.fd_sang = %f\n", psrdata->fd_sang);
	}else if(strcmp(identifier,"xyph") == 0) {
	  psrdata->fd_xyph = atof(value);
	  if(verbose) printf("  hdr.fd_xyph = %f\n", psrdata->fd_xyph);
	}else if(strcmp(identifier,"poltype") == 0) {
	  psrdata->poltype = atoi(value);
	  if(verbose) printf("  hdr.poltype = %d\n", psrdata->poltype);
	}else if(strcmp(identifier,"dm") == 0) {
	  psrdata->dm = atof(value);
	  if(verbose) printf("  hdr.dm = %f\n", psrdata->dm);
	}else {
	  fprintf(stderr, "ERROR PSRDataHeader_parse_commandline:  '%s' not recognized as a header parameter.\n\n", identifier);
	  printHeaderCommandlineOptions(stderr);
	  return 0;	  
	}
      }
    }
  }
  return 1;
}

/* separator is "=" for presto and ":" for sigproc */
char *get_ptr_entry(char *str, char **txt, int nrlines, char *separator)
{
  int i, j;
  char *s_ptr;
  for(i = 0; i < nrlines; i++) {
    s_ptr = strstr(txt[i], str);
    if(s_ptr != NULL) {             /* Found entry */
      s_ptr = strstr(s_ptr, separator);
      if(s_ptr != NULL) {           /* Found = character */
	s_ptr++;
	do {                        /* Remove leading spaces */
	  if(*s_ptr == ' ')
	    s_ptr++;
	}while(*s_ptr == ' ');
	j = 0;
	do {                        /* Remove end of line characters */
	  if(s_ptr[j] == '\r')
	    s_ptr[j] = 0;
	  if(s_ptr[j] == '\n')
	    s_ptr[j] = 0;
	  if(s_ptr[j] != 0)
	    j++;
	}while(s_ptr[j] != 0);
	return s_ptr;               /* Found entry, so quit for loop */
      }
    }
  }
  return NULL;
}

/* Returns the number of filenames in the commandline */
int numberInApplicationFilenameList(void)
{
  return internal_applicationNrfilenames;
}

/* returns 1 if the filenames appear consecutive, or else returns
   0. If the list is empty returns 1. If argv != NULL, an error
   message is produced stating which options failed. */
int applicationFilenameList_checkConsecutive(char **argv)
{
  int i, j, indx_last;
  if(internal_applicationNrfilenames < 2)
    return 1;
  indx_last = internal_applicationFilenameList[0];
  for(i = 1; i < internal_applicationNrfilenames; i++) {
    if(internal_applicationFilenameList[i] != indx_last+1) {
      if(argv != NULL) {
	fprintf(stderr, "Cannot parse command line: ");
	for(j = 0; j < internal_applicationNrfilenames; j++) {
	  fprintf(stderr, "'%s' ", argv[internal_applicationFilenameList[j]]);
	}
	fprintf(stderr, "\n");
      }
      return 0;
    }
    indx_last++;
  }
  return 1;
}

/* Call when you want to add string argi from argv to the list of
filenames.  returns 1 on success, 0 on error */
int applicationAddFilename(int argi)
{
  if(internal_applicationNrfilenames < MaxNrApplicationFilenames) {
    internal_applicationFilenameList[internal_applicationNrfilenames++] = argi;
  }else {
    fprintf(stderr, "applicationAddFilename: Too many filenames on command-line\n");
    return 0;
  }
  return 1;
}


/* Hacked */
int processCommandLine(patrickSoftApplication *application, int argc, char **argv, int *index)
{
  if(strcmp(argv[*index], "-headerlist") == 0 && application->switch_headerlist) {
    printHeaderCommandlineOptions(stdout);
    exit(0);
  }else if(strcmp(argv[*index], "-header") == 0 && application->switch_header) {
    (*index)++;
    return 1;
  }else if(strcmp(argv[*index], "-ext") == 0 && application->switch_ext) {
    application->extension = argv[++(*index)];
    return 1;
  }else if(strcmp(argv[*index], "-output") == 0 && application->switch_output) {
    strcpy(application->outputname, argv[++(*index)]);
    return 1;
  }else if(strcmp(argv[*index], "-v") == 0 && application->switch_verbose) {
    application->verbose = 1;
    return 1;
  }else {
    return 0;
  }
}

/* Clean the struct which is probably filled with random junk at the
   time of declaration. */
void cleanPRSData(datafile_definition *datafile) 
{
  datafile->fptr = NULL;
  datafile->fptr_hdr = NULL;
  datafile->fits_fptr = NULL;
  datafile->scales = NULL;
  datafile->offsets = NULL;
  datafile->weights = NULL;
  datafile->format = 0;
  datafile->opened_flag = 0;
  datafile->enable_write_flag = 0;
  datafile->dd_mode = 0;
  datafile->filename[0] = 0;
  datafile->NrPulses = 0;
  datafile->NrBins = 0;
  datafile->NrBits = 0;
  datafile->NrPols = 0;
  datafile->nrFreqChan = 0;
  datafile->Period = 0;
  datafile->SampTime = 0;
  datafile->freq_cent = 0;
  datafile->bw = 0;
  datafile->ra = 0;
  datafile->dec = 0;
  datafile->dm = 0;
  datafile->channelbw = 0;
  datafile->mjd = 0;
  datafile->psrname[0] = 0;
  datafile->observatory[0] = 0;
  datafile->institute[0] = 0;
  datafile->instrument[0] = 0;
  datafile->scanid[0] = 0;
  datafile->nrpapoints = 0;
  datafile->data_pa = NULL;
  datafile->data_dpa = NULL;
  datafile->longitudes_defined = 0;
  datafile->data_long = NULL;
  datafile->bins_defined = 0;
  datafile->data_bin = NULL;
  datafile->RMSI = 0;
  datafile->RMSL = 0;
  datafile->RMSV = 0;
  datafile->fd_type = 0;
  datafile->fd_sang = 0;
  datafile->fd_xyph = 0;
  datafile->poltype = 0;
  datafile->datastart = 0;
}

/* Hacked */
void initApplication(patrickSoftApplication *application, char *name, char *genusage)
{
  internal_applicationNrfilenames = 0;
  internal_applicationCurFilename = 0;
  strcpy(application->progname, name);
  strcpy(application->genusage, genusage);
  application->switch_headerlist = 0;
  application->switch_header = 0;
  application->switch_ext = 0;
  application->extension = NULL;
  application->switch_output = 0;
  sprintf(application->outputname, "output.dat");
  application->switch_verbose = 0;
  application->verbose = 0;
}

int constructFITSsearchsubint(datafile_definition datafile, float *data, int subintnr, unsigned char **subintdata, float **scales, float **offsets, int allocmem, int destroymem)
{
  long subintsize, f, p, b, samplenr, bitnr, bytenr;
  int bitoff, ivalue, bitofffac[8];
  float offset, scale, fvalue;

  if(datafile.NrBits == 4) {
    bitofffac[0] = 16;
    bitofffac[4] = 1;
  }else if(datafile.NrBits == 8) {   /* Nothing to difficult do */
    bitofffac[0] = 1;
  }else if(datafile.NrBits == 2) {  
    bitofffac[0] = 64;
    bitofffac[2] = 16;
    bitofffac[4] = 4;
    bitofffac[6] = 1;
  }else {
    fprintf(stderr, "Error constructFITSsearchsubint: Writing of %d bits data is not supported\n", datafile.NrBits);
    return 0;
  }

  if(destroymem) {
    free(*subintdata);
    free(*scales);
    free(*offsets);
    return 1;
  }
  subintsize = datafile.NrPols*datafile.NrBins*datafile.nrFreqChan;
  subintsize *= datafile.NrBits;
  subintsize /= 8;
  if(allocmem) {
    *subintdata = (unsigned char *)malloc(subintsize);
    *scales  = (float *)malloc(datafile.NrPols*datafile.nrFreqChan*sizeof(float));
    *offsets = (float *)malloc(datafile.NrPols*datafile.nrFreqChan*sizeof(float));
    if(*subintdata == NULL || *scales == NULL || *offsets == NULL) {
      fprintf(stderr, "Error constructFITSsearchsubint: Cannot allocate %ld bytes\n", subintsize);
      return 0;
    }
    return 1;
  }
  /* Clear memory */
  memset(*subintdata, 0, subintsize);
  for(f = 0; f < datafile.nrFreqChan; f++) {
    for(p = 0; p < datafile.NrPols; p++) {
      internalFITSscalePulse(&data[datafile.NrBins*(p+datafile.NrPols*(f+subintnr*datafile.nrFreqChan))], datafile.NrBins, &offset, &scale, pow(2, datafile.NrBits)-1);
      (*offsets)[p*datafile.nrFreqChan+f] = offset;
      (*scales)[p*datafile.nrFreqChan+f] = scale;
      /*      fprintf(stderr, "f=%ld p=%ld\n", f, p);
	      fprintf(stderr, "  scale=%e offset=%e\n", scale, offset);*/
      /* Now pack the data */
      for(b = 0; b < datafile.NrBins; b++) {
	fvalue = (data[datafile.NrBins*(p+datafile.NrPols*(f+subintnr*datafile.nrFreqChan))+b]-(*offsets)[p*datafile.nrFreqChan+f])/(*scales)[p*datafile.nrFreqChan+f];
	/* Add 0.5 to make it rounding rather than truncating. fvalue should be positive. */
	ivalue = fvalue+0.5;
	/*	fprintf(stderr, "  b=%ld data=%f ivalue=%d\n", b, data[datafile.NrBins*(p+datafile.NrPols*(f+subintnr*datafile.nrFreqChan))+b], ivalue);*/

	samplenr = p*datafile.NrBins*datafile.nrFreqChan+b*datafile.nrFreqChan+f;
	bitnr    = samplenr * datafile.NrBits;
	bytenr   = bitnr/8;
	bitoff   = bitnr % 8;

	ivalue *= bitofffac[bitoff];
	if(ivalue > 255) {
	  fprintf(stderr, "ERROR constructFITSsearchsubint: Packing error (sample %ld: val=%d mult=%d nrbits=%d)\n", samplenr, ivalue, bitofffac[bitoff], datafile.NrBits);
	  return 0;
	}
	(*subintdata)[bytenr] += ivalue;
      }
    }
  }

  return 1;
}

int writeFITSsubint(datafile_definition datafile, long subintnr, unsigned char *subintdata, float *scales, float *offsets)
{
  int status = 0;   /* CFITSIO status value MUST be initialized to zero! */
  long subintsize, i;
  float weight;

  if(lookupSubintTable(datafile) == 0) {
    fprintf(stderr, "ERROR writeFITSsubint: Cannot mode to subint table.\n");    
    return 0;
  }
  if(datafile.dd_mode == 0 && datafile.Period > 0) {
    fprintf(stderr, "ERROR writeFITSsubint: This function assumes file is search mode data, which it isn't.\n");    
    return 0;
  }
  subintsize = datafile.NrPols*datafile.NrBins*datafile.nrFreqChan;
  subintsize *= datafile.NrBits;
  subintsize /= 8;
  /*  fprintf(stderr, "\nXXXXXX %ld\n", subintsize); */
  if(fits_write_col(datafile.fits_fptr, TBYTE, 18, 1+subintnr, 1, subintsize, subintdata, &status) != 0) {
    fprintf(stderr, "ERROR writeFITSsubint: Error writing data.\n");
    fits_report_error(stderr, status); /* print any error message */
    return 0;
  }

  if(fits_write_col(datafile.fits_fptr, TFLOAT, 17, 1+subintnr, 1, datafile.NrPols*datafile.nrFreqChan, scales, &status) != 0) {
    fprintf(stderr, "ERROR writeFITSsubint: Error writing scales.\n");
    fits_report_error(stderr, status); /* print any error message */
    return 0;
  }

  /*  for(i = 0; i < datafile.NrPols*datafile.nrFreqChan; i++)
      printf("XXX %d: %e %e\n", subintnr, offsets[i], scales[i]);*/

  if(fits_write_col(datafile.fits_fptr, TFLOAT, 16, 1+subintnr, 1, datafile.NrPols*datafile.nrFreqChan, offsets, &status) != 0) {
    fprintf(stderr, "ERROR writeFITSsubint: Error writing offsets.\n");
    fits_report_error(stderr, status); /* print any error message */
    return 0;
  }

  /* Assume that weight is already applied */
  weight = 1;
  for(i = 0; i < datafile.nrFreqChan; i++) {
    if(fits_write_col(datafile.fits_fptr, TFLOAT, 15, 1+subintnr, 1+i, 1, &weight, &status) != 0) {
      fprintf(stderr, "ERROR writeFITSsubint: Error writing weights.\n");
      fits_report_error(stderr, status); /* print any error message */
      return 0;
    }
  }

  return 1;
}




int writePulsePSRData(datafile_definition datafile, long pulsenr, int polarization, int freq, int binnr, long nrSamples, float *pulse)
{
  if(pulsenr < 0 || binnr < 0 || freq < 0) {
    fprintf(stderr, "ERROR writePulsePSRData: Parameters outside boundaries.\n");
    return 0;
  }
  if(datafile.format == MEMORY_format) {
    memcpy(&datafile.data[datafile.NrBins*(polarization+datafile.NrPols*(freq+pulsenr*datafile.nrFreqChan))+binnr], pulse, sizeof(float)*nrSamples);
  }else {
    fprintf(stderr, "ERROR writePulsePSRData: Writing in this format (%d) is not implemented\n", datafile.format);
    return 0;
  }
  return 1;
}
