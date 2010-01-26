//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Factory.h,v $
   $Revision: 1.1 $
   $Date: 2009/02/18 05:22:17 $
   $Author: straten $ */

#ifndef __MEAL_Factory_H
#define __MEAL_Factory_H

#include "Functor.h"
#include "stringtok.h"

#include <fstream>

namespace Factory {

  template< class T >
  T* load (const std::string& filename,
	   Functor< T*(std::string) >& constructor,
	   bool verbose = false)
  {
    std::ifstream input (filename.c_str());
    if (!input)
      throw Error (FailedSys, "Factory::load", "ifstream (" + filename + ")");

    std::string line;

    T* instance = 0;

    while (!input.eof()) {

      std::getline (input, line);
      line = stringtok (line, "#\n", false);  // get rid of comments

      if (!line.length())
	continue;

      if (!instance) {

	// the first key loaded should be the name of the instance
	std::string key = stringtok (line, " \t");

	if (verbose)
	  std::cerr << "Factory::load construct new " << key << std::endl;

	instance = constructor (key);
	
	if (!line.length())
	  continue;

      }

      if (verbose)
	std::cerr << "Factory::load parse line '" << line << "'" << std::endl;
      
      instance->parse (line);

    }

    return instance;
 
  }

}

#endif

