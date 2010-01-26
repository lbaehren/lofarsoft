//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeasurementEquation.h,v $
   $Revision: 1.4 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __Calibration_MeasurementEquation_H
#define __Calibration_MeasurementEquation_H

#include "MEAL/Transformation.h"
#include "MEAL/VectorRule.h"
#include "MEAL/Complex2.h"
#include "MEAL/Real4.h"

namespace Calibration {

  //! Models multiple transformations of multiple inputs
  /*! This represents the propagation of multiple source states
    through multiple signal paths. */

  class MeasurementEquation : public MEAL::Complex2
  {

  public:

    //! Default constructor
    MeasurementEquation ();

    //! Destructor
    ~MeasurementEquation ();

    //! Set the input, \f$ \rho_j \f$, where \f$ j \f$ = get_input_index
    virtual void set_input (Complex2* state);

    //! Add an input, \f$ \rho_N \f$, where \f$ N \f$ = get_num_input
    virtual void add_input (Complex2* state = 0);

    //! Get the current input
    Complex2* get_input ();

    //! Get the number of input states
    unsigned get_num_input () const;

    //! Get the input index, \f$ j \f$
    unsigned get_input_index () const;

    //! Set the input index, \f$ j \f$
    void set_input_index (unsigned index);

    //! Set the transformation,\f$J_i\f$,where \f$i\f$=get_transformation_index
    virtual void set_transformation (Complex2* xform);

    //! Add a transformation, \f$J_P\f$, where \f$P\f$ = get_num_transformation
    virtual void add_transformation (Complex2* xform = 0);

    //! Set the transformation,\f$M_i\f$,where \f$i\f$=get_transformation_index
    virtual void set_transformation (MEAL::Real4* xform);

    //! Add a transformation, \f$M_P\f$, where \f$P\f$ = get_num_transformation
    virtual void add_transformation (MEAL::Real4* xform = 0);

    //! Get the current transformation
    MEAL::Transformation<Complex2>* get_transformation ();

    //! Get the number of transformation states
    unsigned get_num_transformation () const;

    //! Get the transformation index, \f$ i \f$
    unsigned get_transformation_index () const;

    //! Set the transformation index, \f$ i \f$
    void set_transformation_index (unsigned index);

  protected:

    //! Returns \f$ \rho^\prime \f$ and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! Construct new integrated instance of CongruenceTransformation
    MEAL::Transformation<Complex2>* new_transformation (Complex2*);

    //! Construct new integrated instance of MuellerTransformation
    MEAL::Transformation<Complex2>* new_transformation (MEAL::Real4*);

    //! Composite parameter policy
    MEAL::Composite composite;

    //! The inputs, \f$ \rho_j \f$
    MEAL::VectorRule< MEAL::Complex2 > inputs;

    //! The transformations, \f$ J_i \f$ and \f$ M_k \f$
    MEAL::VectorRule< MEAL::Transformation<MEAL::Complex2> > xforms;

  private:

    MEAL::Project<Complex2> inputs_map;
    MEAL::Project<Complex2> xforms_map;

  };

}

#endif

