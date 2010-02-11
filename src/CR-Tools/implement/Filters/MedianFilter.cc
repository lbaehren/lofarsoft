/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <Filters/MedianFilter.h>

// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------------------ applyFilter

template <class T>
Vector<T> MedianFilter<T>::applyFilter (const Vector<T>& data)
{
  int i;
  Vector<T> result (blocksize_p);
  Matrix<T> tmp ((blocksize_p+strength_p),strength_p);

//   cout << "[MedianFilter<T>::applyFilter]" << endl;
//   cout << " - blocksize : " << blocksize_p << endl;
//   cout << " - strength  : " << strength_p << endl;
//   cout << " - result    : " << result.shape() << endl;
//   cout << " - tmp       : " << tmp.shape()<< endl;
  
  for (i=0; i<strength_p; i++) {
    tmp(Slice(i,blocksize_p),Slice(i)) = data;
  };

  for (i=0; i<blocksize_p; i++) {
    result(i) = median(tmp.row(i+strength_p/2));
  };

  return result;
}

// ----------------------------------------------------------------------- filter

template <class T>
Matrix<T> MedianFilter<T>::filter (const Matrix<T>& data)
{
  IPosition shape (data.shape());

  blocksize_p = shape(0);

  if ((strength_p < 1) ||(strength_p > blocksize_p)) {
    strength_p = 1;
  }

  Matrix<T> result (data);

  try {
    for (int n(0); n<shape(1); n++) {
      result.column(n) = applyFilter (data.column(n));
    }
  } catch (AipsError x) {
    cerr << "[MedianFilter::filter]" << x.getMesg() << endl;
  }

  return result;
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

template class MedianFilter<Float>;
template class MedianFilter<Double>;
