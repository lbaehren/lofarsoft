/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *   Andreas Nigl (anigl@astron.nl)                                        *
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

#ifndef ITS_BEAM_H
#define ITS_BEAM_H

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <Data/ITSMetadata.h>
#include <IO/DataReader.h>
#include <Utilities/StringTools.h>

using CR::DataReader;


/*!
  \class ITS_Beam

  \ingroup CR_Data

  \brief Beamformed ITS waveform data 

  \author Lars B&auml;hren and Andreas Nigl

  \date 2006/08/04

  \test tITS_Beam.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[CR] DataReader
    <li>[CR] ITSMetadata
  </ul>

  <h3>Synopsis</h3>

  The data are saved as float32 little endian in a binary file (on a typical
  Linux machine with x86 architecture).

  As for the ITSCapture class an <i>experiment.meta</i> must be provided along
  with the data (the example below is from <tt>/data/ITS/jupX9tb</tt>):
  \verbatim
  [OBSERVATION]
  description=NDA-ITS observations of Jupiter - ITS beam
  experiment_type=capturing
  basename=jupiter_20051130_09
  starttime=Fri Jan  1 12:00:00 2010
  interval=0
  capturemode=SYNCSTOP
  capturesize=S16_512M
  antennas=0 1
  signextension=true
  skipcapture=false
  iterations=1
  current_status=scheduled
  current_iteration=0
  observation_id=1133341829
  observation_owner=ITS observer
  submittime=2005-11-30T09:10:47.00Z
  # machineformat=ieee-le
  # precision=float32
  #EOF
  triggertime[1]=2005-11-30T09:15:23.30Z
  file=/data/ITS/jupX9tb/jupX9tbE.dat
  file=/data/ITS/jupX9tb/jupX9tbN.dat
  \endverbatim

  <b>Notes:</b>
  
  <ol>
    <li>Use the <tt>experiment.meta</tt> file of the original ITS experiment as
    starting point for the metafile provided with the beamformed data; this not only
    will reduce the amount of required editing but also of possible errors.
    <li>To reduce the risk of runtime errors, provide the absolute path for the
    data files.
  </ol>
  
  <h3>Example(s)</h3>

  <ol>
    <li>Direct usage of an ITS_Beam object in application code:
    \code
    uint blocksize (8192);
    String metafile ("/data/ITS/jupX9tb/experiment.meta");
    
    ITS_Beam beam (metafile, blocksize);
    
    Matrix<Double> fx = beam.fx();
    Matrix<DComplex> fft = beam.fft();
    \endcode
    <li>Indirect use via a pointer to a DataReader object:
    \code
    uint blocksize (8192);
    String metafile ("/data/ITS/jupX9tb/experiment.meta");

    DataReader *dr;
    ITS_Beam *beam = new ITS_Beam (metafile, blocksize);
    
    dr = beam;
    
    Matrix<Double> fx = beam->fx();
    Matrix<DComplex> fft = beam->fft();
    \endcode
  </ol>
*/

class ITS_Beam : public DataReader {

  //! Information contained in experiment.meta are stored in their own object
  ITSMetadata metadata_p;

  //! Type as which the data are stored in the data file
  float datatype_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
   */
  ITS_Beam ();

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
  */
  ITS_Beam (String const &metafile);

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
  */
  ITS_Beam (String const &metafile,
	    uint const &blocksize);
  
  /*!
    \brief Argumented constructor

    \param metafile    -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  ITS_Beam (String const &metafile,
	    uint const &blocksize,
	    Vector<Double> const &adc2voltage,
	    Matrix<DComplex> const &fft2calfft);
  /*!
    \brief Copy constructor

    \param other -- Another ITS_Beam object from which to create this new
                    one.
  */
  ITS_Beam (ITS_Beam const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~ITS_Beam ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ITS_Beam object from which to make a copy.
  */
  ITS_Beam& operator= (ITS_Beam const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Name of the metafile
    
    \return metafile -- Name of the metafile
  */
  inline String metafile () const {
    return metadata_p.metafile();
  }
  
  /*!
    \brief Set the name of the metafile
    
    \param metafile -- Full path to the metafile of the data set; the string 
                       internally will be decomposed into filename and directory
		       path -- the resulting substrings are stored in separate
		       variables.
  */
  void setMetafile (String const &metafile);

  /*!
    \brief Get the to the directory in which the metafile and the datafiles are

    \return directory -- Path to the directory in which the metafile and the
                         datafiles are
  */
  inline String directory () const {
    return metadata_p.directory();
  }

  /*!
    \brief Get the names of the data files

    \param fullPath -- Return the full path to each of the data files?

    \return datafiles -- The names of the files, in which the actual data are
                         stored
   */
  inline Vector<String> datafiles (bool const &fullPath=true) const {
    return metadata_p.datafiles(fullPath);
  }  

  /*!
    \brief Number of the antennas included in the experiment

    \param validDataOnly -- Return antenna for which there exist valid
                            (<i>true</i>) data files.

    \return antennas -- Numbers of the antennas included in the experiment
  */
  inline Vector<uint> antennas (bool const &validDataOnly=true) const {
    return metadata_p.antennas (validDataOnly);
  }

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Get the raw time series after ADC conversion
    
    \return fx -- Raw ADC time series, [Counts]
  */
  Matrix<Double> fx ();

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (ITS_Beam const &other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

 protected:

  /*!
    \brief Connect the data streams used for reading in the data

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setStreams ();

  /*!
    \brief Set the record collecting header information.
    
    \return status -- Status of the operation; returns <tt>false</tt> in case an
    error was encountered.
  */
  bool setHeaderRecord () { return true; };
  
};

#endif /* ITS_BEAM_H */
