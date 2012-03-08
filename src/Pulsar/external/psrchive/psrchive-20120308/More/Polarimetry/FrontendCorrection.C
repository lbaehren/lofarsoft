/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrontendCorrection.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/IntegrationExpert.h"

#include "Pauli.h"

using namespace std;

bool Pulsar::FrontendCorrection::required (const Archive* archive)
{
  if (basis_correction.required (archive))
    return true;

  projection_correction.set_archive (archive);
  for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
    if (projection_correction.required (isub))
      return true;

  return false;
}

//! Set the archive for which corrections will be calculated
void Pulsar::FrontendCorrection::set_archive (const Archive* archive)
{
  projection_correction.set_archive (archive);
  basis_transformation = basis_correction (archive);
}

//! Return the transformation from sky to receptor basis
Jones<double> Pulsar::FrontendCorrection::get_transformation (unsigned isub)
{
  Jones<double> basis = get_basis();
  if (norm(det(basis)) == 0.0)
    throw Error (InvalidState, "Pulsar::FrontendCorrection::get_transformation",
                 "determinant of basis transformation equals zero");

  Jones<double> projection = get_projection(isub);
  if (norm(det(basis)) == 0.0)
    throw Error (InvalidState, "Pulsar::FrontendCorrection::get_transformation",
                 "determinant of projection transformation equals zero");

  return basis * projection;
}

//! Return the transformation to the receptor basis
Jones<double> Pulsar::FrontendCorrection::get_basis ()
{
  return basis_transformation;
}

//! Return the transformation from sky to receptors
Jones<double> Pulsar::FrontendCorrection::get_projection (unsigned isub)
{
  return projection_correction (isub);
}

//! Return a summary of parameters relevant to get_transformation
std::string Pulsar::FrontendCorrection::get_summary () const
{
  return projection_correction.get_summary();
}

void Pulsar::FrontendCorrection::calibrate (Archive* archive)
{
  if (!archive->get_nsubint())
  {
    if (verbose > 2)
      cerr << "Pulsar::FrontendCorrection no data to correct" << endl;
    return;
  }

  if (!archive->get_poln_calibrated() && Archive::verbose > 1) 
    cerr << "Pulsar::FrontendCorrection::calibrate " 
      "WARNING: data not calibrated" << endl;

  set_archive (archive);

  bool projection_correction_required = false;

  unsigned nsub = archive->get_nsubint();

  for (unsigned isub=0; isub < nsub; isub++)
  {
    if (projection_correction.required (isub))
      projection_correction_required = true;

    Integration* integration = archive->get_Integration (isub);
      
    Jones<float> xform = inv( get_transformation(isub) );

    integration->expert()->transform (xform);
  }

  Receiver* receiver = archive->get<Receiver>();

  if (basis_correction.required (archive))
    receiver->set_basis_corrected (true);

  if (projection_correction_required)
    receiver->set_projection_corrected (true);
}

