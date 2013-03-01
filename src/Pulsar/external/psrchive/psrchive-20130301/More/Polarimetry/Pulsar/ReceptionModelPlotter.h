//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionModelPlotter.h,v $
   $Revision: 1.7 $
   $Date: 2009/08/14 01:53:39 $
   $Author: straten $ */

#ifndef __Calibration_ReceptionModelPlotter_H
#define __Calibration_ReceptionModelPlotter_H

#include "Reference.h"

class EstimatePlotter;

namespace Calibration {

  class ReceptionModel;

  //! Plots a ReceptionModel
  class ReceptionModelPlotter : public Reference::Able
  {

  public:

    //! Default constructor
    ReceptionModelPlotter ();

    //! Destructor
    ~ReceptionModelPlotter ();

    //! Set the model to yield the Stokes parameters
    void set_model (ReceptionModel* model);

    //! Defines the abscissa
    class Abscissa;

    //! Set the epoch angle model to yield the plot abscissa
    void set_abscissa (Abscissa* para);

    //! Set the index of the Stokes parameter to be plotted
    void set_ipol (unsigned ipol);

    //! Set the index of the source to be plotted
    void set_isource (unsigned isource);

    //! Set the model solved flag
    void set_model_solved (bool solved = true);

    //! Set the plot residual flag
    void set_plot_residual (bool residual = true);

    //! Set the name of the file to which text format will be written
    void set_output (const char* filename);

    //! Plot the model in the current configuration
    virtual void plot_model ();

    //! Plot the observations corresponding to the current configuration
    virtual void plot_observations ();

    //! Plot the Stokes[ipol]
    virtual void plot_stokes (EstimatePlotter& plotter, unsigned iplot);

  protected:

    //! The model from which plot data will be taken
    Reference::To<ReceptionModel> model;

    //! The independent variable in each plot
    Reference::To<Abscissa> abscissa;

    //! The index of the Stokes parameter to be plotted
    unsigned ipol;

    //! The index of the source to be plotted
    unsigned isource;

    //! The index of the signal path
    unsigned ipath;

    //! The name of the file to which text format data will be written
    std::string output_filename;

    //! Flag set true when the model has been solved
    bool model_solved;

    //! Flag set true when the data-model residual should be plotted
    bool plot_residual;

  };


  class ReceptionModelPlotter::Abscissa : public Reference::Able
  {
  public:
    virtual void push_back () = 0;
    virtual void get_values (std::vector<double>& values) const = 0;
    virtual std::string get_label () const = 0;
  };

}

#endif
