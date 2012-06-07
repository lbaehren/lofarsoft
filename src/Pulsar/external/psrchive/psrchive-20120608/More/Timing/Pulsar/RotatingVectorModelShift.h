//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/FluxCentroid.h,v $
   $Revision: 1.2 $
   $Date: 2009/10/02 03:40:45 $
   $Author: straten $ */

#ifndef __Pulsar_RotatingVectorModelShift_h
#define __Pulsar_RotatingVectorModelShift_h

#include "Pulsar/PolnProfileShiftEstimator.h"

namespace Pulsar {

  class ComplexRVMFit;

  //! Estimates phase shift using phi0 from the best-fit rotating vector model
  class RotatingVectorModelShift : public PolnProfileShiftEstimator
  {

  public:

    //! Default constructor
    RotatingVectorModelShift ();

    //! Copy constructor
    RotatingVectorModelShift (const RotatingVectorModelShift&);

    //! Assignment operator
    const RotatingVectorModelShift&
    operator= (const RotatingVectorModelShift&);

    //! Destructor
    ~RotatingVectorModelShift ();

    //! Return a copy constructed instance of self
    RotatingVectorModelShift* clone () const;

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Prepare the data for use
    void preprocess (Archive*);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<RotatingVectorModelShift> ("RVM"); }

  protected:

    //! The ComplexRVM class used to perform the fit
    Reference::To<ComplexRVMFit> rvmfit;

  };

}


#endif // !defined __Pulsar_RotatingVectorModelShift_h
