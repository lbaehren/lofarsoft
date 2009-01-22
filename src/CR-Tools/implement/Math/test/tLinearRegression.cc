/***************************************************************************
 *   Copyright (C) 2005                                                    *
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
/* $Id$*/

#include <fstream>
#include <Math/LinearRegression.h>

/*!
  \file tLinearRegression.cc
  
  \ingroup CR_Math

  \brief A collection of test routines for LinearRegression
 
  \author Lars Baehren
 
  \date 2005/08/03
*/

// -----------------------------------------------------------------------------

/*!
  \brief Show the contents of the created LinearRegression object
  
  Display the values of the internal variables:
  \verbatim
  (a,b) = (1,2)
  y(x)  = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
  chisq = 0
  \endverbatim

  \param lin -- LinearRegression object
*/
void show_LinearRegression (LinearRegression<Float> lin)
{
  cout << " (a,b) = (" << lin.a() << "," << lin.b() << ")" << endl;
  cout << " y(x)  = " << lin.y() << endl;
  cout << " chisq = " << lin.chisq() << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Provide some real sample data for fitting.
*/
Vector<Float> example_data ()
{
  Float y[] = {0.0531764795, 1.17347522,  1.14976728,  0.384503697, 1.40723353,
	      1.21539044,   1.4547672,   1.40983837,  0.901485945, 0.593897401,
	      0.655114106,  0.597640621, 0.265222218, 0.453128763, 0.669882561,
	      0.854122224,  0.927600787, 0.971596453, 1.42041695,  0.879754609,
	      0.888949371,  1.03757755,  0.644073327, 0.248250415, 0.495196001,
	      0.375922571,  0.68606029,  0.828812534, 0.123985908, 0.535226382,
	      0.75707959,   1.19680321,  0.831793505, 2.52422904,  0.806579966,
	      0.0947893878, 0.74773088,  0.84039565,  0.544428743, 1.512878,
	      0.867915396,  1.17834586,  1.00839173,  0.592085484, 0.589209353,
	      1.03222445,   1.56016933,  0.989786074, 1.02964115,  1.51119852,
	      1.68536771,   0.313017016, 0.794183103, 0.831792775, 0.213412071,
	      0.657337574,  1.48329455,  0.905621773, 0.554897732, 1.42259566,
	      0.646848961,  0.696654637, 0.754453794, 0.666503824, 0.682037666,
	      1.62980536,   1.93873094,  0.614583471, 0.283031082, 1.00030571,
	      0.107799883,  0.671886006, 0.991833852, 1.59475559,  0.723973789,
	      1.49343166,   1.11969247,  0.507817911, 0.485046257, 1.42727701,
	      0.738715488,  0.743512662, 0.782151216, 0.767350814, 1.81670105,
	      0.853836289,  0.227669725, 0.870169931, 0.618600254, 0.714683716,
	      0.973140701,  0.62721375,  1.02815795,  1.59803879,  0.684330113,
	      0.446803045,  0.555408659, 1.60056327,  2.00796943,  1.39058963,
	      0.11037502,   1.14050738,  0.709748799, 1.18652929,  0.583233185,
	      0.398554151,  0.33529415,  0.939581235, 1.22079265,  0.823695722,
	      0.461100077,  0.540441657, 0.642730453, 1.34473597,  1.34174315,
	      0.408948361,  1.34907416,  0.602897604, 0.224409359, 1.17609159,
	      0.402199621,  0.693213345, 0.816660087, 1.38252059,  0.52015737,
	      2.09618669,   0.276957419, 0.99985327,  0.754538366, 2.26523176,
	      1.09766461,   1.17552611,  0.671897004, 0.765993965, 0.957103975,
	      1.66703406,   0.937084934, 0.741282551, 1.34950875,  0.860289816,
	      1.13347645,   0.522457986, 2.53787944,  1.346187,    1.01645921,
	      0.309526498,  1.07058239,  1.15288779,  0.25452078,  0.633613801,
	      2.01993283,   1.0224014,   0.976322379, 0.407982751, 0.0465989491,
	      0.749159682,  0.557426157, 0.969399335, 1.12974347,  0.426578813,
	      1.87294417,   1.38245795,  0.999702828, 0.796200762, 1.24445464,
	      0.858044711,  0.809494508, 1.20040703,  0.795327036, 2.24044992,
	      0.991282178,  1.5108546,   2.23533178,  0.33573346,  0.866550149,
	      0.187098049,  1.32326148,  0.873538733, 1.34640417,  1.53116975,
	      0.650292357,  0.946174356, 0.859020804, 0.89737188,  0.817272162,
	      0.599823182,  0.675655253, 0.579936217, 0.644873809, 0.319041674,
	      0.568179946,  2.25282422,  2.06905581,  1.09724595,  0.901612743,
	      1.438116,     0.979270525, 0.183940851, 1.20129263, 0.15169142};

  Vector<Float> yVec (200);

  for (int i(0); i<200; i++) {
    yVec(i) = y[i];
  }

  return yVec;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new LinearRegression object
*/
Int test_LinearRegression ()
{
  Int nofFailedTests (0);
  Int nofPoints (10);
  Vector<Float> x (nofPoints);
  Vector<Float> y (nofPoints);

  for (int i(0); i<nofPoints; i++) {
    x(i) = i*0.5;
    y(i) = 2*x(i)+1.0;
  }
  
  cout << "\n[tLinearRegression] Testing constructors\n" << endl;

  cout << "[1] LinearRegression<Float>" << endl;
  try {
    LinearRegression<Float> lin;
    show_LinearRegression (lin);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] LinearRegression<Float> (Vector<Float>,Vector<Float>)" << endl;
  try {
    LinearRegression<Float> lin (x,y);
    show_LinearRegression (lin);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Compute linear regression for a real set of data.

  This will test the fitting routine on a sample of real data, taken form an
  ITS data set. The original data points and the fit are exported to a simple 
  ASCII table for later plotting.

  <img src="../figures/tLinearRegression.png">
*/
Int test_fit ()
{
  Int nofFailedTests (0);
  Vector<Float> y (example_data());
  Int nelements (y.nelements());

  Vector<Float> x (nelements);
  Vector<Float> fit (nelements);

  indgen (x);

  try { 
    LinearRegression<Float> lin (x,y);
    show_LinearRegression (lin);
    //
    fit = lin.y();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // -----------------------------------------------------------------
  // Export the result of the fitting test to simple ASCII table for 
  // later plotting

  ofstream logfile;
  logfile.open ("tLinearRegression.dat",ios::out);
  
  for (int n(0); n<nelements; n++) {
    logfile << n << "\t" << y(n) << "\t" << fit(n) << endl;
  }
  
  logfile.close();
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_LinearRegression ();
  }

  {
    nofFailedTests += test_fit ();
  }

  return nofFailedTests;
}
