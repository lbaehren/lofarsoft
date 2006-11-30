/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
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

/* $Id: dynamicspectrum.h,v 1.13 2006/10/31 18:37:08 bahren Exp $ */

#ifndef _DYNAMICSPECTRUM_H_
#define _DYNAMICSPECTRUM_H_

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/stdio.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
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
#include <fits/FITS/BasicFITS.h>
#include <tasking/Glish.h>
#include <tasking/Glish/GlishRecord.h>

#include <fitsio.h>

/* // WCS library */
#include <wcslib/prj.h>

// Custom header files
#include <lopes/Beamforming/Beamformer.h>
#include <lopes/Beamforming/ccBeam.h>
#include <lopes/Data/LopesEvent.h>
#include <lopes/Data/ITSCapture.h>
#include <lopes/IO/DataReader.h>
#include <lopes/IO/DataReaderTools.h>
#include <lopes/Math/MathGeometry.h>
#include <lopes/Math/MathStatistics.h>
#include <lopes/Observation/ObservationData.h>
#include <lopes/Utilities/StringTools.h>

#include <casa/namespace.h>

/*!
  \file dynamicspectrum.h

  \ingroup Applications

  \brief A Glish CLI for the generation dynamic spectra.

  \author Lars B&auml;hren

  <h3>Prerequisite</h3>

  <ul type="square">
    <li><a href="http://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/cfitsio.html">CFITSIO User's Reference Guide</a>

    <li>[LOPES-Tools] Beamformer -- An implementation for various beamforming
                      methods.
    <li>[LOPES-Tools] DataReader -- Direct access for programs to data stored on
                      disk.
  </ul>

  <h3>Synopsis</h3>

  Sort of minimal implementation for the computation of a dynamic spectrum,
  based on the Beamformer class. While in principle the computation could be
  carried out also by the Skymapper, this implementation is of considerable less
  overhead.
  
  The following modes for creating a dynamic spectrum are supported:
  <ol>
    <li><i>beamforming</i> forms a beam towards a direction \f$ \mathbf{\rho} \f$
    as in the computation of a Skymap.
  </ol>

  <h3>Testing</h3>

  The short code snippet below can be used to test the functionality of the 
  <i>dynamicspectrum</i> application:
  \code
  blocksize := 4096;
  data.setblock(blocksize=blocksize);
  data.replot();
  DynamicSpectrum (data,blockinfo=[blocksize],dsMethod='power')
  \endcode
*/

// --- Data structures ---------------------------------------------------------

