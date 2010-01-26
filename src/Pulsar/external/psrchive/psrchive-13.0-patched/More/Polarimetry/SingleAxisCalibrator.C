/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/CalibratorType.h"

#include "Pauli.h"
#include "Estimate.h"

using namespace std;

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::SingleAxisCalibrator::SingleAxisCalibrator (const Archive* archive) 
  : ReferenceCalibrator (archive)
{
  type = Pulsar::Calibrator::Type::factory (this);
}

Pulsar::SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxis
::MEAL::Complex2*
Pulsar::SingleAxisCalibrator::solve (const vector<Estimate<double> >& source,
				     const vector<Estimate<double> >& sky)
{
  Reference::To<Calibration::SingleAxis> model = new Calibration::SingleAxis;

  if ( !source_set || source.size() != 4 )
  {
    if (verbose > 2)
      cerr << "Pulsar::SingleAxisCalibrator::solve" << endl;
    model->solve (source);
    return model.release();
  }

  if (verbose > 2)
    cerr << "Pulsar::SingleAxisCalibrator::solve reference source=" 
         << reference_source << endl;

  // Convert the coherency vectors into Stokes parameters.
  Stokes< Estimate<double> > stokes_source = coherency( convert (source) );

  if (!solver)
    solver = new Calibration::SingleAxisSolver;

  solver->set_input (reference_source);
  solver->set_output (stokes_source);
  solver->solve (model);

  return model.release();
}




Pulsar::SingleAxisCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

string Pulsar::SingleAxisCalibrator::Info::get_title () const
{
  return "Polar Decomposition of Complex Gains";
}

//! Return the number of parameter classes
unsigned Pulsar::SingleAxisCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
string Pulsar::SingleAxisCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "\\fiG\\fr (\\fic\\fr\\d0\\u)";
  case 1:
    return "\\gg (%)";
  case 2:
    return "\\gf (deg.)";
  default:
    return "";
  }
} 
  
//! Return the number of parameters in the specified class
unsigned Pulsar::SingleAxisCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 3)
    return 1;
  return 0;
}

Estimate<float>
Pulsar::SingleAxisCalibrator::Info::get_param (unsigned ichan, 
					       unsigned iclass,
					       unsigned iparam) const
{
  if (iclass == 0)
    return PolnCalibrator::Info::get_param (ichan, iclass, iparam);

  if (iclass == 1)
    return 100.0 * 
      ( exp( 2*PolnCalibrator::Info::get_param (ichan, iclass, iparam) ) - 1 );

  // iclass == 2
  return 180.0/M_PI * PolnCalibrator::Info::get_param (ichan, iclass, iparam);
}



Pulsar::SingleAxisCalibrator::Info* 
Pulsar::SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibrator::Info (this);
}

