//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Parkes.h,v $
   $Revision: 1.5 $
   $Date: 2008/07/04 23:40:36 $
   $Author: straten $ */

#ifndef __Parkes_h
#define __Parkes_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains Parkes-specific routines
  namespace Parkes {

    //! Initialize the Receiver Extension with Parkes Multibeam attributes
    void Multibeam (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes H-OH attributes
    void H_OH (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes 10/50 attributes
    void COAX10_50 (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes best guess
    void guess (Receiver* receiver, Archive* archive);

  } 

}

#endif
