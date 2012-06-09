//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotScale.h,v $
   $Revision: 1.16 $
   $Date: 2010/09/19 04:33:33 $
   $Author: straten $ */

#ifndef __Pulsar_PlotScale_h
#define __Pulsar_PlotScale_h

#include "Pulsar/PlotEdge.h"

namespace Pulsar {

  class Archive;

  //! The scale on an axis
  class PlotScale : public PlotEdge {

  public:

    //! Default constructor
    PlotScale ();

    // Text interface to the PlotScale class
    class Interface : public TextInterface::To<PlotScale>
    {
    public:
      Interface (PlotScale* = 0);
    };

    virtual TextInterface::Parser* get_interface () 
    { return new Interface(this); }

    //! Initialize internal attributes according to Archive data
    virtual void init (const Archive*);

    //! Expand as necessary to include another PlotScale
    void include (PlotScale* other);

    //! Set the minimum and maximum value in the data
    void set_minmax (float min, float max);

    //! Return true when minimum and maximum value have been set
    bool get_minmaxset () const { return minmaxvalset; }

    //! Get the minimum and maximum value in the data
    void get_minmax (float& min, float& max) const;

    //! Return min and max scaled according to zoom
    void get_range (float& min, float& max) const;

    //! Return indeces scaled according to zoom
    void get_indeces (unsigned n, unsigned& imin, unsigned& imax,
		      bool cyclic = false) const;

    //! Return min and max scaled according to zoom
    void get_range_external (float& min, float& max) const;

    //! Set the world-normalized range on the axis
    void set_range_norm (const std::pair<float,float>& f);

    //! Get the world-normalized range on the axis
    std::pair<float,float> get_range_norm () const { return range_norm; }

    //! Convenience interface
    void set_range_norm (float min, float max) 
    { set_range_norm (std::pair<float,float>(min, max)); }

    //! Set the world-normalized buffer space on either side of the axis
    void set_buf_norm (float f) { buf_norm = f; }

    //! Get the world-normalized buffer space on either side of the axis
    float get_buf_norm () const { return buf_norm; }

    //! Set the world coordinate range in the viewport
    void set_world (const std::pair<float,float>& f);

    //! Get the world coordinate range in the viewport
    std::pair<float,float> get_world () const { return world; }

    //! Set the world coordinate range printed on the axis
    void set_world_external (const std::pair<float,float>& f);

    //! Get the world coordinate range printed on the axis
    std::pair<float,float> get_world_external () const;

    std::pair<float,float> viewport_to_world (const std::pair<float,float>&);
    double viewport_to_world (const double);

  protected:

    std::pair<float,float> world;

    std::pair<float,float> range_norm;

    std::pair<float,float> world_external;

    float buf_norm;

    float minval;
    float maxval;
    bool minmaxvalset;

  };

  
}

#endif
