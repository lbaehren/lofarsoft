/***************************************************************************
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

#ifndef OBSERVATIONMETA_H
#define OBSERVATIONMETA_H

/* $Id: ObservationMeta.h,v 1.5 2006/07/05 13:10:22 bahren Exp $ */

// C++ Standard library
#include <ostream>
#include <fstream>
#include <sstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/sstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>
#include <casa/Quanta/MVEpoch.h>
#include <casa/Quanta/MVTime.h>
#include <casa/IO/AipsIO.h>
#include <casa/BasicMath/Math.h>
#include <measures/Measures.h>
#include <measures/Measures/MEpoch.h>
#include <tasking/Glish.h>

// LOPES-Tools header files
#include <Utilities/StringTools.h>

#define LOPESMETA "lopeshdr.hdr"

using casa::AipsError;
using casa::GlishArray;
using casa::GlishRecord;
using casa::IPosition;
using casa::Matrix;
using casa::MVEpoch;
using casa::MVTime;
using casa::MEpoch;
using casa::Quantity;
using casa::String;
using casa::Time;
using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class ObservationMeta
    
    \ingroup Observation
    
    \brief A class for storage of metadata to an ITS experiment
    
    \author Lars B&auml;hren
    
    <h3>Prerequisites</h3>
    
    <ul type="square">
    <li>[AVIARY] Observation.h: An describing a single observation
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Glish/GlishRecord.html">GlishRecord</a> -- holds a Glish record
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Quanta.html">Quanta</a> -- a module for units and quantities
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Measures.html">Measures</a> -- quantities with a reference frame
    </ul>
    
    \test
    - tObservationMeta.cc
    
    <h3>Synopsis</h3>
    
    Observations at the ITS are scheduled and controlled by the AVIARY software,
    which through the plugin mechanism allows processing of the captured data
    as part of the experiment.
    
    <h3>AVIARY plugins</h3>
    
    \verbatim
    [OBSERVATION]
    description=Data for deep integration with ITS
    experiment_type=processing
    plugin1=fftPlugin -c 8192 -e 1050:3900 -d --window=hanning
    plugin2=correlatorPlugin -c 2851 -i 65535
    basename=acm20040924-1
    starttime=now
    interval=0
    capturemode=SYNCSTART
    capturesize=S16_512M
    antennas=0 1 2 3 4 [....] 57 58 59 
    signextension=true
    skipcapture=false
    iterations=100
    current_status=scheduled
    current_iteration=0
    observation_id=1096019104
    observation_owner=
    submittime=2004-09-24T12:21:42.00Z
    # machineformat=ieee-le
    # precision=int16
    \endverbatim
    
    <h3>Antenna positions</h3>
    
    The antenna positions are expected to be stored as a simple ASCII table:
    \verbatim
    nrows  ncols
    pos[1,1]      pos[1,2]      ...  pos[1,ncols]
    .             .                  .
    .             .                  .
    .             .                  .
    pos[nrows,1]  pos[nrows,2]  ...  pos[nrows,ncols]
    \endverbatim
    
    If generating a metafile as used by the LOPES Tools per antenna the following
    section will be present in the metafile:
    \verbatim
    #ITSfile:  /var/opt/aviary/dat/20040916-testdata/e01.i0001.dat
    #AntPos: 18.7287 , -14.0166 , 0
    #presync:  0
    #AntNum: 1
    #AntName: 1
    #Files: e01.i0001.dat
    \endverbatim
    
    <i>Important note:</i> While the contents of the antenna position file is
    organized as \f$ (x,y,z) \f$, the LOPES Tools header file lists the antenna
    positions as \f$ (y,-x,z) \f$.
  */
  
  class ObservationMeta {
    
    String filename_;      // location where the metafile is stored on disk
    String directory_;     // 
    String owner_;         // 
    String basename_;      // 
    String starttime_;     // 
    String submittime_;     // 
    String description_;   // 
    Vector<int> antennas_; // Set of array elements used in the observation
    String antposFile_;    // location of the antenna position file
    Matrix<double> antennaPositions_;
    int iterations_;       // 
    int interval_;         // 
    String capturemode_;   // 
    String capturesize_;   // 
    String type_;          // 
    String plugin1_;       // 
    String plugin2_;       // 
    bool signextension_;   // 
    bool skipcapture_;           // 
    int observationID_;          //
    Vector<String> triggertime_; //
    Matrix<String> datafiles_;   //
    
  public:

  // === Construction / Destruction ============================================

  /*!
    \brief Argumented constructor

    \param filename - Location where the metafile is stored on disk.
  */
  ObservationMeta (String);

  /*!
    \brief Argumented constructor

    \param metaFile   - Location where the metafile is stored on disk.
    \param antposFile - Location of the antenna position file.
  */
  ObservationMeta (String,
		   String);

  /*!
    \brief Argumented constructor

    \param filename - Location where the metafile is stored on disk.
    \param datarec  - Record containing fields for data members.
  */
  ObservationMeta (String,
		   GlishRecord&);

  //! Destructor
  ~ObservationMeta ();

  // === Experiment metafile =========================================

  /*!
    \brief Get the location where the metafile is stored on disk.

    Note: This returns the full path to the file.
  */
  String metafile ();
  
  //! Get the directory in which the metafile is contained
  String directory () {
    return directory_;
  }

  /*!
    \brief Set member data from Glish record
  */
  void metadata (GlishRecord&);

  // === Data I/O ====================================================

  /*!
    \brief Read in the contents of a metafile
    
    \param filename - Location where the metafile is stored on disk.

    \todo Guard against empty lines before reaching EOF.
  */
  bool readMetafile (String);

  /*!
    \brief Read in the contents of the metafile

    Open and parse an existing metafile; the contents of the file - i.e. the
    keyword/value structure - is copied into a GlishRecord and then written to
    internal storage via the ObservationMeta::metadata method.
  */
  bool readMetafile ();

  /*!
    \brief Export meta information in LOPES header format
  */
  void lopesheader ();
  void lopesheader (std::ostream&);

  // === Access to member data =================================================

  /*!
    \brief Obtain member data as Glish record.
  */
  GlishRecord metadata ();

  /*!
    \brief Show the member data on the defined output stream.
   */
  void metadata (std::ostream&);

  // === Timing information ==========================================

  // (a) Time at which the experiment was submitted to the queue
  
  /*!
    \brief Set the time at which the experiment was submitted to the queue

    This time records when the experiment specification was received by 
    experiment control framework,
    \verbatim
    submittime=2004-09-24T12:21:42.00Z
    \endverbatim
    The information is stored in a string according to the ISO 8601 standard.
  */
  void submittime (String);
  //! Get the time at which the experiment was started
  String submittime ();
  //! Get the experiment submittime as Quantity.
  void submittime (Quantity&);
  //! Get the experiment submittime as Measure.
  void submittime (MEpoch&);

// (b) Time at which the experiment is to be started.

  /*!
    \brief Set the time at which the experiment was started
    
    Time defined by the observer for when the experiment is about to be 
    started; possible formats are
    \verbatim
    starttime=now

    starttime=2004/11/07 06:00:00
    \endverbatim
  */
  void starttime (String);
  //! Get the time at which the experiment was started
  String starttime ();

// (c) Time(s) at which data where captured

  //! Set the time(s) at which data where captured
  void triggertime (const Vector<String>&);
  //! Get the time(s) at which data where captured
  Vector<String> triggertime ();
  //! Get the experiment triggertime(s) as Quanta
  void triggertime (Vector<Quantity>&);
  //! Get the experiment triggertime(s) as Measure(s)
  void triggertime (Vector<MEpoch>&);


  // === Data generated during the experiment ========================

  void observationID (const int);
  
  int observationID ();

  /*!
    \brief Get the list of datafiles generated during the experiment

    In a successfully completed experiment \f$ N_{\rm iter} \times N_{\rm ant} \f$
    files of binary data will be generated; here \f$ N_{\rm ant} \f$ is the
    number of array element (antennae) and \f$ N_{\rm iter} \f$ the number
    of iteration (i.e. repititions of an experiment). Depending on the setup
    of the experiment, different sets of datafiles will be produced:
    <ol>
      <li>In <i>capture mode</i> the digitized time-series data for each antenna
      are written to disk; this will result in \f$ N_{\rm ant} \f$ files per
      iteration.
      <li>Beamforming or cross-correlation on the concentrator at
      <i>processing mode</i> will produce a single file per iteration.
    </ol>
   */
  Matrix<String> datafiles ();
  
  // === Misc. meta data =============================================

  //! Set the name of the experiment owner
  void owner (String);

  //! Get the name of the experiment owner
  String owner () const {
    return owner_;
  }

  //! Set the basename of the data directory
  void basename (String);

  //! Get the basename of the data directory
  String basename ();

  //! Set the contextual description of the experiment.
  void description (String);
  //! Get the contextual description of the experiment.
  String description () const {
    return description_;
  }

  //! Set the set of selected array elements
  void antennas (Vector<int>&);
  //! Get the set of selected array elements
  Vector<int> antennas ();
  /*!
    \brief Get the set of selected array elements
    
    \param offset - Offset of the vector elements; for a C-type numbering 
                    elements are zero-based, while for a Glish-type array the
		    values are one-based. The value of the offset is added to
		    the array elements before returning the vector.
  */
  Vector<int> antennas (int);

  /*!
    \brief Read in the 3-dim positions of the array elements
    
    \param filename - File containing ASCII table of the 3-dim antenna
                      positions.
     */
  void setAntennaPositions (String);

  //! Return the 3-dim positions of the array elements
  Matrix<double> antennaPositions ();

  /*!
    \brief Set the number of experiment repititions
    
    \param iterations -- The number of experiment repititions
  */
  void iterations (int iterations);

  /*!
    \brief Get the number of experiment repititions
    
    \return iterations -- The number of experiment repititions
  */
  int iterations () const {
    return iterations_;
  }

  //! Set the interval between two subsequent experiment iterations, [sec].
  void interval (int);
  //! Get the interval between two subsequent experiment iterations, [sec].
  int interval ();

  //! Set the capture mode of the experiment.
  void capturemode (String);
  //! Get the capture mode of the experiment.
  String capturemode () const {
    return capturemode_;
  }

  void capturesize (String);
  String capturesize ();
  
  void type (String);
  String type ();

  void signextension (bool);
  void signextension (String);
  bool signextension ();

  void skipcapture (bool);
  void skipcapture (String);
  bool skipcapture ();

  // === AVIARY plugins ========================================================

  //! Set the plugin option strings
  void plugins (Vector<String>&);

  //! Set the plugin options string
  void plugin1 (String plugin1);

  /*!
    \brief Get the parameter string for the first plugin

    \return Parameter string for the first plugin
  */
  String plugin1 () {
    return plugin1_;
  } 
  
  //! Set the plugin options string
  void plugin2 (String plugin2);

  /*!
    \brief Get the parameter string for the second plugin

    \return Parameter string for the second plugin
  */
  String plugin2 () {
    return plugin2_;
  }
  
 private:

  //! Path to the experiment metafile used as input
  void setMetafile (String);
  //! Set the list of datafiles generated during the experiment
  void setDatafiles (const Matrix<String>&);
  //! Read in the 3dim positions of the array elements
    bool readAntennaPositions ();
  //! Parse the plugin options string
  void parsePluginOptions (String);
  //! Convert a string to a boolean variable
  bool string2boolean (String);
  //! Convert a string to a vector of integers
  Vector<int> string2vector (String);
  //! Extract the date from a time string
  void extractFromISO8601 (String&,
			   String&,
			   const String);
  //! Convert the ISO 8601 time string to a measure
  Time iso8601ToTime (const String);
  //! Extract the name of the data file from its complete path
  String nameDatafile (int, int);

};

}  // Namespace LOPES -- end

#endif
