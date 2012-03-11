//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Nancay_h
#define __Nancay_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains Nancay-specifc info
  namespace Nancay {

    //! Init the Receiver Extension with Nancay info
    void L_band(Receiver *r);

    //! Initialize the Receiver Extension with Nancay best guess
    void guess (Receiver* r, Archive* a);

  } 

}

#endif
