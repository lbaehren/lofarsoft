//#  Exception.h: Sinfoni exception base class.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: Exception.h,v 1.3 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_EXCEPTION_H
#define SINFONI_EXCEPTION_H

#include <Sinfoni/Config.h>
#include <exception>
#include <string>
#include <sstream>

namespace ASTRON
{
  namespace SINFONI 
  {
    //
    // This is the base class for all SINFONI related exceptions.
    //
    class Exception : public std::exception
    {
    public:
      Exception(const std::string& text, const std::string& file="",
                int line=0, const std::string& func="");

      virtual ~Exception() throw();

      // Implementation of std::exception::what().
      // Returns the user-supplied text as C-style char array.
      virtual const char* what() const throw();

      // Return the class type of the exception.
      virtual const std::string& type() const {
        static const std::string itsType("Exception");
        return itsType;
      }

      // Return the user-supplied text
      const std::string& text() const {
        return itsText;
      }

      // Return the name of the file where the exception occurred.
      const std::string& file() const {
        return itsFile;
      }

      // Return the line number where the exception occurred.
      int line() const {
        return itsLine;
      }

      // Return the function name where the exception occurred.
      const std::string& function() const {
        return itsFunction;
      }

      // Return exception type, user-supplied text, function name,
      // filename, and line number as a formatted string.
      const std::string message() const;
    
    private:
      std::string itsText;
      std::string itsFile;
      int         itsLine;
      std::string itsFunction;

    };

    // Put the exception message into an ostream.
    inline std::ostream& operator<<(std::ostream& os, const Exception& ex)
    {
      return os << ex.message();
    }

  } // namespace SINFONI

} // namespace ASTRON


//
//  @name Useful macros for lazy people
//@{

//
//  Define the \c __HERE__ macro, using \c __PRETTY_FUNCTION__ or
//  \c __FUNCTION__ if available.
//
#if defined(HAVE_PRETTY_FUNCTION)
#define __HERE__ __FILE__,__LINE__,__PRETTY_FUNCTION__
#elif defined(HAVE_FUNCTION)
#define __HERE__ __FILE__,__LINE__,__FUNCTION__
#else
#define __HERE__ __FILE__,__LINE__
#endif

//
// Declare and define an exception class of type \c excp, which is derived
// from the exception class \c super.
//
#define EXCEPTION_CLASS(excp,super) \
class excp : public super \
{ \
public: \
  excp( const std::string& text, const std::string& file="", \
	int line=0, const std::string& function="" ) : \
    super(text, file, line, function) {} \
  virtual const std::string& type( void ) const \
  { \
    static const std::string itsType(#excp); \
    return itsType; \
  } \
};

//
// Throw an exception of type \c excp; use \c strm for the message. 
// Use this macro to insure that the  \c __HERE__ macro expands properly.
//
#define THROW(excp,strm) \
{ \
  std::ostringstream oss; \
  oss << strm; \
  throw excp(oss.str(),__HERE__); \
}

//@}

#endif
