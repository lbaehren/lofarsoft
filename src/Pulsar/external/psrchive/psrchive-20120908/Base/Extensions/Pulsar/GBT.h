//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __GBT_h
#define __GBT_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains GBT receiver info
  namespace GBT {

    //! Initialize the Receiver Extension with 350Mhz PF
    void Rcvr_350(Receiver *r);

    //! Initialize the Receiver Extension with 800Mhz PF
    void Rcvr_800(Receiver *r);

    //! Initialize the Receiver Extension with L-band
    void Rcvr1_2(Receiver *r);

    //! Initialize the Receiver Extension with S-band
    void Rcvr2_3(Receiver *r);

    //! Initialize the Receiver Extension with GBT best guess
    void guess (Receiver* r, Archive* a);

  } 

}

#endif
