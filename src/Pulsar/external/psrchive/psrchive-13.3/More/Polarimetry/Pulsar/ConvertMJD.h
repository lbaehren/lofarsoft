//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ConvertMJD.h,v $
   $Revision: 1.4 $
   $Date: 2007/11/09 04:48:14 $
   $Author: straten $ */

#ifndef __Calibration_ConvertMJD_Header
#define __Calibration_ConvertMJD_Header

#include "MEAL/Axis.h"
#include "MJD.h"

namespace Calibration {

  //! Converts Argument type from MJD to double
  class ConvertMJD : public MEAL::Axis<double> {

  public:

    //! Default constructor
    ConvertMJD ();

    //! Set the epoch
    void set_epoch (const MJD&);

    //! Set the reference epoch
    void set_reference_epoch (const MJD&);

    //! Get the reference epoch
    MJD get_reference_epoch () const;

    //! Set the scale to be used during conversion
    void set_scale (double scale);

  protected:

    //! The reference epoch
    MJD reference_epoch;

    //! The scale used during conversion
    double scale;

  };

}

#endif

