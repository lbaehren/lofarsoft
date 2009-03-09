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

/* $Id: skymapper.h,v 1.11 2007/05/02 09:37:10 bahren Exp $ */

#ifndef SKYMAPPER_H
#define SKYMAPPER_H

// C++ Standard library
#include <stdio.h>
#include <iomanip>
#include <fstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>
#include <images/Images/PagedImage.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>
#include <Glish/Client.h>

// Custom header files
#include <ApplicationSupport/DataReaderTools.h>
#include <ApplicationSupport/Glish.h>
#include <ApplicationSupport/ParameterFromRecord.h>
#include <ApplicationSupport/Skymap.h>
#include <ApplicationSupport/SkymapperTools.h>
#include <ApplicationSupport/DataCC.h>
#include <IO/DataReader.h>
#include <IO/LogfileName.h>
#include <ApplicationSupport/DataFrequency.h>
#include <Data/ObservationData.h>

using casa::String;

#include <casa/namespace.h>

/*!
  \file skymapper.h

  \ingroup CR_Applications

  \brief A Glish CLI for the generation of skymaps.

  \date 2004/05/17 - 2005/07

  \author Lars B&auml;hren

  For a detailed description on the functionality see the \ref skymapper_intro
  section of the Skymapper Tool (\ref LOPESTools_skymapper).

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/user/General/node24.html">AIPS++
    images module</a> -- This module contains functionality to access, create,
    and analyze AIPS++ images.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a> -- A templated N-D Array class with zero origin. Vector,
    Matrix, and Cube are one, two, and three dimensional specializations of Array.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">ArrayMath</a> -- Mathematical operations for Arrays.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Glish/GlishEvent.html">GlishSysEvent</a>
    -- Glish event wrapper
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Glish/GlishValue.html">GlishValue</a>
    -- holds any type of Glish value
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Glish/GlishRecord.html">GlishRecord</a>
    -- holds a Glish record

    <li>[LOPES-Tools] DataCC
    <li>[LOPES-Tools] DataFrequency
    <li>[LOPES-Tools] ObservationData
    <li>[LOPES-Tools] LogfileName
    <li>[LOPES-Tools] Math
    <li>[LOPES-Tools] Skymap
    <li>[LOPES-Tools] SkymapGrid
    <li>[LOPES-Tools] Skymapper
    <li>[LOPES-Tools] SkymapperTools
  </ul>

  <h3>Synopsis</h3>

  The main routine defines the targets for incoming Glish events:  
  \verbatim
  glishStream.addTarget(init_event, "initSkymapper");
  glishStream.addTarget(setBeamformer,"setBeamformer");
  glishStream.addTarget(data2Map, "data2Map");
  glishStream.addTarget(returnMap, "returnMap");
  glishStream.addTarget(clearMap, "clearMap");
  glishStream.addTarget(addtoLogfile, "addtoLogfile");
  glishStream.addTarget(dataBlocksizes, "dataBlocksizes");
  \endverbatim
*/

// =============================================================================
//
//  C++ computation routines
//
// =============================================================================

/*!
  \fn void toLogfile (String)

  \brief Add a comment/information to the logfile

  \param message - String to be written to the Logfile
 */
void toLogfile (String);

/*!
  \fn void decomposeIntegerNumber (Vector<Int>&,Int);

  \brief But which integer numbers can the input be divided without reminder?

  \param factors       = Array with the number by which <i>IntegerNumber</i>
                         can be divided without reminder.
  \param IntegerNumber = The integer number to check for division without
                         reminder.
 */
void decomposeIntegerNumber (Vector<Int>&,
			     Int);

/*!
  \fn void signalStatistics (Matrix<DComplex>&)

  \brief Generate some statistical information on the antenna signals

  \param antennaSignals = [nfreq,antennas] matrix with one FFT datablock per
                          selected antenna.
 */
void signalStatistics (Matrix<DComplex>&);

/*!
  \brief Conversion of antenna signal data
  
  Convert antenna signal data from Voltage in time to Voltage in Frequency.
  In order to select individual frequency channels, a masking array can be
  provided.

  \param dataFFT     -- Antenna signal data (Voltage in frequency).
  \param dataVoltage -- Antenna signal data (Voltage in time).
*/
void Voltage2FFT (Matrix<DComplex>& dataFFT,
		  Matrix<Double>& dataVoltage);

/*!
  \brief Compute Skymap from cc-Cube.

  \param datadir          -- Data directory containing metafile and data of
                             the experiment.
  \param antennafile      -- Location of file containing 3dim antenna positions
  \param antennaSelection -- Boolean array to mark selected/deselected
                             array elements.
 */
void ccMatrix2Map (const String datadir,
		   const String antennafile,
		   Vector<Bool>& antennaSelection);

// =============================================================================
//
//  Event handling routines
//
// =============================================================================

