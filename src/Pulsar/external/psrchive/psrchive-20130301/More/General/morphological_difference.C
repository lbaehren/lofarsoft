/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "morphological_difference.h"
#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/Profile.h"
#include "Error.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::morphological_difference
//
/*!
  This function is designed to be used for detecting morphological
  differences in pulse shape irrespective of the amplitudes of the
  individual profiles involved.
*/

Pulsar::Profile* 
Pulsar::morphological_difference (const Profile* p1, const Profile* p2,
                                  double& scale, double& shift,
                                  float phs1, float phs2)
{
  if (p1->get_nbin() != p2->get_nbin())
    throw Error (InvalidRange, "Pulsar::Profile::morphological_difference",
		 "nbin mismatch");

  Reference::To<Pulsar::Profile> temp1 = p1->clone();
  Reference::To<Pulsar::Profile> temp2 = p2->clone();
  
  // First it is essential to align the profiles in phase, so that our
  // subtraction occurs between the equivalent bins in both profiles.
  
  float ephase;
  double phase;
  
  // First try the FFT based shift routine and if it fails, revert
  // to an incoherent cross correlation, accurate only to the
  // nearest bin.
  
  try
  {
    PhaseGradShift shift;
    shift.set_standard (temp2);
    shift.set_observation (temp1);
    Estimate<double> Ephase = shift.get_shift();
    phase = Ephase.val;
    ephase = sqrt(Ephase.var);
  }
  catch (Error& error)
  {
    cerr << "Pulsar::difference FFT shift failed" 
	 << endl;
    cerr << "  Trying simple cross correlation instead..." << endl;
    
    Reference::To<Pulsar::Profile> ptr = p1->clone();
    
    *ptr -= ptr->mean(ptr->find_min_phase(0.15));
    ptr->correlate(temp2);
    phase = ptr->find_max_phase(0.15);
  }
  
  // Perform the rotation in phase

  temp1->rotate_phase (phase);
  shift = phase;
  
  float minphs = 0.0;

  // Remove the baselines from our reference and subject profiles

  minphs = temp1->find_min_phase(0.6);
  *temp1 -= (temp1->mean(minphs));

  minphs = temp2->find_min_phase(0.6);
  *temp2 -= (temp2->mean(minphs));
  
  // Perform the difference operation, using one of the following
  // possible schemes
  
  // This section scales the total flux under the profile to
  // be the same as under the standard
  
  float t2sum = temp2->sum(int(phs1*(p2->get_nbin())),
			   int(phs2*(p2->get_nbin())));
  
  float ratio = t2sum / temp1->sum(int(phs1*(p2->get_nbin())),
				   int(phs2*(p2->get_nbin())));
  
  *temp1 *= ratio;

  scale = ratio;
  
  // End section
  
  /*
    
  // This section normalises the standard deviation in the
  // baseline of the reference and subject profiles to be
  // (arbitrarily) 10 units. This normalisation should be
  // more susceptible to variations in intrinsic brightness
  // but perhaps more physically meaningful?
  
  phs1 = temp1->find_min_phase();
  phs2 = temp2->find_min_phase();
  
  double mean    = 0.0;
  double var     = 0.0;
  double varmean = 0.0;
  
  temp1->stats(phs1,&mean, &var, &varmean);
  *temp1 *= 10/sqrt(var);
  
  temp2->stats(phs2,&mean, &var, &varmean);
  *temp2 *= 10/sqrt(var);
  
  // End section
  
  */

  // Subtract the reference from the subject
  
  float* amps1 = temp1->get_amps();
  float* amps2 = temp2->get_amps();
  
  for (unsigned i = 0; i < temp1->get_nbin(); i++) {
    amps1[i] = amps1[i] - amps2[i];
  }

  return temp1.release();
}




