/*-------------------------------------------------------------------------*
 | $Id:: checkNoiseInfluence.h 3949 2010-01-08 15:22:30Z schroeder        $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                   *
 *   Frank Schroeder (<mail>)                                              *
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

#ifndef CHECKNOISEINFLUENCE_H
#define CHECKNOISEINFLUENCE_H


#include <crtools.h>

#include <string>

#ifdef HAVE_STARTOOLS

#include <Analysis/analyseLOPESevent2.h>

namespace CR { // Namespace CR -- begin

  /*!
    \class checkNoiseInfluence

    \ingroup Analysis

    \brief Checks influence of noise on pulse height

    \author Frank Schr&ouml;der

    \date 2010/Jan/11

    <h3>Motivation</h3>

    The aim of this class is to resolve the question how typical
    LOPES radio noise affects the height of a radio pulse.

    <h3>Synopsis</h3>

    adds a test pulse to noise and looks how the pulse height changes

  */
  class checkNoiseInfluence : public analyseLOPESevent2 {

  protected:
  //! trace of radio pulse to be added to data
  Vector<Double> pulsePattern;

  //! properties of test pulse
  PulseProperties testPulseProperties;
  
  //! height of pulse in pulse pattern (used to scale it down or up)
  double pulseHeight;
  
  //! time of pulse in pulse pattern (maximum of envelope)
  double pulseTime;
  
  //! time in trace for first noise interval (default -150 µs)
  double startTime;
  
  //! method for noise calculation in CompletePipeline
  int noiseMethod;

  //! length of noise interval (default 10 µs)
  double noiseIntervalLength;
  
  //! gap between noise intervals (default 5 µs)
  double noiseIntervalGap;

  //! number of noise intervals (default 20)
  int NnoiseIntervals;

  //! upsampled trace of noise event
  Matrix<Double> upsampledNoise;

  //! time axis of upsampled noise traces
  Vector<Double> noiseTimeAxis;
  
  //! height of noise in the different antennas and different noise intervals
  vector< vector<double> > noiseHeight;
  
  public:

    // ------------------------------------------------------------- Construction

    /*!
      \brief Default constructor
    */
    checkNoiseInfluence ();

    /*!
      \brief Copy constructor

      \param other -- Another checkNoiseInfluence object from which to create
             this new one.
    */
    checkNoiseInfluence (checkNoiseInfluence const &other);

    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~checkNoiseInfluence ();

    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the class

      \return className -- The name of the class, checkNoiseInfluence.
    */
    std::string className () const {
      return "checkNoiseInfluence";
    }

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get number of noise intervals (do not use more than 20)

      \return noiseIntervals -- number of noise intervals
    */
    inline int getNnoiseIntervals () {
      return NnoiseIntervals;
    }

    /*!
      \brief Set number of noise intervals (do not use more than 20)

      \param noiseIntervals -- number of noise intervals
    */
    inline void setNnoiseIntervals (int const &intervals) {
      NnoiseIntervals = intervals;
    }


    /*!
      \brief Loads the pattern of a (calibration) test pulse

      \param evname          - path to the eventfile with the pulse
      \param antenna         - Antenna number to take the test pulse from
      \param pulseStart      - start time of pulse interval (in s)
    */
    void loadPulsePattern(const string& evname,
                          const int& antenna, 
                          const double& pulseStart);
                          
    /*!
      \brief Loads noise event
      
      \param evname          - path to the eventfile with noise
    */
    void loadNoiseEvent(const string& evname);                          

    /*!
      \brief Adds pulse to the noise 
      
      \param lowerSNR          - upper bound of SNR values used for generating the random SNR of the pulse
      \param upperSNR          - lower bound of SNR values used for generating the random SNR of the pulse
      \param resultsFileName   - name of file for ASCII results
    */
    void addPulseToNoise(const double& lowerSNR,
                         const double& upperSNR,
                         const string& resultsFileName = "");                          
 
    /*!
      \brief Loads noise event, and studies SNR of pure noise with different noise defintions
      
      \param evname          - path to the eventfile with noise
    */
    void SNRofNoise(const string& evname);                          

                          
  }; // class definition
  
} // Namespace CR -- end

#endif /* HAVE_STARTOOLS */

#endif /* CHECKNOISEINFLUENCE_H */
  
