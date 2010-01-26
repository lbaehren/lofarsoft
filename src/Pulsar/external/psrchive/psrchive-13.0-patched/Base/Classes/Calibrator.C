/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/Calibrator.h"
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

using namespace std;

unsigned Pulsar::Calibrator::verbose = 0;

Pulsar::Option<float>
Pulsar::Calibrator::median_smoothing
(
 "Calibrator::median_smoothing", 0.0,

 "Size of median filtering window [band fraction]",

 "The size of the window used during median filtering is given by the \n"
 "number of frequency channels, nchan, multiplied by median_smoothing.\n"
 "If set to zero, no smoothing is performed.  A sensible value is around 0.05."
);

Pulsar::Option<float>
Pulsar::Calibrator::interpolating 
(
 "Calibrator::interpolating", 0.0,

 "Maximum interpolation window [band fraction]",

 "The maximum number of channels over which a linear interpolation will \n"
 "be performed is given by the number of frequency channels, nchan, \n"
 "multiplied by interpolating.  If set to zero, no interpolation is performed."
);

Pulsar::Option<float>
Pulsar::Calibrator::det_threshold
(
 "Calibrator::det_threshold", 0.0,

 "Tolerance to non-physical interval [sigma]",

 "Non-physical values of the determinant of the coherency matrix are \n"
 "defined as less than -(det_threshold * sigma), where sigma is the error \n"
 "in the estimate of the determinant due to noise.  If set to zero, \n"
 "no check is performed."
);


Pulsar::Calibrator::Calibrator ()
{
}

Pulsar::Calibrator::~Calibrator ()
{
}

const Pulsar::Calibrator::Type* Pulsar::Calibrator::get_type () const
{
  return type;
}

//! Provide derived classes with access to the calibrator
bool Pulsar::Calibrator::has_calibrator () const
{
  return calibrator;
}

//! Provide derived classes with access to the calibrator
const Pulsar::Archive* Pulsar::Calibrator::get_calibrator () const
{
  return calibrator;
}

//! Provide derived classes with access to the calibrator
void Pulsar::Calibrator::set_calibrator (const Archive* archive)
{
  calibrator = archive;
}

//! Return a string containing the file information
string Pulsar::Calibrator::get_filenames () const
{
  string names;

  if (filenames.size())
    names = filenames[0];

  for (unsigned i=1; i<filenames.size(); i++)
    names += "\n\t" + filenames[i];

  return names;

}

//! Return a const reference to the calibrator archive
const Pulsar::Archive* Pulsar::Calibrator::get_Archive () const
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::Calibrator::get_Archive",
		 "no calibrator Archive");

  return calibrator; 
}

MJD Pulsar::Calibrator::get_epoch () const
{
  if (extension)
    return extension->get_epoch ();

  return 0.5 * (calibrator->start_time() + calibrator->end_time());
}

Pulsar::Archive* Pulsar::Calibrator::new_solution (const string& arclass) const
{
  if (verbose > 2) cerr << "Pulsar::Calibrator::new_solution"
		     " create CalibratorExtension" << endl;

  Reference::To<CalibratorExtension> ext = new_Extension ();

  if (verbose > 2) 
    cerr << "Pulsar::Calibrator::new_solution create " << arclass << endl;
  
  Reference::To<Archive> output = Pulsar::Archive::new_Archive (arclass);
  output -> copy (*calibrator);

  output -> set_type (Signal::Calibrator);
  output -> resize (0);
  output -> add_extension (ext);

  return output.release();
}

