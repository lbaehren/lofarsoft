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

#ifndef ANTENNAGAIN_H
#define ANTENNAGAIN_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/BasicSL/Complex.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class AntennaGain
    
    \ingroup Imaging
    
    \brief Brief description for class AntennaGain
    
    \author Lars B&auml;hren

    \date 2007/11/27

    \test tAntennaGain.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Beamformer
      <li>GeometricalWeight
    </ul>
    
    <h3>Synopsis</h3>

    For each antenna \f$j\f$ we need to able to assign a direction- and
    frequency-dependent gain factor
    \f[ g_{j} = g_{j} (\vec\rho,\nu) \f]
    
    <h3>Example(s)</h3>
    
  */  
  class AntennaGain {

    //! Buffer the antenna gains?
    bool bufferAntennaGains_p;

  protected:

    casa::Cube<casa::DComplex> gains_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    AntennaGain ();
    
    /*!
      \brief Argumented constructor
      
      \param gains -- [freq,antenna,direction] Array holding the weighting
             factors \f$g_{j}(\vec\rho,\nu)\f$ describing the antenna gain(s).
    */
    AntennaGain (casa::Array<casa::DComplex> const &gains);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another AntennaGain object from which to create this new
      one.
    */
    AntennaGain (AntennaGain const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~AntennaGain ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another AntennaGain object from which to make a copy.
    */
    AntennaGain& operator= (AntennaGain const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, AntennaGain.
    */
    std::string className () const {
      return "AntennaGain";
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
    
    bool setGains (casa::Array<casa::DComplex> const &gains);

    inline casa::Array<casa::DComplex> gains () {
      return gains_p;
    }
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (AntennaGain const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ANTENNAGAIN_H */
  
