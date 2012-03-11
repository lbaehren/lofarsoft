//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibratorData.h,v $
   $Revision: 1.4 $
   $Date: 2007/05/01 06:44:59 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibratorData_H
#define __Pulsar_FluxCalibratorData_H

#include "Pulsar/FluxCalibrator.h"

namespace Pulsar {

  //! Flux calibration data for each receptor
  class FluxCalibrator::Data {

  public:

    //! Default constructor
    Data ();
    
    //! Set the number of receptors
    void set_nreceptor (unsigned);
    
    //! Get the number of receptors
    unsigned get_nreceptor () const;
    
    //! Add to the mean hi/lo ratio on source for the specified receptor
    void add_ratio_on (unsigned receptor, Estimate<double>& ratio_on);
    
    //! Add to the mean hi/lo ratio off source for the specified receptor
    void add_ratio_off (unsigned receptor, Estimate<double>& ratio_on);
    
    //! Set the flux density of the standard candle
    void set_S_std (double S_std);
    
    //! Get the flux density of the standard candle
    double get_S_std () const;
    
    //! Set the data validity flag
    void set_valid (bool f = true) { valid = f; }
    
    //! Get the data validity flag
    bool get_valid () const { return valid && calculated; }
    
    //! Return the total system equivalent flux density
    Estimate<double> get_S_sys () const;
    
    //! Return the total calibrator flux density
    Estimate<double> get_S_cal () const;
    
    //! Return the system equivalent flux density in the specified receptor
    Estimate<double> get_S_sys (unsigned receptor) const;
    
    //! Return the calibrator flux density in the specified receptor
    Estimate<double> get_S_cal (unsigned receptor) const;
    
    //! Construct from known flux densities
    Data ( const std::vector< Estimate<double> >& S_sys,
	   const std::vector< Estimate<double> >& S_cal );
    
    //! Return the flux densities
    void get ( std::vector< Estimate<double> >& S_sys,
	       std::vector< Estimate<double> >& S_cal ) const;
    
  protected:
    
    //! Flag set when data are valid
    bool valid;
    
    //! Flag set when S_cal and S_sys have been computed
    bool calculated;
    
    //! Flux density of the standard candle
    double S_std;
    
    //! Calibrator flux density in each receptor
    std::vector< Estimate<double> > S_cal;
    
    //! System equivalent flux density in each receptor
    std::vector< Estimate<double> > S_sys;
    
    //! Ratio of cal hi/lo on source in each receptor
    std::vector< MeanEstimate<double> > mean_ratio_on;
    
    //! Ratio of cal hi/lo off source in each receptor
    std::vector< MeanEstimate<double> > mean_ratio_off;
    
    //! Calculate S_cal and S_sys based on ratio_on and ratio_off
    void calculate () const;

    //! The actual calculator
    void compute ();
    
  };
  
}

#endif
