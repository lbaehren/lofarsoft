/*-------------------------------------------------------------------------*
 | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                           $ |
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

#include <Math/BasicFilter.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

template <class T>
BasicFilter<T>::BasicFilter ()
{
  Vector<T> weights (1,1);
  setWeights(weights);
}

template <class T>
BasicFilter<T>::BasicFilter (const unsigned int& blocksize)
{
  Vector<T> weights (blocksize,1);
  setWeights(weights);
}

template <class T>
BasicFilter<T>::BasicFilter (const Vector<T>& weights)
{
  setWeights(weights);
}

template <class T>
BasicFilter<T>::BasicFilter (const BasicFilter<T>& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

template <class T>
BasicFilter<T>::~BasicFilter ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

// ------------------------------------------------------------ BasicFilter::copy

template <class T>
void BasicFilter<T>::copy (BasicFilter<T> const& other)
{
  blocksize_p = other.blocksize_p;
  //
  weights_p.resize (blocksize_p);
  weights_p = other.weights_p;
}

// --------------------------------------------------------- BasicFilter::destroy

template <class T>
void BasicFilter<T>::destroy ()
{;}

// ------------------------------------------------------- BasicFilter::operator=

template <class T>
BasicFilter<T> &BasicFilter<T>::operator= (BasicFilter<T> const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

// ------------------------------------------------------ BasicFilter::setWeights

template <class T>
void BasicFilter<T>::setWeights (const Vector<T>& weights)
{
  blocksize_p = weights.nelements();
  //
  weights_p.resize(blocksize_p);
  weights_p = weights;
}

// ---------------------------------------------------------- BasicFilter::filter

template <class T>
void BasicFilter<T>::filter (Vector<T>& data)
{
  // only apply if number of elements if correct
  if (blocksize_p == data.nelements()) {
    data *= weights_p;
  }
}

template <class T>
void BasicFilter<T>::filter (Vector<T> filteredData,
			     const Vector<T>& data)
{
  filteredData.resize (data.shape());
  //
  filteredData = data;
  filter (filteredData);
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

template class BasicFilter<Float>;
template class BasicFilter<Double>;
template class BasicFilter<Complex>;
template class BasicFilter<DComplex>;
