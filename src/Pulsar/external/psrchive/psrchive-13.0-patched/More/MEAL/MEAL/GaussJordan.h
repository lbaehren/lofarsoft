/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <vector>
#include <algorithm>

#include "Error.h"

//#define __DEBUG

namespace MEAL {
  template <class T, class U>
    void GaussJordan (std::vector<std::vector<T> >& a,
		      std::vector<std::vector<U> >& b,
		      int nrow = -1, double singular_threshold = 0.0);
}

inline double inv (double x) { return 1.0/x; }

template <class T, class U>
void MEAL::GaussJordan (std::vector<std::vector<T> >& a,
			std::vector<std::vector<U> >& b,
			int nrow, double singular_threshold)
{
  if (nrow < 0)
    nrow = a.size();

  if (nrow == 0)
  {
    std::cerr << "MEAL::GaussJordan nrow=" << nrow << std::endl;
    return;
  }

  int ncol = 0;

  if (b.size())
  {
    if (b.size() < unsigned(nrow))
      throw Error (InvalidState, "MEAL::GaussJordan",
		   "b.size()=%d < nrow=%d", b.size(), nrow);

    ncol = b[0].size();
  }

#ifdef _DEBUG
  std::cerr << "MEAL::GaussJordan nrow=" << nrow
	    << " ncol=" << ncol << std::endl;
#endif

  int irow = 0;
  int icol = 0;

  for (irow=0; irow < nrow; irow++)
    if (a[irow].size() < unsigned(nrow))
      throw Error (InvalidState, "MEAL::GaussJordan",
		   "a[%d].size()=%d < nrow=%d", irow, a[irow].size(), nrow);

  // pivot book-keeping arrays
  std::vector<int> indxc (nrow);
  std::vector<int> indxr (nrow);
  std::vector<int> ipiv (nrow, 0);

  int i, j, k;
  for (i=0; i<nrow; i++)
  {
    // search for the pivot element

    double big = 0.0;
    for (j=0; j<nrow; j++)
      if (ipiv[j] != 1)
	for (k=0; k<nrow; k++)
	{
	  if (ipiv[k] == 0)
	  {
	    if (fabs(a[j][k]) >= big)
	    {
	      big=fabs(a[j][k]);
	      irow=j;
	      icol=k;
	    }
	  }
	}

    ipiv[icol]++;

    if (fabs(a[icol][icol]) <= singular_threshold)
      throw Error (InvalidState, "MEAL::GaussJordan",
		   "Singular Matrix.  irow=%d nrow=%d", irow, nrow);

    if (irow != icol)
    {
      for (j=0; j<nrow; j++) std::swap (a[irow][j], a[icol][j]);
      for (j=0; j<ncol; j++) std::swap (b[irow][j], b[icol][j]);
    }

    indxr[i]=irow;
    indxc[i]=icol;

    T pivinv = inv(a[icol][icol]);
    a[icol][icol]=1.0;

#ifdef _DEBUG
    std::cerr << "icol=" << icol << " irow=" << irow << " 1/piv=" << pivinv << std::endl;
#endif
    
    for (j=0; j<nrow; j++)  a[icol][j] *= pivinv;
    for (j=0; j<ncol; j++)  b[icol][j] *= pivinv;

    // reduce the rows except for the pivot
    for (j=0; j<nrow; j++)
      if (j != icol)
      {
	T dum = a[j][icol];
	a[j][icol]=0.0;

	for (k=0; k<nrow; k++)  a[j][k] -= a[icol][k]*dum;
	for (k=0; k<ncol; k++)  b[j][k] -= b[icol][k]*dum;
      }

  }

  // unscramble the column interchanges
  for (i=nrow-1; i>=0; i--)
    if (indxr[i] != indxc[i])
      for (j=0; j<nrow; j++)
	std::swap(a[j][indxr[i]],a[j][indxc[i]]);

}
