//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/DoPCalibrator.h,v $
   $Revision: 1.8 $
   $Date: 2009/03/01 18:04:41 $
   $Author: straten $ */

#ifndef __Pulsar_DoPCalibrator_H
#define __Pulsar_DoPCalibrator_H

#include "Pulsar/SingleAxisCalibrator.h"

namespace Pulsar {

  //! Degree of Polarization Calibrator (P236)
  /*! This calibrator should probably never be used.  It was developed to
    deal with Wideband Correlator data observed during a period of heavy
    testing and development (P236).  Many of the archives recorded at this
    time have non-physical Stokes parameters (ie. p > I).  It was observed
    in the off-pulse baseline statistics that the rms in the real and
    imaginary components of X*Y are equal.  Therefore, this class scales
    X*Y in each channel so that the calibrated reference source has
    the expected degree of polarization.
  */
  class DoPCalibrator : public SingleAxisCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    DoPCalibrator (const Archive* archive);

    //! Destructor
    ~DoPCalibrator ();

    //! Calibrate the polarization of the given archive
    void calibrate (Archive* archive);

    //! Communicates DoPCalibrator parameters
    class Info : public SingleAxisCalibrator::Info {

    public:

      //! Constructor
      Info (const DoPCalibrator* calibrator);

      //! Return the title
      std::string get_title () const;

      //! Return the number of parameter classes
      unsigned get_nclass () const;

      //! Return the name of the specified class
      std::string get_name (unsigned iclass) const;
      
      //! Return the number of parameters in the specified class
      unsigned get_nparam (unsigned iclass) const;

      Estimate<float> get_param (unsigned ichan, 
				 unsigned iclass,
				 unsigned iparam) const;

    protected:

      //! The DoPCalibrator to which this instance refers
      Reference::To<const DoPCalibrator> dopcal;

    };

    //! Return the DoPCalibrator information
    Info* get_Info () const;

  protected:

    friend class Info;

    //! The scale factor as a function of frequency
    std::vector< Estimate<double> > dop_scale;

    //! The degree of polarization of the reference source
    double dop_reference;

    //! Compute the scale factor for the specified channel
    void extra (unsigned ichan,
		const std::vector< Estimate<double> >& source,
		const std::vector< Estimate<double> >& sky);

  };

}


#endif
