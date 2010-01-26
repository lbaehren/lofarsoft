//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Accumulate.h,v $
   $Revision: 1.2 $
   $Date: 2008/08/22 04:50:46 $
   $Author: straten $ */

#ifndef __Pulsar_Accumulate_h
#define __Pulsar_Accumulate_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile differentiation algorithm
  class Accumulate : public Transformation<Profile> {

  public:

    //! Default constructor
    Accumulate ();

    //! Destructor
    ~Accumulate ();

    //! Set the offset
    void set_offset (unsigned);

    //! Get the offset
    unsigned get_offset () const;

    //! Set the baseline
    void set_baseline (double);

    //! Get the baseline
    double get_baseline () const;

    //! Form the cummulative profile
    void transform (Profile* profile);

  protected:

    //! The offset
    unsigned offset;

    //! The baseline
    double baseline;

  };

}

#endif
