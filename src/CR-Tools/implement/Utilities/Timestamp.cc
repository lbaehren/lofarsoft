/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (lbaehren@yahoo.de)                                      *
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

#include <iostream>
#include <Utilities/Timestamp.h>

namespace CR {

/*!
  \class Timestamp
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

Timestamp::Timestamp ()
{
  time (&time_p);
  
  setTime();
}

Timestamp::Timestamp (Timestamp const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

Timestamp::~Timestamp ()
{
  destroy();
}

void Timestamp::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

Timestamp& Timestamp::operator= (Timestamp const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void Timestamp::copy (Timestamp const &other)
{
  time_p = other.time_p;
  
  year_p  = other.year_p;
  month_p = other.month_p;
  day_p   = other.day_p;

  hour_p   = other.hour_p;
  minute_p = other.minute_p;
  second_p = other.second_p;

  dayOfWeek_p   = other.dayOfWeek_p;
  nameOfMonth_p = other.nameOfMonth_p;
}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void Timestamp::setTime (time_t const &timer)
{
  time_p = timer;
  setTime();
}

void Timestamp::setTime()
{
  std::string date        (ctime(&time_p));
  std::string dayOfWeek   ("day");
  std::string nameOfMonth ("mon");
  std::string year        ("yyyy");
  std::string day         ("dd");
  std::string hour        ("hh");
  std::string minute      ("mm");
  std::string second      ("ss");

  dayOfWeek[0]  = date[0];
  dayOfWeek[1]  = date[1];
  dayOfWeek[2]  = date[2];

  nameOfMonth[0] = date[4];
  nameOfMonth[1] = date[5];
  nameOfMonth[2] = date[6];

  day[0]       = date[8];
  day[1]       = date[9];

  hour[0]      = date[11];
  hour[1]      = date[12];

  minute[0]    = date[14];
  minute[1]    = date[15];

  second[0]    = date[17];
  second[1]    = date[18];

  year[0]      = date[20];
  year[1]      = date[21];
  year[2]      = date[22];
  year[3]      = date[23];

  // store the time information
  setYear (year);
  setMonth (nameOfMonth);
  setDay (day);
  setDayOfWeek (dayOfWeek);
  setHour   (hour);
  setMinute (minute);
  setSecond (second);
}

// ---------------------------------------------------------------------- setYear

void Timestamp::setYear (std::string const &year)
{
  year_p = year;
}

// --------------------------------------------------------------------- setMonth

void Timestamp::setMonth (std::string const &month,
			    bool const &inputIsName)
{
  if (inputIsName) {
    if      (month == "Jan" || month == "January") {
      month_p       = "01";
      nameOfMonth_p = "Jan";
    }
    else if (month == "Feb" || month == "February") {
      month_p       = "02";
      nameOfMonth_p = "Feb";
    }
    else if (month == "Mar" || month == "March") {
      month_p       = "03";
      nameOfMonth_p = "Mar";
    }
    else if (month == "Apr" || month == "April") {
      month_p       = "04";
      nameOfMonth_p = "Apr";
    }
    else if (month == "May") {
      month_p       = "05";
      nameOfMonth_p = "May";
    }
    else if (month == "Jun" || month == "June") {
      month_p       = "06";
      nameOfMonth_p = "Jun";
    }
    else if (month == "Jul" || month == "July") {
      month_p="07";
      nameOfMonth_p = "Jul";
    }
    else if (month == "Aug" || month == "August") {
      month_p       = "08";
      nameOfMonth_p = "Aug";
    }
    else if (month == "Sep" || month == "September") {
      month_p       = "09";
      nameOfMonth_p = "Sep";
    }
    else if (month == "Oct" || month == "October")   month_p="10";
    else if (month == "Nov" || month == "November")  month_p="11";
    else if (month == "Dec" || month == "December")  month_p="12";
    else {
      std::cerr << "[Timestamp::setMonth] "
		<< month 
		<< " is not a valid month identifier."
		<< std::endl;
    }
  }
}

// ----------------------------------------------------------------------- setDay

void Timestamp::setDay (std::string const &day)
{
  day_p = day;
}

// ----------------------------------------------------------------- setDayOfWeek

void Timestamp::setDayOfWeek (std::string const &day)
{
  if      (day == "Mon" || day == "Monday")    dayOfWeek_p = "Mon";
  else if (day == "Tue" || day == "Tuesday")   dayOfWeek_p = "Tue";
  else if (day == "Wed" || day == "Wednesday") dayOfWeek_p = "Wed";
  else if (day == "Thu" || day == "Thursday")  dayOfWeek_p = "Thu";
  else if (day == "Fri" || day == "Friday")    dayOfWeek_p = "Fri";
  else if (day == "Sat" || day == "Saturday")  dayOfWeek_p = "Sat";
  else if (day == "Sun" || day == "Sunday")    dayOfWeek_p = "Sun";
  else {
      std::cerr << "[Timestamp::setDayOfWeek] "
		<< day
		<< " is not a valid day identifier."
		<< std::endl;
  }
}

void Timestamp::setHour (std::string const &hour)
{
  hour_p = hour;
}

void Timestamp::setMinute (std::string const &minute)
{
  minute_p = minute;
}

void Timestamp::setSecond (std::string const &second)
{
  second_p = second;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

std::string Timestamp::hms (std::string const &sep)
{
  std::string timeOfDay ("");

  timeOfDay += hour_p;
  timeOfDay += sep;
  timeOfDay += minute_p;
  timeOfDay += sep;
  timeOfDay += second_p;
       
  return timeOfDay;
}

std::string Timestamp::ymd (std::string const &sep)
{
  std::string out ("");

  if (sep == "") {
    out  = year_p;
    out += month_p;
    out += day_p;
  } else {
    out  = year_p;
    out += sep;
    out += month_p;
    out += sep;
    out += day_p;
  }
    
  return out;
}

// ---------------------------------------------------------------------- iso8601

std::string Timestamp::iso8601 ()
{
  std::string iso;

  iso  = ymd();
  iso += "T";
  iso += hms();
  iso += ".00Z";

  return iso;
}

// ------------------------------------------------------------------ nanoblogger

std::string Timestamp::nanoblogger ()
{
  std::string date;

  date  = dayOfWeek_p;
  date += ", ";
  date += day_p;
  date += ". ";
  date += nameOfMonth_p;
  date += " ";
  date += year_p;
  date += " - ";
  date += hour_p;
  date += ":";
  date += minute_p;

  return date;
}

}
