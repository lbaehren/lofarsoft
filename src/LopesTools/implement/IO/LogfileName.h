/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: LogfileName.h,v 1.2 2006/04/26 10:21:36 bahren Exp $*/

#ifndef _LOGFILENAME_H_
#define _LOGFILENAME_H_

// C++ Standard library
#include <iomanip>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Time.h>

#include <casa/namespace.h>

namespace lopestools {

/*!
  \class LogfileName.h
  
  \ingroup IO

  \brief Help for putting together the name of a logfile.

  \author Lars B&auml;hren

  \date 2006/01/09

  \test tLogfileName.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  In order to (a) avoid overwriting of previous logfiles and (b) speed up search
  for a specific logfile it turns out useful to include a timestamp into the
  filename. Typically the complete filename is assembled according to
  \verbatim
  <PREFIX><SEP><TIMESTAMP>.<SUFFIX>
  \endverbatim
  where 
  - <tt>PREFIX</tt> is a constant prefix for the filename,
  - <tt>SEP</tt> is a separator (binding string) inserted in between the various
  building blocks of the filename,
  - <tt>TIMESTAMP</tt> is a timestamp of format <i>yyyy.mm.dd-hh:mm</i>, and
  - <tt>SUFFIX</tt> is a filetype identifier appended.

  <h3>Example(s)</h3>

  \verbatim
  logfile-2006.01.09-15:51.log
  \endverbatim
*/

class LogfileName {

  //! The base-string of the filename (prefix)
  String prefix_p;
  //! The ending of the filename (suffix)
  String suffix_p;
  //! Timestamp
  String timestamp_p;
  //! String used as separator between the substrings
  String separator_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  LogfileName ();

  /*!
    \brief Argumented constructor

    \param prefix -- base-string of the filename (prefix)
  */
  LogfileName (const String& prefix);

  /*!
    \brief Argumented constructor

    \param prefix -- base-string of the filename (prefix)
    \param suffix -- ending of the filename (suffix)
  */
  LogfileName (const String& prefix,
	       const String& suffix);

  /*!
    \brief Copy constructor

    \param other -- Another LogfileName object from which to create this new
                    one.
  */
  LogfileName (LogfileName const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~LogfileName ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another LogfileName object from which to make a copy.
  */
  LogfileName &operator= (LogfileName const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the prefix used for the filename

    \return prefix -- base-string of the filename (prefix)
  */
  String prefix () {
    return prefix_p;
  }

  /*!
    \brief Set the prefix used for the filename

    \param prefix -- base-string of the filename (prefix)
  */
  void setPrefix (const String& prefix) {
    prefix_p = prefix;
  }

  /*!
    \brief Get the filename
    
    \return filename -- 
  */
  String filename ();

  // -------------------------------------------------------------------- Methods



 private:

  /*!
    \brief Initialization of internal parameters
   */
  void init ();

  /*!
    \brief Unconditional copying
  */
  void copy (LogfileName const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Create timestamp to be used as part of the filename
   */
  void setTimestamp ();

  /*!
    \brief Put the filename together
  */
  void setFilename ();

};

}

#endif /* _LOGFILENAME_H_ */
