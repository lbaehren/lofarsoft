/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Baehren (baehren@astron.nl)                                      *
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

/* $Id: tPluginBase.cc,v 1.4 2006/07/12 10:13:16 bahren Exp $*/

#include <Calibration/PluginBase.h>

/*!
  \file tPluginBase.cc

  \ingroup Calibration

  \brief A collection of test routines for PluginBase
 
  \author Lars B&auml;hren
 
  \date 2006/03/03
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new PluginBase object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_PluginBase ()
{
  Int nofFailedTests (0);

  cout << "\n[test_PluginBase]\n" << endl;

  cout << "[1] Testing the default constructor" << endl;
  {
    PluginBase<Int> plugin_int;
    PluginBase<Float> plugin_float;
    PluginBase<Double> plugin_double;
  }

  cout << "[2] Testing the argumented constructor" << endl;
  {
    // int
    {
      Matrix<int> weights (10,10,1);
      PluginBase<Int> plugin (weights);
    }
    // float
    {
      Matrix<float> weights (10,10,1);
      PluginBase<Float> plugin (weights);
    }
    // double
    {
      Matrix<double> weights (10,10,1);
      PluginBase<Double> plugin (weights);
    }
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_PluginBase ();
  }

  return nofFailedTests;
}
