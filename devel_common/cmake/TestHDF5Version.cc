/*-------------------------------------------------------------------------*
 | $Id:: HDF5Common.h 1628 2008-06-10 15:46:50Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <iostream>

#ifdef HAVE_H5PUBLIC_H
#include <H5public.h> 
#endif

int main ()
{
  int result         = 1;
  herr_t h5error     = 0;
  uint majorVersion  = 0;
  uint minorVersion  = 0;
  uint releaseNumber = 0;
  
  try {
    h5error = H5check_version (&majorVersion,
			       &minorVersion,
			       &releaseNumber);
  } catch (std::string message) {
    std::cerr << "[TestHDF5Version] Caught error getting library version!"
	      << std::endl;
    result = 2;
  } 

  if (!h5error) {
    std::cout << "maj="<< majorVersion
	      << ":min=" << minorVersion
	      << ":rel=" << releaseNumber
	      << ":" << std::endl;
  } else {
    result = 3;
  }
  
  return result;
}
