/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <Display/SimplePlot.h>

/*!
  \file tSimplePlot.cc

  \ingroup Display

  \brief A collection of test routines for SimplePlot
 
  \author Andreas Horneffer
 
  \date 2007/06/19
*/

// -----------------------------------------------------------------------------

using CR::SimplePlot;

/*!
  \brief Test constructors for a new SimplePlot object

  \return nofFailedTests -- The number of failed tests.
*/
int test_SimplePlot ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_SimplePlot]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    SimplePlot newObject;
    Vector<Double> xval(100), yval(100), errs, empty;
    indgen(xval);
    yval = square(xval);
    newObject.quickPlot("tSimplePlot-line.ps", xval, yval, empty, empty,
    			"X-axis", "Y-Axis", "plotting-test with lines",
    			4, True, 1, False, False, True);
    errs = yval * 0.1;
    newObject.quickPlot("tSimplePlot-symbol.ps", xval, yval, errs, empty,
    			"X-axis", "Y-Axis", "plotting-test with symbols",
    			2, False, 2, False, False, False);

    Matrix<Double> twoDvalues(3,10);
    int x,y,z=0;
    for (x=0; x<10; x++){
      for (y=0; y<3; y++){
	twoDvalues(y,x) = z++;
      };
    };

    newObject.quick2Dplot("tSimplePlot-2d.ps", twoDvalues, 0., 1., 0., 1.,	     
			  "X-axis", "Y-Axis", "2d plotting-test",
			  False, 30, 0, 0, 3);
    newObject.addContourLines(twoDvalues, 0., 1., 0., 1., 4, 3);    
    newObject.addContourLines(twoDvalues, 0., 1., 0., 1., 5, 4);    

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_SimplePlot ();
  }

  return nofFailedTests;
}
