/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>

#include <dal/dal.h>
#include <dal/dalDataset.h>

#include <Data/LOFAR_DipoleDataset.h>

namespace DAL { // Namespace DAL -- begin
  
  /*!
    \class LOFAR_StationGroup
    
    \ingroup CR_Data
    \ingroup DAL
    
    \brief Container for the data in the StationGroup of LOFAR times-series data
    
    \author Lars B&auml;hren

    \date 2007/12/10

    \test tLOFAR_StationGroup.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Definition of the LOFAR time-series data format
      <li>[DAL] DAL::LOFAR_Timeseries
      <li>[DAL] DAL::LOFAR_DipoleDataset
      <li>[CR] CR::LOFAR_TBB -- Interface between Data Access Library (DAL) and
      CR::DataReader framework
    </ul>
    
    <h3>Synopsis</h3>
    
    <ol>
      <li>Structure of the HDF5 group inside the time-series data format:
      \verbatim
      /
      |-- Station001                ... Group
      |   |-- TELESCOPE             ... Attribute       ... string
      |   |-- OBSERVER              ... Attribute       ... string
      |   |-- PROJECT               ... Attribute       ... string
      |   |-- OBSERVATION_ID        ... Attribute       ... string
      |   |-- OBSERVATION_MODE      ... Attribute       ... string
      |   |-- TRIGGER_TYPE          ... Attribute       ... string
      |   |-- TRIGGER_OFFSET        ... Attribute       ... string
      |   |-- TRIGGERED_ANTENNAS    ... Attribute       ... array<int,1>
      |   |-- BEAM_DIRECTION        ... Attribute       ... array<double,2>
      |   |-- 001000000             ... Dataset         ... array<uint,1>
      |   |   `-- 
      |   |-- 001000001             ... Dataset         ... array<uint,1>
      |   |   `-- 
      \endverbatim
    </ol>

    <h3>Example(s)</h3>
    
  */
  class LOFAR_StationGroup {

    //! Identifier for this group within the HDF5 file
    hid_t groupID_p;
    //! Datasets contained within this group
    std::vector<LOFAR_DipoleDataset> datasets_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LOFAR_StationGroup ();
    
    /*!
      \brief Argumented constructor

      \param filename -- Name of the HDF5 file within which the group is
             contained.
      \param group    -- Name of the group, in this case the full path from
             the base of the hierarchical structure within the HDF5 file.
    */
    LOFAR_StationGroup (std::string const &filename,
			std::string const &group);
    
    /*!
      \brief Argumented constructor

      \param location -- Identifier for the location within the HDF5 file, below
             which the group is placed.
      \param group    -- Name of the group.
    */
    LOFAR_StationGroup (hid_t const &location,
			std::string const &group);
    
    /*!
      \brief Argumented constructor

      \param group_id -- Identifier for the group contained within the HDF5
             file
    */
    LOFAR_StationGroup (hid_t const &group_id);
    
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
      
      \param other -- Another LOFAR_StationGroup object from which to make a
             copy.
    */
    LOFAR_StationGroup& operator= (LOFAR_StationGroup const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the identifier for this group within the HDF5 file
      
      \return groupID -- The identifier for this group within the HDF5 file
    */
    inline hid_t group_id () const {
      return groupID_p;
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

      \return observation_id -- The observation ID; returns an empty string in
              case no keyword value could be extracted.
    */
    std::string observation_id ();
    
    /*!
      \brief Get the description of the observation mode

      \return observation_mode -- Description/type of observation mode
    */
    std::string observation_mode ();
    
    /*!
      \brief Get the trigger type which cause recording this data

      \return trigger_type -- The trigger type which cause recording this data
    */
    std::string trigger_type ();

    /*!
      \brief Get the trigger offset

      \return trigger_offset -- The trigger offset.
    */
    double trigger_offset ();

    /*!
      \brief Get the list of triggered antennas

      \return triggered_antennas -- List of antennas, for which the trigger
              condition was fullfilled.
    */
#ifdef HAVE_CASA
    casa::Vector<uint> triggered_antennas ();
#else 
    std::vector<uint> triggered_antennas ();
#endif

    /*!
      \brief Get the direction of the station beam

      \return beam_direction -- The direction towards which a station beam is
              formed before forwarding the data to central processing
    */
#ifdef HAVE_CASA
    casa::Vector<double> beam_direction ();
#else 
    std::vector<double> beam_direction ();
#endif

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
      \brief Get the number of dipole datasets within this station group
      
      \return nofDipoleDatasets -- The number of dipole datasets contained with
              this station group.
    */
    inline uint nofDipoleDatasets () {
      return datasets_p.size();
    }

    /*!
      \brief Get the values of the TIME attribute for all present datasets
      
      \return times -- Values of the TIME attribute for all datasets present in
              this station group
    */
#ifdef HAVE_CASA
    casa::Vector<uint> times ();
#else
    std::vector<uint> times ();
#endif

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
      \brief Get identifiers to the datasets within the station group

      \return dataset_ids -- Vector with a list of the identifiers to the
              HDF5 dataset objects within this station group.
    */
    std::vector<hid_t> datasetIDs ();
    
    /*!
      \brief Get a casa::Record containing the values of the attributes

      \return record -- A casa::Record container holding the values of the 
              attributes attached to the dataset for this dipole
    */
    casa::Record attributes2record ();
    
  private:

    /*!
      \brief Initialize the internal dataspace
    */
    void init ();
    
    /*!
      \brief Initialize the internal dataspace

      \param dataset_id -- Identifier for the group within the HDF5 file
     */
    void init (hid_t const &group_id);
    
    /*!
      \brief Initialize the internal dataspace

      \param filename -- HDF5 file within which the dataset in question is
             contained
      \param group    -- Name of the group which this object is to encapsulate.
     */
    void init (std::string const &filename,
	       std::string const &group);
    
    /*!
      \brief Initialize the internal dataspace

      \param location -- Location below which the group is found within the
             file.
      \param group  -- Name of the group which this object is to encapsulate.
     */
    void init (hid_t const &location,
	       std::string const &group);
    
    /*!
      \brief Unconditional copying

      \param other -- Another LOFAR_StationGroup object from which to create
             this new one.
    */
    void copy (LOFAR_StationGroup const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Set up the list of dipole datasets contained within this group

      Essentially there are two ways by which to construct the list of dipole
      datasets contained within this station group:
      <ol>
        <li>Test if the possible names for the datasets belong to a valid 
	object.
      </ol>

      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool setDipoleDatasets ();

  };
  
} // Namespace DAL -- end

#endif /* LOFAR_STATIONGROUP_H */
  
