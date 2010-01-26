//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ComplexRVM.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/11 07:33:27 $
   $Author: straten $ */

#ifndef __ComplexRVM_H
#define __ComplexRVM_H

#include "MEAL/Complex.h"
#include "MEAL/ProductRule.h"

namespace MEAL {

  template<typename> class VectorRule;
  class RotatingVectorModel;

  //! Rotating Vector Model of Stokes Q and U as a function of pulse phase
  class ComplexRVM : public ProductRule<Complex>
  {

  public:

    //! Default constructor
    ComplexRVM ();

    //! Copy constructor
    ComplexRVM (const ComplexRVM& copy);

    //! Assignment operator
    ComplexRVM& operator = (const ComplexRVM& copy);

    //! Destructor
    ~ComplexRVM ();

    //! Return the rotating vector model
    RotatingVectorModel* get_rvm ();

    //! Add a state: phase in radians, L is first guess of linear polarization
    void add_state (double phase, double L);
    //! Set the current state for which the model will be evaluated
    void set_state (unsigned i);

    //! Get the number of states
    unsigned get_nstate () const;

    //! Set the phase of the ith state
    void set_phase (unsigned i, double phase);
    //! Get the phase of the ith state
    double get_phase (unsigned i) const;

    //! Set the linear polarization of the ith state
    void set_linear (unsigned i, const Estimate<double>& L);
    //! Get the linear polarization of the ith state
    Estimate<double> get_linear (unsigned i) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    Reference::To<RotatingVectorModel> rvm;
    Reference::To< VectorRule<Complex> > gain;

    class State;
    std::vector<State> state;

    void check (unsigned i, const char* method) const;
    void init ();
  };

}

#endif
