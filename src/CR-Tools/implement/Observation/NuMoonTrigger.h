/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Kalpana Singh (<k.singh@rug.nl>)                                      *
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

#ifndef NUMOONTRIGGER_H
#define NUMOONTRIGGER_H

// Standard library header files
#include <iostream>
#include <string>
#include <cmath>

// AIPS++/CASA header files

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>

#include <casa/Quanta.h>
#include <casa/Exceptions/Error.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>

#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>

#include <casa/namespace.h>

#include <Display/SimplePlot.h>

using CR::SimplePlot;

namespace  { // Namespace  -- begin
  
  /*!
    \class NuMoonTrigger
    
    \ingroup Observation
    
    \brief Brief description for class NuMoonTrigger
    
    \author Kalpana Singh

    \date 2008/08/20

    \test tNuMoonTrigger.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class NuMoonTrigger {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    NuMoonTrigger ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another NuMoonTrigger object from which to create this new
      one.
    */
    NuMoonTrigger ( const uint& observing_time,
    		    const Double& sampling_rate,
		    const Double& mean,
		    const Double& variance,
                    const uint& cutoff_level,
		    const uint& dead_time,
		    const uint& P3 ) ;
		    
   		    
		    
		 
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~NuMoonTrigger ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another NuMoonTrigger object from which to make a copy.
    */
    NuMoonTrigger& operator= (NuMoonTrigger const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, NuMoonTrigger.
    */
    std::string className () const {
      return "NuMoonTrigger";
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
    
    Vector<Double> setDataPoints (const uint& observing_time,
    		                  const Double& sampling_rate,
				  const Double& mean,
		                  const Double& variance );
				  
    
    				  				  
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (NuMoonTrigger const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace  -- end

#endif /* NUMOONTRIGGER_H */
  
