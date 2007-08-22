/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: antennagaincurve.h,v 1.11 2006/10/31 18:34:11 bahren Exp $ */

// AIPS++ includes
#include <casa/Arrays.h>

#include <ApplicationSupport/DataReaderTools.h>
#include <ApplicationSupport/GlishArray.h>
#include <ApplicationSupport/GlishEvent.h>
#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/GlishValue.h>
#include <Calibration/AverageGain.h>
#include <Calibration/BaselineGain.h>
#include <IO/DataBlockSettings.h>
#include <IO/DataReader.h>
#include <Math/MathAlgebra.h>
#include <Utilities/ProgressBar.h>

#include <casa/namespace.h>

using CR::DataReader;
using CR::ProgressBar;

/*!
  \file antennagaincurve.h

  \brief Extraction of antenna gain curves from data

  \author Lars B&auml;hren, Brett Deaton

  \date 2005/07/26

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>DataReader
    <li>DataBlockSettings
    <li>BaselineGain
    <li>AverageGain
    <li>ProgressBar
  </ul>

  <h3>Synopsis</h3>

  The GUI should allow setting the following parameters:

  <table border="0">
    <tr valign="top">
      <td class="indexvalue">blocksize</td>
      <td>The number of samples per block of data</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">nofBands</td>
      <td>The number of sub-bands into which the spectrum is divided</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">scanMethod</td>
      <td>scanning method: <i>median</i>, <i>mean</i>, <i>minimum</i></td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">averageBaselines</td>
      <td>verage the baselines?</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">nofBlocks</td>
      <td>The number of blocks to be scanned</td>
    </tr>
    <tr valign="top">
      <td class="indexvalue">nofGroups</td>
      <td>The number of groups to average</td>
    </tr>
  </table>

*/

// --- Data structures ---------------------------------------------------------

typedef struct {
  //! DataReader object, used to access data stored on disk.
  DataReader *dr;
  //! Values on the frequency axis of the spectra
  Vector<Double> frequencies;
  //! Number of frequency sub-bands for which a gain value is determined. 
  Int nofSubBands;
  //! Number of data blocks to be processed.
  Int nofBlocks;
  //! Number of antennas.
  Int nofAntennas;
  //! Scanning method ("median", "mean", "minimum")
  String scanMethod;
  //! Average the baselines?
  Bool averageSpectra;
  //! Number of groups to average
  Int nofGroups;
} ClientData;


// --- Glish event handlers ----------------------------------------------------

/*!
  \brief Event handler: Initialize the flagger.

  \return errstat -- Returns <i>True</i> if an error was encountered.
*/
Bool init_event (GlishSysEvent &event, void *);

/*!
  \brief Event handler: Extract the antenna gaincurves.

  \return errstat -- Returns <i>True</i> if an error was encountered.

  \retval rec -- Record with the results.
*/
Bool extractGaincurves_event (GlishSysEvent &event, void *);

// --- Client computation functions --------------------------------------------

/*!
  \brief Extract the antenna gaincurves from the data.

  \param gaincurves  -- 
  \param frequencies -- 
 */
void extractGaincurves_func (Matrix<Double>& gaincurves,
			     Vector<Double>& frequencies);
