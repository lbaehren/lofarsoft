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

// Standard library header files
// #include <iostream>
// #include <iomanip>
// #include <fstream>
#include <string>
// #include <algorithm>

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
  
  //! length of noise interval (default 10 µs)
  double noiseIntervalLength;
  
  //! gap between noise intervals (default 5 µs)
  double noiseIntervalGap;

  //! number of noise intervals (default 20)
  int NnoiseIntervals;

  //! upsampled trace of noise event
  Matrix<Double> upsampledNoise;

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

    // ---------------------------------------------------------------- Operators

    /*!
      \brief Overloading of the copy operator

      \param other -- Another checkNoiseInfluence object from which to make a copy.
    */
    checkNoiseInfluence& operator= (checkNoiseInfluence const &other); 

    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the class

      \return className -- The name of the class, checkNoiseInfluence.
    */
    std::string className () const {
      return "checkNoiseInfluence";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    


    // ------------------------------------------------------------------ Methods

    /*!
      \brief Loads the pattern of a (calibration) test pulse

      \param evname          - path to the eventfile to be processed
      \param antenna         - Antenna number to take the test pulse from
      \param pulseStart      - start time of pulse (in s)
      \param pulseStop       - stop time of pulse (in s)
    */
    void loadPulsePattern(const string& evname,
                          const int& antenna, 
                          const double& pulseStart,
                          const double& pulseStop);
                          
    /*!
      \brief Loads noise event
      
      \param evname          - path to the eventfile to be processed
    */
    void loadNoiseEvent(const string& evname);                          

    /*!
      \brief Adds pulse to the noise
      
      \param pulseSNR          - signal-to-noise ratio to which the test pulse is scaled
    */
    void addPulseToNoise(const double& pulseSNR);                          

                          
  }; // class definition
  
} // Namespace CR -- end

#endif /* HAVE_STARTOOLS */

#endif /* CHECKNOISEINFLUENCE_H */
  
