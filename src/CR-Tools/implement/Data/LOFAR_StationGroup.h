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

#ifndef LOFAR_STATIONGROUP_H
#define LOFAR_STATIONGROUP_H

// Standard library header files
#include <iostream>
#include <string>

#include <dal/dal.h>
#include <dal/dalDataset.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class LOFAR_StationGroup
    
    \ingroup CR_Data
    
    \brief Container for the data in the StationGroup of LOFAR times-series data
    
    \author Lars B&auml;hren

    \date 2007/12/10

    \test tLOFAR_StationGroup.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Definition of the LOFAR time-series data format
      <li>[DAL] dalDataset
      <li>[DAL] dalGroup
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class LOFAR_StationGroup {

    //! Group object of the Data Access Library 
    dalGroup *group_p;
    //! HDF5 file handle required to probe further into the group
    hid_t h5file_p;

    
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
    //! If detection was done inside a beam, what was its (local) direction
    std::vector<double> beamDirection_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LOFAR_StationGroup ();

    /*!
      \brief Argumented constructor

      \param dataset -- Dataset containing the group to be encapsulated using
             this class.
      \param name    -- Name of the group within the dalDataset object to be
             extracted.
    */
    LOFAR_StationGroup (dalDataset &dataset,
			std::string const &name);

    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_StationGroup object from which to create
             this new one.
    */
    LOFAR_StationGroup (LOFAR_StationGroup const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_StationGroup ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_StationGroup object from which to make a copy.
    */
    LOFAR_StationGroup& operator= (LOFAR_StationGroup const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the name of the group encapsulated by this class

      \return groupName -- The name of the group encapsulated by this class;
              this is the same name as used to identify the group within the
	      dalDataset object, which forms the next-higher entity of a data
	      object within the DAL.
    */
    inline std::string groupName () const {
      group_p->getName();
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
      \brief Local direction of the beam if one was used for detection within

      \return beamDirection -- Local [Azimuth,Elevation] direction coordinates
              of the beam, in case peak detection was performed in-beam.
    */
    inline std::vector<double> beamDirection () const {
      return beamDirection_p;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_StationGroup.
    */
    std::string className () const {
      return "LOFAR_StationGroup";
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
      \brief Extract the dalGroup object of a given name from a dalDataset
    
      \param dataset -- Dataset containing the group to be encapsulated using
             this class.
      \param name    -- Name of the group within the dalDataset object to be
             extracted.

      \return status -- Status of the operation; returns <tt>false</tt> if an 
              error was encountered.
     */
    bool setStationGroup (dalDataset &dataset,
			  std::string const &name);    
    
  private:

    /*!
      \brief Initialize the object's internal parameters
    */
    void init ();

    /*!
      \brief Extract the keywords values from the station group
      
      \return status -- Status of the operation; returns <tt>false</tt> if an 
              error was encountered.
    */
    bool extractAttributes ();
    
    /*!
      \brief Extract the attributes from the HDF5 dataset
      
      \return status -- Status of the operation; returns <tt>false</tt> if an 
              error was encountered.
    */
    bool extractDatasetAttributes ();
    
    /*!
      \brief Unconditional copying
    */
    void copy (LOFAR_StationGroup const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* LOFAR_STATIONGROUP_H */
  
