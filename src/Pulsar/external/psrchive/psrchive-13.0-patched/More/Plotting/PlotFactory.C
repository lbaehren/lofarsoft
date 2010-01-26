/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFactory.h"

#include "Pulsar/ProfilePlot.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PhaseVsHist.h"
#include "Pulsar/PosAngPlot.h"
#include "Pulsar/EllAngPlot.h"
#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/StokesSpherical.h"
#include "Pulsar/SpectrumPlot.h"
#include "Pulsar/PhaseVsFrequencyPlus.h"
#include "Pulsar/PhaseVsHistPlus.h"
#include "Pulsar/StokesFluctPlot.h"
#include "Pulsar/TwoBitPlot.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/CalibratorSpectrum.h"
#include "Pulsar/LinePhasePlot.h"
#include "Pulsar/DynamicSNSpectrumPlot.h"
#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicCalSpectrumPlot.h"
#include "Pulsar/DynamicWeightSpectrumPlot.h"
#include "Pulsar/FreqVsSNRPlot.h"
#include "Pulsar/CalPhVFreqPlot.h"
#include "Pulsar/BandpassPlot.h"
#include "Pulsar/ChannelWeightsPlot.h"
#include "Pulsar/BandpassChannelWeightPlot.h"
#include "Pulsar/DigitiserStatsPlot.h"
#include "Pulsar/DigitiserCountsPlot.h"

#include "Error.h"
#include "pad.h"

// Add plots here
Pulsar::PlotFactory::PlotFactory ()
{
  add ( new PlotFactory::Advocate<ProfilePlot>
	('D', "flux", "Single plot of flux") );

  add ( new PlotFactory::Advocate<StokesPlot>
	('s', "stokes", "Stokes parameters") );

  add ( new PlotFactory::Advocate<StokesCylindrical>
	('S', "Scyl",   "Stokes; vector in cylindrical") );

  add ( new PlotFactory::Advocate<StokesSpherical>
	('m', "Ssph",   "Stokes; vector in spherical") );

  add ( new PlotFactory::Advocate<StokesFluctPlot>
	('U', "Sfluct", "Stokes; fluctuation power spectra") );

  add ( new PlotFactory::Advocate<PhaseVsFrequency>
	('G', "freq", "Phase vs. frequency image of flux") );

  add ( new PlotFactory::Advocate<PhaseVsFrequencyPlus>
	('F', "freq+", "freq + integrated profile and spectrum") );

  add ( new PlotFactory::Advocate<PhaseVsTime>
	('Y', "time", "Phase vs. time image of flux") );

  add ( new PlotFactory::Advocate<PosAngPlot>
	('o', "pa", "Orientation (Position) angle") );

  add ( new PlotFactory::Advocate<EllAngPlot>
	('e', "ell", "Ellipticity angle") );

  add ( new PlotFactory::Advocate<Poincare>
	('P', "p3d", "Stokes vector in Poincare space") );

  add ( new PlotFactory::Advocate<PhaseVsHist>
	('h', "pahist", "Phase-resolved position angle histogram") );

  add ( new PlotFactory::Advocate<PhaseVsHistPlus>
	('H', "pahist+", "Phase-resolved PA histogram and total profile") );

  add ( new PlotFactory::Advocate<SpectrumPlot>
	('b', "psd", "Pulsed power spectrum") );

  add ( new PlotFactory::Advocate<TwoBitPlot>
	('2', "2bit", "Two-bit distribution") );

  add ( new PlotFactory::Advocate<CalibratorSpectrum>
	('C', "cal", "Calibrator Spectrum") );
  
  add ( new PlotFactory::Advocate<LinePhasePlot>
        ('R', "line", "Line phase subints" ) );
  
  add ( new PlotFactory::Advocate<DynamicSNSpectrumPlot>
        ( 'j', "dspec", "Dynamic S/N spectrum" ) );
  
  add ( new PlotFactory::Advocate<DynamicBaselineSpectrumPlot>
        ( 'r', "offdspec", "Dynamic baseline spectrum" ) );
  
  add ( new PlotFactory::Advocate<DynamicCalSpectrumPlot>
        ( 'l', "dcal", "Calibrator dynamic spectrum" ) );
  
  add ( new PlotFactory::Advocate<DynamicWeightSpectrumPlot>
        ( 'w', "dweight", "Weights dynamic spectrum" ) );
  
  add ( new PlotFactory::Advocate<FreqVsSNRPlot>
        ( 'n', "snrspec", "S/N ratio" ) );
  
  add ( new PlotFactory::Advocate<CalPhVFreqPlot>
        ( 'X', "calphvf", "Calibrator phase vs frequency plot" ) );
  
  add ( new PlotFactory::Advocate<BandpassPlot>
        ( 'd', "bandpass", "Display off-pulse bandpass" ) );
  
  add ( new PlotFactory::Advocate<ChannelWeightsPlot>
        ( 'c', "chweight", "Display Channel Weights" ) );
  
  add ( new PlotFactory::Advocate<BandpassChannelWeightPlot>
        ( 'B', "bpcw", "Plot off-pulse bandpass and channel weights" ) );
  
  add ( new PlotFactory::Advocate<DigitiserStatsPlot>
        ( 'K', "digstats", "Digitiser Statistics" ) );
  
  add ( new PlotFactory::Advocate<DigitiserCountsPlot>
        ( 'A', "digcnts", "Digitiser Counts histogram" ) );
}

//! Return a new instance of the named plotter
Pulsar::Plot* Pulsar::PlotFactory::construct (std::string name)
{
  char shortcut = 0;
  if (name.length() == 1)
    shortcut = name[0];

  for (unsigned i=0; i < agents.size(); i++)
    if ( (shortcut && shortcut == agents[i]->get_shortcut()) ||
	 (name == agents[i]->get_name()) )
      return agents[i]->construct();

  throw Error (InvalidParam, "Pulsar::PlotFactory::construct",
	       "no Plot named " + name);
}
    
//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_name (unsigned i)
{
  return agents[i]->get_name();
}

//! Return the name of the ith agent
std::string Pulsar::PlotFactory::get_description (unsigned i)
{
  return agents[i]->get_description();
}

std::string Pulsar::PlotFactory::help()
{
  unsigned ia, maxlen = 0;
  for (ia=0; ia < agents.size(); ia++)
    if (get_name(ia).length() > maxlen)
      maxlen = get_name(ia).length();

  maxlen += 2;

  std::string result;

  for (ia=0; ia < agents.size(); ia++)
    result += pad(maxlen, get_name(ia)) + 
      "[" + agents[ia]->get_shortcut() + "]  " + get_description(ia) + "\n";
  
  return result;
}
