/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FilePtr.h"
#include "Error.h"

FilePtr::FilePtr (const std::string& filename, const std::string& mode)
{
  fptr = fopen (filename.c_str(), mode.c_str());
  if (!fptr)
    throw Error (FailedSys, "FilePtr::FilePtr",
		 "could not open " + filename + " for " + mode);
}
