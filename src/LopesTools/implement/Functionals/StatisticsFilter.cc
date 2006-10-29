/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: StatisticsFilter.cc,v 1.7 2006/05/19 14:49:10 bahren Exp $*/

#include <Functionals/StatisticsFilter.h>

namespace lopestools {

// ==============================================================================
//
//  Construction
//
// ==============================================================================

template <class T>
StatisticsFilter<T>::StatisticsFilter ()
  : strength_p (3)
{
  setFilterType (FilterType::MEDIAN);
}

template <class T>
StatisticsFilter<T>::StatisticsFilter (const unsigned int& strength)
  : strength_p (strength)
{
  setFilterType (FilterType::MEDIAN);
}

template <class T>
StatisticsFilter<T>::StatisticsFilter (const unsigned int& strength,
				       const FilterType::filter filterType)
  : strength_p (strength)
{
  setFilterType (filterType);
}

template <class T>
StatisticsFilter<T>::StatisticsFilter (const StatisticsFilter<T>& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

template <class T>
StatisticsFilter<T>::~StatisticsFilter ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

// -------------------------------------------------------------------- operator=

template <class T>
StatisticsFilter<T> &StatisticsFilter<T>::operator= (const StatisticsFilter<T>& other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ------------------------------------------------------------------------- copy

template <class T>
void StatisticsFilter<T>::copy (const StatisticsFilter<T>& other)
{
  filterType_p  = other.filterType_p;
  strength_p    = other.strength_p;
  blocksize_p   = other.blocksize_p;
  nofDataSets_p = other.nofDataSets_p;
}

// ---------------------------------------------------------------------- destroy

template <class T>
void StatisticsFilter<T>::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================


// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------------------ applyFilter

template <class T>
Vector<T> StatisticsFilter<T>::applyFilter (const Vector<T>& data)
{
  unsigned int i;
  Vector<T> result (blocksize_p);
  Matrix<T> tmp ((blocksize_p+strength_p),strength_p);

//   cout << "[StatisticsFilter<T>::applyFilter]" << endl;
//   cout << " - blocksize : " << blocksize_p << endl;
//   cout << " - strength  : " << strength_p << endl;
//   cout << " - result    : " << result.shape() << endl;
//   cout << " - tmp       : " << tmp.shape()<< endl;

  // Preparation of the data
  for (i=0; i<strength_p; i++) {
    tmp(Slice(i,blocksize_p),Slice(i)) = data;
  };

  // This is the only section, where we actually need to distinguish between
  // the different filter types; main computation handled by ArrayMath.
  switch (filterType_p) {
    case FilterType::MAXIMUM:
    for (i=0; i<blocksize_p; i++) {
      result(i) = max(tmp.row(i+strength_p/2));
    };
    break;
    case FilterType::MEAN:
    for (i=0; i<blocksize_p; i++) {
      result(i) = mean(tmp.row(i+strength_p/2));
    };
    break;
    case FilterType::MEDIAN:
    for (i=0; i<blocksize_p; i++) {
      result(i) = median(tmp.row(i+strength_p/2));
    };
    break;
    case FilterType::MINIMUM:
    for (i=0; i<blocksize_p; i++) {
      result(i) = min(tmp.row(i+strength_p/2));
    };
    break;
    case FilterType::RMS:
    for (i=0; i<blocksize_p; i++) {
      result(i) = sqrt(
		       sum(
			   (tmp.row(i+strength_p/2))
			   *(tmp.row(i+strength_p/2))
			   )/blocksize_p
		       );
    };
    break;
    case FilterType::STDDEV:
    for (i=0; i<blocksize_p; i++) {
      result(i) = stddev(tmp.row(i+strength_p/2));
    };
    break;
  }

  return result;
}

// ----------------------------------------------------------------------- filter

template <class T>
Vector<T> StatisticsFilter<T>::filter (const Vector<T>& data)
{
  blocksize_p = data.nelements();

  if ((strength_p < 1) ||(strength_p > blocksize_p)) {
    strength_p = 1;
    cerr << "[StatisticsFilter::filter] Filter strength set to 1" << endl;
  }

  return applyFilter (data);
}

// ----------------------------------------------------------------------- filter

template <class T>
Matrix<T> StatisticsFilter<T>::filter (const Matrix<T>& data)
{
  IPosition shape (data.shape());

  blocksize_p = shape(0);

  if ((strength_p < 1) ||(strength_p > blocksize_p)) {
    strength_p = 1;
    cerr << "[StatisticsFilter::filter] Filter strength set to 1" << endl;
  }

  Matrix<T> result (shape);

  try {
    for (int n(0); n<shape(1); n++) {
      result.column(n) = applyFilter (data.column(n));
    }
  } catch (AipsError x) {
    cerr << "[StatisticsFilter::filter]" << x.getMesg() << endl;
  }

  return result;
}

}
