//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Effelsberg_h
#define __Effelsberg_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains Effelsberg receiver info
  namespace Effelsberg {

    //! Initialize the Receiver Extension for the UHF receiver
    void Rcvr_UHF (Receiver*);

    //! Initialize the Receiver Extension for the Multibeam receiver
    void Rcvr_Multibeam (Receiver*);

    //! Initialize the Receiver Extension for the 20cm receiver
    void Rcvr_20cm (Receiver*);

    //! Initialize the Receiver Extension for the 11cm receiver
    void Rcvr_11cm (Receiver*);

    //! Initialize the Receiver Extension for the 6cm receiver
    void Rcvr_6cm (Receiver*);

    //! Initialize the Receiver Extension for the 3.6cm receiver
    void Rcvr_3_6cm (Receiver*);

    //! Initialize the Receiver Extension with Effelsberg best guess
    void guess (Receiver*, Archive*);

  } 

}

#endif
