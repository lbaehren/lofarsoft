/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                  *
 *   Kalpana Singh (<mail>)                                                     *
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
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <scimath/Mathematics/FFTServer.h>

/*DAL header files*/

#include <dal/dalCommon.h>
#include <dal/dalDataset.h>
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>
#include <dal/TBB_Timeseries.h>

/*CR-Tools header files*/

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Analysis/SubbandID.h>
#include <Calibration/RFIMitigation.h>
#include <Coordinates/SkymapQuantity.h>
#include <Coordinates/TimeFreq.h>
#include <Imaging/GeomWeight.h>
#include <Imaging/Beamformer.h>

#include <Data/LOFAR_TBB.h>
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

#endif

using std::cout ;
using std::endl ;

using casa::DComplex ;
using casa::Matrix ;
using casa::Vector ;

using CR::CoordinateType ;
using CR::GeomDelay ;
//using CR::GeaomPhase ;
using CR::GeomWeight ;
using CR::Beamformer ;
using CR::SkymapQuantity ;
using CR::ppfinversion  ;
using CR::ppfimplement ;
using CR::SubbandID ;
using CR::TimeFreq ;
using CR::RFIMitigation ; 

using DAL::TBB_Timeseries ;
using DAL::TBB_StationGroup ;


//enum Type_data : int
//       { FFT =0, PPF=1 } ;

namespace CR{ // Namespace  -- begin
  
  /*!
    \class NuMoonTrigger
    
    \ingroup Analysis
    
    \brief Brief description for class NuMoonTrigger
    
    \author Kalpana Singh

    \date 2009/04/21

    \test tNuMoonTrigger.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class NuMoonTrigger {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    NuMoonTrigger ();
    
    /*!
    \brief Argumented constructor
    
    */
    NuMoonTrigger( std::string const &filename,
    		   uint const& n_samples,
		   double const& simTEC,
		   double const& sampling_rate,
		   uint const& nyquist_zone,
		   uint const& time_int_bins,
		   double const& TEC,
		   const Vector<double>& ppf_coeff,
		   const Vector<double>& ppf_invcoeff,
		   const Vector<double> freq_range,
		   double const& peak_power,
		   const Vector<uint>& RCU_id ) ;
    
    /*!
      \brief Copy constructor
      
      \param other -- Another NuMoonTrigger object from which to create this new
             one.
    */
    NuMoonTrigger (NuMoonTrigger const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~NuMoonTrigger ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another NuMoonTrigger object from which to make a copy.
    */
    NuMoonTrigger& operator= (NuMoonTrigger const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, NuMoonTrigger.
    */
    inline std::string className () const {
      return "NuMoonTrigger";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    #ifdef HAVE_ROOT
  
  Matrix<double> reading_data( std::string const &filename,
    			       uint const& n_frames ) ;
		     
  Matrix<DComplex> fft_data( Vector<double>& data,
    		 	     uint const& n_frames,
			     uint const& nyquist_zone ) ;
  
  Matrix<DComplex> RFI_removal( Matrix<DComplex> fft_samples ) ;
		     
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
    
  Matrix<double> cal_AvPower( Matrix<double> const& data_array,
  	       	 	     uint const& n_frames ) ;

  Vector<double> Sim_Signal( Vector<double> const& data,
    		 	    double const& simTEC,
			    const Vector<double>& freq_Vector,
			    double const& peak_height,
			    double const& pulse_loc,
			    double const& rnd_phase,
			    Vector<DComplex> geom_Weights_factor  ) ;
    
   Matrix<DComplex> weights_applied( Matrix<DComplex> const& data,
  			             Matrix<DComplex> const& geom_weights,
				     uint const& rcu_id ) ;

   Matrix<DComplex> Geom_weights( const Vector<uint>& RCU_id,
			          const Vector<double>& freq_vector ) ;     
				  
   Matrix<double> Cleaned_data(  std::string const &filename,
    				 uint const& n_samples,
				 double const& simTEC,
				 uint const& nyquist_zone,
				 double const& peak_power,
				 const Vector<uint>& RCU_id,
				 double const& sampling_rate,
				 double const& TEC,
				 const Vector<double> freq_range ) ;
				 
   Matrix<double> Added_SignalData(  std::string const &filename,
    				     uint const& n_samples,
				     double const& simTEC,
				     uint const& nyquist_zone,
				     double const& peak_power,
				     const Vector<uint>& RCU_id,
				     double const& sampling_rate,
				     double const& TEC,
				     const Vector<double> freq_range ) ;    
				     
  Vector<double> PPF_processed( std::string const &filename,
    				uint const& n_samples,
				double const& simTEC,
				double const& sampling_rate,
				uint const& nyquist_zone,
				uint const& time_int_bins,
  				double const& TEC,
				const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		  		const Vector<double> freq_range,
				double const& peak_power,
				const Vector<uint>& RCU_id ) ;
				
  Vector<double> FFT_processed( std::string const &filename,
    				uint const& n_samples,
				double const& simTEC,
				double const& sampling_rate,
				uint const& nyquist_zone,
				uint const& time_int_bins,
  				double const& TEC,
				const Vector<double>& ppf_coeff,
		   		const Vector<double>& ppf_invcoeff,
		  		const Vector<double> freq_range,
				double const& peak_power,
				const Vector<uint>& RCU_id ) ;
				
  Vector<double> without_Signal(  std::string const &filename,
    				  uint const& n_samples,
				  double const& simTEC,
				  double const& sampling_rate,
				  uint const& nyquist_zone,
				  uint const& time_int_bins,
  				  double const& TEC,
				  const Vector<double>& ppf_coeff,
		   		  const Vector<double>& ppf_invcoeff,
		  		  const Vector<double> freq_range,
				  double const& peak_power,
				  const Vector<uint>& RCU_id ) ;       
  
  
     void root_ntuples( std::string const &filename,
    			 uint const& n_samples,
			 double const& simTEC,
			 double const& sampling_rate,
			 uint const& nyquist_zone,
			 uint const& time_int_bins,
  			 double const& TEC,
			 const Vector<double>& ppf_coeff,
		   	 const Vector<double>& ppf_invcoeff,
		  	 const Vector<double> freq_range,
			 double const& peak_power,
			 const Vector<uint>& RCU_id ) ;
   
  #endif    
 
 
    
    
  private:
    
    //! Unconditional copying
    void copy (NuMoonTrigger const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class NuMoonTrigger -- end
  
} // Namespace  -- end

#endif /* NUMOONTRIGGER_H */
  
