/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Andreas Nigl (anigl@astro.ru.nl                                       *
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

/* $Id: timebeam.h,v 1.7 2007/03/19 12:29:46 bahren Exp $ */

/*!
  \file timebeam.h

  \ingroup CR_Applications
  
  \brief Generation of a timebeam
  
  \author Andreas Nigl
 */

// casa wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/stdio.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Map.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>

#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>

#include <scimath/Mathematics.h>
#include <tasking/Glish.h>
#include <tasking/Glish/GlishRecord.h>
#include <wcslib/prj.h>

// custom header files
#include <ApplicationSupport/Beamformer.h>
#include <ApplicationSupport/DataReaderTools.h>
#include <IO/DataReader.h>
#include <Math.h>
#include <Observation/ObservationData.h>
#include <Utilities/StringTools.h>

// Namespace usage

#include <casa/namespace.h>

using LOPES::Beamformer;

// -------------------------------------------------------------- Data structures

// data structure
typedef struct {
  Int fileTypeID;
  //! Method to be used computing the dynamic spectrum.
  String dsMethod;
  //! Name of the telecope
  String telescope;
  //! Name of the observer 
  String observer;
  //! Previous computed spectrum; buffered for comparison.
  Vector<Double> lastSpectrum;
  //! Currently analyzed data block
  Int nBlock;
  //! Generate and display additional statistics?
  Bool statistics;
  //! Compute differential dynamic spectrum?
  Bool differential;
  // Instance of the Beamformer class
  Beamformer bf;
} ClientData;

// ----------------------------------------------------- Event handling functions

/*!
  \brief Handling of the 'timebeam' Glish-Event
*/
Bool timebeam (GlishSysEvent &event,
	       void *);

// -------------------------------------------------------- Computation functions

/*!
  \brief Apply a hanning window to the frequency data

  \param freqblock -- A vector of frequency data; modified on return.
*/
void applyHammingWindow (Vector<DComplex>& freqblock);

/*!
  \brief Apply a hanning window to the frequency data

  \param freqblock -- A vector of frequency data; modified on return.
  \param alpha -- 0.5 = Hanning Window & 0.54 = Hamming Window
*/
void applyHanningWindow (Vector<DComplex>& freqblock,
			 double& alpha);

/*!
  \brief Inverse Fourier transform back to time domain

  \param fft     -- Data in the frequency domain
  \param fftsize -- Input blocksize of the forward FFT

  \return fx -- Time-series
*/
Vector<Double> FFT2Fx (Vector<DComplex>& fft,
		       Int const &fftsize);
