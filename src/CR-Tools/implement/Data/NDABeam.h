/***************************************************************************
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

/* $Id: NDABeam.h,v 1.4 2006/10/31 18:24:08 bahren Exp $*/

#ifndef NDABEAM_H
#define NDABEAM_H

#include <casa/aips.h>
#include <casa/Arrays.h>

#include <Data/ITSMetadata.h>
#include <IO/DataReader.h>
#include <Utilities/StringTools.h>

using CR::DataReader;

/*!
  \class NDABeam

  \ingroup CR_Data

  \brief Beamformed NDA waveform data 

  \author Lars B&auml;hren and Andreas Nigl

  \date 2006/05/16

  \test tNDABeam.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[CR] DataReader
    <li>[CR] ITSMetadata
  </ul>

  The data are saved as shorts little endian in a binary file.

  <h3>Synopsis</h3>

  \verbatim
  [OBSERVATION]
  description=NANCAY Jupiter data
  experiment_type=capturing
  basename=jupiter_20051130_03
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
  observation_id=1133333588
  observation_owner=NDA
  submittime=2005-11-30T06:53:15.00Z
  # machineformat=ieee-le
  # precision=int16
  #EOF
  triggertime[1]=2005-11-30T07:15:02.00Z
  file=/data/NDA/jupX3/test3.rd14.10t21
  file=/data/NDA/jupX3/test3.rd14.0t21.org
  \endverbatim

  <h3>Example(s)</h3>

  See the examples section of ITSBeam -- this class works just the same.
*/

class NDABeam : public DataReader {

  //! Information contained in experiment.meta are stored in their own object
  ITSMetadata metadata_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
   */
  NDABeam ();

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
  */
  NDABeam (String const &metafile);

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
  */
  NDABeam (String const &metafile,
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
  NDABeam (String const &metafile,
	   uint const &blocksize,
	   Vector<Double> const &adc2voltage,
	   Matrix<DComplex> const &fft2calfft);
  /*!
    \brief Copy constructor

    \param other -- Another NDABeam object from which to create this new
                    one.
  */
  NDABeam (NDABeam const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~NDABeam ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another NDABeam object from which to make a copy.
  */
  NDABeam& operator= (NDABeam const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Name of the metafile
    
    \return metafile -- Name of the metafile
  */
  String metafile () const {
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
  String directory () const {
    return metadata_p.directory();
  }

  /*!
    \brief Get the names of the data files

    \param fullPath -- Return the full path to each of the data files?

    \return datafiles -- The names of the files, in which the actual data are
                         stored
   */
  Vector<String> datafiles (bool const &fullPath=true) const {
    return metadata_p.datafiles(fullPath);
  }  

  /*!
    \brief Number of the antennas included in the experiment

    \param validDataOnly -- Return antenna for which there exist valid
                            (<i>true</i>) data files.

    \return antennas -- Numbers of the antennas included in the experiment
  */
  Vector<uint> antennas (bool const &validDataOnly=true) const {
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
  void copy (NDABeam const &other);

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
  Bool setStreams ();

};

#endif /* NDABEAM_H */
