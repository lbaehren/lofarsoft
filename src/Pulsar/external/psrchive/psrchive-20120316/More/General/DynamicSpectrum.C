/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicSpectrum.h"
#include "Pulsar/Flux.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <algorithm>
#include <stdio.h>

Pulsar::DynamicSpectrum::DynamicSpectrum ()
{
  arch = NULL;
  flux = NULL; // XXX auto-set this somehow?
  computed = false;
  ds_data = NULL;
  ds_data_err = NULL;
  nchan = nsub = 0;
  ds_max = ds_min = 0.0;
}

Pulsar::DynamicSpectrum::~DynamicSpectrum ()
{
  reset();
}

void Pulsar::DynamicSpectrum::reset()
{
  if (ds_data!=NULL) delete [] ds_data;
  if (ds_data_err!=NULL) delete [] ds_data_err;
  nchan = nsub = 0;
  ds_max = ds_min = 0.0;
  ds_data = ds_data_err = NULL;
  computed = false;
}

void Pulsar::DynamicSpectrum::set_Archive(const Archive *_arch)
{
  // Copy a pscrunched clone (safer)?  Or assume inupt archive 
  // is already ok (less memory usage)?
  //arch = _arch->clone();
  //arch->pscrunch();
  arch = _arch;
  computed = false;

  if (nchan!=arch->get_nchan() || nsub!=arch->get_nsubint()) 
  {
    reset();
    nchan = arch->get_nchan();
    nsub = arch->get_nsubint();
    ds_data = new double[nchan*nsub];
    ds_data_err = new double[nchan*nsub];
  }
}

void Pulsar::DynamicSpectrum::set_flux_method(Flux *_flux)
{
  flux = _flux;
  computed = false;
}

#define idx(c,s) (s*nchan+c)

Estimate<double> Pulsar::DynamicSpectrum::get_ds(unsigned ichan, unsigned isub) 
{
  if (!computed) compute();
  double d = ds_data[idx(ichan,isub)];
  double e = ds_data_err[idx(ichan,isub)];
  return Estimate<double>(d, e*e);
}

const double *Pulsar::DynamicSpectrum::get_ds()
{
  if (!computed) compute();
  return (const double *)ds_data;
}

const double *Pulsar::DynamicSpectrum::get_ds_err()
{
  if (!computed) compute();
  return (const double *)ds_data_err;
}

const MJD Pulsar::DynamicSpectrum::get_rel_epoch(unsigned isub) 
{
  return arch->get_Integration(isub)->get_epoch() - arch->start_time();
}

const double Pulsar::DynamicSpectrum::get_freq(unsigned ichan, unsigned isub)
{
  return arch->get_Integration(isub)->get_centre_frequency(ichan);
}

// The actual dyn spectrum computation
void Pulsar::DynamicSpectrum::compute() 
{
  // Check that all necessary stuff is set
  if (arch==NULL)
    throw Error(InvalidState, "DynamicSpectrum::compute", 
        "compute() called without input Archive set.");
  if (flux==NULL)
    throw Error(InvalidState, "DynamicSpectrum::compute", 
        "compute() called without flux method set.");
  if (nchan==0 || nsub==0 || ds_data==NULL || ds_data_err==NULL)
    throw Error(InvalidState, "DynamicSpectrum::compute",
        "Inconsistent internal state.");

  // Loop over subints, chans, compute flux
  for (int isub=0; isub<nsub; isub++) {
    Reference::To<const Integration> i = arch->get_Integration(isub);
    for (int ichan=0; ichan<nchan; ichan++) {
      Reference::To<const Profile> p = i->get_Profile(0,ichan);
      Estimate<double> e(0.0,0.0);
      if (p->get_weight()!=0.0) e = flux->get_flux(p);
      ds_data[idx(ichan,isub)] = e.get_value();
      ds_data_err[idx(ichan,isub)] = e.get_error();
      if (ichan==0 && isub==0) { ds_max=ds_min=e.get_value(); }
      else {
        if (e.get_value() > ds_max) ds_max=e.get_value();
        if (e.get_value() < ds_min) ds_min=e.get_value();
      }
    }
  }

  computed = true;
}

// Unload to a file in some format...
void Pulsar::DynamicSpectrum::unload(const std::string& filename)
{
  if (!computed) compute();

  // Just dump ascii for now, replace with FITS (or ??) sometime soon
  FILE *fout = fopen(filename.c_str(), "w");
  if (fout==NULL) 
    throw Error(FailedSys, "DynamicSpectrum::unload",
        "fopen failed on %s", filename.c_str());

  // Write some header info
  fprintf(fout, "# Dynamic spectrum computed by psrflux\n");
  fprintf(fout, "# Data file: %s\n", arch->get_filename().c_str());
  fprintf(fout, "# Flux method: %s\n", flux->get_method().c_str());
  fprintf(fout, "# Flux units: %s\n", Scale2string(arch->get_scale()).c_str());
  fprintf(fout, "# MJD0: %.10f\n", arch->start_time().in_days());

  fprintf(fout, "# Data columns:\n");
  fprintf(fout, "# isub ichan time(min) freq(MHz) flux flux_err\n");
  for (int isub=0; isub<nsub; isub++) {
    for (int ichan=0; ichan<nchan; ichan++) {
      fprintf(fout, "%4d %4d %8.2f %8.2f %+.6e %+.6e\n", isub, ichan,
          get_rel_epoch(isub).in_minutes(),
          get_freq(ichan),
          ds_data[idx(ichan,isub)],
          ds_data_err[idx(ichan,isub)]);
    }
  }

  fclose(fout);

}

// Load from a file...
Pulsar::DynamicSpectrum 
*Pulsar::DynamicSpectrum::load(const std::string& filename)
{
  throw Error(InvalidState, "DynamicSpectrum::load",
      "load not implemented yet.");
}

