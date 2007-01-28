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

/* $Id: ITSCorrelation.h,v 1.3 2006/08/01 15:00:31 bahren Exp $*/

#ifndef ITSCORRELATION_H
#define ITSCORRELATION_H

#include <Data/ITSMetadata.h>
#include <IO/DataReader.h>

using casa::AipsError;
using casa::Complex;
using casa::Cube;
using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using LOPES::DataReader;
using LOPES::ITSMetadata;

namespace LOPES {  // namespace LOPES -- begin

/*!
  \class ITSCorrelation

  \ingroup Data

  \brief Brief description for class ITSCorrelation

  \author Lars Bahren

  \date 2006/05/16

  \test tITSCorrelation.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[start filling in your text here]
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class ITSCorrelation : public DataReader {

  //! Metadata of the experiment
  ITSMetadata metadata_p;

  //! Frequency channels selected in the experiment
  Vector<uint> frequencyChannels_p;

  //! Baselines for the visibility data, [2,nofBaselines]
  Matrix<uint> baselines_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  ITSCorrelation ();

  /*!
    \brief Argumented constructor

    \param metafile -- Name of the experiment metafile
  */
  ITSCorrelation (String const &metafile);

  /*!
    \brief Copy constructor

    \param other -- Another ITSCorrelation object from which to create this new
                    one.
  */
  ITSCorrelation (ITSCorrelation const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~ITSCorrelation ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ITSCorrelation object from which to make a copy.
  */
  ITSCorrelation& operator= (ITSCorrelation const &other); 

  // ----------------------------------------------------------------- Parameters

  

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Get the cross-correlation spectra

    While in the case of raw time series as initial starting point the effect of
    this switch considered at requesting the input for the ccSpectra cube --
    simply either call <tt>fft()</tt> or <tt>calfft()</tt> -- in the case of
    ITSCorrelation data both conversion steps -- <i>adc2voltage</i> and
    <i>fft2calfft</i> need to be applied on the values stored in the data cube
    read in from disk!

    \param fromCalFFT -- Cross-correlation spectra from the calibrated FFT data?
                         If set to <i>False</i>, the cross-correlation is carried
			 out on the raw FFT. 

    \return ccSpectra -- Data cube with the cross-correlation spectra,
                         [nfreq,nant,nant]
  */
  Cube<DComplex> ccSpectra (bool const &fromCalFFT=true);

 private:

  /*!
    \brief Unconditional copying
  */
  void copy (ITSCorrelation const &other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

 protected:
  
  void setMetafile (String const &metafile);

  /*!
    \brief Connect the data streams used for reading in the data

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setStreams ();

};

}  // namespace LOPES -- end

#endif /* ITSCORRELATION_H */
