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

#ifndef GENTVSPEC_H
#define GENTVSPEC_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/Arrays.h>
#include <casa/BasicMath/Math.h>

#include <Calibration/CalTableReader.h> 
#include <Calibration/CalTableInterpolater.h>


namespace CR { // Namespace CR -- begin
  
  /*!
    \class genTVSpec
    
    \ingroup IO
    
    \brief Generate a mock spectrum of the "LOPES TV-transmitter"
    
    \author Andreas Horneffer

    \date 2007/10/17

    \test tgenTVSpec.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>initialized CalTableReader
    </ul>
    
    <h3>Synopsis</h3>

    This class generates a mock spectrum of the "LOPES TV-transmitter" by setting one
    bin (in frequency space) for each "carrier frequency" to a given amplitude and the 
    correct phase. Before it can be used it needs an initialized CalTableReader, the
    needed AntennaIDs and the frequency values for the frequency space data.
    
    <h3>Example(s)</h3>
    
  */  
  class genTVSpec {

  protected:
    
    /*!
      \brief The CalTableReader
    */
    CalTableReader *CTRead;

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
    genTVSpec ();
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~genTVSpec ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, genTVSpec.
    */
    std::string className () const {
      return "genTVSpec";
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
      \brief Set the CalTableReader object
      
      \param newCTRead - pointer to a new and initialized CalTableReader object
    */
    void setCalTable(CalTableReader *newCTRead){
      CTRead = newCTRead;
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
      frequency_p.resize(newFrequency.shape());
      frequency_p = newFrequency;
    }
    
    /*!
      \brief Generate a spectrum that contains the signals for the TV-calibration
      
      \param date        - date for which to get the reference data from the CalTables
      \param Amplitude=1 - Amplitude of the signals

      \return Matrix with the data
    */
    Matrix<DComplex> GetTVSpectrum(uInt date, Double Amplitude=1e3);
    
  private:
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* GENTVSPEC_H */
  
