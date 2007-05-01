/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

using std::vector;

/*!
  \file tUseVector.cc

  \brief A number of simple tests for working with vectors

  \author Lars B&auml;hren

  \date 2007/03/13

*/

/*!
  \brief Provide a short summary of the vector
*/
template <class T> void show_vector (vector<T> &vec)
{
  uint nofElements (vec.size());
  double sum (0.0);

  // Sum of the vector elements
  for (uint n(0); n<nofElements; n++) {
    sum += double(vec[n]);
  }

  std::cout << "-- size(Vector) : " << vec.size() << std::endl;
  std::cout << "-- sum(Vector)  : " << sum        << std::endl;
}

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  uint length (20);

  try {
    vector<bool> vec (length);
    //
    vec[0] = vec [1] = true;
    //
    show_vector(vec);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ==============================================================================
//
// Template instantiation
//
// ==============================================================================

template void show_vector (vector<bool> &vec);
