//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Differentiate.h,v $
   $Revision: 1.1 $
   $Date: 2007/09/24 09:38:30 $
   $Author: straten $ */

#ifndef __Pulsar_Differentiate_h
#define __Pulsar_Differentiate_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile differentiation algorithm
  class Differentiate : public Transformation<Profile> {

  public:

    //! Default constructor
    Differentiate ();

    //! Destructor
    ~Differentiate ();

    //! Set the number of phase bins in the span used to smooth
    void set_span (unsigned);

    //! Get the number of phase bins in the span used to smooth
    unsigned get_span () const;

    //! Differentiate the given Profile
    void transform (Profile* profile);

  protected:

    //! The number of phase bins in the window used to smooth
    unsigned span;

  };

}

#endif
