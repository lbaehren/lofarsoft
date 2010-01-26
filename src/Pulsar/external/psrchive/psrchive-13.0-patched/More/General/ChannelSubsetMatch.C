/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ChannelSubsetMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Database.h"

#include "strutil.h"

using namespace std;

Pulsar::ChannelSubsetMatch::ChannelSubsetMatch()
{
  freq_tol = 1e-7;
  bw_tol   = 1e-3;

  reason="";
}

Pulsar::ChannelSubsetMatch::~ChannelSubsetMatch()
{
}

bool Pulsar::ChannelSubsetMatch::match (const Pulsar::Archive* super,
    const Pulsar::Archive* sub)
{
  reason="";

  // If n_sub > n_super, we're done
  if (sub->get_nchan() > super->get_nchan()) {
    reason = stringprintf("Subset n_chan (%d) > n_chan (%d)", 
        sub->get_nchan(), super->get_nchan());
    return false;
  }

  const double sub_chbw = sub->get_bandwidth() / (double)sub->get_nchan();
  const double super_chbw = super->get_bandwidth() / (double)super->get_nchan();

  // If channel BWs don't match, return false for now.
  if (fabs((sub_chbw-super_chbw)/super_chbw) > bw_tol) {
    reason = stringprintf("Subset chan_bw (%f) != chan_bw (%f)", 
        sub_chbw, super_chbw);
    return false;
  }

  // Loop over "sub" chans, make sure they all exist in super
  unsigned n_matched=0;
  for (unsigned i=0; i<sub->get_nchan(); i++) {
    double sub_freq = sub->get_Integration(0)->get_centre_frequency(i);
    for (unsigned j=0; j<super->get_nchan(); j++) {
      double super_freq = super->get_Integration(0)->get_centre_frequency(j);
      if (fabs((sub_freq-super_freq)/super_freq) < freq_tol) {
        n_matched++;
        break;
      }
    }
  }

  if (n_matched==sub->get_nchan())
    return true;
  else {
    reason = stringprintf("Only matched %u/%u channels", 
        sub->get_nchan(), n_matched);
    return false;
  }
}

bool Pulsar::ChannelSubsetMatch::match (const Pulsar::Database::Entry& super,
    const Pulsar::Database::Entry& sub)
{
  reason="";

  // If n_sub > n_super, we can exit right away
  if (sub.nchan > super.nchan) {
    reason = stringprintf("Subset n_chan (%d) > n_chan (%d)", 
        sub.nchan, super.nchan);
    return false;
  }

  const double sub_chbw = sub.bandwidth / (double)sub.nchan;
  const double super_chbw = super.bandwidth / (double)super.nchan;

  // If channel BWs don't match, return false for now.
  if (fabs((sub_chbw-super_chbw)/super_chbw) > bw_tol) {
    reason = stringprintf("Subset chan_bw (%f) != chan_bw (%f)", 
        sub_chbw, super_chbw);
    return false;
  }

  // Loop over "sub" chans, make sure they all exist in super
  unsigned n_matched=0;
  for (unsigned i=0; i<sub.nchan; i++) {
    double sub_freq = sub.frequency - sub.bandwidth/2.0 + 
      (double)i * sub_chbw;
    for (unsigned j=0; j<super.nchan; j++) {
      double super_freq = super.frequency - super.bandwidth/2.0 + 
        (double)j * super_chbw;
      if (fabs((sub_freq-super_freq)/super_freq) < freq_tol) {
        n_matched++;
        break;
      }
    }
  }

  if (n_matched==sub.nchan)
    return true;
  else {
    reason = stringprintf("Only matched %u/%u channels", 
        sub.nchan, n_matched);
    return false;
  }
}

int Pulsar::ChannelSubsetMatch::super_channel (const Pulsar::Archive* super,
    const Pulsar::Archive* sub, int subchan) try
{
  double freq = sub->get_Integration(0)->get_centre_frequency(subchan);
  return match_channel (super->get_Integration(0), freq);
			
}
 catch (Error& error)
   {
     return -1;
   }

int Pulsar::ChannelSubsetMatch::sub_channel (const Pulsar::Archive* super,
    const Pulsar::Archive* sub, int superchan) try
{
  double freq = super->get_Integration(0)->get_centre_frequency(superchan);
  return match_channel (sub->get_Integration(0), freq);
}
 catch (Error& error)
   {
     return -1;
   }
