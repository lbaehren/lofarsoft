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

#ifndef ITS_CAPTURE_H
#define ITS_CAPTURE_H

#include <casa/aips.h>
#include <casa/Arrays.h>

#include <Data/ITSMetadata.h>
#include <IO/DataReader.h>
#include <Utilities/StringTools.h>

using CR::DataReader;

/*!
  \class ITS_Capture

  \ingroup CR_Data

  \brief Brief description for class ITS_Capture

  \author Lars B&auml;hren

  \date 2006/05/16

  \test tITS_Capture.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[CR] DataReader
    <li>[CR] ITSMetadata
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class ITS_Capture : public DataReader {

  //! Information contained in experiment.meta are stored in their own object
  ITSMetadata metadata_p;

  //! Type as which the data are stored in the data file
  short datatype_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
   */
  ITS_Capture ();

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
  */
  ITS_Capture (String const &metafile);

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
  */
  ITS_Capture (String const &metafile,
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
  ITS_Capture (String const &metafile,
	       uint const &blocksize,
	       Vector<Double> const &adc2voltage,
	       Matrix<DComplex> const &fft2calfft);
  /*!
    \brief Copy constructor

    \param other -- Another ITS_Capture object from which to create this new
                    one.
  */
  ITS_Capture (ITS_Capture const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~ITS_Capture ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ITS_Capture object from which to make a copy.
  */
  ITS_Capture& operator= (ITS_Capture const &other); 

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
  void fx (Matrix<Double> &data);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (ITS_Capture const &other);

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

#endif /* ITSCAPTURE_H */
