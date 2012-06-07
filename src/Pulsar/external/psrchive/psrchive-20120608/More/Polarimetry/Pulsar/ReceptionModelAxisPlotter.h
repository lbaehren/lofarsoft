//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionModelAxisPlotter.h,v $
   $Revision: 1.10 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __Calibration_ReceptionModelAxisPlotter_H
#define __Calibration_ReceptionModelAxisPlotter_H

#include "Pulsar/ReceptionModelPlotter.h"
#include "MEAL/Axis.h"
#include "Pauli.h"

#include <cpgplot.h>

namespace Calibration {

  //! Plots model Stokes parameters as a function of time
  template<class Type>
  class ReceptionModelAxisPlotter : public ReceptionModelPlotter
  {

  public:

    ReceptionModelAxisPlotter () { axis = 0; npt = 100; }

    //! Set the Axis<Type> to which the epoch is connected
    void set_axis (MEAL::Axis<Type>* _axis) { axis = _axis; }

    //! Set the number of points in the plot
    void set_npt (unsigned _npt) { npt = _npt; }

    //! Set the minimum value on the axis
    void set_min (Type _min) { min = _min; }

    //! Set the maximum value on the axis
    void set_max (Type _max) { max = _max; }

    //! Plot the model in the current configuration
    void plot_model ();

  protected:

    //! The Axis<Type> to which the epoch is connected
    MEAL::Axis<Type>* axis;

    //! The number of points in the plot
    unsigned npt;

    //! The minimum value on the axis
    Type min;

    //! The maximum value on the axis
    Type max;

  };

}

template<typename T>
float cast_to_float (T t)
{
  return (float) t;
}

float cast_to_float (const MJD& t)
{
  return t.in_days();
}

/*! The Parallactic model must be connected to the specified Axis<Type> */
template<class Type>
void Calibration::ReceptionModelAxisPlotter<Type>::plot_model ()
{
  Type step = (max - min) / (npt - 1);

  model->set_transformation_index (ipath);
  model->set_input_index (isource);

  for (unsigned ipt=0; ipt<npt; ipt++) try
  {
    Type index = min + step * ipt;
    float val = cast_to_float (index);

    axis->set_value (index);

    Stokes<double> stokes = coherency( model->evaluate () );

    if (ipt == 0)
      cpgmove (val, stokes[ipol]);
    else
      cpgdraw (val, stokes[ipol]);
    
  }
  catch (Error& error)
  {
#ifdef _DEBUG
    std::cerr << "Calibration::ReceptionModelAxisPlotter<Type>::plot_model "
                 "ipt=" << ipt << " " << error.get_message() << std::endl;
#endif
  }
}


#endif

