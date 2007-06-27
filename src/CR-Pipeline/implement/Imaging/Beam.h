/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef BEAM_H
#define BEAM_H

// Standard library header files
#include <string>
#include <complex>

#ifdef HAVE_CASA
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#endif

// Custom header files
#include <Imaging/GeometricalWeight.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class Beam
    
    \ingroup Imaging
    
    \brief Brief description for class Beam
    
    \author Lars B&auml;hren

    \date 2007/01/29

    \test tBeam.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class Beam {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Beam ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Beam object from which to create this new
      one.
    */
    Beam (Beam const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~Beam ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Beam object from which to make a copy.
    */
    Beam& operator= (Beam const &other); 
    
    // --------------------------------------------------------------- Parameters


    
    // ------------------------------------------------------------------ Methods

#ifdef HAVE_BLITZ

    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval dataBeam -- [nofSkyPosition,nofChannels] 
      \param  dataFFT  -- [nofDatasets,nofChannels] 

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
     */
    virtual bool beam (blitz::Array<double,2> &dataBeam,
		       const blitz::Array<double,2> &dataFFT) {
      return false;
    }
    
    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval dataBeam -- [nofSkyPosition,nofChannels] 
      \param  dataFFT  -- 

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    virtual bool beam (blitz::Array<std::complex<double>,2> &dataBeam,
		       const blitz::Array<double,2> &dataFFT) {
      return false;
    }
    
#endif

#ifdef HAVE_CASA

    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval dataBeam -- [nofSkyPosition,nofChannels] 
      \param  dataFFT  -- 

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
     */
    virtual bool beam (casa::Matrix<double> &dataBeam,
		       const casa::Matrix<double> &dataFFT) {
      return false;
    }
    
    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval dataBeam -- [nofSkyPosition,nofChannels] 
      \param  dataFFT  -- 

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
    */
    virtual bool beam (casa::Matrix<casa::DComplex> &dataBeam,
		       const casa::Matrix<double> &dataFFT) {
      return false;
    }
    
#endif

    // ----------------------------------------------------------------- Feedback
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Beam.
    */
    std::string className () const {
      return "Beam";
    }
    
    /*!
      \brief Provide a summary of the class's internal data and status
    */
    void summary ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Beam const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // CLASS BEAM -- END
  
}  // NAMESPACE CR -- END

#endif /* BEAM_H */
  
