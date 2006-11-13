//#  Exception.cc: implementation of the SINFONI Exception class
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
//#  $Id: Exception.cc,v 1.2 2006/05/19 16:03:23 loose Exp $

#include <Sinfoni/Exception.h>

namespace ASTRON
{
  namespace SINFONI 
  {

    Exception::Exception(const std::string& text, const std::string& file,
                         int line, const std::string& func) :
      itsText(text), itsFile(file), itsLine(line), itsFunction(func)
    {
    }  
  
    Exception::~Exception() throw()
    {
    }

    const std::string Exception::message() const
    {
      std::ostringstream oss;

      oss << "[" << type() << ": " << text() << "]\n"
          << "in function " << (function().empty() ? "??" : function()) << "\n"
          << "(" << (file().empty() ? "??" : file()) << ":" << line() << ")\n";

      return oss.str();
    }
  
    const char* Exception::what() const throw()
    {
      return text().c_str();
    }

  } // namespace SINFONI

} // namespace ASTRON
