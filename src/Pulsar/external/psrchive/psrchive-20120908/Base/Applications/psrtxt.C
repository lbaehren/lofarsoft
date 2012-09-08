/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/MoreProfiles.h"

#include <stdlib.h>
#include <unistd.h>

using namespace std;

void usage ()
{
  cerr << 
    "psrtxt - pulsar archive ascii print\n"
    "\n"
    "psrtxt [options] filename\n"
    "options:\n"
    "  -b ibin    select a single phase bin, from 0 to nbin-1 \n"
    "  -c ichan   select a single frequency channel, from 0 to nchan-1 \n"
    "  -i isub    select a single integration, from 0 to nsubint-1 \n"
    "  -s ipol    select a single polarization, from 0 to npol-1 \n"
    "\n"
    "  -p phase   select a single phase, from 0.0 to 1.0 (overrides -b) \n"
    "  -m         search for minimum and maximum value in data \n"
    "  -a         access auxiliary data \n"
    "\n"
    "Each row output by psrtxt contains:\n"
    "\n"
    "isub ichan ibin S0 [S1 ... SN]\n" 
    "\n"
    "Where Si are fluxes of the type stored in the archive (e.g. I Q U V)\n"
       << endl;
}

class where
{
public:

  where () { set = false; value = 0.0; subint = chan = pol = bin = 0; }

  bool set;
  float value;

  unsigned subint;
  unsigned chan;
  unsigned pol;
  unsigned bin;
};

#define SET_WHERE(x)    \
  { x.value = value;    \
    x.set = true;       \
    x.subint = isub;    \
    x.chan = ichan;     \
    x.pol = ipol;       \
    x.bin = ibin; }

#define GET_WHERE(x) \
  "subint=" << x.subint << " chan=" << x.chan << \
  " pol=" << x.pol << " bin=" << x.bin << " value=" << x.value

int main (int argc, char** argv) try
{
  bool minmax = false;
  bool phase_chosen = false;
  bool auxiliary = false;

  float phase = 0.0;

  int cbin  = -1;
  int cchan = -1;
  int csub  = -1;
  int cpol  = -1;

  char c;
  while ((c = getopt(argc, argv, "ab:c:i:mp:s:hqvV")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'a':
      auxiliary = true;
      break;

    case 'b':
      cbin = atoi (optarg);
      break;

    case 'c':
      cchan = atoi (optarg);
      break;

    case 'i':
      csub = atoi (optarg);
      break;

    case 'm':
      minmax = true;
      break;

    case 'p':
      phase_chosen = true;
      phase = atof (optarg);
      break;

    case 's':
      cpol = atoi (optarg);
      break;

    } 


  if (optind >= argc)
  {
    cerr << "psrtxt: specify filename" << endl;
    return -1;
  }

  Pulsar::Archive* archive = Pulsar::Archive::load( argv[optind] );

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  if (auxiliary)
    npol=archive->get_Profile(0,0,0)->get<Pulsar::MoreProfiles>()->get_size();

  if (phase_chosen)
    cbin = int (phase * (nbin-1));

  if (cbin > 0 && unsigned(cbin) >= nbin)
  {
    cerr << "psrtxt: -b " << cbin << " >= nbin=" << nbin << endl;
    return -1;
  }

  if (cchan > 0 && unsigned(cchan) >= nchan)
  {
    cerr << "psrtxt: -c " << cchan << " >= nchan=" << nchan << endl;
    return -1;
  }

  if (cpol > 0 && unsigned(cpol) >= npol)
  {
    cerr << "psrtxt: -c " << cpol << " >= npol=" << npol << endl;
    return -1;
  }

  if (csub > 0 && unsigned(csub) >= nsub)
  {
    cerr << "psrtxt: -i " << csub << " >= nsub=" << nsub << endl;
    return -1;
  }

  where min, max;

  for (unsigned isub=0; isub < nsub; isub++)
  {
    if (csub >= 0)
      isub = csub;

    Pulsar::Integration* integration = archive->get_Integration(isub);

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (cchan >= 0)
	ichan = cchan;
      
      for (unsigned ibin=0; ibin < nbin; ibin++)
      {
	if (cbin >= 0)
	  ibin = cbin;

	if (!minmax)
	  cout << isub << " " << ichan << " " << ibin;

	for (unsigned ipol=0; ipol < npol; ipol++)
	{
	  if (cpol >= 0)
	    ipol = cpol;

	  Pulsar::Profile* profile = integration->get_Profile (0,ichan);

	  if (auxiliary)
	    profile = profile->get<Pulsar::MoreProfiles>()->get_Profile(ipol);
	  else
	    profile = integration->get_Profile (ipol,ichan);

	  float value = profile->get_amps()[ibin];

	  if (minmax)
	  {
	    if (value > max.value || !max.set)
	      SET_WHERE (max);
	    if (value < min.value || !min.set)
	      SET_WHERE (min);
	  }
	  else
	    cout << " " << value;

	  if (cpol >= 0)
	    break;
	}

	if (!minmax)
	  cout << endl;
	
	if (cbin >= 0)
	  break;
      }

      if (cchan >= 0)
	break;
    }

    if (csub >= 0)
      break;
  }

  if (minmax)
  {
    cout << "MIN: " << GET_WHERE(min) << endl
	 << "MAX: " << GET_WHERE(max) << endl;
  }

  return 0;
}
catch (Error& er)
{
  cerr << "psrtxt: " << er << endl;
  return -1;
}

