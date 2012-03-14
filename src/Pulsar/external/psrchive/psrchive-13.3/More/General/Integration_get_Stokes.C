/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "Stokes.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes
//
/*!  
  \retval Stokes 4-vector for the given frequency channel and phase bin
  \param ichan the frequency channel
  \param ibin the phase bin

  \pre The Integration must contain full polarimetric information.
  That is, get_state must return either Signal::Stokes or
  Signal::Coherence.  An exception is thrown otherwise. 
*/
Stokes<float> Pulsar::Integration::get_Stokes (unsigned ichan,
					       unsigned ibin) const
{
  if (verbose)
    cerr << "Pulsar::Integration::get_Stokes";

  Stokes<float> S;

  for (unsigned ipol=0; ipol<4; ++ipol)
    S[ipol] = profiles[ipol][ichan]->get_amps()[ibin];
  
  if (get_state() == Signal::Stokes)
    return S;
  
  if (get_state() != Signal::Coherence)
    throw Error (InvalidState, "Integration::get_Stokes",
		 "Invalid state=" + State2string(get_state()));
  
  float PP   = S[0];
  float QQ   = S[1];
  float RePQ = S[2];
  float ImPQ = S[3];

  if (get_basis() == Signal::Linear) {
    S[0] = PP + QQ;
    S[1] = PP - QQ;
    S[2] = 2.0 * RePQ;
    S[3] = 2.0 * ImPQ;
  }
  else {
    S[0] = PP + QQ;
    S[3] = PP - QQ;
    S[1] = 2.0 * RePQ;
    S[2] = 2.0 * ImPQ;
  }

  return S;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes vector
//
/*!  
  The vector of Stokes parameters may be calculated with either pulse
  phase or observing frequency as the abscissa.

  \retval S a vector of Stokes parameters as a function of the given dimension
  \param iother the index in the other dimension from which to draw S
  \param abscissa the dimension of the vector abscissa

  By default, "get_Stokes (S, ichan);" will return the polarimetric
  profile from chan, ichan.  To get the polarimetric spectra from the
  middle of the pulse profile, for example, call "get_Stokes (S,
  get_nbin()/2, Axis::Frequency);"

*/
void Pulsar::Integration::get_Stokes (vector< Stokes<float> >& S,
				      unsigned iother,
				      Signal::Dimension abscissa) const
{
  unsigned ndim = 0;
  unsigned ndim_other = 0;

  if (!(get_state()==Signal::Stokes || get_state()==Signal::Coherence))
    throw Error (InvalidState, "Integration::get_Stokes");

  if (abscissa == Signal::Frequency) {
    ndim = get_nchan();
    ndim_other = get_nbin();
  }
  else if (abscissa == Signal::Phase) {
    ndim = get_nbin();
    ndim_other = get_nchan();
  }
  else
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid abscissa");

  if (iother>=ndim_other)
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid dimension");

  S.resize(ndim);

  unsigned ibin=0, ichan=0;

  if (abscissa == Signal::Frequency)
    ibin = iother;    // all Stokes values come from the same bin
  else // dim == Phase
    ichan = iother;   // all Stokes values come from the same chan

  for (unsigned idim=0; idim<ndim; idim++) {

    if (abscissa == Signal::Frequency)
      ichan = idim;
    else
      ibin = idim;

    S[idim] = get_Stokes (ichan, ibin);

  }
}

