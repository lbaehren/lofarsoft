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


#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TSmatrix2Event
    
    \ingroup IO
    
    \brief Convert a time-series matrix into a LOPES event file
    
    \author Andreas Horneffer

    \date 2007/10/12

    \test tTSmatrix2Event.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casacore or CASA
    </ul>
    
    <h3>Synopsis</h3>
    
    Generate a LOPES event file with (simulation) data. The quantisation is done by 
    converting values of type <tt>double</tt> to the type <tt>short</tt>. No check is
    done if the values exceed the valid range for shorts. Before a file can be written
    at least the time-series data (e.g. from a toLOPESgrid object) and the "observation"
    date has to be supplied. (The defaults assume LOPES simulation data.)
    
    <h3>Example(s)</h3>
    
  */  
  class TSmatrix2Event {

  protected:

    lopesevent *eventheader_p;

    Bool hasData_p, hasDate_p;


    Matrix<short> data_p;

    Vector<int> AntIDs_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TSmatrix2Event ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TSmatrix2Event ();
        
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
    

    /*!
      \brief Clear the internal data-storage

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes

      Deletes all stored data and resets the header to the default values
    */
    Bool reset(void);

    /*!
      \brief Set the data

      \param data    - Matrix with the time series data for all channels
      \param AntIDs  - Vector with the antenna IDs (data.ncolumn() must be equal to AntIDs.nelements())
      \param presync - Number of samples before the "trigger", sets the presync header field

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes
    */
    Bool SetData(Matrix<double> data,
		 Vector<int> AntIDs,
		 int presync=0);

    /*!
      \brief Set the date of the "observation"

      \param date - Date of the "observation" in seconds since 1.1.1970

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes

      This sets the JDR, TL, and LTL header fields. The integer part of the input value
      sets the JDR field, and the non-integer part the TL and LTL fields.
    */
    Bool SetDate(double date);

    /*!
      \brief Set the header of the LOPES event

      \param version     - Header field: Version
      \param JDR         - Header field: Julian Date Register
      \param TL          - Header field: Time Label
      \param LTL         - Header field: LOPES Time Label
      \param Type        - Header field: Event Type
      \param evclass     - Header field: Event Class
      \param blocksize   - Header field: blocksize
      \param presync     - Header field: presync
      \param observatory - Header field: Observatory

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes
      
      For the definition of the different header fields please refer to the documentation 
      of the LOPES event file format.
    */
    Bool SetHeader(int version=VERSION,
		   uInt JDR=0,
		   int TL=0,
		   int LTL=0,
		   int Type=1,
		   int evclass=2,
		   int blocksize=0,
		   int presync=0,
		   int observatory=0);

    /*!
      \brief Write the internal data into a LOPES event file.

      \param filename - Path/Filename of the file to be written

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes

      Before you can successfully call this function you have to set the time-series
      data and header data.
    */
    Bool WriteEvent(String filename);

    /*!
      \brief Get the header information of the internal data

      \param version     - Header field: Version (output)
      \param JDR         - Header field: Julian Date Register (output)
      \param TL          - Header field: Time Label (output)
      \param LTL         - Header field: LOPES Time Label (output)
      \param Type        - Header field: Event Type (output)
      \param evclass     - Header field: Event Class (output)
      \param blocksize   - Header field: blocksize (output)
      \param presync     - Header field: presync (output)
      \param observatory - Header field: Observatory (output)

      \return ok -- Was operation successful? Returns <tt>True</tt> if yes
    */
    Bool GetHeader(int &version,
		   uInt &JDR,
		   int &TL,
		   int &LTL,
		   int &Type,
		   int &evclass,
		   int &blocksize,
		   int &presync,
		   int &observatory);
    
  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TSMATRIX2EVENT_H */
  
