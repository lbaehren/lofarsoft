
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BasisCorrection.h"
#include "Pulsar/Receiver.h"
#include "MEAL/Rotation1.h"
#include "Pauli.h"

using namespace std;

//! Return the handedness correction matrix
Jones<double>
Pulsar::BasisCorrection::get_hand (const Receiver* rcvr) const
{
  if (!rcvr)
    return 1.0;

  if (rcvr->get_basis_corrected() || rcvr->get_hand() == Signal::Right)
    return 1.0;

  summary += " left-handed basis\n";

  // return the exchange matrix for which det(J)=1
  complex<double> i (0,1);
  return Jones<double> (0, i,
                        i, 0);
} 

//! Return the transformation due to receiver basis
Jones<double>
Pulsar::BasisCorrection::get_basis (const Receiver* rcvr) const
{
  if (!rcvr)
    return 1.0;

  if (rcvr->get_basis_corrected() || rcvr->get_basis() == Signal::Linear)
    return 1.0;

  summary += " circular basis\n";

  const double r = 1.0/sqrt(2.0);
  complex<double> p (r,-r);

  // return the circular basis in which S=(V,Q,U) and det(J)=1
  complex<double> i (0,1);
  return r * p * Jones<double> (1, -i,
				1, i);
}

//! Return the transformation due to receiver symmetry axis
Jones<double>
Pulsar::BasisCorrection::get_symmetry (const Receiver* rcvr) const
{
  if (!rcvr)
    return 1.0;

  if (rcvr->get_basis_corrected() || rcvr->get_orientation().getRadians() == 0)
    return 1.0;

  Angle::default_type = Angle::Degrees;

  summary += " orientation=" + tostring(rcvr->get_orientation()) + "\n";

  // rotate the basis about the Stokes V axis
  MEAL::Rotation1 rotation ( Pauli::basis().get_basis_vector(2) );

  // the sign of this rotation may depend on handedness
  rotation.set_phi ( rcvr->get_orientation().getRadians() );

  return rotation.evaluate();
}

//! Return the feed correction matrix
Jones<double> Pulsar::BasisCorrection::operator () (const Archive* a) const
{
  return operator () (a->get<Receiver>());
}

/*!
  The basis transformation is not included in the basis correction.
  Rather, all transformations to the right of the basis tranformation are
  expressed in that basis.
 */
Jones<double> Pulsar::BasisCorrection::operator () (const Receiver* rcvr) const
{
  summary = "";
  return get_hand (rcvr) * get_symmetry (rcvr);
}

bool Pulsar::BasisCorrection::required (const Archive* archive) const
{
  // determine if it is necesary to correct for known receptor projections
  
  const Receiver* receiver = archive->get<Receiver>();

  if (!receiver || receiver->get_basis_corrected())
  {
    if (Archive::verbose > 2)
      cerr << "Pulsar::BasisCorrection::required basis corrected" << endl;
    return false;
  }

  bool required = 
    receiver->get_orientation() != 0 ||
    receiver->get_hand() != Signal::Right ||
    receiver->get_basis() != Signal::Linear;

  if (Archive::verbose > 2)
    cerr << "Pulsar::BasisCorrection::required"
      "\n  orientation=" << receiver->get_orientation() <<
      "\n  hand=" << receiver->get_hand() <<
      "\n  basis=" << receiver->get_basis() <<
      "\n  -> required=" << required << endl;

  return required;
}

std::string Pulsar::BasisCorrection::get_summary () const
{
  return summary;
}
