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

#ifndef LOFAR_TBB_H
#define LOFAR_TBB_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_StationGroup.h>
#include <IO/DataReader.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class LOFAR_TBB
    
    \ingroup CR_Data
    
    \brief Interface between Data Access Library (DAL) and DataReader framework
    
    \author Lars B&auml;hren

    \date 2007/12/07

    \test tLOFAR_TBB.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[CR] DataReader
      <li>[DAL] dalDataset
      <li>[DAL] dalGroup
    </ul>
    
    <h3>Synopsis</h3>

    This class encapsulates the required functionality to connect the DataReader
    framework of the CR-Tools to the lower-level functionality of the Data Access
    Library (DAL), which handles the access to the standard LOFAR data products.

    <ul>
      <li><b>Signal channel ID</b> <br> 
      The signal channel for an individual reception element (dipole) can be
      uniquely identified through a combination of the following three separate
      identifiers:
      <ol>
        <li>Station ID
	<li>RSP ID
	<li>RCU ID
      </ol>
    </ul>
    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_TBB : public DataReader {

    //! Filename of the dataset
    std::string filename_p;
    //! DAL Dataset object to handle the basic I/O
    dalDataset *dataset_p;

    //! Set of station groups within the data set
    std::vector<CR::LOFAR_StationGroup> stationGroups_p;

    //! Name of the telescope
    std::string telescope_p;
    //! Name of the observer
    std::string observer_p;
    //! Project name/description
    std::string project_p;
    //! Observation ID
    std::string observationID_p;
    //! Observation mode
    std::string observationMode_p;
    //! Trigger type
    std::string triggerType_p;
    //! Trigger offset
    double triggerOffset_p;
    
    //! Identifiers for the individual channels/dipoles
    std::vector<std::string> channelID_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Argumented constructor
    */
    LOFAR_TBB (std::string const &filename);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_TBB object from which to create this new
      one.
    */
    LOFAR_TBB (LOFAR_TBB const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_TBB ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_TBB object from which to make a copy.
    */
    LOFAR_TBB& operator= (LOFAR_TBB const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the data file
      
      \return filename -- The name of the data file
    */
    inline std::string filename () const {
      return filename_p;
    }

    /*!
      \brief Get the name of the telescope
      
      \return telescope -- The name of the telescope with which the data were
              recorded.
    */
    inline std::string telescope () const {
      return telescope_p;
    }

    /*!
      \brief Get the name of the observer
      
      \return observer -- The name of the observer.
    */
    inline std::string observer () const {
      return observer_p;
    }
    
    /*!
      \brief Get the project name/description
      
      \return project -- Name/Description of the project for which this 
              observation was carried out.
    */
    inline std::string project () const {
      return project_p;
    }

    /*!
      \brief Get the observation ID

      \return observationID -- The observation ID.
    */
    inline std::string observationID () const {
      return observationID_p;
    }
    
    /*!
      \brief Get the description of the observation mode

      \return observationMode -- Description/type of observation mode
    */
    inline std::string observationMode () const {
      return observationMode_p;
    }
    
    /*!
      \brief Get the trigger type which cause recording this data

      \return triggerType -- The trigger type which cause recording this data
    */
    inline std::string triggerType () const {
      return triggerType_p;
    }

    /*!
      \brief Get the trigger offset

      \return triggerOffset -- The trigger offset.
    */
    inline double triggerOffset () const {
      return triggerOffset_p;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_TBB.
    */
    std::string className () const {
      return "LOFAR_TBB";
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
    
    casa::Matrix<double> fx ();
    
  private:

    /*
      \brief Initialize the internal settings

      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    bool init ();

    /*!
      \brief Get the list of available station groups within the data file
     */
    std::vector<std::string> getStationGroups ();

    /*!
      \brief Extract the keywords from the station group

      \param groupName -- Name of the station group from which to extract the
             keyword values.

      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
     */
    bool getStationGroupKeywords (std::string const &groupName);

    /*!
      \brief Unconditional copying
    */
    void copy (LOFAR_TBB const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* LOFAR_TBB_H */
  
