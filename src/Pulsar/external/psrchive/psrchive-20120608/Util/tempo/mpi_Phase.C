/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Phase.h"

int mpiPack_size (const Phase& ph, MPI_Comm comm, int* size)
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // turns
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // fracturns
  total_size += temp_size;

  *size = total_size;
  return 0;
}

int mpiPack (const Phase& ph, void* outbuf, int outcount, int* position, 
	     MPI_Comm comm)
{
  MPI_Pack ((void*)&ph.turns, 1, MPI_INT, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&ph.fturns, 1, MPI_DOUBLE, outbuf, outcount, position,comm);
  return MPI_SUCCESS;
}

int mpiUnpack (void* inbuf, int insize, int* position, Phase* ph,
	       MPI_Comm comm)
{
  MPI_Unpack (inbuf, insize, position, &(ph->turns), 1, MPI_INT, comm);
  MPI_Unpack (inbuf, insize, position, &(ph->fturns), 1, MPI_DOUBLE, comm);
  return MPI_SUCCESS;
}

