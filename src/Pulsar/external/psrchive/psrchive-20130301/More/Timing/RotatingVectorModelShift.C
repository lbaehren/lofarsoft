/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RotatingVectorModelShift.h"
#include "Pulsar/ComplexRVMFit.h"
#include "Pulsar/FaradayRotation.h"

#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ComplexRVM.h"
#include "MEAL/ScalarParameter.h"

using namespace Pulsar;
using namespace std;

//! Return the shift estimate
Estimate<double> RotatingVectorModelShift::get_shift () const try
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::RotatingVectorModelShift::get_shift set data" << endl;

  ComplexRVMFit::verbose = Archive::verbose > 2;

  rvmfit->set_observation (observation);

  if (Archive::verbose > 2)
    cerr << "Pulsar::RotatingVectorModelShift::get_shift solve" << endl;

  rvmfit->solve();

  MEAL::RotatingVectorModel* RVM = rvmfit->get_model()->get_rvm();

  // divide by 2pi because ArrivalTime expect answer in dimensionless turns
  return RVM->magnetic_meridian->get_Estimate (0) * 0.5 / M_PI;
}
 catch (Error& error)
   {
     throw error += "Pulsar::RotatingVectorModelShift::get_shift";
   }

/*! Correct Faraday rotation to infinite frequency / zero wavelength */
void RotatingVectorModelShift::preprocess (Archive* archive) try
{
  FaradayRotation xform;
  xform.set_reference_wavelength( 0 );
  xform.set_measure( archive->get_rotation_measure() );
  xform.execute( archive );

  archive->convert_state (Signal::Stokes);
  archive->remove_baseline ();
}
 catch (Error& error)
   {
     throw error += "Pulsar::RotatingVectorModelShift::preprocess";
   }

//
// the rest is scaffolding ...
//

//! Default constructor
RotatingVectorModelShift::RotatingVectorModelShift ()
{
  rvmfit = new ComplexRVMFit;
}

//! Copy constructor
RotatingVectorModelShift::RotatingVectorModelShift
(const RotatingVectorModelShift& that)
{
  rvmfit = new ComplexRVMFit;
}

//! Assignment operator
const RotatingVectorModelShift&
RotatingVectorModelShift::operator= (const RotatingVectorModelShift& that)
{
  return *this;
}

//! Destructor
RotatingVectorModelShift::~RotatingVectorModelShift ()
{
}

RotatingVectorModelShift* RotatingVectorModelShift::clone () const
{
  return new RotatingVectorModelShift(*this);
}


