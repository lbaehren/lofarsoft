/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FilePtr.h"
#include "Error.h"
#include "debug.h"

FilePtr::FilePtr (const std::string& filename, const std::string& mode)
{
  fptr = fopen (filename.c_str(), mode.c_str());
  if (!fptr)
    throw Error (FailedSys, "FilePtr::FilePtr",
		 "could not open " + filename + " for " + mode);
}

FilePtr::FilePtr (FILE* f)
{
  DEBUG("FilePtr ctor FILE* this=" << this);
  fptr = f;
}

FilePtr::~FilePtr ()
{
  DEBUG("FilePtr dtor this=" << this);
  close();
}

const FilePtr& FilePtr::operator = (FILE* f)
{
  DEBUG("FilePtr::operator= fptr=" << f);
  close();
  fptr = f;
  return *this;
}

void FilePtr::close ()
{
  DEBUG("FilePtr::close");

  if (fptr)
  {
    DEBUG("closing ptr=" << fptr);
    fclose (fptr);
  }

  fptr=0;
}

