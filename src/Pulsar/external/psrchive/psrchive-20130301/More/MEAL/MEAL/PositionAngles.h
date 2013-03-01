//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/PositionAngles.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __PositionAngles_H
#define __PositionAngles_H

#include "MEAL/Scalar.h"
#include "MEAL/VectorRule.h"

namespace MEAL {

  class ScalarValue;
  class ScalarParameter;

  //! Function returns position angle(s) as a function of frequency
  /*! This function is parameterized by the rotation measure and one or
   more position angles at infinite frequency.  Its arguments are the
   position angle array index and the frequency. */
  class PositionAngles : public Scalar
  {

  public:

    //! Default constructor
    PositionAngles ();

    //! Copy constructor
    PositionAngles (const PositionAngles& copy);

    //! Assignment operator
    PositionAngles& operator = (const PositionAngles& copy);

    //! Destructor
    ~PositionAngles ();

    //! Get the size of the position angle array
    unsigned size () const;

    //! Set the index of the position angle array
    void set_index (unsigned index);

    //! Get the index of the position angle array
    unsigned get_index () const;

    //! Add to the position angle array
    void push_back (const Estimate<double>& pa);

    //! Set the radio frequency in MHz
    void set_frequency (double frequency);

    //! Get the radio frequency in MHz
    double get_frequency () const;

    //! Set the position angle at infinite frequency
    void set_position_angle (unsigned index, const Estimate<double>& pa);

    //! Get the position angle at infinite frequency
    Estimate<double> get_position_angle (unsigned index) const;

    //! Set the rotation measure
    void set_rotation_measure (const Estimate<double>& rotation_measure);

    //! Get the rotation measure
    Estimate<double> get_rotation_measure () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    //! The position angle array
    std::vector<ScalarParameter*> position_angle;

    //! The rotation measure
    ScalarParameter* rotation_measure;

    //! The frequency
    ScalarValue* frequency;

    //! The change in position angle
    Reference::To<Scalar> delta_pa;

    //! The implementation
    VectorRule<Scalar> pa;

    void calculate (double&, std::vector<double>*) {}
    void init ();

  };

}

#endif
