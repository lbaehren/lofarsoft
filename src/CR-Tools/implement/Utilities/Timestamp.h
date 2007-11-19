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

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdio.h>
#include <string>
#include <time.h>

namespace CR {

/*!
  \class Timestamp

  \ingroup CR_Utilities

  \brief Wrapper for the time information in its various formats

  \author Lars B&auml;hren

  \date 2006/07/16

  \test tTimestamp.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>Usage of <tt>time</tt> of the C++ STDLIB
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

  By the default the internal time is set to the time at which the object was
  created; thus 
  \code 
  Timestamp ts;
  //
  std::cout << " -- ymd         = " << ts.ymd()         << std::endl;
  std::cout << " -- hms         = " << ts.hms()         << std::endl;
  std::cout << " -- iso8601     = " << ts.iso8601()     << std::endl;
  std::cout << " -- Unix time   = " << ts.timer()       << std::endl;
  std::cout << " -- Nanoblogger = " << ts.nanoblogger() << std::endl;
  \endcode
  will result in something like (keep in mind you'll be running this at
  another instance in time):
  \verbatim
  -- ymd         = 2006-07-18
  -- hms         = 19:05:09
  -- iso8601     = 2006-07-18T19:05:09.00Z
  -- Unix time   = 1153242309
  -- Nanoblogger = Tue, 18. Jul 2006 - 19:05
  \endverbatim
*/

class Timestamp {

  time_t time_p;
  
  std::string year_p;
  std::string month_p;
  std::string day_p;

  std::string hour_p;
  std::string minute_p;
  std::string second_p;

  std::string dayOfWeek_p;
  std::string nameOfMonth_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  Timestamp ();

  /*!
    \brief Copy constructor

    \param other -- Another Timestamp object from which to create this new
                    one.
  */
  Timestamp (Timestamp const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~Timestamp ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another Timestamp object from which to make a copy.
  */
  Timestamp& operator= (Timestamp const &other); 

  // ----------------------------------------------------------------- Parameters

/*   void setTime (long const &unixTime); */

  /*!
    \brief Set the time

    \param timer -- Timer object
  */
  void setTime (time_t const &timer);

  /*!
    \brief Get the year
    
    \return year -- The year, <tt>yyyy</tt>.
   */
  std::string year () const {
    return year_p;
  }

  /*!
    \brief Set the year
    
    \param year -- The year, <tt>yyyy</tt>.
   */
  void setYear (std::string const &year);

  /*!
    \brief Get the month
    
    \return month -- The month, <tt>mm</tt>.
  */
  std::string month () const {
    return month_p;
  }

  /*!
    \brief Set the month

    \param month -- The month, either by (a) abbreviated name (e.g.
                    <tt>Jan</tt>), (b) full name (e.g. <tt>January</tt>, or
		    (c) as number (e.g. <tt>01</tt>). For the latter case, 
		    <tt>inputIsName</tt> has to be set to <tt>false</tt>.
    \param inputIsName -- Indicate wether or not the month is set via its
                          (abbreviated) name or by its number.
  */
  void setMonth (std::string const &month,
		 bool const &inputIsName=true);

  /*!
    \brief Set the day of the week

    \param day -- The day of the week, either by its three-letter abbreviation
                  or its full name (e.g. <tt>Mon</tt> or <tt>Monday</tt>).
  */
  void setDayOfWeek (std::string const &day);
  
  /*!
    \brief Get the day of the month

    \return day -- The day of the month, <tt>dd</tt>.
  */
  std::string day () const {
    return day_p;
  }
  
  /*!
    \brief Set the day of the month

    \param day -- The day of the month, <tt>dd</tt>.
  */
  void setDay (std::string const &day);
  
  /*!
    \brief Get the hour of day

    \return hour -- The hour of day, <tt>hh</tt>.
  */
  std::string hour () const {
    return hour_p;
  }

  /*!
    \brief Set the hour of day

    \param hour -- The hour of day, <tt>hh</tt>.
  */
  void setHour (std::string const &hour);
  
  /*!
    \brief Get the minutes within the hour

    \return minute -- The minutes within the hour, <tt>mm</tt>.
  */
  std::string minute () const {
    return minute_p;
  }

  /*!
    \brief Set the minutes within the hour

    \param minute -- The minutes within the hour, <tt>mm</tt>.
  */
  void setMinute (std::string const &minute);
  
  /*!
    \brief Get the seconds within the minute

    \return second -- The seconds within the minute, <tt>ss</tt>.
  */
  std::string second () const {
    return second_p;
  }

  /*!
    \brief Set the seconds within the minute

    \param second -- The seconds within the minute, <tt>ss</tt>.
  */
  void setSecond (std::string const &second);
  
  // -------------------------------------------------------------------- Methods

  /*!
    \brief Get the time as <tt>timer</tt> object

    \return timer -- The time as a <tt>timer</tt> object
  */
  time_t timer () const {
    return time_p;
  }

  /*!
    \brief Get string combining year, month and day

    \param sep -- Filling character to be inserted between year/month and
                  month/day. By default the filling character is set to
		  <tt>-</tt>, such that the returned string will be
		  <tt>yyyy-mm-dd</tt>.

    \return ymd -- Formatted string including year, month and day.
  */
  std::string ymd (std::string const &sep="-");

  /*!
    \brief Get the time of day

    \param sep -- Separator inserted between components of the time string;
                  by default the string is returned as <tt>hh:mm:ss</tt>

    \return timeOfWeek -- The time of the day, consisting of hour, minutes
                          and seconds.
  */
  std::string hms (std::string const &sep=":");

  /*!
    \brief Retrieve ISO 8601 conform version of the timestamp string
    
    \return iso -- ISO 8601 conform version of the timestamp string
   */
  std::string iso8601 ();

  /*!
    \brief Get time as formatted string as used by the Nanoblogger
    
    \return date -- Date as formatted string as inserted into the <tt>DATE</tt>
                    field of a
		    <a href="http://nanoblogger.sourceforge.net/">Nanoblogger</a>
		    entry file.
  */
  std::string nanoblogger ();

  // ------------------------------------------------------------ private methods

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (Timestamp const &other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Set the time variable
  */
  void setTime ();

};

}

#endif /* TIMESTAMP_H */
