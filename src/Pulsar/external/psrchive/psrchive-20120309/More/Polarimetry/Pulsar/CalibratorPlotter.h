//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalibratorPlotter.h,v $
   $Revision: 1.14 $
   $Date: 2008/06/20 12:47:46 $
   $Author: straten $ */

#ifndef __CalibratorPlotter_H
#define __CalibratorPlotter_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  //! Plots Calibrator parameters
  class CalibratorPlotter : public Reference::Able
  {
    
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    CalibratorPlotter ();
    
    //! Destructor
    virtual ~CalibratorPlotter ();

    //! Plot the Pulsar::Calibrator
    virtual void plot (const Calibrator* calibrator);

    //! Plot the Pulsar::Calibrator
    virtual void plot (const Calibrator::Info* info, unsigned nchan,
		       double centre_frequency, double bandwidth);

    //! Plot the titles over the plot frames
    void plot_labels (const Calibrator::Info* info);

    //! Set whether the default output from EstimatePlotter is displayed
    void set_display_mean_single_line(const bool _display_mean_single_line);

    //! Get whether the default output from EstimatePlotter is displayed
    bool get_display_mean_single_line() const;

    //! Number of panels (vertical)
    unsigned npanel;

    //! Fraction of total plotting space placed between panels
    float between_panels;

    //! Use colour in plots
    bool use_colour;

    //! Print titles over top of panel
    bool print_titles;

    //! Title to print over top panel
    std::string title;

  private:

    //! Whether the default output (of the means) from EstimatePlotter
    //  should be displayed
    bool display_mean_single_line;

  };

}

#endif
