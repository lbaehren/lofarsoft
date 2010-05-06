/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Kalpana Singh (<mail>)                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NUMOONTRIGGER_H
#define NUMOONTRIGGER_H

#include <crtools.h>

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>

/*AIPS++/CASA header files */

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicMath/Random.h>
#include <casa/Quanta.h>
#include <casa/namespace.h>
#include <casa/Exceptions/Error.h>
#include <casa/HDF5/HDF5File.h>
#include <casa/HDF5/HDF5Record.h>
#include <scimath/Mathematics/FFTServer.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

/*DAL header files*/

#include <dal/dalCommon.h>
#include <dal/dalDataset.h>
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>
#include <dal/TBB_Timeseries.h>
#include <dal/TBB_StationGroup.h>
#include <dal/TBB_DipoleDataset.h>

/*CR-Tools header files*/

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Analysis/SubbandID.h>
#include <Calibration/RFIMitigation.h>
#include <Coordinates/SkymapQuantity.h>
#include <Coordinates/TimeFreq.h>
#include <Imaging/GeomWeight.h>
#include <Imaging/Beamformer.h>
#include <IO/LOFAR_TBB.h>
#include <Utilities/StringTools.h>

/* ROOT header files */
#ifdef HAVE_ROOT
#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TVirtualFFT.h"
#endif

using std::cout ;
using std::endl ;

using casa::DComplex ;
using casa::Matrix ;
using casa::Vector ;

using DAL::TBB_Timeseries ;
using DAL::TBB_StationGroup ;

using CR::CoordinateType ;
using CR::GeomDelay ;
using CR::GeomWeight ;
using CR::Beamformer ;
using CR::SkymapQuantity ;
using CR::ppfinversion  ;
using CR::ppfimplement ;
using CR::SubbandID ;
using CR::RFIMitigation ; 

namespace CR{ // Namespace  -- begin
  
  /*!
    \class NuMoonTrigger
    
    \ingroup Analysis
    
    \brief Brief description for class NuMoonTrigger
    
    \author Kalpana Singh

    \date 2009/04/21

    \test tNuMoonTrigger.cc
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class NuMoonTrigger {
    
    uint itsNyquistZone;
    casa::Vector<double> itsFrequencyRange;

  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    NuMoonTrigger ();
    
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, NuMoonTrigger.
    */
    inline std::string className () const {
      return "NuMoonTrigger";
    }

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // === Methods ==============================================================

#ifdef HAVE_ROOT

  Matrix<DComplex> fft_data( Vector<double>& data,
    		 	     uint const& n_frames,
			     uint const& nyquist_zone ) ;
  
  casa::Matrix<DComplex> zero_channel_flagging (Vector<double>&  data,
     		                         	uint const& n_frames,
					        uint const& nyquist_zone ) ;
  
  Matrix<DComplex> RFI_removal( Matrix<DComplex> fft_samples ) ;
  
  Matrix<DComplex> Average_effect_RFI( Matrix<DComplex> fft_samples,
				       Matrix<DComplex> RFI_mitigated );
  
  Matrix<DComplex> de_dispersion( Matrix<DComplex> fft_samples,
		      		  double const& TEC,
				  const Vector<double> freq_Vector ) ;
  
  Vector<double> ifft_data( Matrix<DComplex> const& data,
    			    uint const& n_frames,
			    uint const& nyquist_zone ) ;
  
  Matrix<DComplex> ppf_implement( Vector<double>& samples,
                                  uint const& nyquist_zone,
				  const Vector<double>& ppf_coeff ) ;
  
  Vector<double> ppf_inversion( const Matrix<DComplex>& FTData,
                               	const Vector<double>& ppf_invcoeff,
				const Vector<uint> subBand_ID )  ;

  Vector<uint> PPFBand_vector( double const& sampling_rate,
   	   	       		uint const& nyquist_zone,
		       		const Vector<double> freq_range ) ;

  Vector<double> freq_vector( double const& sampling_rate,
		   	      uint const& nyquist_zone ) ;

  Matrix<DComplex> ppfdata_cutshort( Matrix<DComplex>& samples,
   				    Vector<uint> subband_ID ) ;
    
  Vector<double> cal_AvPower( Vector<double> const& beamed_Array ) ;

  Vector<double> Sim_Signal( Vector<double> const& data,
    		 	    double const& simTEC,
			    const Vector<double>& freq_Vector,
			    double const& peak_height,
			    double const& pulse_loc,
			    double const& rnd_phase,
			    Vector<double> geom_phase_factor,
			    uint const& nyquist_zone ) ;
    
   Matrix<DComplex> weights_applied( Matrix<DComplex> const& data,
  			             Matrix<double> const& phase_Delay,
				     const Vector<double>& gain_scale_factor,
			     	     uint const& rcu_id ) ;


 Vector<double> geom_Delay( const double& source_latitude,
			    const double& source_longitude,
			    const Vector<double>& position_x,
		   	    const Vector<double>& position_y,
		   	    const Vector<double>& position_z ) ;  
 
