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

#include <Math/BasicFilter.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                  BasicFilter
  
  template <class T>
  BasicFilter<T>::BasicFilter ()
  {
    Vector<T> weights (1,1);
    setWeights(weights);
  }
  
  //_____________________________________________________________________________
  //                                                                  BasicFilter
  
  template <class T>
  BasicFilter<T>::BasicFilter (const unsigned int& blocksize)
  {
    Vector<T> weights (blocksize,1);
    setWeights(weights);
  }
  
  //_____________________________________________________________________________
  //                                                                  BasicFilter
  
  template <class T>
  BasicFilter<T>::BasicFilter (const Vector<T>& weights)
  {
    setWeights(weights);
  }
  
  //_____________________________________________________________________________
  //                                                                  BasicFilter
  
  template <class T>
  BasicFilter<T>::BasicFilter (const BasicFilter<T>& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  /* everything defined as inline */
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  template <class T>
  void BasicFilter<T>::copy (BasicFilter<T> const& other)
  {
    blocksize_p = other.blocksize_p;
    //
    weights_p.resize (blocksize_p);
    weights_p = other.weights_p;
  }
  
  //_____________________________________________________________________________
  //                                                                      destroy
  
  template <class T>
  void BasicFilter<T>::destroy ()
  {;}
  
  //_____________________________________________________________________________
  //                                                                    operator=
  
  template <class T>
  BasicFilter<T> &BasicFilter<T>::operator= (BasicFilter<T> const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  /* everything defined as inline */
  
  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================
  
  template class BasicFilter<casa::Int>;
  template class BasicFilter<casa::Float>;
  template class BasicFilter<casa::Double>;
  template class BasicFilter<casa::Complex>;
  template class BasicFilter<casa::DComplex>;
  
}  // Namespace CR -- end
