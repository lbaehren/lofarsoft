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

#ifndef TSMATRIX2EVENT_H
#define TSMATRIX2EVENT_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/Arrays.h>
#include <casa/BasicMath/Math.h>
#include <Data/lopesevent_def.h>

using casa::Bool;
using casa::Matrix;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TSmatrix2Event
    
    \ingroup IO
    
    \brief Brief description for class TSmatrix2Event
    
    \author Andreas Horneffer

    \date 2007/10/12

    \test tTSmatrix2Event.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class TSmatrix2Event {

  protected:

    lopesevent_v1 *eventheader_p;

    Bool hasData_p, hasDate_p;


    Matrix<Short> data_p;

    Vector<Int> AntIDs_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TSmatrix2Event ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TSmatrix2Event object from which to create this new
      one.
    */
    TSmatrix2Event (TSmatrix2Event const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TSmatrix2Event ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TSmatrix2Event object from which to make a copy.
    */
    TSmatrix2Event& operator= (TSmatrix2Event const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TSmatrix2Event.
    */
    std::string className () const {
      return "TSmatrix2Event";
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
    

    Bool reset(void);

    Bool SetData(Matrix<Double> data, Vector<Int> AntIDs, Int presync=0);

    Bool SetDate(Double date);

    Bool SetHeader(Int version=VERSION, uInt JDR=0, Int TL=0, Int LTL=0, Int Type=1,
		   Int evclass=2, Int blocksize=0, Int presync=0, Int observatory=0);

    Bool WriteEvent(String filename);

    Bool GetHeader(Int &version, uInt &JDR, Int &TL, Int &LTL, Int &Type,
		   Int &evclass, Int &blocksize, Int &presync, Int &observatory);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TSmatrix2Event const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TSMATRIX2EVENT_H */
  
