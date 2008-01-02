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
      <li>[CR] LOFAR_TBB -- Interface between Data Access Library (DAL) and
      DataReader framework
      <li>[DAL] dalDataset
      <li>[DAL] dalGroup
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class LOFAR_StationGroup {

    //! HDF5 file handle ID
    hid_t H5fileID_p;
    //! Group object of the Data Access Library 
    dalGroup *group_p;
    
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
      \brief Get the file handle ID for the underlying HDF5 data file

      \return H5fileID -- The file handle ID for the underlying HDF5 data file;
              given this ID we can directly access the contents of the opened
	      HDF5 file through the functionality of the HDF5 library, thereby
	      bypassing the DAL layer.
    */
    inline hid_t fileID () const {
      return H5fileID_p;
    }
    
    /*!
      \brief Get the name of the telescope
      
      \return telescope -- The name of the telescope with which the data were
              recorded; returns an empty string in case no keyword value could
	      be extracted.
    */
    std::string telescope ();

    /*!
      \brief Get the name of the observer
      
      \return observer -- The name of the observer; returns an empty string in
              case no keyword value could be extracted.
    */
    std::string observer ();
    
    /*!
      \brief Get the project name/description
      
      \return project -- Name/Description of the project for which this 
              observation was carried out; returns an empty string in case no
	      keyword value could be extracted.
    */
    std::string project ();

    /*!
      \brief Get the observation ID

      \return observationID -- The observation ID; returns an empty string in
              case no keyword value could be extracted.
    */
    std::string observationID ();
    
    /*!
      \brief Get the description of the observation mode

      \return observationMode -- Description/type of observation mode
    */
    std::string observationMode ();
    
    /*!
      \brief Get the trigger type which cause recording this data

      \return triggerType -- The trigger type which cause recording this data
    */
    std::string triggerType ();

    /*!
      \brief Get the trigger offset

      \return triggerOffset -- The trigger offset.
    */
    double triggerOffset ();

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
      \brief Retrieve the station IDs from the antenna datasets within this group

      \return stationIDs -- The station IDs from the antenna datasets within this
              group
    */
    std::vector<uint> stationIDs ();
    
    /*!
      \brief Retrieve the RSP IDs from the antenna datasets within this group

      \return rspIDs -- The RSP IDs from the antenna datasets within this
              group
    */
    std::vector<uint> rspIDs ();
    
    /*!
      \brief Retrieve the RCU IDs from the antenna datasets within this group

      \return rcuIDs -- The RCU IDs from the antenna datasets within this
              group
    */
    std::vector<uint> rcuIDs ();
    
    /*!
      \brief Retrieve the list of channels IDs contained within this group

      \return channelIDs -- A list of the channel IDs for all the dipoles within
              this LOFAR station.
     */
    std::vector<std::string> channelIDs ();

    /*!
      \brief Convert individual ID number to joint unique ID

      \param station_id -- Identification number of the LOFAR station within the
             complete array.
      \param rsp_id -- Identification number of a Remote Station Processing
             (RSP) board within a given LOFAR station.
      \param rcu_id -- Identification number of a Receiver Unit (RCU) attached to
             a given RSP board.

      \return channel_id -- Unique identifier string for an individual dipole 
              within the full LOFAR array.
    */
    inline std::string channelID (unsigned int const &station_id,
				  unsigned int const &rsp_id,
				  unsigned int const &rcu_id) {
      char uid[10];
      sprintf(uid, "%03d%03d%03d",
	      station_id,rsp_id,rcu_id);
      std::string channel_id (uid);
      return channel_id;
    }
    
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
      \brief Get the value of a given attribute
    */
    void getAttribute (std::string &value,
		       std::string const &keyword);
    
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
  
