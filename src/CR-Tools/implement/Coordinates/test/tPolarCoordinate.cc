/*-------------------------------------------------------------------------*
 | $Id:: LOFAR_StationGroup.h 1291 2008-03-07 14:22:33Z baehren          $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Coordinates/PolarCoordinate.h>

/*!
  \file tPolarCoordinate.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for PolarCoordinate
 
  \author Lars B&auml;hren
 
  \date 2006/02/28
*/

using std::cerr;
using std::cout;
using std::endl;

using CR::PolarCoordinate;

// -----------------------------------------------------------------------------

/*!
  \brief Show a summary of the internal values of the PolarCoordinate object

  The output will e.g. look like this:
  \verbatim
  - Summary of polar coordinate
  -- (r,theta) : (1.41421,45)
  -- (x,y)     : (1,1)
  \endverbatim
 */
void show_PolarCoordinate (PolarCoordinate& pc)
{
  vector<double> xy (pc.cartesian());

  cout << " - Summary of polar coordinate" << endl;
  cout << " -- (r,theta) : (" << pc.radius() << "," << pc.theta() << ")" << endl;
  cout << " -- (x,y)     : (" << xy[0] << "," <<xy[1] << ")" << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new PolarCoordinate object

  \return nofFailedTests -- The number of failed tests.
*/
int test_PolarCoordinate ()
{
  int nofFailedTests (0);

  cout << "\n[test_PolarCoordinate]\n" << endl;

  cout << "[1] Default constructor" << endl;
  {
    PolarCoordinate pc;
    show_PolarCoordinate (pc);
  }

  cout << "[2] Construction from (r,theta)" << endl;
  {
    double r (1.0);
    double theta (45.0);

    cout << " - (r,theta) : (" << r << "," << theta << ")" << endl;

    PolarCoordinate pc (r,theta);
    show_PolarCoordinate (pc);
  }
  
  cout << "[3] Construction from cartesian coordinate (x,y)" << endl;
  {
    vector<double> xy (2);

    xy[0] = xy[1] =1.0;

    cout << " - (x,y) : (" << xy[0] << "," << xy[1] << ")" << endl;

    PolarCoordinate pc (xy);
    show_PolarCoordinate (pc);
  }
  
  cout << "[4] Construction from complex number" << endl;
  {
    complex<double> z (2.0,2.0);

    cout << " - z : " << z << endl;

    PolarCoordinate pc (z);
    show_PolarCoordinate (pc);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test operator overloading for a PolarCoordinate object
 */
int test_operators ()
{
  int nofFailedTests (0);

  cout << "\n[test_operators]\n" << endl;

  vector<double> xy (2);
  xy[0] = xy[1] =1.0;
  PolarCoordinate pc1 (xy);
  PolarCoordinate pc2 (xy);
  
  cout << "[1] Testing operator+" << endl;
  {
    PolarCoordinate pc;
    
    pc = pc1 + pc2;

    show_PolarCoordinate (pc1);
    show_PolarCoordinate (pc2);
    show_PolarCoordinate (pc);
  }

  cout << "[2] Testing operator+=" << endl;
  {
    PolarCoordinate pc;

    pc += pc1;

    show_PolarCoordinate (pc1);
    show_PolarCoordinate (pc);
  }
  
  cout << "[3] Testing operator-" << endl;
  {
    PolarCoordinate pc;
    
    pc = pc1 - pc2;

    show_PolarCoordinate (pc1);
    show_PolarCoordinate (pc2);
    show_PolarCoordinate (pc);
  }
  
  cout << "[4] Testing operator-=" << endl;
  {
    PolarCoordinate pc;

    pc -= pc1;

    show_PolarCoordinate (pc1);
    show_PolarCoordinate (pc);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_positionErrors () 
{
  int nofFailedTests (0);

  cout << "\n[test_positionErrors]\n" << endl;

  vector<double> xy (2);
  double dRadius (0.5);
  double dTheta;

  xy[0] = 1.0;
  xy[1] = 0.5;

  cout << "[1] Test case with well defined geometry" << endl;
  {
    PolarCoordinate pc (xy);
    dTheta = pc.dTheta(dRadius);
    
    show_PolarCoordinate (pc);
    cout << " -- d(radius) : " << dRadius << endl;
    cout << " -- d(theta)  : " << dTheta << endl;
  }
  
  xy[0] = 0.5;
  xy[1] = 0.5;

  cout << "[2] Test case with well defined geometry" << endl;
  {
    PolarCoordinate pc (xy);
    dTheta = pc.dTheta(dRadius);
    
    show_PolarCoordinate (pc);
    cout << " -- d(radius) : " << dRadius << endl;
    cout << " -- d(theta)  : " << dTheta << endl;
  }

  // Below are actual positions: located lightning events from the SAFIR system
  cout << "[3] Common lightning events for SAFIR and LOFAR ITS" << endl;
  {
    int nofPoints (3);
    vector<double> r (nofPoints);
    vector<double> theta (nofPoints);
    vector<double> dR (nofPoints);

    r[0] = 141.24; dR[0] = 1.470; theta[0] = 148.2;
    r[1] = 127.75; dR[1] = 0.890; theta[1] = 300.5;
    r[2] = 137.67; dR[2] = 1.910; theta[2] = 150.3;

    for (int n(0); n<nofPoints; n++) {
      PolarCoordinate pc (r[n],theta[n]);
      //
      cout << "\tn = " << n
	   << "\tr = " << r[n] << "+-" << dR[n] << " [km]"
	   << "\ttheta = " << theta[n] << "+-" << pc.dTheta(dR[n]) << " [deg]"
	   << endl;
    } 
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Tests for the constructor(s)
  {
    nofFailedTests += test_PolarCoordinate ();
  }

  // Tests for the operators
  {
    nofFailedTests += test_operators ();
  }

  // Tests for computation of position errors
  {
    nofFailedTests += test_positionErrors ();
  }

  return nofFailedTests;
}
