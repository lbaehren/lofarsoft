//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/MatrixTemplateMatching.h,v $
   $Revision: 1.2 $
   $Date: 2009/10/02 03:40:50 $
   $Author: straten $ */

#ifndef __Pulsar_MatrixTemplateMatching_h
#define __Pulsar_MatrixTemplateMatching_h

#include "Pulsar/ArrivalTime.h"

namespace Pulsar {

  class PulsarCalibrator;

  //! Estimates phase shift in Fourier domain using matrix template matching
  class MatrixTemplateMatching : public ArrivalTime
  {

  public:

    MatrixTemplateMatching ();
    ~MatrixTemplateMatching ();

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Allow software to choose the maximum harmonic
    void set_choose_maximum_harmonic (bool flag = true);

    //! Prepare the data for use
    void preprocess (Archive* archive);

    //! Set the observation from which the arrival times will be derived
    void set_observation (const Archive*);

    //! Set the standard/template to which observation will be matched
    void set_standard (const Archive*);

  protected:

    //! The PulsarCalibrator class takes care of many details
    Reference::To<PulsarCalibrator> engine;

    //! get the arrival times for the specified sub-integration
    void get_toas (unsigned subint, std::vector<Tempo::toa>& toas);
  };

}


#endif // !defined __Pulsar_MatrixTemplateMatching_h
