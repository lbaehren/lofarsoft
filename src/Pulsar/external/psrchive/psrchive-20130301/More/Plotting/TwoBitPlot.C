/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TwoBitPlot.h"
#include "Pulsar/TwoBitStats.h"

Pulsar::TwoBitPlot::TwoBitPlot ()
{
}

TextInterface::Parser* Pulsar::TwoBitPlot::get_interface ()
{
  return new Interface (this);
}

//! Return the label for the y-axis
std::string Pulsar::TwoBitPlot::get_ylabel (const Archive* data)
{
  return plot.get_ylabel();
}

//! Return the label for the x-axis
std::string Pulsar::TwoBitPlot::get_xlabel (const Archive* data)
{
  return plot.get_xlabel();
}

/*! The ProfileVectorPlotter class draws the profile */
void Pulsar::TwoBitPlot::draw (const Archive* data)
{
  plot.plot ();
}

class TwoBitProbability : public JenetAnderson98::Probability {
  
public:
  
  TwoBitProbability (const Pulsar::Archive* archive)
  {
    tbs = archive->get<Pulsar::TwoBitStats>();
    if (!tbs)
      throw Error (InvalidParam, "TwoBitProbability",
		   "Archive does not contain two bit statistics extension");
  }

  //! Get the number of samples in each histogram
  unsigned get_nsample() const
  { return tbs->get_nsample(); }
  
  //! Get the sampling threshold as a fraction of the noise power
  float get_threshold () const
  { return tbs->get_threshold(); }
  
  //! Get the cut off power for impulsive interference excision
  float get_cutoff_sigma () const
  { return tbs->get_cutoff_sigma(); }

  //! Get the number of digitizers
  unsigned get_ndig () const
  { return tbs->get_ndig(); }

  //! Get the specified histogram
  void get_histogram (std::vector<float>& h, unsigned dig) const
  { h = tbs->get_histogram(dig); }

protected:

  Reference::To<const Pulsar::TwoBitStats> tbs;
  
};

/*!  */
void Pulsar::TwoBitPlot::prepare (const Archive* data) try
{
  plot.set_interface( new TwoBitProbability(data) );
  get_frame()->get_x_scale()->set_minmax (plot.get_xmin(), plot.get_xmax());
  get_frame()->get_y_scale()->set_minmax (plot.get_ymin(), plot.get_ymax());
}
 catch (Error& error)
   {
     throw error += "Pulsar::TwoBitPlot::prepare";
   }
