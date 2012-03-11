//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/parse.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_FileParse_H
#define __MEAL_FileParse_H

#include "stringtok.h"
#include <fstream>

namespace MEAL {

  template< class T >
  void parse (const std::string& filename, T* instance)
  {
    std::ifstream input (filename.c_str());
    if (!input)
      throw Error (FailedSys, "MEAL::parse", "ifstream (" + filename + ")");

    std::string line;

    while (!input.eof()) {

      std::getline (input, line);
      line = stringtok (line, "#\n", false);  // get rid of comments

      if (!line.length())
	continue;

      instance->parse (line);

    }

  }

}

#endif

