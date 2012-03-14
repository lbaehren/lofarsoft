//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FluxCalibratorExtension.h,v $
   $Revision: 1.13 $
   $Date: 2009/02/20 06:34:32 $
   $Author: straten $ */

#ifndef __FluxCalibratorExtension_h
#define __FluxCalibratorExtension_h

#include "Pulsar/CalibratorExtension.h"

namespace Pulsar
{

  class FluxCalibrator;
  //! Flux Calibrator Extension
  /*! This Extension implements the storage of FluxCalibrator data. */

  class FluxCalibratorExtension : public CalibratorExtension
  {

    friend class FluxCalibrator;

  public:

    //! Default constructor
    FluxCalibratorExtension ();

    //! Copy constructor
    FluxCalibratorExtension (const FluxCalibratorExtension&);

    //! Operator =
    const FluxCalibratorExtension& operator= (const FluxCalibratorExtension&);

    //! Destructor
    ~FluxCalibratorExtension ();

    //! Clone method
    FluxCalibratorExtension* clone () const
      { return new FluxCalibratorExtension( *this ); }

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    // Text interface to a FluxCalibratorExtension
    class Interface : public TextInterface::To<FluxCalibratorExtension>
    {
      public:
	Interface( FluxCalibratorExtension *s_instance = NULL );
    };

    //! Construct from a FluxCalibrator instance
    FluxCalibratorExtension (const FluxCalibrator*);

    //! Return a short name
    std::string get_short_name () const { return "fcal"; }

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);

    //! Get the number of frequency channels
    unsigned int get_nchan( void ) const;

    //! Set the number of receptors
    void set_nreceptor (unsigned nreceptor);

    //! Get the number of receptors
    unsigned get_nreceptor () const;

    //! Set the system equivalent flux density of the specified channel
    void set_S_sys (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the system equivalent flux density of the specified channel
    Estimate<double> get_S_sys (unsigned chan, unsigned receptor) const;

    //! Set the calibrator flux density of the specified channel
    void set_S_cal (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the calibrator flux density of the specified channel
    Estimate<double> get_S_cal (unsigned chan, unsigned receptor) const;

  protected:

    //! System equivalent flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_sys;

    //! Calibrator flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_cal;

  };


}

#endif
