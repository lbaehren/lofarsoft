/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#ifndef CRINVFFT_H
#define CRINVFFT_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Analysis/SecondStagePipeline.h>
#include <Calibration/CalTableInterpolater.h>
#include <Imaging/GeometricalPhase.h>
// --> needs to be replaced by GeometricalDelay
#ifdef HAVE_GLISH
#include <ApplicationSupport/Beamformer.h>
#endif

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CRinvFFT
    
    \ingroup Analysis
    
    \brief Calculate (shifted) inverse FFT and beams for the CR pipeline
    
    \author Andreas Horneffer

    \date 2007/04/20

    \test tCRinvFFT.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>

    Takes the data from a second stage pipeline and performs beamforming and
    backward Fourier transform back to time domain.
    If the correct calibration data is available in the CalTables then the output
    is in V/m/MHz. Rescaling to uV/m/MHz may be desirable.
    
    <h3>Example(s)</h3>
    See tCRinvFFT.cc

  */  
  class CRinvFFT : public SecondStagePipeline {

  protected:

    //! Record to store direction parameters
    Record DirParams_p;
    
    //! Unmodified antenna positions, i.e. relative to coordinate system and not shower core.
    Matrix<Double> AntPositions_p;
   
    //! Are the antenna positions valid?
    Bool AntPosValid_p;

    //! For which event (observation time) was the antenna position data cached
    uInt posCachedDate_p;   

#ifdef HAVE_GLISH
    //! local instance of a beamformer object
    LOPES::Beamformer bf_p;
#endif

    //! Interpolater for the antenna gain
    CalTableInterpolater<casa::Double> *InterAntGain_p;

    //! Has the antenna gain interpolater been initialized?
    Bool AntGainInterpInit_p;

    //! Value of the extra delay
    Double ExtraDelay_p;
 
    // ---------------------------------------------------------- Private Methods

    /*!
      \brief Get the "shifted" and calibrated frequency domain data
      
      \param dr - Pointer to the (initialized) DataReader

      \return The processed frequency domain data

      "shifted" means the the phase gradients for beamforming into the stored 
      direction are already multiplied to the data.
      The calibration is such that after the backwards FFT the output is in V/m/MHz.
    */
    Matrix<DComplex> GetShiftedFFT(DataReader *dr);

    /*!
      \brief Initialize the CalTableInterpolater for the antenna gain
      
      \param dr - Pointer to the (initialized) DataReader

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool initGainInterp(DataReader *dr);

    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CRinvFFT ();
    
    /*!
      \brief Destructor
    */
    ~CRinvFFT ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CRinvFFT.
    */
    std::string className () const {
      return "CRinvFFT";
    }

    /*!
      \brief Provide a summary of the internal status

      (Not implemented yet.)
    */
    void summary();    

    /*!
      \brief Set the phase center (shower core)

      \param XC - east-west coordinate (east is positive, in m)
      \param YC - north-south coordinate (north is positive, in m)
      \param rotate - rotate the axis by 15 degrees (For KASCADE coordinates)
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setPhaseCenter(Double XC, Double YC, Bool rotate=False);

    /*!
      \brief Set the direction parameters

      \param AZ - Azimuth (in degrees)
      \param EL - Elevation (in degrees)
      \param Curvature - Curvature parameter (in m, default value: 0 == infinite)
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool setDirection(Double AZ, Double EL, Double Curvature=0);

    /*!
      \brief Set the an additional dealy

      \param extraDelay - additional delay in seconds
      
      \return 
    */
    inline void setExtraDelay(Double extraDelay) { ExtraDelay_p = extraDelay;};

    /*!
      \brief Get the shifted antenna positions (i.e. coordinate origin is the phase center)
      
      \return the antenna positions
    */
    Matrix<Double> GetAntPositions();

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Get the time series for all antennas
      
      \param dr - Pointer to the (initialized) DataReader
      \param antennaSelection - (Optinal) Vector of bool to select only part of the antennas.

      \return Matrix with the processed and shifted time series data
    */
    Matrix<Double> GetTimeSeries(DataReader *dr, Vector<Bool> antennaSelection = Vector<Bool>());
    
    /*!
      \brief Get the CCBeam
      
      \param dr - Pointer to the (initialized) DataReader
      \param antennaSelection - (Optinal) Vector of bool to select only part of the antennas.

      \return The formed cc-beam
    */
    Vector<Double> GetCCBeam(DataReader *dr, Vector<Bool> antennaSelection = Vector<Bool>());
    
    /*!
      \brief Get the XBeam
      
      \param dr - Pointer to the (initialized) DataReader
      \param antennaSelection - (Optinal) Vector of bool to select only part of the antennas.

      \return The formed X-beam.
    */
    Vector<Double> GetXBeam(DataReader *dr, Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Get the PBeam 
      
      \param dr - Pointer to the (initialized) DataReader
      \param antennaSelection - (Optinal) Vector of bool to select only part of the antennas.

      \return The formed Power-beam.
    */
    Vector<Double> GetPBeam(DataReader *dr, Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Get the time series, CCBeam and XBeam in one go
      
      \param dr - Pointer to the (initialized) DataReader
      \retval TimeSeries - The time series for all antennas 
      \retval ccBeam - The CCBeam
      \retval xBeam - The XBeam
      \param antennaSelection - (Optinal) Vector of bool to select only part of the antennas.

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool GetTCXP(DataReader *dr, Matrix<Double> & TimeSeries, 
		Vector<Double> & ccBeamData, Vector<Double> & xBeamData, 
		Vector<Double> & pBeamData, Vector<Bool> antennaSelection = Vector<Bool>());
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize parameters
    */
    void init(void);
    
  };
  
} // Namespace CR -- end

#endif /* CRINVFFT_H */
  
