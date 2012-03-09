//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Feed.h,v $
   $Revision: 1.8 $
   $Date: 2007/10/30 02:31:27 $
   $Author: straten $ */

#ifndef __Calibration_Feed_H
#define __Calibration_Feed_H

#include "MEAL/SumRule.h"
#include "MEAL/Rotation1.h"

#include "Estimate.h"

namespace Calibration {

  //! Phenomenological description of the receiver feed
  /*! This model represents the receiver feed as a non-ideal combination of
    two ideal receptors, as described by Equation 16 of van Straten (2004)
    which is equivalent to the transformation in Equation 16 of Britton
    (2000). */

  class Feed : public MEAL::SumRule<MEAL::Complex2> {

  public:

    //! Default Constructor
    Feed ();

    //! Copy Constructor
    Feed (const Feed& feed);

    //! Assignment Operator
    Feed& operator = (const Feed& feed);

    //! Destructor
    ~Feed ();

    //! Enforce cyclical limits on parameters
    void set_cyclic (bool flag = true);

    //! Get the orientation, \f$\theta\f$, of the specified receptor (radians)
    Estimate<double> get_orientation (unsigned ireceptor) const;

    //! Get the ellipticity, \f$\chi\f$, of the specified receptor in radians
    Estimate<double> get_ellipticity (unsigned ireceptor) const;

    //! Set the orientation, \f$\theta\f$, of the specified receptor (radians)
    void set_orientation (unsigned ireceptor, const Estimate<double>& theta);

    //! Set the ellipticity, \f$\chi\f$, of the specified receptor in radians
    void set_ellipticity (unsigned ireceptor, const Estimate<double>& chi);

    //! Parse the feed parameters from the specified file
    void load (const std::string& filename);

    //! Parse a feed parameter from a line of text
    void parse (std::string line);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Get the orientation tranformation for the specified receptor
    MEAL::Rotation1* get_orientation_transformation( unsigned receptor );

    //! Get the ellipticity tranformation for the specified receptor
    MEAL::Rotation1* get_ellipticity_transformation( unsigned receptor );

  protected:

    //! The receptor orientation tranformations
    Reference::To<MEAL::Rotation1> orientation[2];

    //! The receptor ellipticity tranformations
    Reference::To<MEAL::Rotation1> ellipticity[2];

  private:
    
    //! Working method for the constructors
    void init ();

  };

}

#endif

