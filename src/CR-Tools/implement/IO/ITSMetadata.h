/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id$*/

#ifndef ITSMETADATA_H
#define ITSMETADATA_H

#include <stdlib.h>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/iostream.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>

// LOPES-Tools header files
#include <Utilities/StringTools.h>

#include <casa/namespace.h>

/*!
  \class ITSMetadata

  \ingroup CR
  \ingroup IO

  \brief Storage of meta information from an ITS experiment

  \author Lars B&auml;hren

  \date 2006/02/15

  \test tITSMetadata.cc

  <h3>Prerequisite</h3>

  This implementation merges and replaces the functionality of the classes 
  ObservationMeta and [???]

  <h3>Synopsis</h3>

  The basic format of the <i>experiment.meta</i> file as produced at an LOFAR ITS
  observation is independent on the specific design of the experiment. Therefore
  ist makes sense separate reading of the metadata from reading of the actual
  data; the latter is handled by the class ITSCapture and ITSCorrelation.

  Depending on the type of experiment nevertheless there are subtle differences
  in the set of keyword/value combinations found in the metafile:
  <ul>
    <li>In capture mode raw time series ADC data are recorded, therefore no
    processing plugins are activated.
    \verbatim
    [OBSERVATION]
    description=NDA-ITS observations of Jupiter
    experiment_type=capturing
    basename=jupiter_20060130_2
    starttime=Fri Jan  1 12:00:00 2010
    interval=0
    capturemode=SYNCSTOP
    capturesize=S16_512M
    antennas=0 1 2 3 4 ... 58 59 
    signextension=true
    skipcapture=false
    iterations=1
    current_status=scheduled
    current_iteration=0
    observation_id=1138600386
    observation_owner=ERTC
    submittime=2006-01-30T05:53:16.00Z
    # machineformat=ieee-le
    # precision=int16
    #EOF
    triggertime[1]=2006-01-30T06:03:01.04Z
    file=/var/opt/aviary/dat/jupiter_20060130_2/e01.i0001.dat
    file=/var/opt/aviary/dat/jupiter_20060130_2/e02.i0001.dat
    file=/var/opt/aviary/dat/jupiter_20060130_2/e03.i0001.dat
    file=/var/opt/aviary/dat/jupiter_20060130_2/e04.i0001.dat
    \endverbatim
    <li>In the case of a correlation experiment, two separate plugins are
    activated: the <tt>fftPlugin</tt> running on each of the data aquisition PCs
    and the <tt>correlatorPlugin</tt> running on the central machine (as
    concentrator process).
    \verbatim
    [OBSERVATION]
    description=Monitoring of system stability
    experiment_type=processing
    plugin1=fftPlugin -c 8192 -e 1550:3900 -d --window=hanning
    plugin2=correlatorPlugin -c 2351 -i 255
    basename=monitoring_20050217_512
    starttime=now
    interval=0
    capturemode=SYNCSTART
    capturesize=S16_2M
    antennas=0 1 2 3 4 ... 58 59 
    signextension=true
    skipcapture=false
    iterations=1
    current_status=scheduled
    current_iteration=0
    observation_id=1108671001
    observation_owner=ERTC
    submittime=2005-02-17T22:30:42.00Z
    # machineformat=ieee-le
    # precision=int16
    #EOF
    triggertime[1]=2005-02-17T23:40:08.61Z
    file=/var/opt/aviary/dat/monitoring_20050217_512/i0001.dat
    \endverbatim
  </ul>
  
  <h3>Example(s)</h3>

*/

class ITSMetadata {

  // ---------------------------------------------------------------- public data

 public:

  /*!
    \brief Capture mode 

    This controls the start of capturing and recording data.
  */
  enum CaptureMode {
    //! Start recording at a given time
    SYNCSTART,
    //! Start recording when a trigger signal is received
    SYNSTOP,
    //! Start recording immediately
    IMMEDIATE
  };

  /*!
    \brief Capture size [samples]

    Keep in mind, that the total memory available on the Tim-board is 1GB; thus 
    if you want to use of the maximum time interval that can be capture, only one
    of the two antennas connected to the board can be selected in the experiment.
  */
  enum CaptureSize {
    S16_16,
    S16_32,
    S16_128,
    S16_1K,
    S16_2K,
    S16_16K,
    S16_32K,
    S16_512K,
    S16_1M,
    S16_2M,
    S16_16M,
    S16_32M,
    S16_128M,
    S16_256M,
    S16_512M,
    S16_1G
  };

  /*!
    \brief Current status of the experiment
  */
  enum CurrentStatus {
    //! The experiment is scheduled and will be carried out somewhen in the future
    scheduled,
    //! The data are being recorded from the TIM-boards
    recording,
    //! The experiment has been completed
    completed
  };

  // --------------------------------------------------------------- private data

 private:

  //! Fully specified path to the experiment metafile
  String metafile_p;
  //! Directory in which the experiment meta file is located
  String directory_p;

  //! Description of the experiment
  String description_p;
  //! Type of experiment (processing, capture)
  String experimentType_p;
  //! Plugin 1 (running on the capture PCs)
  String plugin1_p;
  //! Plugin 2 (running on the central machine)
  String plugin2_p;
  //! Base name for the experiment data directory
  String basename_p;
  //! Time for which the experiment was scheduled to start
  String starttime_p;
  //! Interval between two subsequent data captures [sec]
  Int interval_p;
  //! The type of capture mode
  CaptureMode captureMode_p;
  //! The size of the captured data [samples]
  CaptureSize captureSize_p;
  //! Number of the antennas included in the experiment
  Vector<uint> antennas_p;
  //! Use sign extension for the recorded data?
  Bool signExtension_p;
  //!
  Bool skipcapture_p;
  //!
  Int iterations_p;
  //!
  CurrentStatus currentStatus_p;
  //!
  Int currentIteration_p;
  //!
  Long observationId_p;
  //!
  String observationOwner_p;
  