/*!
  \brief Structure to store global variables
*/
typedef struct {
  //! Type of data (TIM40, TIM40f)
  String datatype;
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


// --- Glish event handling routines -------------------------------------------

/*!
  \brief Initialize the internal settings of the Beamformer

  \param EpochValue       - Epoch of the observation; value of the quantity.
  \param EpochUnit        - Epoch of the observation; unit of the quantity.
  \param Observatory      - Name of the Observatory.
  \param WCS              - Reference code of the celestial coordinate system,
                            in which the direction of the beam is defined.
  \param Projection       - Reference code of the spherical-to-cartesian
                            projection.
  \param Frequencies      - Array with the frequency values.
  \param AntennaPositions - Array with the 3D antenna positions
  \param Coordinates      - Direction, \f$ (\alpha, \delta) \f$, towards which
                            the beam is formed.

  \retval BeamDirection   - AZEL coordinates, \f$ (\phi, \theta) \f$, of the
                            direction towards which the beam is formed.
 */
Bool initBeamformer_event (GlishSysEvent &event, void *);

/*!
  \brief Process data bypassing data transfer via Glish bus.

  This utilizes the DataReader class for direct I/O; this will work only for 
  raw data.
 */
Bool computeSpectra_event (GlishSysEvent &event, void *);

/*!
  \brief Compute a spectrum from a block of data per antenna

  \param antennaSignals - Array with one block of FFT data from each selected
                          antenna.
  \retval spectrum - Array with the power spectrum for a beam towards to defined 
                     direction.
*/
Bool processDatablock_event (GlishSysEvent &event, void *);

/*!
  \brief Test higher level functions for parameter extraction from GlishRecord

  Strictly speaking this has nothing to do with the dynamic spectrum client, but 
  I simply needed a place where to test some of the functionality implemented
  via ParameterFromRecord.cc
*/
Bool paramExtraction_event (GlishSysEvent &event, void *);

// --- Computation functions ---------------------------------------------------

/*!
  \brief Log some statistics for the current spectrum

  \param dunspec -- Dynamic spectrum for which to produce the statistics.
 */
void logStatistics (Vector<Double>& dynspec);

/*!
  \brief Accumulated power over datablock per antenna.

  \param power - Vector with the accumulated power over the datablock per
                 antenna, [nofAntennas].
  \param fft   - Array with one block of FFT data from each selected antenna.
 */
void calcAntennaPower (Vector<Double>& power,
		       const Matrix<DComplex>& fft);

/*!
  \brief Get the direction of the beam in AZEL coordinates

  \param obsData   -- 
  \param refcode   -- 
  \param direction -- 

  \return beamDirection -- 
*/
Vector<Double> adjustBeamDirection (const ObservationData& obsData,
				    const String& refcode,
				    const Vector<Double>& direction);

/*!
  \brief Compute the mean-power spectrum

  \param antennaSignals --   

  \retval spectrum -- 
 */
void calcMeanPower (Vector<Double>& spectrum,
		    const Matrix<DComplex>& dataFx);

/*!
  \brief Compute dynamic spectrum for an array beam

  \param dynamicSpectrum -- Array holding the computed values of the dynamic
                            spectrum, [freq,block].
  \param antennaPower    -- [ant,block]
  \param dr              -- DataReader object.
  \param nofBlocks       -- The number of blocks which are processed.
 */
void beam2DynamicSpectrum (Matrix<DComplex>& dynamicSpectrum,
			   Matrix<Double>& antennaPower,
			   DataReader *dr,
			   const Int& nofBlocks);

/*!
  \brief Compute dynamic spectrum from mean power

  Arrays storing the results needs to be a the correct size, since internal
  variable derivations are done based on the array shapes.

  \param dynamicSpectrum -- Array holding the computed values of the dynamic
                            spectrum, [freq,block].
  \param antennaPower    -- [ant,block]
  \param dr              -- DataReader object.
 */
void power2DynamicSpectrum (Matrix<DComplex>& dynamicSpectrum,
			    Matrix<Double>& antennaPower,
			    DataReader *dr);

/*!
  \brief Create a FITS image from the dynamic spectrum

  Directly export the generated (complex) dynamic spectrum to a set of FITS
  files; in order to keep the full information, we store both the amplitude and
  the pase information in a separate file.

  Existing FITS files of the same name will be overwritten. According to the
  CFITSIO User's Reference Guide this can be obtained by prefixing the filename
  with an exclamation point (!).

  \param outfile         -- (Base) name of the output file.
  \param dynamicSpectrum -- Array with the complex dynamic spectrum.
  \param crval           -- Coordinate reference value (FITS keyword).
  \param cdelt           -- Coordinate increment (FITS keyword).

  \return ok -- Status of the routine.

  
*/
Bool DynamicSpectrum2FITS (const String& outfile,
			   const Matrix<DComplex>& dynamicSpectrum,
			   const Vector<Double>& crval,
			   const Vector<Double>& cdelt);

/*!
  \brief Add WCS keywords to the FITS header

  This will add cards for CRPIX, CRVAL and CDELT to the header of the FITS file,
  thus allowing conversion of pixel to world coordinates.

  \param crval   -- Coordinate reference value (FITS keyword).
  \param cdelt   -- Coordinate increment (FITS keyword).

  \return status -- Error status of the cfitsio routines
*/
Int keywordsDynamicSpectrum (fitsfile *fptr,
			     const Vector<Double>& crval,
			     const Vector<Double>& cdelt);

/*!
  \fn crosscorrFrequencies (GlishSysEvent &event, void *)

  \brief Compute the cross-correlation of frequency channels at a certain time.

  \param dynspec - Array containing a previously computed dynamic spectrum;
                   time is on the \f$x\f$-axis and frequency is on the
		   \f$y\f$-axis.

  \retval crosscorr - Array with the cross-correlation between the frequency
                      channels as function of time.
 */
Bool crosscorrDynspec (GlishSysEvent &event, void *);

/*!
  \fn Bool cmsFrequency (GlishSysEvent &event, void *)

  \brief Compute the weighted center frequencies of a dynamic spectrum.

  Compute the weighted center frequency of all spectra contained in the 
  dynamic spectrum; the <i>weighted center frequency</i> is defined as 
  equivalent to the center of mass,
  \f[ \overline{\nu} = \frac{\sum_i \nu_i \, P(\nu_i)}{\sum_i P(\nu_i)} \f]

  \param Frequencies - Vector with the frequency values.
  \param DynSpec     - Array containing the dynamic spectrum.
  \retval cmsFreq    - Vector with the values of the weighted center
                       frequencies.
 */
Bool cmsFrequency (GlishSysEvent &event, void *);

/*!
  \fn Bool FindSources (GlishSysEvent &event, void *)

  \brief Find individual sources within a dynamic spectrum.
 */
Bool FindSources (GlishSysEvent &event, void *);

#endif
