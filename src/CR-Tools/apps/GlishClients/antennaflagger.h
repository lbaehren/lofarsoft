/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <fits/FITS/BasicFITS.h>

#include <ApplicationSupport/DataReaderTools.h>
#include <ApplicationSupport/GlishArray.h>
#include <ApplicationSupport/GlishEvent.h>
#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/GlishValue.h>
#include <Data/ITSCapture.h>
#include <IO/DataReader.h>
#include <Analysis/ClippingFraction.h>
#include <Utilities/Masking.h>
#include <Utilities/ProgressBar.h>
#include <Utilities/StringTools.h>

#include <casa/namespace.h>

using CR::DataReader;
using CR::ProgressBar;

/*!
  \file antennaflagger.h
  
  \ingroup CR_Glish

  \brief Flagging of antennae based on a set of statistics.

  \author Lars B&auml;hren

  \date 2005/07/18

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>DataReader
    <li>ClippingFraction

    <li>[AIPS++] pksstats.cc -- Glish client that computes statistics for
    single-dish data; <tt>atnf/apps/pksstats/pksstats.cc</tt>
  </ul>

  <h3>Synopsis</h3>

  \verbatim
  for (n in seq(data.len)) {
    fx := data.get('f(x)',n);
    print n,min(fx),max(fx),sum(fx),mean(fx),variance(fx);
  }
  \endverbatim

  Usage on the Glish shell:
  \verbatim
  include 'antennaflagger.g';
  data.setblock(blocksize=8192);
  data.replot();
  antennaflagger(data, nofBlocks=100, threshold=2);
  \endverbatim

  In order to display the contents of the created ROOT data file start up ROOT
  annd then enter 
  \verbatim
  TFile f1 ("antennaflagger.root");
  tree->StartViewer();
  \endverbatim
*/

// --- Data structures ---------------------------------------------------------

typedef struct {
  //! DataReader object, used for the accessing data stored on disk.
  DataReader *dr;
  //! Number of antennas in th selected data set
  Int nofAntennas;
  //! Number of data blocks to be processed.
  Int nofBlocks;
  //! Threshold for marking an antenna
  Float threshold;
  //! Keep track of how many times a certain antenna was marked flagged
  Vector<Int> nofTimesFlagged;
  //! The array holding the flags
  Vector<Bool> antennaFlags;
} ClientData;

/* typedef struct { */
/*   Int_t antennaNumbers; */
/*   Int_t nofTimesFlagged; */
/* } ArrayStats; */

/* typedef struct { */
/*   Float_t min; */
/*   Float_t max; */
/*   Float_t sum; */
/*   Float_t mean; */
/* } AntennaStats; */

// --- Glish event handlers ----------------------------------------------------

/*!
  \brief Initialize the flagger.

  \return errstat -- Returns <i>True</i> if an error was encountered.
*/
Bool init_event (GlishSysEvent &event, void *);

/*!
  \brief Run the flagger on a set of data; event handler.

  \return errstat -- Returns <i>True</i> if an error was encountered.
 */
Bool flag_event (GlishSysEvent &event, void *);

/*!
  \brief Retrieve the generated statistics

  \return errstat -- Returns <i>True</i> if an error was encountered.
*/
Bool stats_event (GlishSysEvent &event, void *);

// --- Client computation functions --------------------------------------------

/*!
  \brief Run the flagger on a set of data; computation function.

  \return ok -- Was the flagging successful?
*/
Bool flag_func ();

/*!
  \brief Export extracted statistics to ROOT file.

  This will create a browsable ROOT data tree.
*/

Bool exportToRoot_func ();
