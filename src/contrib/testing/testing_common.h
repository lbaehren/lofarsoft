/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <cmath>
#include <iostream>
#include <vector>

/*!
  \file testing_common.h

  \ingroup contrib

  \brief A set of common functions used by the various test programs

  \author Lars B&auml;hren
*/

//_______________________________________________________________________________
//                                                                        runtime

/*!
  \brief Compute runtime of an operation based on start and end time
  
  \param start -- Clock cycle counter at the start of the operation
  \param end   -- Clock cycle counter at the end of the operation

  \return runtime -- Time-interval between the moments the two values
          <tt>start</tt> and <tt>end</tt> were extracted.
*/
double runtime (clock_t const &start,
		clock_t const &end)
{
  return ((double)( end - start ) / (double)CLOCKS_PER_SEC );
}

//_______________________________________________________________________________
//                                                             nof_array_elements

/*!
  \brief Get a list of array sidelength values for setting up array shapes

  \return nelem -- Vector with a list of values used to set the size of an array,
          something like <tt>[10,20,50,100,200,500,1000,2000,5000,10000]</tt>
*/
std::vector<int> nof_array_elements ()
{
  std::vector<int> nelem;

/*   nelem.push_back(1); */
  nelem.push_back(2);
  nelem.push_back(5);
  nelem.push_back(10);
  nelem.push_back(20);
  nelem.push_back(50);
  nelem.push_back(100);
  nelem.push_back(200);
  nelem.push_back(500);
  nelem.push_back(1000);
  nelem.push_back(2000);
  nelem.push_back(5000);
  nelem.push_back(10000);

  return nelem;
}

//_______________________________________________________________________________
//                                                                number_sequence

std::vector<int> number_sequence () {
  // vector returning the result
  std::vector<int> nelem;
  // generate sequence
  nelem.push_back(1);
  nelem.push_back(2);
  nelem.push_back(5);
  nelem.push_back(10);
  nelem.push_back(20);
  nelem.push_back(50);
  nelem.push_back(100);
  nelem.push_back(200);
  nelem.push_back(500);
  nelem.push_back(1000);
  nelem.push_back(2000);
  nelem.push_back(5000);
  nelem.push_back(10000);
  // return sequence of numbers
  return nelem;
}
