//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/EllAngPlot.h,v $
   $Revision: 1.7 $
   $Date: 2006/12/21 15:07:51 $
   $Author: straten $ */

#ifndef __Pulsar_EllAngPlot_h
#define __Pulsar_EllAngPlot_h

#include "Pulsar/AnglePlot.h"

namespace Pulsar {

  //! Plots the ellipticity angle of the polarized radiation
  class EllAngPlot : public AnglePlot {

  public:

    //! Default constructor
    EllAngPlot ();

    //! Return the default label for the y axis
    std::string get_ylabel (const Archive* data);
    
    //! Compute ellipticity angles and store in AnglePlot::angles
    void get_angles (const Archive*);

  };

}

#endif
