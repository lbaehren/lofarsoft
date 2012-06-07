/***************************************************************************
 *  *
 *  *   Copyright (C) 2011 by Stefan Oslowski
 *  *   Licensed under the Academic Free License version 2.1
 *  *
 ****************************************************************************/

#include "Pulsar/TimeDomainCovariance.h"
#include "Warning.h"
#include <algorithm>

using namespace std;
using namespace Pulsar;

static Warning warn;

//TODO implement first / last bin

//! Default constructor
TimeDomainCovariance::TimeDomainCovariance ()
{
  covariance_matrix = NULL;
  rank = 0;
  count = 0;
  wt_sum = 0.0;
  wt_sum2 = 0.0;
  first_bin = 0;
  last_bin = 0;
  p_damps = NULL;
  finalized = false;
}

//! Destructor
TimeDomainCovariance::~TimeDomainCovariance ()
{
  if ( covariance_matrix != NULL ) delete [] covariance_matrix;
}

void TimeDomainCovariance::reset ()
{
  wt_sum = 0.0;
  wt_sum2 = 0.0;
  count = 0;
  for (unsigned i = 0; i < rank * rank; ++i)
    covariance_matrix[i] = 0.0;
}

void TimeDomainCovariance::set_rank ( unsigned value )
{
  if ( rank == 0 )
    rank = value;
  else
    throw Error (InvalidState, "TimeDomainCovariance::set_rank",
		    "Rank can't be changed after it was set");
  if ( rank < 0 )
    throw Error (InvalidParam, "TimeDomainCovariance::set_rank",
		    "Rank can't be negative");
  if ( covariance_matrix == NULL )
  {
    covariance_matrix = new double[rank * rank];
    covariance_gsl = gsl_matrix_calloc ( rank, rank );
    p_damps = new double [rank];
  }
}

unsigned TimeDomainCovariance::get_rank ()
{
  return rank;
}

void TimeDomainCovariance::set_count ( unsigned value )
{
  count = value;
}

unsigned TimeDomainCovariance::get_count ()
{
  return count;
}

void TimeDomainCovariance::add_Profile ( const Profile* p )
{
  float wt = p->get_weight() ;

  add_Profile ( p, wt );
}

void TimeDomainCovariance::add_Profile ( const Profile* p, float wt )
{
  if (finalized)
    cerr << "TimeDomainCovariance::add_Profile WARNING: the covariance matrix has already been finalised" << endl;
  if (wt == 0.0 )
    return;

  if (rank == 0)
    set_rank ( p->get_nbin () );
  else if (rank != p->get_nbin () )
    throw Error (InvalidParam, "TimeDomainCovariance::addProfile",
		    "Mismatch between number of bins and rank of covariance matrix");

  count += 1;
  wt_sum += wt;
  wt_sum2 += wt * wt;

  transform( p->get_amps(), p->get_amps() + rank, p_damps, CastToDouble() );
  p_gsl = gsl_vector_view_array (const_cast<double *> (p_damps), rank );

  gsl_blas_dger (wt, &p_gsl.vector, &p_gsl.vector, covariance_gsl );
}

void TimeDomainCovariance::get_covariance_matrix ( double *dest )
{
  finalize ();
  memcpy ( (void*)covariance_matrix, (void*)covariance_gsl->data, rank * rank * sizeof(double) ); 
  //TODO take the last first bin into account
  memcpy( dest, covariance_matrix, rank * rank * sizeof(double) );
}

void TimeDomainCovariance::get_covariance_matrix_gsl ( gsl_matrix *dest )
{
  finalize ();
  gsl_matrix_memcpy( dest, covariance_gsl );
}

void TimeDomainCovariance::set_covariance_matrix ( double *src )
{
  //TODO take the last first bin into account
  memcpy ( covariance_matrix, src, rank * rank * sizeof(double) );
}

double TimeDomainCovariance::get_covariance_matrix_value ( unsigned i, unsigned j)
{
  finalize ();
  //TODO take the last first bin into account
  return gsl_matrix_get( covariance_gsl, i, j );
}

void TimeDomainCovariance::finalize ()
{
  if ( ! finalized )
    gsl_matrix_scale ( covariance_gsl, 1.0 / wt_sum );
  else 
    cerr << "WARNING: Pulsar::TimeDomainCovariance::finalize covariance matrix already finalized" << endl;
  finalized = true;
}

void TimeDomainCovariance::choose_bins ( unsigned val_1, unsigned val_2 )
{
  if (val_1 < val_2)
  { 
    first_bin = val_1;
    last_bin = val_2;
  }
  else
  {
    first_bin = val_2;
    last_bin = val_1;
  }

  warn << "TimeDomainCovariance::choose_bins WARNING" << " calculation of covariance matrix for some bins only not implemented yet" << endl;
  set_rank ( last_bin - first_bin + 1 );
}

string TimeDomainCovariance::get_name () const
{
  return "TimeDomain";
}
