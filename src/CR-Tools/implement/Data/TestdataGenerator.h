/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef TESTDATAGENERATOR_H
#define TESTDATAGENERATOR_H

// Standard library header files
#include <iostream>
#include <string>

/* CR-Tools header files */
#include <IO/DataReader.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TestdataGenerator
    
    \ingroup CR_Data
    
    \brief A class for the generator of test data
    
    \author Lars B&auml;hren

    \date 2009/06/04

    \test tTestdataGenerator.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class TestdataGenerator : public DataReader {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    TestdataGenerator ();

    /*!
      \brief Argumented constructor 
      
      \param blocksize   -- Number of samples per block of data
      \param nofAntennas -- Number of antennas
    */
    TestdataGenerator (uint const &blocksize,
		       uint const &nofAntennas);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TestdataGenerator object from which to create this new
             one.
    */
    TestdataGenerator (TestdataGenerator const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~TestdataGenerator ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TestdataGenerator object from which to make a copy.
    */
    TestdataGenerator& operator= (TestdataGenerator const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TestdataGenerator.
    */
    inline std::string className () const {
      return "TestdataGenerator";
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
      \brief Get the raw time series after ADC conversion.

      \return fx -- [sample,antenna] Raw ADC time series, [counts]
    */
    Matrix<double> fx ();
    
  private:
    
    //! Unconditional copying
    void copy (TestdataGenerator const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class TestdataGenerator -- end
  
} // Namespace CR -- end

#endif /* TESTDATAGENERATOR_H */
  
