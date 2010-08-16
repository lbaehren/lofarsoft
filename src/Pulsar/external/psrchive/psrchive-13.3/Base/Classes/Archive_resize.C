/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"

using namespace std;

/*!
  If any current dimension is greater than that requested, the extra
  Profiles will be deleted and the dimension resized.  If any current
  dimension is smaller than that requested, the dimension will be
  resized and new Profiles will be constructed.  If one or more of the
  npol, nchan, or nbin arguments is set to zero, the dimension is left
  unchanged.
*/

void Pulsar::Archive::resize (unsigned nsubint, unsigned npol,
			      unsigned nchan, unsigned nbin)
{
  unsigned cur_nsub = expert()->get_size();
  unsigned cur_npol = get_npol();
  unsigned cur_nchan = get_nchan();
  unsigned cur_nbin = get_nbin();

  if (npol == 0)
    npol = cur_npol;
  if (nchan == 0)
    nchan = cur_nchan;
  if (nbin == 0)
    nbin = cur_nbin;
  
  if (verbose == 3)
    cerr << "Archive::resize nsub=" << nsubint << " npol=" << npol
	 << " nchan=" << nchan << " nbin=" << nbin << endl 
	 << " old nsub=" << cur_nsub << " npol=" << cur_npol
	 << " nchan=" << cur_nchan << " nbin=" << cur_nbin << endl;


  if (verbose == 3)
    cerr << "Pulsar::Archive::resize subints" << endl;

  IntegrationManager::resize (nsubint);

  // convenience for calibrators
  if (get_type() != Signal::Pulsar)
  {
    ephemeris = 0;
    model = 0;
  }

  if (get<Pulsar::IntegrationOrder>())
    get<Pulsar::IntegrationOrder>()->resize(nsubint);

  for (unsigned isub=0; isub<nsubint; isub++)
  {
    // initialize only the new Integrations (preserve internal book-keeping)
    if (isub >= cur_nsub)
      init_Integration( get_Integration(isub) );
    get_Integration(isub) -> resize (npol, nchan, nbin);
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::resize calling book-keeping functions" << endl;

  set_npol (npol);
  set_nchan (nchan);
  set_nbin (nbin);

  if (verbose == 3)
    cerr << "Pulsar::Archive::resize exit" << endl;
}

void Pulsar::Archive::resize_Integration (Integration* integration)
{
  integration->resize (get_npol(), get_nchan(), get_nbin());
}

//! Remove the specified sub-integration
void Pulsar::Archive::erase (unsigned isubint)
{
  IntegrationManager::unmanage (isubint);

  IntegrationOrder* order = get<IntegrationOrder>();
  if (order)
    order->erase (isubint);
}
