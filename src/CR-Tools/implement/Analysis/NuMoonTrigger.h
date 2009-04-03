/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Kalpana Singh (<k.singh@rug.nl>)                                      *
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

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>
#include <string>

// AIPS++/CASA header files

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicMath/Random.h>
#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>

#include <casa/HDF5/HDF5File.h>
#include <casa/HDF5/HDF5Record.h>

#include <Imaging/GeomWeight.h>
#include <Imaging/Beamformer.h>
#include <Analysis/SubbandID.h>

#include <Analysis/ppfinversion.h>
#include <Analysis/ppfimplement.h>
#include <Coordinates/SkymapQuantity.h>
#include <Coordinates/TimeFreq.h>

#include <dal/dalCommon.h>
//#include <dal/DataReader.h>
#include <dal/dalDataset.h>
#include <dal/BeamFormed.h>
#include <dal/BeamGroup.h>
#include <dal/TBB_Timeseries.h>
#include <dal/TBB_DipoleDataset.h>


#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <Imaging/Beamformer.h>
//#include "create_data.h"
// root files to be included

#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TSpectrum.h"


using std::cout;
using std::endl;

using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using CR::CoordinateType ;
using CR::GeomDelay;
using CR::GeomPhase;
using CR::GeomWeight;
using CR::Beamformer;
using CR::SkymapQuantity;
using CR::ppfinversion ;
using CR::ppfimplement ;
using CR::SubbandID ;
using CR::TimeFreq ;

using DAL::TBB_Timeseries ;
using DAL::TBB_DipoleDataset ;

typedef enum { TIME_CC }Type ;

namespace CR { // Namespace  -- begin
  
  /*!
    \class NuMoonTrigger
    
    \ingroup Analysis
    
    \brief Brief description for class NuMoonTrigger
    
    \author Kalpana Singh

    \date 2008/08/20

    \test tNuMoonTrigger.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    <i>Currently this class is not only not doing something useful, but actually
    it is not doing anything at all, since there is nothing else but a few
    function prototypes and not implementation of anything.</i>
    
    <h3>Example(s)</h3>
    
  */  
  class NuMoonTrigger {

    uint start_p ;
    
    uint nofSamples_p ;

    //! Time of the observation
    uint observingTime_p;
    //! Sampling rate
    double samplingRate_p ;

    uint nyquistZone_p ;
    //
    double mean_p;
    //
    double variance_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    NuMoonTrigger ();
    
    /*!
      \brief Argumented constructor
      
      \param observing_time -- 
      \param sampling_rate  -- 
      \param mean           -- 
      \param variance       -- 
      \param cutoff_level   -- 
      \param dead_time      -- 
      \param P3             -- 
    */
    NuMoonTrigger ( std::string const & filename_t,
		    Type const &quantity,
		    std::string const & group,
		    double samplingRate_p, 
		   uint nyquistZone_p   ) ;
	
    //! Copy constructor
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
    
    inline uint observingTime () const {
      return observingTime_p;
    }

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, NuMoonTrigger.
    */
    std::string className () const {
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
    
    // ------------------------------------------------------------------ Methods
  
  casa::Matrix<Double> timeSeries_data( std::string const & filename_t,
				        std::string const & group,
				        uint start_p,
				        uint nofSamples_p  ) ;

  casa::Matrix<Double> antenna_position( std::string const & filename_t,
	  			         std::string const & group ) ;

   string  beamAngle_ra( std::string const & filename ) ;

   string  beamAngle_dec( std::string const & filename ) ;
  
   casa::Vector<uint>  obs_time( std::string const & filename ) ;
    
   casa::Vector<Double>  Direction_Conversion( double ra,
                 	 		     double dec,
					     double obs_epoch ) ;


   casa::Matrix<Double> Beam_forming( std::string const &filename,
				      Type const &quantity,
				      std::string const &group,
  				      double samplingRate_p, 
		                      uint nyquistZone_p  ) ;

   void root_ntuple( std::string const &filename,
		      Type const &quantity,
		      std::string const &group,
		      double samplingRate_p, 
	              uint nyquistZone_p ) ;

    

 private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (NuMoonTrigger const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  
};
  
} // Namespace  -- end

#endif /* NUMOONTRIGGER_H */
  