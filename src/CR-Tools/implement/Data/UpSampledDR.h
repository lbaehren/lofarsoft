/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#ifndef UPSAMPLEDDR_H
#define UPSAMPLEDDR_H

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

#include <casa/namespace.h>

//CR-tools header files
#include <IO/DataReader.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class UpSampledDR
    
    \ingroup Data
    
    \brief DataReader class that upsamples the the output of another DataReader
    
    \author Andreas Horneffer

    \date 2008/04/23

    \test tUpSampledDR.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li> DataReader
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class UpSampledDR : public DataReader{

  private:

    /*!
      \brief Pointer to the data Raeder from which the data is read
    */
    DataReader *inpDR_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    UpSampledDR ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~UpSampledDR ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, UpSampledDR.
    */
    std::string className () const {
      return "UpSampledDR";
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
      \brief Setup this DataReader

      \param inputDR -- pointer to the DataReader object to read from 
      \param newSampleFrequency -- the new sample frequency (in Hz), should be larger 
                                   than the one in <tt>inputDR<\tt>
      
      \return ok -- True if successfull
    */
    Bool setup(DataReader *inputDR, double newSampleFrequency);

    /*!
      \brief return the upsampled data in the original units.
      
      \return Matrix with the data
    */
    Matrix<Double> fx();
    
  protected:
    /*!
      \brief Fill the header information from the into a header record.
      
      \return status -- Status of the operation; returns <i>true</i> if everything
      went fine.
    */
    Bool generateHeaderRecord();
    
  private:
        
    /*!
      \brief blank initialization
    */
    void init(void);

    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* UPSAMPLEDDR_H */
  
