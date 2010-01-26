/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CoherencyMeasurementSet.h"

using namespace std;

Calibration::CoherencyMeasurementSet::CoherencyMeasurementSet (unsigned index)
{
  transformation_index = index;
}

Calibration::CoherencyMeasurementSet::~CoherencyMeasurementSet ()
{
}

//! Set the transformation through which the measurements are made
void 
Calibration::CoherencyMeasurementSet::set_transformation_index (unsigned index)
{
  transformation_index = index;
}

//! Set the transformation through which the measurements are made
unsigned 
Calibration::CoherencyMeasurementSet::get_transformation_index () const
{
  return transformation_index;
}

//! Add an independent variable
void Calibration::CoherencyMeasurementSet::add_coordinate (MEAL::Argument::Value* x)
{
  coordinates.push_back (x);
}

void Calibration::CoherencyMeasurementSet::set_coordinates () const
{
  for (unsigned ic=0; ic<coordinates.size(); ic++)
    coordinates[ic]->apply();
}

//! Set the identifier
void Calibration::CoherencyMeasurementSet::set_identifier (const string& id)
{
  identifier = id;
}

//! Get the identifier
const string Calibration::CoherencyMeasurementSet::get_identifier () const
{
  return identifier;
}
