/*-------------------------------------------------------------------------*
 | $Id:: tUseCFITSIO.cc 1059 2007-11-19 10:43:30Z baehren                $ |
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

#include <iostream>
#include <string>

// Python header files
#include <Python.h>
#include <modsupport.h>

// -----------------------------------------------------------------------------

/*!
  \file tUsePython.cc

  \ingroup CR_test

  \brief A number of tests to work with Python library

  \author Lars B&auml;hren

  \date 2008/06/25

  One of the things we need to be able to determine is the Python API version,
  which is defined in "modsupport.h" via
  \code
  #define PYTHON_API_VERSION 1013
  #define PYTHON_API_STRING "1013"
  \endcode
  Retrival of this information needs to be added to the CMake configuration
  in order to ensure we are not getting a mismatch when actually running scripts
  and applications.
*/

// -----------------------------------------------------------------------------

int get_api_version ()
{
  if (PYTHON_API_VERSION > 0) {
    return PYTHON_API_VERSION;
  } else {
    return 0;
  }
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  std::cout << "PYTHON_API_VERSION = " << get_api_version () << std::endl;
  
  return nofFailedTests;
}
