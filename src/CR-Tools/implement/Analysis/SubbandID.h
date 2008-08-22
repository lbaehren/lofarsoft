/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

#ifndef SUBBANDID_H
#define SUBBANDID_H

#include <crtools.h>

// Standard library header files
#include <string.h>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <casa/namespace.h>


namespace CR { // Namespace CR -- begin
  
  /*!
    \class SubbandID
    
    \ingroup Analysis
    
    \brief Brief description for class SubbandID
    
    \author Kalpana Singh

    \date 2007/06/05

    \test tSubbandID.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    This class is developed to calculate the subband IDs that can be used for
    polyphase filter bank inversion or for ionospheric calibration. In case if
    subband IDs are provided then in that case to calculate the starting
    frequency of the subband with bandwidth depending on the sampling frequency,
    in other specific case if vector of various subband (initial)
    frequencies is provided then in that case also subband IDs can be generated
    
    <h3>Example(s)</h3>
    
  */  
  class SubbandID {
    
    //! Clock rate at which data is sampled
    double sampleFrequency_p;
    //! Subband ID of first subband used for observation
    uint bandID_p;
    //! Number of subbands used for observation
    uint nofSubbands_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SubbandID ();
    
    /*!
      \brief Argumented Constructor
      
      Sets a vector of initial subband frequencies of all the subbands if ID of
      first subband is given and number of subbands is given
      
      \param  sampleFrequency -- clock rate at which data is sampled
      \param  bandID          -- subband ID of first subband used for observation
      \param  nofsubbands     -- number of subbands used for observation, basically
                                 for beamforming, number of datafiles which is
				 given for processing
    */
    SubbandID (double const &sampleFrequency,
	       uint const &bandID,
	       uint const &nofSubbands);
    
    /*!
      \brief Argumented Constructor
      
      Sets the  vector of subband IDs if the initial subband frequency of first
      subband, which is used in observation is given, and number of subbands
      used in observation is given.
      
      \param sampleFrequency  -- clock rate at which data is sampled
      \param subband_freq_1 -- initial frequency of first subband which is used
                               for observation
      \param nofSubbands    -- number of subbands used for observation, basically
                               for beamforming, number of datafiles which is
			       given for processing
    */
    SubbandID (double const &sampleFrequency,
	       double const &subband_freq_1,
	       uint const &nofSubbands);

    /*!
      \brief Arguemented Constructor
      
      Sets the vector of subband IDs for given vector of subband intial
      frequencies which are used for beamforming at station level or for tied
      array beam
      
      \param sampleFrequency     -- clock rate at which data is sampled
      \param subband_frequencies -- vector of subband frequencies.
    */
    SubbandID (double const &sampleFrequency,
	       const Vector<double>& subband_frequencies ) ;
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SubbandID object from which to create this new
                      one.
    */
    SubbandID (SubbandID const &other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~SubbandID ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SubbandID object from which to make a copy.
    */
    SubbandID& operator= (SubbandID const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the clock rate at which data is sampled

      \return sampleFrequency -- Clock rate at which data is sampled, [Hz]
    */
    double sampleFrequency () const {
      return sampleFrequency_p;
    }

    /*!
      \brief Set the clock rate at which data is sampled

      \param sampleFrequency -- Clock rate at which data is sampled, [Hz]
    */
    inline void setSampleFrequency (double const &sampleFrequency) {
      sampleFrequency_p = sampleFrequency;
    }
    
    /*!
      \brief Get the Subband ID of first subband used for observation

      \return bandID -- Subband ID of first subband used for observation
    */
    inline uint bandID () const {
      return bandID_p;
    }

    /*!
      \brief Set the subband ID of first subband used for observation

      \param bandID -- Subband ID of first subband used for observation
    */
    inline void bandID (uint const &bandID) {
      bandID_p = bandID;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SubbandID.
    */
    std::string className () const {
      return "SubbandID";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written
    */
    void summary (std::ostream &os);
    
    /*!
      \brief Calculate initial frequencies of subbands used for beamforming.
      
      \param sampleFrequency -- Clock rate at which data is sampled
      \param subband_ID      -- subband ID of first subband used for observation
      \param nofSubbands      -- number of subbands used for observation,
                                basically for beamforming, number of datafiles
				which is given for processing
      
      \return frequencies -- Vector of vector of initial frequencies of subbands
                             which are used for beamforming.      
    */
    Vector<double> calcFrequency ( const double& sampleFrequency,
                                   const uint& subband_ID,
                                   const uint& nofSubbands ) ;
 
     /*!
      \brief Calculate initial frequencies of subbands used for beamforming.
      
      \param sampleFrequency -- Clock rate at which data is sampled
      \param subband_ID      -- Vector of subband IDs used for observation
            
      \return frequencies -- Vector of vector of initial frequencies of subbands
                             which are used for beamforming.      
    */
    Vector<double> calcFreqVector ( const double& sampleFrequency,
                                    const Vector<uint>& subband_ID ) ;
				   				   
    /*!
      \brief Calculate subbands IDs which are used for beamforming.
      
      \param sampleFrequency -- clock rate at which data is sampled
      \param subband_freq    -- subband frequency of first subband of the
                                subband vector which are used for observation
      \param nofSubbands      -- number of subbands used for observation,
                                basically for beamforming, number of datafiles
				which is given for processing
      
      \return subbandIDs -- Vector of subbands IDs which are used for beamforming
    */
    Vector<uint> calcSubbandID ( const double& sampleFrequency,
				 const double& subband_freq,
				 const uint& nofSubbands  ) ;
    
    /*!
      \brief calculating subbands IDs which are used for beamforming.
      
      \param sampleFrequency 	 -- clock rate at which data is sampled
      \param subband_frequencies -- vector of intial frequencies of all subbands
                                    which are used for beamforming.
      
      \return subbands -- Vector of subbands IDs which are used for
                          beamforming.
    */
    Vector<uint> calcbandIDVector ( const double& sampleFrequency,
				    const Vector<double>& subband_frequencies ) ;
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy(SubbandID const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* SUBBANDID_H */
