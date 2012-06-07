//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/unload_text.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/15 06:55:52 $
   $Author: straten $ */

#ifndef __unload_text_h
#define __unload_text_h

#include "FITSError.h"
#include "FilePtr.h"

#include <fitsio.h>
#include <string.h>
#include <vector>
#include <iostream>

template<class T>
void unload_text (fitsfile* fptr, const char* table, const char* column,
		  const T* instance, bool verbose = false)
{
  // Move to the HDU named table
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, const_cast<char*>(table), 0, &status);
  
  if (status)
    throw FITSError (status, "unload_text", 
                     "fits_movnam_hdu %s", table);

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, const_cast<char*>(column),
		   &colnum, &status);

  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (typecode != TSTRING)
    throw Error (InvalidState, "unload_text",
		 "%s typecode != TSTRING", column);

  std::vector<char> text (repeat);
  char* temp = &(text[0]);

  FilePtr stream = tmpfile();
  if (!stream)
    throw Error (FailedSys, "unload_text", "tmpfile");

  try
  {
    instance->unload (stream);
  }
  catch (Error& error)
  {
    throw error += "unload_text";
  }

  // seek back to the start of the file
  fseek (stream, 0, SEEK_SET);

  int row = 0;
  while (fgets (temp, repeat, stream) == temp)
  {    
    row ++;

    // get rid of the newline
    char* newline = strchr (temp, '\n');
    if (newline)
      *newline = '\0';

    fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &temp, &status);

    if (status)
      throw FITSError (status, "polynomial::unload",
		       "fits_write_col row=%d", row);
  }

  if (verbose)
    std::cerr << "unload_text wrote " << row << " rows" << std::endl;
}

#endif
