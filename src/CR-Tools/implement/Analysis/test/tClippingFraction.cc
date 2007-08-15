/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron,nl)                                       *
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

/* $Id: tClippingFraction.cc,v 1.4 2005/08/01 13:23:25 bahren Exp $ */

/*!
  \file tClippingFraction.cc

  \brief A collection of tests for ClippingFraction.

  \author Lars B&auml;hren

  \date 2005/07/19

  <h3>Synopsis</h3>

  In order to some feeling for thhe number to be expected the following simple
  Glish script can be used on a data set:
  \code
  clippingLimits := [as_integer(data.head('ADCMinChann')),
                     as_integer(data.head('ADCMaxChann'))-1];
  fx := data.get('f(x)');
  print 'Clipping at max :',sum(clippingLimits[1] == fx);
  print 'Clipping at min :',sum(clippingLimits[1] == fx);
  \endcode
*/

#include <Analysis/ClippingFraction.h>

using std::cerr;
using std::cout;
using std::endl;

// --- Test constructors -------------------------------------------------------

/*!
  \brief Test ClippingFraction object constructors

  \return failedTests -- The number of failed tests; returns 0 if everything 
                         went fine.
 */
int test_ClippingFraction ()
{
  int failedTests (0);
  double fraction (0.5);
  Vector<double> limits (2);

  limits(0) = -1;
  limits(1) = 1;

  cout << "\n[tClippingFraction] Testing constructors\n" << endl;

  cout << "[1] Testing ClippingFraction<float>() ..." << endl;
  try {
    ClippingFraction<float> clip;
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[2] Testing ClippingFraction<float>() ..." << endl;
  try {
    ClippingFraction<double> clip;
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[3] Testing ClippingFraction(Vector<float>) ..." << endl;
  try {
    ClippingFraction<float> clip (Vector<float>limits);
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[4] Testing ClippingFraction(Vector<double>) ..." << endl;
  try {
    ClippingFraction<double> clip (limits);
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[5] Testing ClippingFraction(Vector<double>, double) ..." << endl;
  try {
    ClippingFraction<float> clip (Vector<float>limits, float(fraction));
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[6] Testing ClippingFraction(Vector<double>, double) ..." << endl;
  try {
    ClippingFraction<double> clip (limits, fraction);
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  return failedTests;
}

// --- Test operators ----------------------------------------------------------

/*!
  \brief Test operators for a ClippingFraction object.

  \return failedTests -- The number of failed tests; returns 0 if everything 
                         went fine.
*/
int test_operators ()
{
  int failedTests (0);

  cout << "\n[tClippingFraction] Testing operators\n" << endl;

  cout << "[1] Testing copy operator for <float> ..." << endl;
  try {
    ClippingFraction<float> clip;
    ClippingFraction<float> clip2 = clip;
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  cout << "[2] Testing copy operator for <double> ..." << endl;
  try {
    ClippingFraction<double> clip;
    ClippingFraction<double> clip2 = clip;
  } catch (std::string message) {
    cerr << message << endl;
    failedTests++;
  }

  return failedTests;
}

// --- Test computation --------------------------------------------------------

int test_eval ()
{
  int failedTests (0);
  int nofSamples = 21;
  Vector<float> data (nofSamples);
  ClippingFraction<float> clip;
  //
  bool isClipping (false);
  float clippingFraction (0.0);

  // Fill the data array 
  for (int n(0); n<nofSamples; n++) {
    data(n) = n*0.2-2.0;
  }

  cout << "\n[tClippingFraction] Testing computation\n" << endl;

  cout << "[1] Testing eval ..." << endl;
  {
    isClipping = clip.eval (data, clippingFraction);
    cout << " - Data values ..... : " << min(data) << " .. " << max(data) << endl;
    cout << " - Limits .......... : " << clip.limits() << endl;
    cout << " - Clipping fraction : " << clippingFraction
	 << " (" << clip.clipFraction() << ")" << endl;
    cout << " - Is clipping ..... : " << isClipping << endl;
  }

  cout << "[2] Testing eval ..." << endl;
  {
    clip.setLimits (-1.0, 3.0);
    isClipping = clip.eval (data, clippingFraction);
    cout << " - Data values ..... : " << min(data) << " .. " << max(data) << endl;
    cout << " - Limits .......... : " << clip.limits() << endl;
    cout << " - Clipping fraction : " << clippingFraction
	 << " (" << clip.clipFraction() << ")" << endl;
    cout << " - Is clipping ..... : " << isClipping << endl;
  }

  cout << "[3] Testing eval ..." << endl;
  {
    clip.setLimits (-1.0, 3.0);
    clip.setClipFraction(0.3);
    isClipping = clip.eval (data, clippingFraction);
    cout << " - Data values ..... : " << min(data) << " .. " << max(data) << endl;
    cout << " - Limits .......... : " << clip.limits() << endl;
    cout << " - Clipping fraction : " << clippingFraction
	 << " (" << clip.clipFraction() << ")" << endl;
    cout << " - Is clipping ..... : " << isClipping << endl;
  }

  return failedTests;
}

// --- Main function -----------------------------------------------------------

int main ()
{
  int retval (0);

  {
    retval += test_ClippingFraction ();
  }
  
  {
    retval += test_operators ();
  }

  {
    retval += test_eval ();
  }

  cout << "\n[tClippingFraction] Summary\n" << endl;
  cout << "Number of failed tests : " << retval << endl;
  
  return retval;
}
