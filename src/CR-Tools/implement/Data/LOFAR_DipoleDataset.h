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

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>

#include <dal/HDF5Common.h>

namespace DAL { // Namespace DAL -- begin
  
  /*!
    \class LOFAR_DipoleDataset
    
    \ingroup CR_Data
    \ingroup DAL
    
    \brief Container for dipole-based data in a LOFAR TBB time-series dataset
    
    \author Lars B&auml;hren

    \date 2008/01/10

    \test tLOFAR_DipoleDataset.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li><a href="http://www.atnf.csiro.au/computing/software/casacore/classcasa_1_1Record.html">casa::Record</a>
      -- A hierarchical collection of named fields of various types.
      <li>DAL::LOFAR_StationGroup
      <li>DAL::LOFAR_Timeseries
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
    
    //! Identifier for this dataset within the HDF5 file
    hid_t datasetID_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
     
    /*!
      \brief Default constructor
    */
    LOFAR_DipoleDataset ();
    
    /*!
      \brief Argumented constructor

      \param filename -- Name of the HDF5 file within which the dataset is
             contained.
      \param dataset  -- Name of the dataset, in this case the full path from
             the base of the hierarchical structure within the HDF5 file.
    */
    LOFAR_DipoleDataset (std::string const &filename,
			 std::string const &dataset);
    
    /*!
      \brief Argumented constructor

      \param location -- Identifier for the location within the HDF5 file, below
             which the dataset is placed.
      \param dataset  -- Name of the dataset.
    */
    LOFAR_DipoleDataset (hid_t const &location,
			 std::string const &dataset);
    
    /*!
      \brief Argumented constructor

      \param dataset_id -- Identifier for the dataset contained within the HDF5
             file
    */
    LOFAR_DipoleDataset (hid_t const &dataset_id);
    
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
      
      \param other -- Another LOFAR_DipoleDataset object from which to make a
             copy.
    */
    LOFAR_DipoleDataset& operator= (LOFAR_DipoleDataset const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the identifier for this dataset within the HDF5 file

      \return H5datasetID -- The identifier for this dataset within the HDF5 file
    */
    inline hid_t dataset_id () const {
      return datasetID_p;
    }

    /*!
      \brief Get the number of attributes attached to the dataset

      \return nofAttributes -- The number of attributes attached to the dataset;
              if the dataset ID is properly connected to a dataset within the 
	      HDF5 file, the returned value will be zero or greater. If a
	      negative value is returned, most likely te connection with the
	      file is broken.
    */
    int nofAttributes ();

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
    double sample_frequency (std::string const &unit="Hz");

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
    
    /*!
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

    /*!
      \brief Get the antenna orientation w.r.t. to the station reference frame

      \return orientation -- The three Euler angles to specify a possible 
              deviation of the antenna's orientation w.r.t. to the reference
	      frame set for the station.
    */
    casa::Vector<double> antenna_orientation ();

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
      \brief Get the unique channel/dipole identifier
      
      \return channelID -- The unique identifier for a signal channel/dipole
              within the whole LOFAR array; this ID is created from a combination
	      of station ID, RSP ID and RCU ID:
	      \f$ N_{\rm RCU} + 10^{3} \times N_{\rm RSP} + 10^{6} N_{\rm Station} \f$
    */
    int channelID ();

    /*!
      \brief Get the unique channel/dipole identifier
      
      \return channel_id -- The unique identifier for a signal channel/dipole
              within the whole LOFAR array; this ID is created from a combination
	      of station ID, RSP ID and RCU ID.
    */
    std::string channelName ();

    /*!
      \brief Get a number of data values as recorded for this dipole
      
      \param start      -- Number of the sample at which to start reading
      \param nofSamples -- Number of samples to read, starting from the position
             given by <tt>start</tt>.
      \retval data       -- [nofSamples] Array with the raw ADC samples
              representing the electric field strength as function of time.
	      
      \return status -- Status of the operation; returns <tt>false</tt> in case
              an error was encountered.
    */
    bool fx (int const &start,
	     int const &nofSamples,
	     short *data);
    
    /*!
      \brief Get a number of data values as recorded for this dipole

      \param start      -- Number of the sample at which to start reading
      \param nofSamples -- Number of samples to read, starting from the position
             given by <tt>start</tt>.

      \return fx -- [nofSamples] Vector of raw ADC samples representing the 
              electric field strength as function of time.
     */
    casa::Vector<double> fx (int const &start=0,
			     int const &nofSamples=1);

    /*!
      \brief Get a casa::RecordDesc object describing the structure of the record

      \return recDesc -- Record descriptor containing the information on how to
              structure the record as which the attributes attached to the dataset
	      can be retrieved.
    */
    casa::RecordDesc recordDescription ();
    
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

      \param dataset_id -- Identifier for the dataset within the HDF5 file
     */
    void init (hid_t const &dataset_id);
    
    /*!
      \brief Initialize the internal dataspace

      \param filename -- HDF5 file within which the dataset in question is
             contained
      \param dataset  -- Name of the dataset which this object is to encapsulate.
     */
    void init (std::string const &filename,
	       std::string const &dataset);
    
    /*!
      \brief Initialize the internal dataspace

      \param location -- Location below which the dataset is found within the
             file.
      \param dataset  -- Name of the dataset which this object if to encapsulate.
     */
    void init (hid_t const &location,
	       std::string const &dataset);
    
    /*!
      \brief Unconditional copying

      \param other -- Another LOFAR_DipoleDataset object from which to create
             this new one.
    */
    void copy (LOFAR_DipoleDataset const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

  };
  
} // Namespace DAL -- end

#endif /* LOFAR_DIPOLEDATASET_H */
  