 Matrix<double> phase_delay( const Vector<double>& freq_Vector,
		             const double& source_latitude,
		             const double& source_longitude,
			     const Vector<double>& position_x,
		   	     const Vector<double>& position_y,
		   	     const Vector<double>& position_z ) ;
			     
 Vector<int> integer_Delay( const double& source_latitude,
			    const double& source_longitude,
			    const double& sampling_frequency,
			    const Vector<double>& position_x,
		   	    const Vector<double>& position_y,
		   	    const Vector<double>& position_z ) ;    
  
  Vector<double> fraction_Delay( const double& source_latitude,
			    	 const double& source_longitude,
				 const double& sampling_frequency,
			    	 const Vector<double>& position_x,
		   	    	 const Vector<double>& position_y,
		   	    	 const Vector<double>& position_z );
				 
   Matrix<double> Cleaned_data(  Matrix<double> const& data,
    				 uint const& n_samples,
				 double const& simTEC,
				 uint const& nyquist_zone,
				 double const& sampling_rate,
				 double const& TEC,
				 const Vector<double> freq_range ) ;
   
   Vector<double> BeamFormed_data(  Matrix<double> const& data,
    				   uint const& n_samples,
			           double const& simTEC,
				   uint const& nyquist_zone,
				   double const& sampling_rate,
				   double const& TEC,
				   const Vector<double> freq_range,
		  		   const double& pointing_latitude,
		  		   const double& pointing_longitude,
				   const Vector<double>& gain_scale_factor,
		  		   const Vector<double>& position_x,
		  		   const Vector<double>& position_y,
		  		   const Vector<double>& position_z ) ; 
   
   Matrix<double> Added_SignalData(  Matrix<double> const& data, 
    				     uint const& n_samples,
				     double const& simTEC,
				     uint const& nyquist_zone,
				     double const& peak_height,
				     double const& sampling_rate,
				     double const& TEC,
				     const double& source_latitude,
				     const double& source_longitude,
		  		     const double& pointing_latitude,
		  		     const double& pointing_longitude,
				     const Vector<double>& gain_scale_factor,
			    	     const Vector<double>& position_x,
		   	    	     const Vector<double>& position_y,
		   	    	     const Vector<double>& position_z,
				     const Vector<double> freq_range ) ;    
		     
  Vector<double> PPF_processed( Matrix<double> const& data,
    				uint const& n_samples,
				double const& simTEC,
				double const& sampling_rate,
				uint const& nyquist_zone,
				uint const& time_int_bins,
  				double const& TEC,
				const double& source_latitude,
				const double& source_longitude,
			    	const double& pointing_latitude,
		   		const double& pointing_longitude,
		   		const Vector<double>& gain_scale_factor,
			    	const Vector<double>& position_x,
		   	    	const Vector<double>& position_y,
		   	    	const Vector<double>& position_z,
				const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		  		const Vector<double> freq_range,
				double const& peak_height ) ;
				
  Vector<double> FFT_processed( Matrix<double> const& data,
    				uint const& n_samples,
				double const& simTEC,
				double const& sampling_rate,
				uint const& nyquist_zone,
				uint const& time_int_bins,
  				double const& TEC,
				const double& source_latitude,
				const double& source_longitude,
			    	const double& pointing_latitude,
		   		const double& pointing_longitude,
		   		const Vector<double>& gain_scale_factor,
			    	const Vector<double>& position_x,
		   	    	const Vector<double>& position_y,
		   	    	const Vector<double>& position_z,
				const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		  		const Vector<double> freq_range,
				double const& peak_height ) ;
				
  Vector<double> without_Signal(  Matrix<double> const& data,
    				  uint const& n_samples,
				  double const& simTEC,
				  double const& sampling_rate,
				  uint const& nyquist_zone,
				  uint const& time_int_bins,
  				  double const& TEC,
				  const double& source_latitude,
				  const double& source_longitude,
				  const double& pointing_latitude,
		   		  const double& pointing_longitude,
		   		  const Vector<double>& gain_scale_factor,
			    	  const Vector<double>& position_x,
		   	    	  const Vector<double>& position_y,
		   	    	  const Vector<double>& position_z,
				  const Vector<double>& ppf_coeff,
		   		  const Vector<double>& ppf_invcoeff,
		  		  const Vector<double> freq_range,
				  double const& peak_height ) ;       
  
  
     void root_ntuples( Matrix<double> const& data,
    			 uint const& n_samples,
			 double const& simTEC,
			 double const& sampling_rate,
			 uint const& nyquist_zone,
			 uint const& time_int_bins,
  			 double const& TEC,
			 const double& source_latitude,
			 const double& source_longitude,
			 const double& pointing_latitude,
		   	 const double& pointing_longitude,
		   	 const Vector<double>& gain_scale_factor,
			 const Vector<double>& position_x,
		   	 const Vector<double>& position_y,
		   	 const Vector<double>& position_z,
			 const Vector<double>& ppf_coeff,
		   	 const Vector<double>& ppf_invcoeff,
		  	 const Vector<double> freq_range,
			 double const& peak_height ) ;
   
  #endif    
   
  }; // Class NuMoonTrigger -- end
 
} // Namespace  -- end

#endif /* NUMOONTRIGGER_H */

