/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#ifndef APPLYINSTRUMENTEFFECTS_H
#define APPLYINSTRUMENTEFFECTS_H

// Standard library header files
#include <iostream>
#include <string>

#include <Calibration/CalTableReader.h> 
#include <Calibration/CalTableInterpolater.h>
#include <IO/Simulation2fftMatrix.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class ApplyInstrumentEffects
    
    \ingroup IO
    
    \brief Brief description for class ApplyInstrumentEffects
    
    \author Andreas Horneffer

    \date 2007/09/21

    \test tApplyInstrumentEffects.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class ApplyInstrumentEffects {

  protected:
    /*!
      \brief Observation date used to get the calibration values from the CalTables
    */
    uInt date_p;

    /*!
      \brief The CalTableReader
    */
    CalTableReader *CTRead;
    
    /*!
      \brief Storage for the fft-ed data
    */
    Matrix<DComplex> fft_p;
    
    /*!
      \brief Storage for the frequency-axis
    */
    Vector<Double> frequency_p;
    
    /*!
      \brief Storage for the Antenna-IDs
    */
    Vector<Int> AntIDs_p;

    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    ApplyInstrumentEffects ();
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~ApplyInstrumentEffects ();
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, ApplyInstrumentEffects.
    */
    std::string className () const {
      return "ApplyInstrumentEffects";
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

    // ------------------------------------------------------------ Input Methods

    /*!
      \brief Set the CalTableReader object
      
      \param newCTRead - pointer to a new and initialized CalTableReader object
    */
    void setCalTable(CalTableReader *newCTRead){
      CTRead = newCTRead;
    }
    
    /*!
      \brief Set the Observation date for which the calibration data is taken from the CalTables
      
      \param newObsDate - New value for the date
    */
    void setObsDate(uInt newObsDate){
      date_p = newObsDate;
    }
    
    /*!
      \brief Set the Antenna IDs
      
      \param newAntIDs - Vector with the Antenna IDs
    */
    void setAntennaIDs(Vector<Int> newAntIDs){
      AntIDs_p = newAntIDs;
    }
    
    /*!
      \brief Set the frequency axis
      
      \param newFrequency - Vector with the frequency values
    */
    void setFreqAxis(Vector<Double> newFrequency){
      frequency_p = newFrequency;
    }
    
  
    // ----------------------------------------------------------- Output Methods

    /*!
      \brief Apply the effects to the frequency domain data
      
      \param input - Matrix with the frequency domain data

      \return Matrix with the modified data
    */
    Matrix<DComplex> ApplyEffects(Matrix<DComplex> input);

    
  private:
    
    /*!
      \brief Interpolater for the electronics-gain
    */
    CalTableInterpolater<Double> *CTIElGain;

    /*!
      \brief Interpolater for the Filter-Effects and Phase-Differences
    */
    CalTableInterpolater<DComplex> *CTIFilter, *CTIPhase;
    
    
    /*!
      \brief initialize the interpolaters
    */    
    Bool InitGPFInterpolaters();   

    /*!
      \brief initialization
    */
    void init(void);

    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* APPLYINSTRUMENTEFFECTS_H */
  
