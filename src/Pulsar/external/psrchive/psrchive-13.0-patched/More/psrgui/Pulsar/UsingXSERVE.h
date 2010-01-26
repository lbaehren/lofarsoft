/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/psrgui/Pulsar/UsingXSERVE.h,v $
   $Revision: 1.1 $
   $Date: 2008/07/13 10:21:51 $
   $Author: straten $ */

#ifndef __Pulsar_UsingXSERVE_h
#define __Pulsar_UsingXSERVE_h

#include "Pulsar/PlotWindow.h"

namespace Pulsar {
  
  //! Manages a window in which to Plot an Archive
  class UsingXSERVE : public PlotWindow
  {
  public:
  
    UsingXSERVE( const std::string& device );
    ~UsingXSERVE ();

    void plotter ();

  protected:
    int devid;
  };

}

#endif

