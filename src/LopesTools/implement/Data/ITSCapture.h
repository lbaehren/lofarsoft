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

/* $Id: ITSCapture.h,v 1.1 2006/07/05 16:22:59 bahren Exp $*/

#ifndef ITSCAPTURE_H
#define ITSCAPTURE_H

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <lopes/Data/ITSMetadata.h>
#include <lopes/IO/DataReader.h>
#include <lopes/Utilities/StringTools.h>

/*!
  \class ITSCapture

  \ingroup Data

  \brief Brief description for class ITSCapture

  \author Lars B&auml;hren

  \date 2006/05/16

  \test tITSCapture.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[LOPES-Tools] DataReader
    <li>[LOPES-Tools] ITSMetadata
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class ITSCapture : public DataReader {

  //! Information contained in experiment.meta are stored in their own object
  ITSMetadata metadata_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
   */
  ITSCapture ();

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
  */
  ITSCapture (String const &metafile);

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
  */
  ITSCapture (String const &metafile,
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
  ITSCapture (String const &metafile,
	      uint const &blocksize,
	      Vector<Float> const &adc2voltage,
	      Matrix<Complex> const &fft2calfft);
  /*!
    \brief Copy constructor

    \param other -- Another ITSCapture object from which to create this new
                    one.
  */
  ITSCapture (ITSCapture const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~ITSCapture ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ITSCapture object from which to make a copy.
  */
  ITSCapture& operator= (ITSCapture const &other); 

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
  Matrix<Float> fx ();

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (ITSCapture const &other);

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

#endif /* ITSCAPTURE_H */
