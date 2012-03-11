/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ASCIIArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "tempo++.h"

#include <fstream>
using namespace std;

void Pulsar::ASCIIArchive::init ()
{
  integration_length = 0.0;
  period = 1.0;
}

Pulsar::ASCIIArchive::ASCIIArchive()
{
  init ();
}

Pulsar::ASCIIArchive::~ASCIIArchive()
{
  // destroy any ASCIIArchive resources
}

Pulsar::ASCIIArchive::ASCIIArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASCIIArchive::ASCIIArchive (const ASCIIArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive construct copy ASCIIArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::ASCIIArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy dynamic cast call" << endl;

  const ASCIIArchive* like_me = dynamic_cast<const ASCIIArchive*>(&archive);
  if (!like_me)
    return;

  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy another ASCIIArchive" << endl;

  integration_length = like_me -> integration_length;
}

Pulsar::ASCIIArchive* Pulsar::ASCIIArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::clone" << endl;
  return new ASCIIArchive (*this);
}

void Pulsar::ASCIIArchive::load_header (const char* filename)
{
  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::ASCIIArchive::load_header",
		 "ifstream(%s)", filename);

  // clear the optional hash
  if (is.peek() == '#')
    hashed_header (is);
  else
    is >> centre_frequency >> nbin >> source >> integration_length;

  if (is.fail())
    throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_header",
		 "could not parse header from '%s'", filename);

  // get the remainder of the header line, if any
  string line;
  getline (is, line);

  // get the first line of data
  getline (is, line);

  double v;
  int nitem = sscanf (line.c_str(), "%lf %lf %lf %lf %lf", &v,&v,&v,&v,&v);
  if (nitem == 5)
  {
    set_npol (4);
    set_state (Signal::Stokes);
  }
  else if (nitem == 3) 
  {
    set_npol (2);
    set_state (Signal::PPQQ);
  }
  else if (nitem == 2)
  {
    set_npol (1);
    set_state (Signal::Intensity);
  }
  else
    throw Error (InvalidState, "Pulsar::ASCIIArchive::load_header",
        "could not parse first data line:\n%s", line.c_str());

  set_nchan   (1);
  set_nsubint (1);

}

static const int ntelcode = 35;
static const char telcodes[ntelcode+1] = "1234567q9abcdefghijklmnopqrstuvwxyz";

void Pulsar::ASCIIArchive::hashed_header (istream& is)
{
  // clear the hash
  is.get();

  double epoch_MJD = 0;
  is >> epoch_MJD;

  double epoch_seconds = 0;
  is >> epoch_seconds;

  epoch = MJD(epoch_MJD);
  epoch += epoch_seconds;

  is >> period;
    
  int subints = 0;
  is >> subints;
  set_nsubint(1); // Only allow single-subint for now

  is >> centre_frequency >> dispersion_measure >> nbin;

  int site_num = 0;
  is >> site_num;
  if (site_num<1 || site_num>ntelcode)
    throw Error (InvalidState, "Pulsar::ASCIIArchive::hashed_header",
        "Invalid telescope site number (%d)", site_num);
  string site(1, telcodes[site_num-1]);
  set_telescope(site);

  int ignore = 0;
  is >> ignore;

  is >> source;

  double phase;
  is >> phase;
  epoch -= phase*period;
}

Pulsar::Integration*
Pulsar::ASCIIArchive::load_Integration (const char* filename, unsigned subint)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::load_Integration " << filename << " " 
	 << subint << endl;

  // load all BasicIntegration attributes and data from filename

  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::ASCIIArchive::load_Integration",
		 "ifstream(%s)", filename);

  // get the header line
  string line;
  getline (is, line);

  Pulsar::BasicIntegration* integration = new BasicIntegration;
  resize_Integration (integration);

  unsigned bin = 0;

  integration->set_folding_period (period);
  integration->set_epoch (epoch);
  integration->set_centre_frequency(0, centre_frequency);

  if (Profile::no_amps)
    return integration;

  for (unsigned ibin=0; ibin < get_nbin(); ibin++)
  {
    if (verbose > 2)
      cerr << "Pulsar::ASCIIArchive::load_Integration ibin=" << ibin << endl;

    is >> bin;

    for (unsigned ipol=0; ipol < get_npol(); ipol++)
    {
      double value;
      is >> value;

      integration->get_Profile(ipol, 0) -> get_amps()[ibin] = value;
    }

    if (is.fail())
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "data read failed");

    if (bin != ibin)
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "bin number=%u != expected=%u", bin, ibin);

    // clear the remainder of the line
    getline (is, line);

    if (is.fail())
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "clear read failed");
  }

  return integration;
}

void Pulsar::ASCIIArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ASCIIArchive attributes as well as
  // BasicIntegration attributes and data to filename

  if (get_nsubint()!=1 || get_nchan()!=1) 
    throw Error (InvalidState, "Pulsar::ASCIIArchive::unload_file",
        "Only nsubint=nchan=1 ASCII output is currently supported");

  FILE *fout = fopen(filename, "w");
  if (fout==NULL) 
    throw Error (FailedSys, "Pulsar::ASCIIArchive::unload_file",
        "Error opening file (%s) for output", filename);

  Reference::To<const Integration> subint = get_Integration(0);

  /* Figure out site number */
  char code = Tempo::code(get_telescope());
  int site_num = 0;
  for (site_num=0; site_num<ntelcode; site_num++) 
    if (telcodes[site_num]==code) break;
  site_num++;

  /* Write header line */
  fprintf(fout, "# %7.1f %.10f %.14f 1 %.3f %.3f %d %d 1 %s %.14f\n",
      (float)subint->get_epoch().intday(), subint->get_epoch().fracday()*86400.0, 
      subint->get_folding_period(), subint->get_centre_frequency(0),
      get_dispersion_measure(), get_nbin(), site_num, get_source().c_str(), 
      0.0);

  /* Write data */
  for (unsigned ibin=0; ibin<get_nbin(); ibin++) {
    fprintf(fout, "%5d  ", ibin);
    for (unsigned ipol=0; ipol<get_npol(); ipol++) {
      fprintf(fout, "%+.8e  ", subint->get_Profile(ipol,0)->get_amps()[ibin]);
    }
    fprintf(fout, "\n");
  }

  fclose(fout);

}


string Pulsar::ASCIIArchive::Agent::get_description () 
{
  return "ASCII/STD Archive (single-profile only)";
}


bool Pulsar::ASCIIArchive::Agent::advocate (const char* filename) try
{
  ASCIIArchive archive;
  archive.load_header (filename);
  return true;
}
catch (Error& e) 
{
  if (Archive::verbose > 2)
    cerr << "ASCII load failed due to: " << e << endl;

  return false;
}
catch (std::exception& e)
{
  return false;
}
