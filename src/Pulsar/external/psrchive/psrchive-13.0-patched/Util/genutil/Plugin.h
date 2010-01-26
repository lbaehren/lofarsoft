//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Plugin.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Plugin_h
#define __Plugin_h

#include <vector>
#include <string>

namespace Registry {

  class Plugin {

  public:
    //! Verbosity flag
    static bool verbose;

    //! Null constructor
    Plugin () { }

    //! Load all shared object libraries in the specified directory
    Plugin (const char* path) { load (path); }

    //! Load all shared object libraries in the specified directory
    void load (const char* path);

    //! Convenience interface
    void load (const std::string& path) { load (path.c_str()); }

    //! List of successfully loaded DSOs
    std::vector<std::string> ok;

    //! List of unsuccessfully loaded DSOs
    std::vector<std::string> fail;

  };

}

#endif
