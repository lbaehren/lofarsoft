/***************************************************************************
 *
 *   Copyright (C) 1998 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "polyco.h"
#include "stdmpi.h"

#include <iostream>

int mpiPack_size (const polynomial& pl, MPI_Comm comm, int* size)
{
  int total_size = 0;
  int temp_size = 0;

  mpiPack_size (pl.psrname, comm, &temp_size);
  total_size += temp_size;
  mpiPack_size (pl.date, comm, &temp_size);
  total_size += temp_size;
  mpiPack_size (pl.utc,  comm, &temp_size);
  total_size += temp_size;

  MPI_Pack_size (1, MPI_CHAR,    comm, &temp_size);  // tempov11
  total_size += temp_size;
  if (pl.tempov11) {
    MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // doppler_shift
    total_size += temp_size;
    MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // log_rms_resid
    total_size += temp_size;
  }
  mpiPack_size (pl.ref_phase, comm, &temp_size);
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // f0
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // telescope
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // freq
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // binary
  total_size += temp_size;
  if (pl.binary) {
    MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // binph
    total_size += temp_size;
    MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // binfreq
    total_size += temp_size;
  }
  MPI_Pack_size (1, MPI_DOUBLE,    comm, &temp_size);  // nspan_mins
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // dm
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // coefs.size()
  total_size += temp_size;

  MPI_Pack_size ((int)pl.coefs.size(), MPI_DOUBLE, comm, &temp_size); 
  // ncoef doubles
  total_size += temp_size;

  mpiPack_size (pl.reftime, comm, &temp_size);
  total_size += temp_size;

  *size = total_size;
  return 0; // no error, not dynamic
}

int mpiPack (const polynomial& pl, void* outbuf, int outcount, int* position, 
	     MPI_Comm comm)
{
  mpiPack (pl.psrname, outbuf, outcount, position, comm);
  mpiPack (pl.date, outbuf, outcount, position, comm);
  mpiPack (pl.utc, outbuf, outcount, position, comm);

  char boolean = pl.tempov11;
  MPI_Pack (&boolean, 1, MPI_CHAR,    outbuf, outcount, position, comm);

  double temp = 0;
  if (pl.tempov11) {
    temp = pl.doppler_shift;
    MPI_Pack (&temp,  1, MPI_DOUBLE, outbuf, outcount, position, comm);
    temp = pl.log_rms_resid;
    MPI_Pack (&temp,  1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }
  mpiPack (pl.ref_phase, outbuf, outcount, position, comm);

  MPI_Pack ((void*)&pl.f0,
	    1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&pl.telescope,
	    1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)&pl.freq,
	    1, MPI_DOUBLE, outbuf, outcount, position, comm);
  boolean = pl.binary;
  MPI_Pack (&boolean,
	    1, MPI_CHAR,   outbuf, outcount, position, comm);
  if (pl.binary) {
    MPI_Pack ((void*)&pl.binph,
	      1, MPI_DOUBLE, outbuf, outcount, position, comm);
    MPI_Pack ((void*)&pl.binfreq,
	      1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }
  MPI_Pack ((void*)&pl.nspan_mins,
	    1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&pl.dm,
	    1, MPI_DOUBLE, outbuf, outcount, position, comm);

  int length = (int)(pl.coefs.size());
  MPI_Pack (&length,
	    1, MPI_INT,    outbuf, outcount, position, comm);

  for (unsigned i=0; i<pl.coefs.size(); ++i) {
    temp = pl.coefs[i];
    MPI_Pack (&temp, 1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }

  mpiPack (pl.reftime, outbuf, outcount, position, comm);

  return MPI_SUCCESS;
}

int mpiUnpack (void* inbuf, int insize, int* position, 
			polynomial* pl, MPI_Comm comm)
{
  char boolean;

  mpiUnpack  (inbuf, insize, position, &(pl->psrname), comm);
  mpiUnpack  (inbuf, insize, position, &(pl->date), comm);
  mpiUnpack  (inbuf, insize, position, &(pl->utc), comm);
  MPI_Unpack (inbuf, insize, position, &boolean, 1, MPI_CHAR, comm);
  pl->tempov11 = boolean;
  if (pl->tempov11) {
    MPI_Unpack (inbuf, insize, position, &(pl->doppler_shift),
		1, MPI_DOUBLE, comm);
    MPI_Unpack (inbuf, insize, position, &(pl->log_rms_resid),
		1, MPI_DOUBLE, comm);
  }
  mpiUnpack(inbuf, insize, position, &(pl->ref_phase), comm);

  MPI_Unpack (inbuf, insize, position, &(pl->f0),
	      1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &(pl->telescope),
	      1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &(pl->freq),
	      1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &boolean,
	      1, MPI_CHAR,   comm);

  pl->binary = boolean;
  if (pl->binary) {
    MPI_Unpack (inbuf, insize, position, &(pl->binph),
		1, MPI_DOUBLE, comm);
    MPI_Unpack (inbuf, insize, position, &(pl->binfreq),
		1, MPI_DOUBLE, comm);
  }
  MPI_Unpack (inbuf, insize, position, &(pl->nspan_mins),
	      1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &(pl->dm),
	      1, MPI_DOUBLE, comm);

  int tmpint = 0;
  MPI_Unpack (inbuf, insize, position, &tmpint,
	      1, MPI_INT,    comm);

  pl->coefs.resize(tmpint);
  for(unsigned i=0; i<pl->coefs.size(); ++i)  
    MPI_Unpack (inbuf, insize, position, &(pl->coefs[i]),
		1, MPI_DOUBLE,   comm);

  mpiUnpack (inbuf, insize, position, &(pl->reftime), comm);

  return MPI_SUCCESS;
}

int mpiPack_size (const polyco& poly, MPI_Comm comm, int* size)
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1,  MPI_INT,  comm, &temp_size);  // npollys
  total_size += temp_size;

  for (unsigned i=0; i<poly.pollys.size(); i++) {
    mpiPack_size (poly.pollys[i], comm, &temp_size);
    total_size += temp_size;
  }
  *size = total_size;
  return 1; // no error, dynamic
}

int mpiPack (const polyco& poly, void* outbuf, int outcount, int* position, 
	     MPI_Comm comm)
{
  int length = (int) poly.pollys.size();
  MPI_Pack (&length, 1, MPI_INT, outbuf, outcount, position, comm);

  for (unsigned i=0; i<poly.pollys.size(); i++) {
    mpiPack (poly.pollys[i], outbuf, outcount, position, comm);
  }
  return MPI_SUCCESS;
}

int mpiUnpack (void* inbuf, int insize, int* position, 
		       polyco* poly, MPI_Comm comm)
{
  int npollys;
  int i = 0;

  MPI_Unpack (inbuf, insize, position, &npollys, 1, MPI_INT, comm);
  poly->pollys.resize(npollys);
  for (i=0; i<npollys; i++) 
    mpiUnpack (inbuf, insize, position, &(poly->pollys[i]), comm);

  return MPI_SUCCESS;
}