  //! Name of the datafiles associated with the experiment/observation
  Vector<String> datafiles_p;
  //! Keep track wether or not valid data are associated with an antenna
  Vector<Bool> antennaHasValidData_p;

  // ------------------------------------------------------------- protected data

 protected:

  //! Status (should be set False, if there is something from with the data)
  Bool isOk_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor

    This will just construct an object, but will not do anything further (as this
    requires an experiment meta file to open).
   */
  ITSMetadata ();

  /*!
    \brief Argumented constructor
    
    \param metafile -- Fully specified path to the experiment metafile
  */
  ITSMetadata (String const &metafile);

  /*!
    \brief Copy constructor

    \param other -- Another ITSMetadata object from which to create this new
                    one.
  */
  ITSMetadata (ITSMetadata const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~ITSMetadata ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ITSMetadata object from which to make a copy.
  */
  ITSMetadata &operator= (ITSMetadata const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the path to the metafile
    
    \return metafile -- Fully specified path to the experiment metafile
  */
  String metafile () const {
    return metafile_p;
  }
  
  /*!
    \brief Set the path to the metafile
    
    \param metafile -- Fully specified path to the experiment metafile
  */
  void setMetafile (String const &metafile);

  /*!
    \brief Get the name of the directory in which the data are stored
    
    \return directory -- The directory in which the data are stored
  */
  String directory () const {
    return directory_p;
  }
  
  /*!
    \brief Name of the datafiles associated with the experiment/observation

    \param fullPath -- Return the full path to each of the data files?
  */
  Vector<String> datafiles (bool const &fullPath=true) const;

  /*!
    \brief Get the description of the experiment
    
    \return description -- A short description of the experiment
  */
  String description () const {
    return description_p;
  }

  //! Type of experiment (processing, capture)
  String experimentType () const {
    return experimentType_p;
  }

  //! Plugin 1 (running on the capture PCs)
  String plugin1 () const {
    return plugin1_p;
  }

  //! Plugin 2 (running on the central machine)
  String plugin2 () const {
    return plugin2_p;
  }

  //! Base name for the experiment data directory
  String basename () {
    return basename_p;
  }

  //! Time for which the experiment was scheduled to start
  String starttime () const {
    return starttime_p;
  }

  //! Interval between two subsequent data captures [sec]
  Int interval () const {
    return interval_p;
  }

  //! Number of subsequent iterations
  Int iterations () const {
    return iterations_p;
  }

  //! The type of capture mode
  CaptureMode captureMode () const {
    return captureMode_p;
  }

  //! The size of the captured data [samples]
  CaptureSize captureSize () const {
    return captureSize_p;
  }
  
  /*!
    \brief Number of the antennas included in the experiment

    \param validDataOnly -- Return antenna for which there exist valid
                            (<i>true</i>) data files.

    \return antennas -- Numbers of the antennas included in the experiment
  */
  Vector<uint> antennas (bool const &validDataOnly=true) const;

  /*!
    \brief Are valid data associated with this antenna?

    \param antenna -- Number of the antenna

    \return hasValidData -- Returns <i>false</i> if no valid data file is
            associated with the provided antenna number
  */
  Bool hasValidData (uint const &antenna) const;

  /*!
    \brief Are valid data associated with the antenna?

    \return hasValidData -- Returns <i>false</i> if no valid data file is
            associated with the antenna number
  */
  Vector<Bool> hasValidData () const {
    return antennaHasValidData_p;
  }
  
  //! Use sign extension for the recorded data?
  Bool signExtension () const {
    return signExtension_p;
  }

  /*!
    \brief Get the ID of the observation

    \return observationId -- ID of the observation
  */
  Long observationId () const {
    return observationId_p;
  }

  /*!
    \brief Get the name of the observation owner

    \return observationOwner -- Name of the observation owner
  */
  String observationOwner () const {
    return observationOwner_p;
  }

  // -------------------------------------------------------------------- Methods



 private:

  /*!
    \brief Initialize set of parameters
  */
  void init ();

  /*!
    \brief Unconditional copying
  */
  void copy (ITSMetadata const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Read in the data from the experiment meta file

    This will try to open the meta file for reading; if all checks are we can
    can continue to parse the contents of the file. As the contents is organized 
    as keyword/keyvalue combinations, we extract these pairs an store them in a
    Record.

  */
  void readMetafile ();

  /*!
    \brief Store the names of the datafiles

    Process the information extracted from the metafile and arrange it in such a
    way, that it can be used for further processing. For the names of the data
    files this means, that the names collected in a single string need to be
    decomposed into a vector of strings, such that we easily can access individual
    elements.

    \param filenames -- Name of the datafiles associated with the observation

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
   */
  bool setDatafiles (String const &filenames);

  /*!
    \brief Check if the listed datafiles really exist and/or are valid

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool checkDatafiles ();

  /*!
    \brief Store the numbers of the antennas participating in the observation

    \param antennas -- The numbers of the antennas participating in the
                       observation

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setAntennas (String const &antennas);

  string adjustPathToMetafile (string const &path);

};

#endif /* ITSMETADATA_H */
