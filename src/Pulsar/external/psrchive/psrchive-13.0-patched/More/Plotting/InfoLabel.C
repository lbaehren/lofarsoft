/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/InfoLabel.h"
#include "Pulsar/FluxPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "strutil.h"

using namespace std;

//! Default constructor
Pulsar::InfoLabel::InfoLabel (FluxPlot* _flux)
{
  if (_flux)
    _flux->get_frame()->set_label_above(this);
  flux = _flux;
}

//! Destructor
Pulsar::InfoLabel::~InfoLabel ()
{
}

const char* wavelen_str (double freq);
const char* weight_str (float weight);

void Pulsar::InfoLabel::plot (const Archive* data)
{
  bool calibrated = data -> get_scale() == Signal::Jansky;

  // short term solution ...
  const Profile* profile = flux->get_plotter()->profiles[0];

  float snr = profile -> snr();
  
  string toplabel = stringprintf ("%6.1f snr  ", snr);
  toplabel += wavelen_str (data -> get_centre_frequency());
  toplabel += stringprintf ("/%d",(int)fabs(data -> get_bandwidth()));
  
  // Work out if calibrated and if so write out flux in mJy 
  if (calibrated) {
    float flux = profile->mean(0.5, 1.0);
    
    double mean,var,varmean;
    profile->stats(profile->find_min_phase(),&mean,&var,&varmean);

    // Note that this is a fairly arbitrary error estimate, since the
    // variance in the mean may change depending on the duty cycle
    // used by Profile::stats. Consider it a guide only. AWH 30/08/2006
    
    float sd = sqrt(varmean);
 
    char temp[64];
    sprintf(temp, " %.1f +/- %.1f mJy", flux, sd);
    toplabel += temp;
  }
  else {
    toplabel += " Uncalibrated";
  }
  
  string third_line = data -> get_source();

  Reference::To<const Integration> subint;
  subint = get_Integration (data, flux->get_subint());

  double seconds_int = subint -> get_duration();
  
  third_line += "  (" + time_string(seconds_int) + ")";

  float the_weight = profile -> get_weight();
  third_line += "  ";
  third_line += weight_str(the_weight);

  row (data->get_filename(), 0, 3, .5);
  row (toplabel,             1, 3, .5);
  row (third_line,           2, 3, .5);

}

const char* wavelen_str (double freq)
{
  if (freq < 500.0)
    return "70cm";
  else if (freq < 800.0)
    return "50cm";
  else if (freq < 1600.0)
    return "20cm";
  else if (freq < 1700.0)
    return "18cm";
  else if (freq < 2000.0)
    return "15cm";
  else if (freq < 2400.0)
    return "13cm";
  else if (freq < 3600.0)
    return "10cm";
  else if (freq < 5500.0)
    return "6cm";
  else if (freq < 9000.0)
    return "3cm";
  else
    return "1cm";
}

const char* weight_str (float weight)
{
  static char weightstr [80];
  
  if (weight < 0.9)
    sprintf (weightstr, "Weight = %.1f (BAD)", weight);
  else
    sprintf (weightstr, "Weight = %.1f (GOOD)", weight);
  
  return weightstr;
}
