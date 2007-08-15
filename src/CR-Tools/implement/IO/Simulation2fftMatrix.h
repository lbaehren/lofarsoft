/***************************************************************************
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

/* $Id: simulation2fftMatrix.h,v 1.2 2007/03/12 10:20:00 horneff Exp $*/

#ifndef SIMULATION2FFTMATRIX_H
#define SIMULATION2FFTMATRIX_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/ImportSimulation.h>
#include <IO/ImportAntenna.h>
#include <Calibration/CalTableReader.h> 
#include <Calibration/CalTableInterpolater.h>

#include <casa/namespace.h>

namespace CR {  //  Namespace CR -- begin
  
  /*!
    \class Simulation2fftMatrix
    
    \ingroup IO
    
    \brief Convert simulation data to a DataReader style matrix.
    
    \author Andreas Horneffer
    
    \date 2007/03/08
    
    \test tSimulation2fftMatrix.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>ImportSimulation
    </ul>
    
    <h3>Synopsis</h3>
    
    Takes the simulation data from an ImportSimulation object and converts it to
    a DataReader style fft-matrix, while applying the polarization characteristics
    of the antennas.
    
    <h3>Example(s)</h3>
    
  */  
  class Simulation2fftMatrix {
    
  protected:
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
    
    /*!
      \brief Original length on the data before fft-ing.
    */
    Int blocklen_p;
    
    /*!
      \brief Length on the data after fft-ing.
    */
    Int fftlen_p;
    
    /*!
      \brief Is there already data?
    */
    Bool has_data_p;
    
    /*!
      \brief Observation date used to get the calibration values from the CalTables
    */
    uInt date_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Simulation2fftMatrix();
    
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~Simulation2fftMatrix ();
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Simulation2fftMatrix.
    */
    std::string className () const {
      return "Simulation2fftMatrix";
    }
    
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
    
    // ----------------------------------------------------------- Output Methods
    
    /*!
      \brief Get the original block length.
      
      \return The original block length.
    */
    Int getBlocklen();
    
    /*!
      \brief Get the frequency axis of the data.
      
      \return Vector with the frequencies in Hz
    */
    Vector<Double> getFrequency();
    
    /*!
      \brief Get the frequency domain data
      
      \return Matrix with the fft-ed data
    */
    Matrix<DComplex> getfft();
    
    /*!
      \brief Get the AntennaIDs 
      
      \return Vector with the AntennaIDs
    */
    Vector<Int> getAntIDs();
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Import the data from a simulation
      
      \param inputSim - An already initialized ImportSimulation object from 
      which to read in the data.
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool SimulationImport (ImportSimulation *inputSim);
    
  private:
    
    /*!
      \brief The two CalTableInterpolater objects
    */
    CalTableInterpolater<Double> *CTIazi, *CTIzeni;
    
    /*!
      \brief Initialize the CalTableInterpolaters
      
      \param azimuth - Azimut direction of the simulated air shower
      \param elevation - Elevation of the simulated air shower
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool initCTinterpolater(Double azimuth, Double elevation);
    
    /*!
      \brief Combine the two polarizations into one set according to the polarization 
      characteristics of the antenna.
      
      \param AntID - ID of the antenna for which the data is combined
      \param FFTazi - FD data of the azimuth polarization component
      \param FFTzeni - FD data of the zenith polarization componen
      
      \return Frequency domain data that combines the 
    */
    Vector<DComplex> combinePolarizations(Int AntID, Vector<DComplex> &FFTazi, 
					  Vector<DComplex> &FFTzeni);
    
    /*!
      \brief Calculate the frequency axis
      
      \param StepSize - Size of a sampling step in seconds, i.e. the time between two samples.
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool CalcFreqeuncies(Double StepSize);
    
    
  };

}  //  Namespace CR -- end
  
#endif /* SIMULATION2FFTMATRIX_H */
  
