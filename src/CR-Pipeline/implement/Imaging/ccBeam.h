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

#ifndef CCBEAM_H
#define CCBEAM_H

// Standard library header files
#include <string>
#include <complex>

// Custom header files
#include <Imaging/Beam.h>
#include <Math/VectorNorms.h>

namespace CR { // NAMESPACE CR -- BEGIN
  
  /*!
    \class ccBeam
    
    \ingroup Imaging
    
    \brief Cross-correlation beam (cc-beam)
    
    \author Lars B&auml;hren

    \date 2007/01/29

    \test tccBeam.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    The data from each unique pair of antennas is multiplied, the resulting
    values are averaged, and then the square root is taken while preserving
    the sign.
    
    \f[
    cc(\vec \rho)[t] = \, ^+_- \sqrt{\left| \frac{1}{N_{Pairs}} \sum^{N-1}_{i=1}
    \sum^{N}_{j>i} s_i(\vec\rho)[t] s_j(\vec\rho)[t] \right|}
    \f]
    
    where 
    \f[
    s_j(\vec\rho)[t]
    = \mathcal{F}^{-1} \left\{ \tilde s_j(\vec\rho)[k] \right\}
    = \mathcal{F}^{-1} \left\{ w_j(\vec\rho)[k] \cdot \tilde s_j[k]\right\}
    \f]
    is the time shifted field strength of the single antennas for a direction
    \f$\vec \rho \f$. \f$ N \f$ is the number of antennas, \f$t\f$ the time or
    pixel index and \f$N_{Pairs}\f$ the number of unique pairs of antennas.
    The negative sign is taken if the sum had a negative sign before taking the
    absolute values, and the positive sign otherwise.
    
    <h3>Example(s)</h3>
    
  */  
  class ccBeam : public Beam {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    ccBeam ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another ccBeam object from which to create this new
      one.
    */
    ccBeam (ccBeam const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~ccBeam ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another ccBeam object from which to make a copy.
    */
    ccBeam& operator= (ccBeam const &other); 
    
    // --------------------------------------------------------------- Parameters


    
    // ------------------------------------------------------------------ Methods

#ifdef HAVE_BLITZ

    /*!
      \brief Beamforming of the data, returning real-valued result

      \retval dataBeam -- [nofSkyPosition,nofChannels] 
      \param  dataFFT  -- 

      \return status   -- Status of the operation; returns <i>false</i> if an
                          an error was encountered
     */
    virtual bool beam (blitz::Array<double,2> &dataBeam,
		       const blitz::Array<double,2> &dataFFT);

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
    
#endif

    // ----------------------------------------------------------------- Feedback
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, ccBeam.
    */
    std::string className () const {
      return "ccBeam";
    }
    
    /*!
      \brief Provide a summary of the class's internal data and status
    */
    void summary ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (ccBeam const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // CLASS BEAM -- END
  
}  // NAMESPACE CR -- END

#endif /* CCBEAM_H */
  
