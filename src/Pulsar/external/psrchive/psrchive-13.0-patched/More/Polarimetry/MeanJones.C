/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MeanJones.h"
#include "Cloude.h"
#include "Jacobi.h"

using namespace std;

bool Calibration::MeanJones::verbose = true;

void Calibration::MeanJones::integrate (const MEAL::Complex2* model)
{
  convert.set_transformation( const_cast<MEAL::Complex2*>( model ) );
  Matrix<4,4, std::complex< Estimate<double> > > T = convert.estimate ();

  if (verbose)
  {
    Jones<double> J = model->evaluate();
    cerr << "Calibration::MeanJones::integrate"
      "\nJ=" << J << "\nT=" << T << endl;
  }

  target_coherency += T;
}

template<unsigned N, typename T>
Matrix< N,N, std::complex<T> >
val (const Matrix< N,N, std::complex<Estimate<T> > >& M)
{
  Matrix< N,N, std::complex<T> > result;
  for (unsigned i=0; i<N; i++)
    for (unsigned j=0; j<N; j++)
      result[i][j] = std::complex<T>(M[i][j].real().val, M[i][j].imag().val);

  return result;
}

Jones<double> Calibration::MeanJones::get_mean ()
{
  Matrix< 4,4, std::complex< Estimate<double> > > T = target_coherency;
  target_coherency.zero();

  Matrix< 4,4,complex<double> > target = val(T);
  Matrix< 4,4,complex<double> > eigenvectors;
  Vector< 4, double > eigenvalues;
    
  Jacobi (target, eigenvectors, eigenvalues);

  unsigned imax=0;
  double max = 0;

  if (verbose)
    cerr << "Calibration::MeanJones::get_mean eigenvalues:" << endl;

  for (unsigned i=0; i<4; i++)
  {
    if (verbose)
      cerr << " " << eigenvalues[i];

    /*
      The target coherency matrix is not a covariance matrix and
      therefore may not be positive semi-definite and may have
      negative eigenvalues.
    */

    eigenvalues[i] = fabs( eigenvalues[i] );

    if (eigenvalues[i] > max)
    {
      max = eigenvalues[i];
      imax = i;
    }
  }

  Jones<double> result = system (eigenvectors[imax]);

  if (verbose)
  {
    double H_T = entropy (eigenvalues);
    cerr << "\n"
      "Calibration::MeanJones::get_mean imax=" << imax << " entropy=" << H_T 
	 << "\n\tresult=" << result << endl;
  }

  return result;
}
