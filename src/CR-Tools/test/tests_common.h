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
  \file tests_common.h

  \brief A set of common functions used by the various test programs

  \defgroup CR_test CR-Pipeline: Tests for required external components
  \ingroup CR

  \author Lars B&auml;hren
*/

// ------------------------------------------------------------------------------

/*!
  \brief Compute runtime of an operation based on start and end time
  
  \param start -- 
  \param end   -- 

  \return runtime -- Time-interval between the moments the two values
          <tt>start</tt> and <tt>end</tt> were extracted.
*/
double runtime (clock_t const &start,
		clock_t const &end)
{
  return ((double)( end - start ) / (double)CLOCKS_PER_SEC );
}

// ------------------------------------------------------------------------------

/*!
  \brief Get a list of array sidelength values for setting up array shapes

  \return nelem -- Vector with a list of values used to set the size of an array,
          something like <tt>[10,20,50,100,200,500,1000,2000,5000,10000]</tt>
*/
std::vector<int> nof_array_elements ()
{
  std::vector<int> nelem (10);

  nelem[0] = 10;
  nelem[1] = 20;
  nelem[2] = 50;
  nelem[3] = 100;
  nelem[4] = 200;
  nelem[5] = 500;
  nelem[6] = 1000;
  nelem[7] = 2000;
  nelem[8] = 5000;
  nelem[9] = 10000;

  return nelem;
}
