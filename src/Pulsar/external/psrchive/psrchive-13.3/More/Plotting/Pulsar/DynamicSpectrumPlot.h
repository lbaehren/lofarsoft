//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicSpectrumPlot_h
#define __Pulsar_DynamicSpectrumPlot_h

#include "Pulsar/SimplePlot.h"
#include "ColourMap.h"

namespace Pulsar {

  class Profile;

  //! Base class for image plots of something vs time and frequency
  class DynamicSpectrumPlot : public SimplePlot {

  public:

    //! Default constructor
    DynamicSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the DynamicSpectrumPlot class
    class Interface : public TextInterface::To<DynamicSpectrumPlot> {
    public:
      Interface (DynamicSpectrumPlot* = 0);
    };

    //! Figure out axis limits
    void prepare(const Archive* data);

    //! Preprocess Archive 
    void preprocess(const Archive* data);

    //! Derived classes must fill in the nsubint by nchan data array
    virtual void get_plot_array(const Archive *data, float *array) = 0;

    //! Draw in the current viewport
    void draw (const Archive*);

    //! Provide access to the colour map
    pgplot::ColourMap* get_colour_map () { return &colour_map; }

    //! srange is the subint range
    std::pair<int,int> get_srange() const { return srange; }
    void set_srange( const std::pair<int,int> &s_srange ) { srange = s_srange; }
    void set_srange( int fsub, int lsub ) { 
      set_srange(std::pair<int,int>(fsub,lsub)); 
    }

    //! Returns x axis label
    std::string get_xlabel (const Archive *data);
    //! Returns y axis label
    std::string get_ylabel (const Archive *data);
      
    //! Pick a polarization
    void set_pol (int s_pol) { pol = s_pol; }
    int get_pol() const { return pol; }
    
  protected:

    pgplot::ColourMap colour_map;

    std::pair<int,int> srange;
    std::pair<int,int> crange;
    int pol;

    bool zero_check;

  };

}

#endif
