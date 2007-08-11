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

#include <<newModule>/<newClass>.h>

using <newNamespace>::<newClass>;  // Namespace usage

/*!
  \file t<newClass>.cc

  \ingroup <newModule>

  \brief A collection of test routines for <newClass>
 
  \author <author>
 
  \date <date>
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new <newClass> object

  \return nofFailedTests -- The number of failed tests.
*/
int test_<newClass> ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_<newClass>]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    <newClass> new<newClass>;
    //
    new<newClass>.summary(); 
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
    nofFailedTests += test_<newClass> ();
  }

  return nofFailedTests;
}
