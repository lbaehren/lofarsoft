/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Passband.h"
#include "TextInterface.h"

using namespace std;

//! Default constructor
Pulsar::Passband::Passband ()
  : Extension ("Passband")
{
  nchan = 0;
  npol = 0;
  nband = 1;
}

//! Copy constructor
Pulsar::Passband::Passband (const Passband& pband)
  : Extension ("Passband")
{
  operator=(pband);
}

//! Operator =
const Pulsar::Passband&
Pulsar::Passband::operator= (const Passband& pband)
{
  nchan = pband.nchan;
  npol  = pband.npol;
  nband = pband.nband;

  passband = pband.passband;

  return *this;
}

Pulsar::Passband::~Passband ()
{
}

//! Get the number of frequency channels in each passband
unsigned Pulsar::Passband::get_nchan () const
{
  return nchan;
}

//! Get the number of polarizations
unsigned Pulsar::Passband::get_npol () const
{
  return npol;
}

//! Get the number of bands
unsigned Pulsar::Passband::get_nband () const
{
  return nband;
}

//! Set the number of channels, polarizations, and bands
void Pulsar::Passband::resize (unsigned _nchan, unsigned _npol, unsigned _nband)
{
  // ensure that none of the above are zero
  if (! (_nchan && _npol && _nband))
    throw Error (InvalidParam, "Pulsar::Passband::resize",
		 "nchan=%d npol=%d nband=%d", _nchan, _npol, _nband);

  nchan = _nchan;
  npol = _npol;
  nband = _nband;

  passband.resize (npol * nband);
  for (unsigned iband=0; iband<passband.size(); iband++)
    passband[iband].resize (nchan);
}

void Pulsar::Passband::zero()
{
  for (unsigned iband=0; iband<passband.size(); iband++)
    for (unsigned ichan=0; ichan<nchan; ichan++)
      passband[iband][ichan] = 0.0;
}

//! Get the specified passband
const vector<float>&
Pulsar::Passband::get_passband (unsigned ipol, unsigned iband) const
{
  range_check (ipol, iband, "Pulsar::Passband::get_passband");
  return passband[ipol+iband*npol];
}

//! Set the specified passband
void Pulsar::Passband::set_passband (const vector<float>& data,
				     unsigned ipol, unsigned iband)
{
  range_check (ipol, iband, "Pulsar::Passband::set_passband");

  if (data.size() != nchan)
    throw Error (InvalidParam, "Pulsar::Passband::set_passband",
		 "data.size=%d >= nchan=%d", data.size(), nchan);

  passband[ipol+iband*npol] = data;
}

//! Set the specified passband
void Pulsar::Passband::set_passband (const float* data,
				     unsigned ipol, unsigned iband)
{
  range_check (ipol, iband, "Pulsar::Passband::set_passband");

  vector<float>& pband = passband[ipol+iband*npol];

  pband.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++)
    pband[ichan] = data[ichan];
}


void Pulsar::Passband::range_check (unsigned ipol, unsigned iband,
				    const char* method) const
{
  if (ipol >= npol)
    throw Error (InvalidRange, method, "ipol=%d >= npol=%d", ipol, npol);

  if (iband >= nband)
    throw Error (InvalidRange, method, "iband=%d >= nband=%d", iband, nband);

  if (ipol+iband*npol > passband.size())
    throw Error (InvalidState, method, "ipol=%d iband=%d size=%d",
		 ipol, iband, passband.size());

}


// Text interface to a Passband object
class Pulsar::Passband::Interface : public TextInterface::To<Passband>
{
public:
  Interface( Passband *s_instance = NULL )
  {
    if( s_instance )
      set_instance( s_instance );

    add( &Passband::get_nchan,
	 "nchan", "Number of channels in bandpass" );

    add( &Passband::get_npol,
	 "npol", "Number of polarizations in bandpass" );
  }
};


//! Return a text interfaces that can be used to access this instance

TextInterface::Parser* Pulsar::Passband::get_interface( void )
{
  return new Interface( this );
}



