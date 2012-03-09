/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ReflectStokes.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

using namespace std;

#define Stokes_Q 0x01
#define Stokes_U 0x02
#define Stokes_V 0x04

static void sign_flip (Pulsar::Archive* archive, unsigned ipol)
{
  for (unsigned isub=0; isub<archive->get_nsubint(); isub++)
    for (unsigned ichan=0; ichan<archive->get_nchan(); ichan++)
      archive->get_Profile(isub, ipol, ichan) -> scale( -1.0 );
}

Pulsar::ReflectStokes::ReflectStokes ()
{
  reflect = 0;
}

void Pulsar::ReflectStokes::transform (Pulsar::Archive* arch)
{
  if (!reflect)
    return;

  cerr << "Flipping the sign of Stokes";
  arch->convert_state (Signal::Stokes);

  if (reflect & Stokes_Q) {
    cerr << " Q";
    sign_flip (arch, 1);
  }

  if (reflect & Stokes_U) {
    cerr << " U";
    sign_flip (arch, 2);
  }

  if (reflect & Stokes_V) {
    cerr << " V";
    sign_flip (arch, 3);
  }

  cerr << endl;
}

void Pulsar::ReflectStokes::add_reflection (char stokes)
{
  switch (stokes) {
    case 'Q':
    case 'q':
      reflect |= Stokes_Q;
      break;

    case 'U':
    case 'u':
      reflect |= Stokes_U;
      break;

    case 'V':
    case 'v':
      reflect |= Stokes_V;
      break;

    default:
      throw Error (InvalidParam, "Pulsar::ReflectStokes::add_reflection",
		   "unknown Stokes parameter '%c'", stokes);
  }
}

