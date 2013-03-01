//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Arecibo_h
#define __Arecibo_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains GBT receiver info
  namespace Arecibo {

    //! Initialize the Receiver Extension as 327 MHz (Gregorian)
    void Greg_327(Receiver *r);

    //! Initialize the Receiver Extension as 430 MHz (Gregorian)
    void Greg_430(Receiver *r);

    //! Initialize the Receiver Extension as 430 MHz (Line Feed)
    void CH_430(Receiver *r);

    //! Initialize the Receiver Extension as L-narrow
    void L_narrow(Receiver *r);

    //! Initialize the Receiver Extension as L-wide
    void L_wide(Receiver *r);

    //! Initialize the Receiver Extension as L-wide (old)
    void L_wide_old(Receiver *r);

    //! Initialize the Receiver Extension as S-wide (aka S-low)
    void S_wide(Receiver *r);

    //! Initialize the Receiver Extension as S-narrow (aka S-radar)
    void S_narrow(Receiver *r);

    //! Initialize the Receiver Extension as C-band
    void C_band(Receiver *r);

    //! Initialize the Receiver Extension with Arecibo best guess
    void guess (Receiver* r, Archive* a);

  } 

}

#endif
