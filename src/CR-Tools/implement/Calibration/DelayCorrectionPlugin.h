/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#ifndef DELAYCORRECTIONPLUGIN_H
#define DELAYCORRECTIONPLUGIN_H

// Standard library header files
#include <iostream>
#include <string>

#include <Calibration/PluginBase.h>

#define PI 3.141592653589793238462643

namespace CR { // Namespace CR -- begin
  
  /*!
    \class DelayCorrectionPlugin
    
    \ingroup Calibration
    
    \brief Plugin that corrects for a given delay by pultiplying a phase gradient at the data
    
    \author Andreas Horneffer

    \date 2008/05/15

    \test tTVCalibrationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>PluginBase<DComplex> 
    </ul>
    
    <h3>Synopsis</h3>

    This class ist the parent class for more useful classes like TVCalibrationPlugin

    <h3>Parameter</h3>
    
    The following is a list of entries in the parameters record that are used by this class.
    those without a default value have to be set before calcWeigths() is called!

    \param delays            --  The delays to be corrected for (in seconds). The length of the vector
                                 has to be the same as number of antennas in the data.
    \param frequencyValues   --  The frequency axis of the input data. Vector of double that gives the 
                                 exact frequency values correspoding to the frequency indices. 
    \param sampleRate        --  Rate with which sampling of the data is performed, default value is 80 MHz.

     
    <h3>Note</h3>
    
    For testing please use the test programm of the derived class: tTVCalibrationPlugin

  */  
  class DelayCorrectionPlugin : public PluginBase<DComplex> {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    DelayCorrectionPlugin ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~DelayCorrectionPlugin ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, DelayCorrectionPlugin.
    */
    virtual std::string className () const {
      return "DelayCorrectionPlugin";
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

    /*!
      \brief Calculate the (complex) weights.

      This function read out the delay and frequency values from the parameters
      record and calculates the weigths.

      \param data           -- Data for which the calibration is to be done.

      \return ok            -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual Bool calcWeights(const Matrix<DComplex> &data);    



    // ---------------------------------------------------------- Service Methods
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
      \param frequencyRanges -- low and high frequency ranges. If low freq = high freq the
                                function will look for the frequency bin nearest to this
                                frequency (instead for a peak).
      \return positions      -- indices of the peak positions
    */
    Vector<uInt> getPeakPos(Vector<DComplex> spectrum, Vector<Double> frequencyValues,
			    Matrix<Double> frequencyRanges);
    
  private:
            
  };
  
} // Namespace CR -- end

#endif /* DELAYCORRECTIONPLUGIN_H */
  
