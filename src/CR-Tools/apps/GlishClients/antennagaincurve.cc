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

/* $Id: antennagaincurve.cc,v 1.11 2007/05/02 08:14:14 bahren Exp $ */

#include <iomanip>

// AIPS++ includes
#include <casa/iostream.h>
#include <casa/stdio.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <scimath/Functionals/Interpolate1D.h>
#include <scimath/Functionals/ScalarSampledFunctional.h>

#include "antennagaincurve.h"

// --- global variables --------------------------------------------------------

ClientData *clientData = new ClientData ();

// --- main function -----------------------------------------------------------

int main (int argc, char **argv)
{
  try {
    // Set up the Glish event stream
    GlishSysEventSource glishStream (argc, argv);
    glishStream.addTarget (init_event, "init");
    glishStream.addTarget (extractGaincurves_event, "extractGaincurves");
    glishStream.loop();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }

  return 0;
}

// --- Initialize the flagger --------------------------------------------------

Bool init_event (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();

  // Check that argument is a record.
  if (glishVal.type() != GlishValue::RECORD) {
    glishBus->reply(GlishArray(False));
    return False;
  }

  GlishRecord glishRec = glishVal;
  GlishArray gtmp;
  
  // Extract parameters from input record
  cout << " - Extracting parameters from record..." << endl;
  try {
    Int offset;
    Int blocksize;
    Int filesize;
    Vector<String> datafiles;
    //
    gtmp = glishRec.get("Files");
    gtmp.get(datafiles);
    gtmp = glishRec.get("filesize");
    gtmp.get(filesize);
    gtmp = glishRec.get("blocksize");
    gtmp.get(blocksize);
    gtmp = glishRec.get("offset");
    gtmp.get(offset);
    //
    gtmp = glishRec.get("nofBlocks");
    gtmp.get(clientData->nofBlocks);
    gtmp = glishRec.get("nofSubBands");
    gtmp.get(clientData->nofSubBands);
    gtmp = glishRec.get("frequencies");
    gtmp.get(clientData->frequencies);
    gtmp = glishRec.get("averageSpectra");
    gtmp.get(clientData->averageSpectra);
    gtmp = glishRec.get("nofGroups");
    gtmp.get(clientData->nofGroups);
    gtmp = glishRec.get("scanMethod");
    gtmp.get(clientData->scanMethod);
    //
    // Make sure data block settings are coherent with the file size.
    // Readjust if necessary.
    cout << " - Checking data block settings ..." << endl;
    DataBlockSettings settings (filesize,DataBlockSettings::ITERATIONS);
    settings.check(clientData->nofGroups,clientData->nofBlocks,blocksize,offset);
    //
    clientData->dr = DataReaderTools::assignFromRecord (glishRec,true);

  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
    
  try {
    //
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }  

  // Set additional global parameter
  clientData->nofAntennas = clientData->dr->nofSelectedAntennas();

  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  } else {
    cerr << "[antennagaincurve] Unable to report status." << endl;
    return False;
  }
  
  return True;
}

// --- Retrieve the extracted gaincurves ---------------------------------------

Bool extractGaincurves_event (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();
  Bool ok (True);

  // --- Compute -----------------------------------------------------

  Matrix<Double> gaincurves;
  Vector<Double> frequencies;

  extractGaincurves_func (gaincurves,
			  frequencies);

  // --- Return the results of the computation -----------------------

  GlishRecord rec;

  // add the results to the returned record
  rec.add("nofGroups",clientData->nofGroups);
  rec.add("nofBlocks",clientData->nofBlocks);
  rec.add("blocksize",int(clientData->dr->blocksize()));
  rec.add("gaincurves",gaincurves);
  rec.add("frequencies",frequencies);
  
  if (glishBus->replyPending()) {
    glishBus->reply(rec);
  } else {
    cerr << "[antennagaincurve] Unable to report status." << endl;
    return False;
  }

  return ok;
}

// --- Analyze the data --------------------------------------------------------

void extractGaincurves_func (Matrix<Double>& gaincurves,
			     Vector<Double>& frequencies)
{
  Int group (0);
  Int block (0);
  IPosition spectraShape ( 2,
			clientData->dr->fftLength(),
			clientData->nofAntennas );
  Matrix<DComplex> spectra;

  gaincurves.resize ( clientData->nofSubBands,
		      clientData->nofAntennas );

  AverageGain avgg ( spectraShape,
		     clientData->frequencies,
		     clientData->nofSubBands );

  if ( clientData->scanMethod == "median") {
    avgg.setMethod( BaselineGain::MED );
  } else if ( clientData->scanMethod == "mean") {
    avgg.setMethod( BaselineGain::MEAN );
  } else if ( clientData->scanMethod == "minimum") {
    avgg.setMethod( BaselineGain::MIN );
  } else {
    cerr << "[antennagaincurve] Scanning method not recognized" << endl;
  }

  cout << "[antennagaincurve] Extracting antenna gain curves from data" << endl;

  // Enable tracking of computation progress
  Int loop ( 0 );
  Int nofLoops ( (clientData->nofGroups) * (clientData->nofBlocks) );
  ProgressBar bar ( nofLoops );
  bar.update( 0 );
  for ( group=0; group<clientData->nofGroups; group++ ) {
    for ( block=0; block<clientData->nofBlocks; block++ ) {
      spectra = clientData->dr->fft();
      // extract antenna gain curves from current block
      avgg.extractBaselineGains( spectra );
      clientData->dr->nextBlock();
      loop++;
      bar.update( loop );
    }
    // add gain curve generated by inner loop to the average
    avgg.addGroupToAverage();
  }
  
  // if one wishes to interpolate the gain curves to a higher resolution, do so
  //   here. Pass frequency reference vector as argument of getAverageGains.
  try {
    gaincurves = avgg.getAverageGains();
    frequencies = avgg.getGainAbscissa();
    //
    cout << "[antennagaincurves::extractGaincurves_func]" << endl;
    cout << " - Shape of the gaincurves array : " << gaincurves.shape() << endl;
    cout << " - Range of gain values ........ : " << min(gaincurves)
	 << " .. " << max(gaincurves) << endl;
    cout << " - Range of frequency values ... : " << min(frequencies)
	 << " .. " << max(frequencies) << endl;
  } catch (AipsError x) {
    cerr << "[antennagaincurves::extractGaincurves_func] " 
	 << x.getMesg() << endl;
  }

}
