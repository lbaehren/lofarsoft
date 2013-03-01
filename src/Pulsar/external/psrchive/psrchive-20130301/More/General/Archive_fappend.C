/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "tempo++.h"

void Pulsar::Archive::fappend (Pulsar::Archive* arch, bool ignore_time_mismatch)
{
  if (arch->get_nsubint() != get_nsubint())
    throw Error (InvalidParam, "Pular::Archive::fappend nsubint mismatch");

  int new_nchan = get_nchan() + arch->get_nchan();
  
  float newcfr = ((arch->get_centre_frequency())+get_centre_frequency())/2.0;

  if( has_model() && !arch->has_model() )
    throw Error(InvalidState,"Pulsar::Archive::fappend()",
		"This archive has a model but arch doesn't");
  if( !has_model() && arch->has_model() )
    throw Error(InvalidState,"Pulsar::Archive::fappend()",
		"This archive hasn't a model but arch does");

  if( has_model() && arch->has_model() ){
    // Correct the polycos in each archive so that they have the same reference
    // sky frequency
    polyco newpol = Tempo::get_polyco(get_ephemeris(),
				      start_time(),
				      end_time(),
				      get_model().get_nspan(),
				      get_model().get_ncoeff(),
				      12,
				      get_model().get_telescope(),
				      newcfr);
    set_model(newpol);
    
    polyco newpol2 = Tempo::get_polyco(arch->get_ephemeris(),
				       arch->start_time(),
				       arch->end_time(),
				       arch->get_model().get_nspan(),
				       arch->get_model().get_ncoeff(),
				       12,
				       arch->get_model().get_telescope(),
				       newcfr);
    arch->set_model(newpol2);
  }    

  // Align the second archive with the epoch of the first, compensating for
  // phase differences that arise from small offsets in the start times

  MJD offset = get_Integration(0)->get_epoch() - 
    arch->get_Integration(0)->get_epoch();

  arch->rotate(offset.in_seconds());

  try {
    
    for (unsigned i = 0; i < get_nsubint(); i++) {
      get_Integration(i)->fappend(arch->get_Integration(i), ignore_time_mismatch);
    }

  }
  catch (Error& error) {
    cerr << error << endl;
  }

  set_nchan(new_nchan);
  set_bandwidth((arch->get_bandwidth())+get_bandwidth());
  set_centre_frequency(newcfr);
  
}
