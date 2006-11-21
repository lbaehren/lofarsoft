/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/*!
  \class  MyArray
*/

#include <Utilities/CMakeTesting.h>

#ifdef HAVE_CASA

template <class T>
MyArray<T>::MyArray ()
{
  casa::IPosition shape(1,1);
  vect_p.resize (shape);
  vect_p = 0;
}

template <class T>
MyArray<T>::MyArray (casa::Array<T> & vect) {
  setArray (vect);
}

template <class T>
MyArray<T>::~MyArray ()
{}

template <class T>
void MyArray<T>::setArray (casa::Array<T> & vect)
{
  vect_p.resize (vect.shape());
  vect_p = vect;
  //
  nofElements_p = vect.nelements();
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

// template class casa::Array<int>;
// template class casa::Array<unsigned int>;
// template class casa::Array<float>;
// template class casa::Array<double>;

template class MyArray<int>;
template class MyArray<unsigned int>;
template class MyArray<float>;
template class MyArray<double>;

#endif
