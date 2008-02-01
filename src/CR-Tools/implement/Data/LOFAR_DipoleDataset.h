/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1159 2007-12-21 15:40:14Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#ifndef LOFAR_DIPOLEDATASET_H
#define LOFAR_DIPOLEDATASET_H

#include <string>
#include <Data/HDF5Common.h>

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>

namespace LOFAR { // Namespace LOFAR -- begin
  
  /*!
    \class LOFAR_DipoleDataset
    
    \ingroup LOFAR
    
    \brief Container for dipole-based data in a LOFAR TBB time-series dataset
    
    \author Lars B&auml;hren

    \date 2008/01/10

    \test tLOFAR_DipoleDataset.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[CR] LOFAR_TBB
      <li>[CR] LOFAR_StationGroup
    </ul>
    
    <h3>Synopsis</h3>

    <ol>
      <li>Structure of the HDF5 dataset inside the time-series data format:
      \verbatim
      /                             ... Group
      |-- Station001                ... Group
      |   |-- 
      |   |-- 001000000             ... Dataset         ... array<uint,1>
      |   |   |-- STATION_ID        ... Attribute       ... uint
      |   |   |-- RSP_ID            ... Attribute       ... uint 
      |   |   |-- RCU_ID            ... Attribute       ... uint
      |   |   |-- SAMPLE_FREQ       ... Attribute       ... double
      |   |   |-- TIME              ... Attribute       ... uint
      |   |   |-- SAMPLE_NR         ... Attribute       ... uint
      |   |   |-- SAMPLES_PER_FRAME ... Attribute       ... uint
      |   |   |-- DATA_LENGTH       ... Attribute       ... uint
      |   |   |-- NYQUIST_ZONE      ... Attribute       ... uint
      |   |   |-- FEED              ... Attribute       ... string
      |   |   |-- ANT_POSITION      ... Attribute       ... array<double,1>
      |   |   `-- ANT_ORIENTATION   ... Attribute       ... array<double,1>
      \endverbatim
    </ol>
    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_DipoleDataset {
    
    //! HDF5 file handle ID
    hid_t H5fileID_p;
    //! Identifier for this dataset within the HDF5 file
    hid_t H5datasetID_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LOFAR_DipoleDataset ();
    
    /*!
      \brief Argumented constructor

      \param fileID -- HDF5 file handle ID
      \param datasetID -- Identifier for this dataset within the HDF5 file
    */
    LOFAR_DipoleDataset (hid_t const &fileID,
			 hid_t const &datasetID);
    
    /*!
      \brief Argumented constructor

      \param fileID -- HDF5 file handle ID
      \param datasetName -- Path/Absolute name of the dataset within the HDF5
             file
    */
    LOFAR_DipoleDataset (hid_t const &fileID,
			 std::string const &datasetName);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_DipoleDataset object from which to create
             this new one.
    */
    LOFAR_DipoleDataset (LOFAR_DipoleDataset const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_DipoleDataset ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_DipoleDataset object from which to make a copy.
    */
    LOFAR_DipoleDataset& operator= (LOFAR_DipoleDataset const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the HDF5 file handle ID

      \return H5fileID -- HDF5 file handle ID
    */
    inline hid_t file_id () const {
      return H5fileID_p;
    }

    /*!
      \brief Get the identifier for this dataset within the HDF5 file

      \return H5datasetID -- The identifier for this dataset within the HDF5 file
    */
    inline hid_t dataset_id () const {
      return H5datasetID_p;
    }

    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_DipoleDataset.
    */
    std::string className () const {
      return "LOFAR_DipoleDataset";
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
      \brief Get the internal HDF5 intentifier for an attribute to the dataset

      \param name -- The name of the attribute attached to the dataset.

      \return attribute_id -- The internal HDF5 identifier of the attribute;
              returns 0 in case no attribute of the given name can be found.
    */
    hid_t attribute_id (std::string const &name);

    /*!
      \brief Get the ID of the LOFAR station this dipole belongs to

      \return STATION_ID -- ID of the LOFAR station this dipole belongs to
    */
    uint station_id ();

    /*!
      \brief Get the ID of the RSP board this dipole is connected with

      \return RSP_ID -- ID of the Remote Station Processing board (RSP) this
              dipole is connected with
    */
    uint rsp_id ();

    /*!
      \brief Get the ID of the receiver unit (RCU) this dipole is connected with

      \return RCU_ID -- ID of the receiver unit (RCU) this dipole is connected with
    */
    uint rcu_id ();

    /*!
      \brief Get the sampling frequency of the analog-to-digital conversion

      \return SAMPLE_FREQ -- The sampling frequency of the analog-to-digital
              conversion; [Hz]
    */
    double sample_frequency ();

    /*!
      \brief Get the Nyquist zone in which the ADC is performed

      \return NYQUIST_ZONE -- The Nyquist zone in which the analog-to-digital
              conversion (ADC) is performed
    */
    uint nyquist_zone ();

    /*!
      \brief Get the (UNIX) time at which the data were recorded

      \return time -- The (UNIX) time at which the data were recorded.
     */
    uint time ();

    /*!
      \brief Get the timespan in samples since the last full second

      \return sample_number -- The timespan in samples since the last full
              second, as stored in <i>time</i>; the absolute time for this 
	      dataset thus is obtained by adding
	      <i>sample_number</i>/<i>sample_frequency</i> to the value of 
	      <i>time</i>.
    */
    uint sample_number ();

    /*!
      \brief The number of samples per original TBB-RSP frame

      \return samplesPerFrame -- The number of samples originally transmitted 
              per frame of data sent from TBB to RSP.
    */
    uint samples_per_frame ();

    /*
      \brief Get the number of samples stored in this dataset

      \return dataLength -- The number of samples stored in this dataset; this
              corresponds to the maximum blocksize, which can be set for this
	      dataset.
    */
    uint data_length ();

    /*!
      \brief Get the type of feed for this dipole

      \return feed -- The type of antenna feed of this dipole
    */
    std::string feed ();

    /*!
      \brief Get the antenna position w.r.t. the center of the station

      \return position -- The three-dimension antenna position w.r.t. to the 
              absolute reference position (typically the center of the station).
    */
    casa::Vector<double> antenna_position ();

    casa::Vector<double> antenna_orientation ();

    /*!
      \brief Get a casa::Record containing the values of the attributes

      \return record -- A casa::Record container holding the values of the 
              attributes attached to the dataset for this dipole
    */
    casa::Record attributes2record ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (LOFAR_DipoleDataset const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Get the value of an attribute

      \retval value -- The value of the attribute
      \param name -- The name the attribute
      \param locationID -- HDF5 identifier of the structure to which the
             attribute is attached

      \return status -- Status of the operation; returns <tt>false</tt> in 
              case an error was encountered
    */
    bool get_attribute (std::string &value,
			std::string const &name,
			hid_t const &locationID);
    bool get_attribute (std::vector<uint> &value,
			std::string const &name,
			hid_t const &locationID);
    bool get_attribute (std::vector<double> &value,
			std::string const &name,
			hid_t const &locationID);
    bool get_attribute (std::vector<std::string> &value,
			std::string const &name,
			hid_t const &locationID);
    
  };
  
} // Namespace LOFAR -- end

#endif /* LOFAR_DIPOLEDATASET_H */
  
