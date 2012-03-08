//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/ParametersLookup.h,v $
   $Revision: 1.1 $
   $Date: 2009/06/05 08:55:55 $
   $Author: straten $ */

#ifndef __PulsarParametersLookup_h
#define __PulsarParametersLookup_h

#include "Pulsar/Parameters.h"

namespace Pulsar {

  //! Looks for pulsar parameters in various places
  class Parameters::Lookup : public Reference::Able {

  public:

    //! Default constructor
    Lookup ();

    //! Factory returns new Parameters for specified pulsar name
    Parameters* operator() (const std::string& psrname) const;

    //! Add a directory to include in the search
    void add_path (const std::string&);

    //! Add a filename extension to include in the search
    void add_extension (const std::string&);

  protected:

    std::vector<std::string> path;
    std::vector<std::string> ext;

  };

}

#endif