/*!
  \brief Initialize the skymapper client.

  \param eventdir         = (Eventfile) directory from which the data file is
                            loaded
  \param epochValue       = Epoch of the observation: its value
  \param epochUnit        = Epoch of the observation: its unit
  \param obsName          = Name of the observatory
  \param antennaPositions = 3-dim antenna positions w.r.t. to the phase center
                            of the array (usually one of the antennas)
  \param antennaSet       = Antennas selected for processing.
  \param mapWCS           = Reference code for the celestial coordinate system 
                            used in the computed skymap.
  \param Projection       = Spherical-to-Cartesian projection for the map
  \param minCSC           = CSC bounding box: lower left corner
  \param maxCSC           = CSC bounding box: upper right corner
  \param incrCSC          = Increment in celestial coordinates
  \param mapCenterWCS     = Reference code of the coordinates system in which
                            the center of the map is defined.
  \param mapCenterCSC     = Coordinates of the map center in coordinates defined by
                            <i>mapCenterWCS</i>
  \param rangeElevation   = \f$ (\theta_{\rm min}, \theta_{\rm max}) \f$, range 
                            in elevation in natural spherical coordinates (i.e. 
			    in the AZEL coordinates w.r.t. the observatory).
  \param numPixels        = Number of pixels of the produced skymap.
  \param Samplerate       = Sampling rate.
  \param FrequencyBand    = Selected frequency band.
  \param frequencies      = Center frequency of the processed frequency channels
  \param Blocksize        = Number of (time-) samples in a data block
  \param NumberBlocks     = Number of datablocks to be processed
  \param IntegrationTime  = Integration time resulting from the number of processed
                            datablocks, samples per datablock and sampling rate.
  \param SignalQuantity   = see Skymap::SignalQuantity
  \param SignalDomain     = see Skymap::SignalDomain
  \param NorthUp          = Is the map oriented such, that north is up?
  \param EastLeft         = Is the map oriented such, that east is left?

  \retval nextblock [GlishEvent] Event send back over the Glish bus.

  \return status = [Bool]

  Basically this routine stores the settings to be used later into a Skymap 
  object; see the documentation of Skymap::Skymap for the full tree of functions
  which are called.  
 */
Bool init_event (GlishSysEvent &event, void *);

/*!
 * \brief Initialize the Beamformer, setting up internal values for computation
 *
 * Initialize the internal member data of the Beamformer, which are used for the
 * processing of the antenna data.
 *
 * \param Distance      = Source distance, [m]; if \f$ r > 0 \f$ the full
 *                        geometry for a source in the near-field region is
 *                        taken into account, otherwise the source is
 *                        considered to be in the far-field region.
 * \param bfMethod      = Beamforming method to be used.
 * \param FrequencyBand = Selected frequency band(s); this information is
 *                        forwarded to the DataFrequency class to return the
 *                        corresponding frequency values, [Hz], required by
 *                        the Beamformer.
 */
Bool setBeamformer (GlishSysEvent &event, void *);

/*!
  \fn Bool returnMap (GlishSysEvent &event, void *)

  \brief Return the pixel values and the mask of the skymap.

  Return the pixel values and the mask of the skymap for a specified signal 
  domain.

  \param channels = The number of channels per image pixel, i.e. the length
                    of the third image axis.

  \retval results [GlishEvent] Glish event with the pixel values and the
  mask of the skymap.

  \return status = [Bool]
 */
Bool returnMap (GlishSysEvent &event, void *);

/*!
  \fn Bool clearMap (GlishSysEvent &event, void *)

  \brief Clear the map, set all pixel values to zero.

  In order to compute time-series of frequency data-cubes, we need to reset the
  contents of the skymap after completing an individual integration cycle. This
  is done using the Skymap::clearSkymap method, which resets the data array
  storing the pixels values and updated all related variables used for
  book-keeping (such as the counter for the number number of added datablocks).
  All other components -- esp. the Beamformer and its internal settings -- are
  left untouched.

  \retval isMapClear - Boolean variable to indicate if clearing was successfull.

  \return status = [Bool]
*/
Bool clearMap (GlishSysEvent &event, void *);

/*!
  \fn addtoLogfile (GlishSysEvent &event, void *)

  \brief Add a comment/information to the logfile

  Add a comment to the logfile. This method enables a Glish script to add
  information to the logfile. The acutal access to the logfile is done by the
  <i>toLogfile</i> function - a pure C++ function - such that the same mechanism
  is available from Glish as well as from C++.

  \param message - String with the message to be added to the logfile; a marker
  with time information is attach before actually appending the message to the 
  file.

  \return status = [Bool]
 */
Bool addtoLogfile (GlishSysEvent &event, void *);


/*!
  \fn Bool dataBlocksizes (GlishSysEvent &event, void *)

  \brief But which integer numbers can the file blocksize be divided without reminder?

  The actual computation is carried out by the routine <i>decomposeIntegerNumber</i>

  \param FileBlocksize = The total number of samples in a datafile.

  \retval Blocksizes [Glish record] = Record containing a single array the 
          blocksizes into which the total data can be divided without reminder.

  \return status = [Bool]
 */
Bool dataBlocksizes (GlishSysEvent &event, void *);

/*!
  \fn Bool getSettingsCC (GlishSysEvent &event, void *)
  
  \brief Get the plugin settings for a set of cross-correlated data

  Construct an ObservationMeta object to map the contents of an
  <i>experiment.meta</i> file to memory. From this extract the parameters
  required to set up the computation of a skymap from a cross-correlated
  dataset.

  \param metafile    [Glish record field] -- Location of the
                     <i>experiment.meta</i> describing the setup of the ITS
		     observation.
  \param antennaFile [Glish record field] -- Location of the file containing
                       the 3-dimensional antenna positions.

  \retval record -- Glish record containing the following fields:
  - <i>FFTSize</i> -- The number of points used in the FFT.
  - <i>frequencies</i> -- The frequency values at the center of the selected
                          frequency channels, [Hz].
  - <i>antennas</i> -- Indices of the array elements/antennae used in the
                       observation.
*/
Bool getSettingsCC (GlishSysEvent &event, void *);

#endif
