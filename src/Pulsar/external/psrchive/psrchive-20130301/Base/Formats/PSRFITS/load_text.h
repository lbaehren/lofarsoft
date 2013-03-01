//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/load_text.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/15 06:55:52 $
   $Author: straten $ */

#ifndef __load_text_h
#define __load_text_h

#include "FITSError.h"
#include "FilePtr.h"

#include <fitsio.h>
#include <string.h>
#include <vector>
#include <iostream>

template<class T>
void load_text (fitsfile* fptr, const char* table, const char* column,
		T* instance, bool verbose = false)
{
  // Move to the HDU named table
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, const_cast<char*>(table), 0, &status);
  
  if (status)
    throw FITSError (status, "unload_text", 
                     "fits_movnam_hdu %s", table);

  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status)
    throw FITSError (status, "load_text", 
                     "fits_get_num_rows %s", table);

  if (!numrows)
    throw Error (InvalidParam, "load_text", "no rows in table");

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, const_cast<char*>(column), 
		   &colnum, &status);

  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (typecode != TSTRING)
    throw Error (InvalidState, "load_text",
		 "%s typecode != TSTRING", column);

  std::vector<char> text (repeat);
  char* temp = &(text[0]);

  FilePtr stream = tmpfile();

  for (int row=1; row <= numrows; row++)
  {
    int anynul = 0;
    fits_read_col (fptr, TSTRING, colnum, row, 1, 1, 0, 
                   &temp, &anynul, &status);

    if (status)
      throw FITSError (status, "load_text", "fits_read_col");

    char* newline = strchr (temp, '\n');
    if (newline)
      *newline = '\0';

    fprintf (stream, "%s\n", temp);
  }

  fseek (stream, 0, SEEK_SET);

  instance->load (stream);

  if (verbose)
  {
    std::cerr << "load_text loaded" << std::endl;
    instance->unload (stderr);
  }
}

#endif
