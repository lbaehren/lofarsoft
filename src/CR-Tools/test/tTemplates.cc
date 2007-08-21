/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) <year>                                                  *
 *   <author> (<mail>)                                                     *
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
#include <vector>

#include <Templates.h>

/*!
  \file tTemplates.cc

  \brief A collection of test routines for working with templates
 
  \author Lars B&auml;hren
 
  \date 2007/08/21
*/

// -----------------------------------------------------------------------------

int test_VectorList ()
{
  int nofFailedTests (0);

  std::cout << "[1] Test construction with single element ..." << std::endl;
  try {
    std::cout << "  -- VectorList<int>" << std::endl;
    int elemInt (1);
    VectorList<int> listInt (elemInt);
    //
    std::cout << "  -- VectorList<float>" << std::endl;
    float elemFloat (1);
    VectorList<float> listFloat (elemFloat);
    //
    std::cout << "  -- VectorList<double>" << std::endl;
    double elemDouble (1);
    VectorList<double> listDouble (elemDouble);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Test construction with vector of elements ..." << std::endl;
  try {
    uint nelem (10);
    //
    std::cout << "  -- VectorList<int>" << std::endl;
    vector<int> elemInt (nelem,1);
    VectorList<int> listInt (elemInt);
    //
    std::cout << "  -- VectorList<float>" << std::endl;
    vector<float> elemFloat (nelem,1);
    VectorList<float> listFloat (elemFloat);
    //
    std::cout << "  -- VectorList<double>" << std::endl;
    vector<double> elemDouble (nelem,1);
    VectorList<double> listDouble (elemDouble);
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

  nofFailedTests += test_VectorList ();

  return nofFailedTests;
}
