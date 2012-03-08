//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CoherencyMeasurementSet.h,v $
   $Revision: 1.6 $
   $Date: 2007/12/09 19:06:05 $
   $Author: straten $ */

#ifndef __Calibration_CoherencyMeasurementSet_H
#define __Calibration_CoherencyMeasurementSet_H

#include "Pulsar/CoherencyMeasurement.h"
#include "MEAL/Argument.h"

namespace Calibration {

  //! A CoherencyMeasurement set and their coordinates
  class CoherencyMeasurementSet : public std::vector<CoherencyMeasurement> {

  public:

    //! Default constructor
    CoherencyMeasurementSet (unsigned transformation_index = 0);

    //! Destructor
    ~CoherencyMeasurementSet ();

    //! Set the transformation through which the measurements are made
    void set_transformation_index (unsigned index);

    //! Set the transformation through which the measurements are made
    unsigned get_transformation_index () const;

    //! Add an independent variable
    void add_coordinate (MEAL::Argument::Value* abscissa);

    //! Apply the independent variables
    void set_coordinates () const;

    //! Set the identifier
    void set_identifier (const std::string&);

    //! Get the identifier
    const std::string get_identifier () const;

  protected:

    //! Index of the transformation through which the measurements are made
    unsigned transformation_index;

    //! The coordinates of the measurement
    std::vector< Reference::To<MEAL::Argument::Value> > coordinates;

    //! The identifier
    std::string identifier;

  };

}

#endif
