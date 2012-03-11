/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RFIMitigation.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

// Destructor
Pulsar::RFIMitigation::~RFIMitigation () {
  // For now there is nothing to do...
}

// Run through an archive, setting the weights arrays in each subint
void Pulsar::RFIMitigation::zap_chans (Pulsar::Archive* arch)
{
  Pulsar::Archive* copy = arch->clone();
  copy->pscrunch();
  
  vector<float> mask(copy->get_nchan());
  
  for (unsigned i = 0; i < copy->get_nsubint(); i++) {
    mask = zap_mask(copy->get_Integration(i));
    apply_mask(arch->get_Integration(i), mask);
  }
}

// Set the weights array in a Pulsar::Integration to zap strong birdies
vector<float> Pulsar::RFIMitigation::zap_mask (Pulsar::Integration* integ) {
  
  int nchan = integ->get_nchan();
  
  // Define a vector to hold the bandpass
  vector<double> mean;
  mean.resize(nchan);
  
  float phase = 0.0;
  Pulsar::Profile* profile = 0;
  
  for (int i = 0; i < nchan; i++) {
    profile = integ->get_Profile(0, i);
    phase = profile->find_min_phase();
    mean[i] = profile->mean(phase);
  }
  
  float global_mean = 0.0;
  
  for (int i = 0; i < nchan; i++) {
    global_mean += mean[i];
  }
  
  vector<double> running;
  running.resize(nchan);

  global_mean = global_mean / float(nchan);

  running[0] = (mean[0] + mean[1] + mean[2]) / 3.0;
  running[1] = (mean[0] + mean[1] + mean[2] + mean[3]) / 4.0;

  for (int i = 0; i < nchan; i++) {
    int hbs = 7;
    for (int j = 0; j < hbs; j++) {
      int num = 0;
      float val = 0.0;
      if ((i-j) > 0) {
	val += mean[i-j];
	num++;
      }
      if ((i+j) < (nchan-1)) {
	val += mean[i+j];
	num++;
      }
      running[i] = val / float(num);
    }
  }

  // Define a vector to hold the mask
  vector<float> mask(nchan);
  
  for (int i = 0; i < nchan; i++) {
    if ( fabs(mean[i] - running[i]) > (global_mean / 2.0) )
      mask[i] = 0.0;
    else
      mask[i] = 1.0;
  }
  
  return mask;
}

// Manually set specific channel weights to zero
void Pulsar::RFIMitigation::zap_specific (Pulsar::Archive* arch, vector<float> mask)
{
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    apply_mask(arch->get_Integration(i), mask);
  }
}

// Manually set specific channel weights in specific subints to zero
void Pulsar::RFIMitigation::zap_very_specific (Pulsar::Archive* arch, 
					       vector<float> mask, vector<unsigned> subs)
{
  for (unsigned i = 0; i < subs.size(); i++) {
    if (subs[i] >= 0 && subs[i] < arch->get_nsubint())
      apply_mask(arch->get_Integration(subs[i]), mask);
  }
}

// Apply a zap mask to an integration. Channels whose mask value is 1.0
// are not changed, all other channels have their weights set to the
// value in the mask vector
void Pulsar::RFIMitigation::apply_mask (Pulsar::Integration* integ, vector<float> mask)
{
  if (mask.size() != integ->get_nchan())
    throw Error(InvalidParam, "RFIMitigation::apply_mask incorrect length");
  
  for (unsigned i = 0; i < integ->get_nchan(); i++) {
    for (unsigned j = 0; j < integ->get_npol(); j++) {
      if (mask[i] != 1.0)
	integ->get_Profile(j,i)->set_weight(mask[i]);
    }
  }
}

void Pulsar::RFIMitigation::init () {
  // For now there is nothing to do...
}















