//#  Assert.h: Assert macro that doesn't call abort but throws an exception.
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
//#  $Id: Assert.h,v 1.5 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_ASSERT_H
#define SINFONI_ASSERT_H

#include <Sinfoni/Config.h>
#include <Sinfoni/Exception.h>


namespace ASTRON
{
  namespace SINFONI
  {
    //
    // This exception will be thrown when an assertion fails.
    //
    EXCEPTION_CLASS(AssertError, Exception);
  }
}

//
// Define the Assert macro as a C++ alternative to the C assert macro. The
// Assert macro will not call abort, but will throw an exception of type
// ASTRON::SINFONI::AssertError. 
//
#define Assert(cond) \
{ \
  if ( !(cond) ) \
    THROW (ASTRON::SINFONI::AssertError, "Assertion `" #cond "' failed.") \
}

//
// The AssertStr macro is similar to the Assert macro. However, with AssertStr
// you can put extra information into the exception message.
//
#define AssertStr(cond,stream) \
{ \
  if ( !(cond) ) { \
     THROW (ASTRON::SINFONI::AssertError, \
            "Assertion `" #cond "' failed. " << stream); \
  } \
}

#endif
