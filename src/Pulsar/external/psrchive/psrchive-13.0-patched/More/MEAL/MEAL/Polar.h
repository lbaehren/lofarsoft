//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Polar.h,v $
   $Revision: 1.11 $
   $Date: 2009/06/10 10:53:53 $
   $Author: straten $ */

#ifndef __MEAL_Polar_H
#define __MEAL_Polar_H

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2.h"
#include "Quaternion.h"
#include "Estimate.h"

namespace MEAL {

  template<typename> class Gain;
  class Boost;
  class Rotation;

  //! Represents the polar decomposition of a transformation
  /*! The transformation is parameterized by the gain, G, the boost vector,
    \f${\bf b}=\sinh\beta\hat{m}\f$, and the rotation vector,
    \f${\bf r}=\phi_i\hat{n}\f$. */
  class Polar : public ProductRule<Complex2> {

  public:

    //! Default Constructor
    Polar ();

    //! Copy Constructor
    Polar (const Polar& s);

    //! Assignment Operator
    const Polar& operator = (const Polar& s);

    //! Destructor
    ~Polar ();

    //! Get the instrumental gain, \f$ G \f$, in calibrator voltage units
    Estimate<double> get_gain () const;

    //! Get the specified component of the boost vector
    Estimate<double> get_boost (unsigned i) const;
    
    //! Get the specified component of the rotation vector
    Estimate<double> get_rotation (unsigned i) const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator voltage units
    void set_gain (const Estimate<double>& gain);

    //! Set the specified component of the boost vector
    void set_boost (unsigned i, const Estimate<double>& b_i);
    
    //! Set the specified component of the rotation vector
    void set_rotation (unsigned i, const Estimate<double>& phi_i);

    //! Polar decompose the Jones matrix
    void solve (Jones< Estimate<double> >& jones);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Provide access to the gain transformation
    const Gain<Complex2>* get_gain_transformation () const;
    Gain<Complex2>* get_gain_transformation ();

    //! Provide access to the boost transformation
    const Boost* get_boost_transformation () const;
    Boost* get_boost_transformation ();

    //! Provide access to the rotation transformation
    const Rotation* get_rotation_transformation () const;
    Rotation* get_rotation_transformation ();

  protected:

    //! Gain<Complex2>
    Reference::To< Gain<Complex2> > gain;

    //! Boost Complex2
    Reference::To<Boost> boost;

    //! Rotation Complex2s
    Reference::To<Rotation> rotation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

