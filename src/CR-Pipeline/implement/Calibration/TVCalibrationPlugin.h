/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: TVCalibrationPlugin.h,v 1.3 2007/04/17 12:07:21 horneff Exp $*/

#ifndef TVCALIBRATIONPLUGIN_H
#define TVCALIBRATIONPLUGIN_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Calibration/PluginBase.h>

#include <casa/namespace.h>

#define PI 3.141592653589793238462643

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TVCalibrationPlugin
    
    \ingroup Calibration
    
    \brief Do the "LOPES-style" phase calibration on a TV transmitter. 
    
    \author Andreas Horneffer

    \date 2007/04/16

    \test tTVCalibrationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>PluginBase<DComplex> 
    </ul>
    
    <h3>Synopsis</h3>

    This class implements the phase calibration on narrow band (RFI) transmitters
    as it was originally implemented into the Glish-based lopestools.

    <h3>Parameter</h3>
    
    The following is a list of entries in the parameters record that are used by this class.
    those without a default value have to be set before calcWeigths() is (indirectly) called!
    
    \param frequencyRanges   --  List of the frequency ranges in which to look for the transmitter 
                                 peaks. Two dimensional matrix of double with the minimum and 
                                 maximum frequencies in each row. The number of rows defines the 
                                 number of peaks, and the number of columns has to be 2.

    \param referencePhases   --  Reference phase values for comparison to the measured phase differences.
                                 The phase differences are the phase (at a peaks frequency) of the antenna
				 minus the phase of the reference antenna.
   
    \param frequencyValues   --  The frequency axis of the input data. Vector of double that gives the 
                                 exact frequency values correspoding tothe frequency indices. 
	
    \param sampleJumps       --  Vector of integers with multi-sample jumps to try out.
    
    \param referenceAntenna  --  Number of the reference antenna. Default value is 0, i.e. the first 
				 antenna of the selected group.  
    
    \param sampleRate        --  Rate with which sampling of the data is performed, default value is 80 MHz.
    
    \param badnessWeight     --  Parameter to calculate and define criteria for the "badness" of an antenna.
                                 Default value is 0.5.

    \param badnessThreshold  --  Maximum acceptable "badness" before antenna is flagged, in sample times. 
                                 Default value is 0.15 sample times.
 
    The following entries in the parameters record are set by this class:

    \return AntennaMask  --  Vector of Bool that lists the antennas for which the algorithm failed.
                             True for good antennas, False for antennas that failed.

     \Note
     <b> Currently (April 2007) the reference values, that are stored in the CalTabels, 
     are defined in such a way that the values from "fft()" (and not "calfft()") of
     the DataReader have to be used. \b>
        
  */  
  class TVCalibrationPlugin : public PluginBase<DComplex> {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TVCalibrationPlugin ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TVCalibrationPlugin ();
    
    // ---------------------------------------------------------------- Operators
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TVCalibrationPlugin.
    */
    std::string className () const {
      return "TVCalibrationPlugin";
    }

    // ------------------------------------------------------------------ Methods
    

    /*!
      \brief Calculate the (complex) weights.

      This function essentially does all tha magic. It reads out all the parameters 
      from the parameters record, so they have to be set correctly before. Then it
      calculates the corrections delays and from that the phase gradients that are 
      then stored in the weights. It also sets the entry in the parameters record
      "AntennaMask" which marks all the channels for which the routine failed.

      \param data -- Data for which the calibration is to be done. 
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual Bool calcWeights(const Matrix<DComplex> &data);    


    
  protected:
    
    /*!
      \brief convert phases to delays
      \param phases         -- The input phases (in degrees!)
      \param frequencies    -- The corresponding frequencies (in Hz) 
      \param samplerate =1. -- The samplerate (in Hz)
      \return delays -- The resulting delay in sample times (== seconds if samplerate = 1.)
    */
    Matrix<Double> phase2delay(Matrix<Double> phases, Vector<Double> frequencies, 
			       Double samplerate=1.);
    
    /*!
      \brief convert delays to phases
      \param delays         -- The input delays (in sample times (== seconds if samplerate = 1.))
      \param frequencies    -- The corresponding frequencies (in Hz)
      \param samplerate =1. -- The samplerate (in Hz)
      \return phases -- The resulting phases in degrees
    */
    Matrix<Double> delay2phase(Matrix<Double> delays, Vector<Double> frequencies, 
			       Double samplerate=1.);
    Vector<Double> delay2phase(Double delay, Vector<Double> frequencies, 
			       Double samplerate=1.);
    
    /*!
      \brief "reduce" phases, i.e. shift them into the range -180. to +180. degrees
      \param phases -- The phases in degrees; will be modified in place!
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */        
    Bool reducePhases(Array<Double> & phases);
    
    /*!
      \brief Find the positions of the peaks
      \param spectrum        -- the frequency domain data
      \param frequencyValues -- The corresponding frequencies (in Hz)
      \param frequencyRanges -- low and high frequency ranges.
      \return positions -- indices of the peak positions
    */
    Vector<uInt> getPeakPos(Vector<DComplex> spectrum, Vector<Double> frequencyValues,
			    Matrix<Double> frequencyRanges);  
    
  }; //Class TVCalibrationPlugin  -- end
} // Namespace CR -- end

#endif /* TVCALIBRATIONPLUGIN_H */
  
