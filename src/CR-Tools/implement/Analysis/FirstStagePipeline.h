/***************************************************************************
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

/* $Id: FirstStagePipeline.h,v 1.4 2007/06/20 15:28:11 horneff Exp $*/

#ifndef FIRSTSTAGEPIPELINE_H
#define FIRSTSTAGEPIPELINE_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/DataReader.h>
#include <Calibration/CalTableReader.h> 
#include <Calibration/CalTableInterpolater.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class FirstStagePipeline
    
    \ingroup Analysis
    
    \brief First stage of a pipeline: Opening file and putting the calibration values 
           into the DataReader
    
    \author Andreas Horneffer

    \date 2007/01/11

    \test tFirstStagePipeline.cc
    
    <h3>Prerequisite</h3>    
    <ul>
      <li> Open an event file via the DataReader
      <li> Get the calibration values from the CalTables
      <li> Feed the calibration values into the DataReader
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>

    Please have a look at "tFirstStagePipeline.cc" 
    I (tried to) added quit a bit of comments into the code...
    
  */  
  class FirstStagePipeline {
  protected:
    
    /*!
      \brief Generate verbose output;
    */
    Bool verbose;

    /*!
      \brief Have the CalTableInterpolator objects been initialized?
    */
    Bool InterpInit;
    
    /*!
      \brief The CalTableInterpolator objects for the different fields
    */
    CalTableInterpolater<Double> *InterElGainCal_p;
    CalTableInterpolater<DComplex> *InterPhaseCal_p;
    
    
    /*!
      \brief blocksize and samplerate, needed to check if freqency axis has changed
    */
    uInt blocksize;
    Double samplerate;

    /*!
      \brief Record with the pathes to the CalTables
    */
    Record ObsToCTPath;

    /*!
      \brief The CalTableReader
    */
    CalTableReader *CTRead;
    
    // ---------------------------------------------------------- Private Methods

    /*!
      \brief Initialize the CalTableInterpolaters

      \param *dr - Pointer to the DataReader that holds the data
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool Init_CalTableInter(DataReader *dr);
    
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    FirstStagePipeline ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~FirstStagePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, FirstStagePipeline.
    */
    std::string className () const {
      return "FirstStagePipeline";
    }
    
    // ------------------------------------------------------------------ Methods
    /*!
      \brief Initialize a new event, and fill the DataReader with the needed calibration 
      values. According to the header data (especially "Observatory", "Date", and 
      "AntennaIDs") the calibration values are retrieved and put into the data reader object.
      
      \param *dr      - Pointer to an already existing DataReader object, that already has
                        (header-)data loaded. 
      
      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */

    Bool InitEvent(DataReader *dr);
    
    /*!
      \brief Retrieve the CalTableReader object.

      The pipeline object creates it's own CalTableReader. This can also be used by
      other routines.
      The returned object is valid as at least as long as the pipeline object exists 
      and no new event was opened. If a new event was opened, the CalTableReader may 
      (but does not have to) change.
            
      \return CalTableReader -- Pointer to the CalTableReader object. 
    */

    CalTableReader *GetCalTableReader(){
      return CTRead;
    };
    
    /*!
      \brief Set the record that matches observatory names to the path to the corresponding CalTable

      \param InputRecord - record with the data

      \return Returns True
    */

    Bool SetObsRecord(Record InputRecord) {
      ObsToCTPath.assign(InputRecord);
      return True;
    };

    /*!
      \brief Set the verbosity
    */
    inline void setVerbosity(Bool newVerbosity) { verbose = newVerbosity; };
    
  private:
        
    /*!
      \brief Standard initialization
    */
    void init(void);

   /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* FIRSTSTAGEPIPELINE_H */
  
