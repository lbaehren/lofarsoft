//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/file_cast.h,v $
   $Revision: 1.4 $
   $Date: 2009/06/03 00:25:35 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_FILE_CAST_H
#define __UTILS_UNITS_FILE_CAST_H

#include "FilePtr.h"
#include <memory>

template<typename T>
T* smart_const_cast (const T* x)
{
  return const_cast<T*> (x);
}

template<typename To, typename From>
To* file_cast (const From* from)
{
  FilePtr temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "file_cast", "tmpfile");
  
  from->unload (temp);
  rewind (temp);

  std::auto_ptr<To> to (new To);

  smart_const_cast(to.get())->load (temp);

  return to.release();
}

template<typename To, typename From>
To* dynamic_file_cast (From* from)
{
  To* to = dynamic_cast<To*>( from );
  if (to)
    return to;

  return file_cast<To>(from);
}

#endif
