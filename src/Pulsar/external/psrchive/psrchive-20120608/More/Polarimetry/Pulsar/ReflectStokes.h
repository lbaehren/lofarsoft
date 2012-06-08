//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReflectStokes.h,v $
   $Revision: 1.4 $
   $Date: 2007/10/27 04:22:13 $
   $Author: straten $ */

#ifndef __Pulsar_ReflectStokes_h
#define __Pulsar_ReflectStokes_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Archive;

  class ReflectStokes : public Transformation<Archive> {

  public:
    //! Constructor
    ReflectStokes ();

    //! Perform reflections on Pulsar Archive
    void transform (Archive*);

    //! Add a reflection; stokes = 'q', 'u', or 'v'
    void add_reflection (char stokes);

  protected:

    //! The reflections
    char reflect;

  };

}

#endif
