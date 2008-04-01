/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (<mail>)                                                 *
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

#include "Templates.h"

// ==============================================================================
//
//  Class VectorList
//
// ==============================================================================

template<class T> VectorList<T>::VectorList (T const &elem)
{
  elements_p.resize(1);
  elements_p[0] = elem;
}

template<class T> VectorList<T>::VectorList (vector<T> const &vect)
{
  elements_p.resize(vect.size());
  elements_p = vect;
}

template<class T> bool VectorList<T>::appendElement (T const &elem)
{
  bool status (true);

  try {
    elements_p.push_back(elem);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    status = false;
  }

  return status;
}

// ------------------------------------------------------- Template instantiation

template class VectorList<int>; 
template class VectorList<float>;
template class VectorList<double>;

