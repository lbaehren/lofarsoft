//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/DispersionDelay.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_DispersionDelay_H
#define __Pulsar_DispersionDelay_H

namespace Pulsar {

  //! Computes the dispersion delay
  /*! This class computes the dispersion delay between two frequencies, as
    parameterized by the dispersion measure.  Although the value:

    \f$ DM\,({\rm pc\,cm^{-3}})
          = 2.410331(2)\times10^{-4} D\,({\rm s\,MHz^{2}}) \f$

    has been derived from "fundamental and primary physical and
    astronomical constants" (section 3 of Backer, Hama, van Hook and
    Foster 1993. ApJ 404, 636-642), the rounded value with three
    digits of precision is in standard use by pulsar astronomers (page
    129 of Manchester and Taylor 1977).
  */
  class DispersionDelay {

  public:

    //! Default constructor
    DispersionDelay ();

    //! Set the dispersion measure in \f$ {\rm pc\,cm}^{-3} \f$
    void set_dispersion_measure (const double dispersion_measure);
    //! Get the dispersion measure in \f$ {\rm pc\,cm}^{-3} \f$
    double get_dispersion_measure () const;

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

    //! Get the dispersion delay
    double evaluate () const;

    //! enable use with the ColdPlasma template
    typedef double Return;
    //! Set the dispersion measure
    void set_measure (double dispersion_measure);
    //! Get the dispersion measure
    double get_measure () const;

  protected:

    //! The dispersion measure
    double dispersion_measure;

    //! Reference wavelength in metres
    double reference_wavelength;

    //! Wavelength in metres
    double wavelength;

  };

}

#endif
