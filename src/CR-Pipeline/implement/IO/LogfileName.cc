/*-------------------------------------------------------------------------*
| $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
*-------------------------------------------------------------------------*
***************************************************************************
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

#include <IO/LogfileName.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

LogfileName::LogfileName ()
  : prefix_p ("logfile"),
    suffix_p ("log")
{
  init ();
}

LogfileName::LogfileName (const String& prefix)
  : prefix_p (prefix),
    suffix_p ("log")
{
  init ();
}

LogfileName::LogfileName (const String& prefix,
			  const String& suffix)
  : prefix_p (prefix),
    suffix_p (suffix)
{
  init ();
}

LogfileName::LogfileName (LogfileName const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

LogfileName::~LogfileName ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

LogfileName &LogfileName::operator= (LogfileName const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void LogfileName::copy (LogfileName const& other)
{
  prefix_p = other.prefix_p;
  suffix_p = other.suffix_p;
}

void LogfileName::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void LogfileName::init ()
{
  // Default values for the internal variables
  separator_p = "-";

  // fill the timestamp variable
  setTimestamp ();
}

String LogfileName::filename ()
{
  ostringstream filename;

  filename << prefix_p
	   << separator_p
	   << timestamp_p
	   << "."
	   << suffix_p;

  return filename.str();
}

void LogfileName::setTimestamp ()
{
  Time timestruct;
  uInt month (timestruct.month());
  uInt dayOfMonth (timestruct.dayOfMonth());
  uInt hours (timestruct.hours());
  uInt minutes (timestruct.minutes());

  ostringstream timestamp;
  
  // Add yyyy.mm.dd part of the filename

  timestamp << timestruct.year() << ".";
  //
  if (month < 10) {
    timestamp << "0";
  }
  timestamp << month << ".";
  //
  if (dayOfMonth < 10) {
    timestamp << "0";
  }
  timestamp << dayOfMonth;

  // Separator

  timestamp << separator_p;

  // Add the hh.mm part of the filename

  if (hours < 10) {
    timestamp << "0";
  }
  timestamp << timestruct.hours() << ":";
  //
  if (minutes < 10) {
    timestamp << "0";
  }
  timestamp << minutes;
  
  timestamp_p = timestamp.str();
}


// ==============================================================================
//
//  Methods
//
// ==============================================================================

void LogfileName::setFilename ()
{
}
