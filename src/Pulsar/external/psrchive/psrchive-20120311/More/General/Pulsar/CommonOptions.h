//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/CommonOptions.h,v $
   $Revision: 1.2 $
   $Date: 2008/01/17 22:05:05 $
   $Author: straten $ */

#ifndef __Pulsar_CommonOptions_h
#define __Pulsar_CommonOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Common command line options
  class CommonOptions : public Application::Options
  {
  public:

    //! Default constructor
    CommonOptions ();

    //! Additional usage information implemented by derived classes
    virtual std::string get_usage ();

    //! Additional getopt options
    virtual std::string get_options ();

    //! Parse a non-standard command
    virtual bool parse (char code, const std::string& arg);

    //! Preprocessing tasks implemented by partially derived classes
    virtual void process (Archive*);

  private:

    bool tscrunch, fscrunch, pscrunch;

  };

}

#endif
