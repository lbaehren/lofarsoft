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

#ifndef TOLOPESGRID_H
#define TOLOPESGRID_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/Arrays.h>
#include <casa/BasicMath/Math.h>
#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class toLOPESgrid
    
    \ingroup CR
    \ingroup IO
    
    \brief Interpolate (simulation-)data to the sampling grid as used e.g. by LOPES
    
    \author Andreas Horneffer

    \date 2007/10/10

    \test ttoLOPESgrid.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore or CASA
    </ul>
    
    <h3>Synopsis</h3>

    This class generates an empty dataset that can then be filled with, e.g., simulated 
    pulse data, noise, TV-transmitter data etc. The input data is interpolated to the 
    sampling grid as, e.g., used by LOPES (the default) and added to the internal dataset.
    This internal dataset can then be extracted and, e.g., with TSmatrix2Event converted
    into a LOPES event file.
    
    <h3>Example(s)</h3>
    
  */  
  class toLOPESgrid { //blubb
    
  protected:

    /*!
      \brief start-, stop-time and sampling frequency.
    */
    Double starttime_p, stoptime_p, samplingfreq_p;
    
    /*!
      \brief length (number of samples) of the output data
    */
    Int output_length_p;

    /*!
      \brief time axis values of the output data
    */
    Vector<Double> times_p;

   /*!
      \brief the output data
    */
    Matrix<Double> data_p;
      

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    toLOPESgrid ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~toLOPESgrid ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, toLOPESgrid.
    */
    std::string className () const {
      return "toLOPESgrid";
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
      \brief Clear the internal data-storage

      \param nAntennas=30 - number of antennas (default: 30 for LOPES30)

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes
    */
    Bool reset(Int nAntennas=30);

    /*!
      \brief Add frequency domain data to the internal dataset
      
      \param inFFT       - The frequency domain data to add
      \param blocksize   - blocksize of the of the time-domain data
      \param frequency   - frequency axis of the data
      \param Centered=T  - the time-domain is centered around t=0
      \param begintime   - time of the first sample (in seconds), unused if Centered==T
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes
    */
    Bool AddFFTData(Matrix<DComplex> inFFT, Int blocksize, Vector<Double> frequency,
		    Bool Centered=True, Double begintime=0.);
    

    /*!
      \brief Get the internal data

      \return data - Matrix with the data
    */
    Matrix<Double> GetData() {
      return data_p;
    };

    /*!
      \brief Get the time axis of the internal data

      \return times - Vector with the time values
    */
    Vector<Double> GetTimes() {
      return times_p;
    };

  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TOLOPESGRID_H */
  
