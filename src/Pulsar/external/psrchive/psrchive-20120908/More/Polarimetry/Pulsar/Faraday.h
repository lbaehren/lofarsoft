//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Faraday.h,v $
   $Revision: 1.10 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_Faraday_H
#define __Calibration_Faraday_H

#include "MEAL/Rotation1.h"

namespace Calibration {

  //! Faraday rotation transformation
  /*! This class computes the Faraday rotation between two frequencies, as
    parameterized by the rotation measure. */
  class Faraday : public MEAL::Complex2 {

  public:
 
    //! Default constructor
    Faraday ();

    //! Return the name of the class
    std::string get_name () const;

    //! Set the rotation measure
    void set_rotation_measure (const Estimate<double>& rotation_measure);
    //! Get the rotation measure
    Estimate<double> get_rotation_measure () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the frequency in MHz
    void set_frequency (double MHz);
    //! Get the frequency in MHz
    double get_frequency () const;

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength in metres
    double get_reference_wavelength () const;

    //! Set the wavelength in metres
    void set_wavelength (double metres);
    //! Get the wavelength in metres
    double get_wavelength () const;

    //! Set the axis about which the rotation occurs
    void set_axis (const Vector<3, double>& axis);

    //! Get the Faraday rotation angle
    double get_rotation () const;

    //! enable use with the ColdPlasma template
    typedef Jones<double> Return;
    //! Set the rotation measure
    void set_measure (double rotation_measure);
    //! Get the rotation measure
    double get_measure () const;

  protected:

    //! The Rotation operation
    MEAL::Rotation1 rotation;

    //! Reference wavelength in metres
    double reference_wavelength;

    //! Wavelength in metres
    double wavelength;

    // ///////////////////////////////////////////////////////////////////
    //
    // MEAL::Complex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* grad);

  };

}

#endif
