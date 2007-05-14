/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

/* $Id: StringTools.h,v 1.4 2006/10/31 18:24:08 bahren Exp $ */

// C++ Standard library
#include <ostream>
#include <fstream>
#include <sstream>
using namespace std;

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/sstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/IO/AipsIO.h>

using casa::String;
using casa::Vector;

namespace CR {
  
  /*!
    \file StringTools.h

    \ingroup Utilities
    
    \brief A collection of routines for the manipulation of strings
    
    \author Lars B&auml;hren
    
    <h3>Prerequisites</h3>
    
    <ul type="square">
    <li>[C++] <a href="http://pleac.sourceforge.net/pleac_c++/strings.html">Characteristics of 'string' types</a>
    <li>[C] <a href="http://www.cplusplus.com/ref/cstring/index.html">string.h --
    standard C library to manipulate C strings</a>.
    </ul>
  */
  
  /*!
    \brief Convert a string to an array of characters
    
    \param myString  -- The string, which is converted to an array of characters.
    
    \return myChar   -- Array of characters, filled with the contents of the 
    provided string.
  */
  char* string2char (String const &mystring);
  
  /*!
    \brief Convert a string to an array of characters
    
    \param myString  -- The string, which is converted to an array of characters.
    \param length    -- Length (number of characters) after which the input string
                        will be cropped.
    
    \return myChar   -- Array of characters, filled with the contents of the 
                        provided string.
  */
  char* string2char (String const &mystring,
		     unsigned int const &length);
  
  /*!
    \param str - Input string in which the replacement(s)' taking place
    \param out - 
    \param in  - 
  */
  String replaceString (const String,
			const String,
			const String);
  
  /*!
    \brief Get the number of substrings in a string, divided by a specific separator

    \param str - Input string to be decomposed into substrings
    \param sep - Separator between the subsequent substrings
    \return num - The number of substring.
  */
  int nofSubstrings (std::string const &str,
		     std::string const &sep);
  
  /*!
    \brief Decompose a string into substrings, divided by a given separator
    
    \param str  - Input string to be decomposed into substrings
    \param sep  - Separator between the subsequent substrings
    \return sub - Vector containing the substrings.
  */
  std::vector<std::string> getSubstrings (std::string const &str,
					  std::string const &sep);

  /*!
    \brief Get the number of substrings in a string, divided by a specific separator

    \param str - Input string to be decomposed into substrings
    \param sep - Separator between the subsequent substrings

    \return num - The number of substring.
  */
  int nofSubstrings (String const &str,
		     String const &sep);
  
  /*!
    \brief Decompose a string into substrings, divided by a given separator
    
    \param str  - Input string to be decomposed into substrings
    \param sep  - Separator between the subsequent substrings
    \return sub - Vector containing the substrings.
  */
  Vector<String> getSubstrings (const String,
				const String);
  
  /*!
    \brief Extract the name of a file from its full path
    
    \param path      - Full path to a file.
    \return filename - The name of the file without the preceding directory
    information.
  */
  String fileFromPath (const String);
  
  /*!
    \brief Extract the directory in which a file is located from its full path.

    \param path     - Full path to a file.
    \return dirname - Directory in which the file is located
  */
  String dirFromPath (const String);
  
  /*!
    \brief Extract date from ISO 8601 timestamp

    \param iso   - String with timestamp in ISO 8601 format.
    \return date - Date, yy:mm
  */
  String dateFromISO8601 (const String);
  
  /*!
    \brief Extract time from ISO 8601 timestamp

    \param iso - String with timestamp in ISO 8601 format.
    \return time - Time, hh:mm:ss
  */
  String timeFromISO8601 (const String);
  
}

#endif
