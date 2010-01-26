/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __RESIO_H
#define __RESIO_H

#ifdef __cplusplus
extern "C" {
#endif

  void fortopen_ (int* lun, char* filename, int filename_length);
  void fortclose_ (int* lun);
  
  /* I/O function to read from the resid2.tmp file produced by TEMPO */
  void resread_ (int* lun, double* mjd, double* resiturns, double* resitimes,
		 double* phase, double* freq, double* weight, double* error,
		 double* preres, int* status);

  void covn_ (int* lun, int* nparm);

  void covread_ (int* lun, int* nparm, char* parm_names,
		 double* gcor, double* sig, double* cov);

#ifdef __cplusplus
}
#endif

#endif


