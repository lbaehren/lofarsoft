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

/* $Id: ccData2Gaincurve.cc,v 1.7 2007/05/02 08:14:14 bahren Exp $ */

// C++ Standard library
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/stdio.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>

#include <ApplicationSupport/Glish.h>
#include <Data/ITSCorrelation.h>
#include <LopesLegacy/DataFrequency.h>
#include <Math.h>
#include <Observation/ObservationMeta.h>

using casa::String;

#include <casa/namespace.h>

// =============================================================================

/*!
  \file ccData2Gaincurve.cc

  \brief Extract antenna gaincurves from a set of cross-correlation datasets.

  \author Lars B&auml;hren

  \date 2005/03/14

  <h3>Prerequisite</h3>
  
  - [AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Tables.html">Tables</a>
  - [AIPS++] ArrayColumn --  The class ArrayColumn allows read/write access to a
             column containing arrays with an arbitrary data type.

  - DataCC - Storage for a set of cross-correlated data.
  - DataFrequency - Frequency domain settings of an observation.
  - Math - A collection of mathematical tools and functions. 
  - ObservationMeta - A class for storage of metadata to an ITS
                  experiment.
 */

// =============================================================================

/*!
  \brief Export the gain-curve values to an AIPS++ table.

  \param gainValues      -- \f$ [N_{\rm Ant},N_{\rm Freq}] \f$ matrix with the
                            gain values.
  \param gainFrequencies -- \f$ [N_{\rm Freq}] \f$ vector with the frequencies,
                            [Hz], for which antenna gain values are provided.
  \param tablename       -- Filename for the exported table.
 */
void exportGaintable (const Matrix<Double>& gainValues,
		      const Vector<Double>& gainFrequencies,
		      const String tablename)
{
  cout << "[exportGaintable]" << endl;

  Vector<String> columnLabels;
  IPosition gainShape (gainValues.shape());
  ostringstream columnLabel;

  // -----------------------------------------------------------------
  // Set up the labels fo the antenna gain columns

  cout << " - Creating labels for the antenna gain columns ..." << endl;

  columnLabels.resize (gainShape(0));

  for (int nant=0; nant<gainShape(0); nant++) {
    columnLabel << "Antenna" << (nant+1);
    columnLabels(nant) = columnLabel.str();
    columnLabel.seekp(0, ios::beg);
  }

  // -----------------------------------------------------------------
  // Start setting up and filling the table
  
  cout << " - Build the table description ..." << endl;
  TableDesc td("gaintable", "1", TableDesc::Scratch);
  td.comment() = "A table of ITS antenna gaincurves";

  // Frequency column
  td.addColumn (ScalarColumnDesc<Double> ("Frequency"));
  // Gain columns
  for (int nant=0; nant<gainShape(0); nant++) {
    td.addColumn (ScalarColumnDesc<Double> (columnLabels(nant)));
  }

  cout << " - Setup a new table from the description ... " << endl;
  SetupNewTable newtab(tablename, td, Table::New);
  Table tab(newtab);

  // -----------------------------------------------------------------
  // Put the frequency values into the table

  cout << " - Writing frequency values to table ... " << flush;
  ScalarColumn<Double> freq (tab, "Frequency");
  for (int nfreq=0; nfreq<gainShape(1); nfreq++) {
    tab.addRow ();
    freq.put (nfreq, gainFrequencies(nfreq));
  }
  cout << "Done." << endl;

  // -----------------------------------------------------------------
  // Put the antenna gain values into the table

  cout << " - Writing antenna gain values to table ... " << endl;
  for (int nant=0; nant<gainShape(0); nant++) {
    //
    ScalarColumn<Double> gain (tab, columnLabels(nant));
    //
    for (int nfreq=0; nfreq<gainShape(1); nfreq++) {
      gain.put (nfreq, gainValues(nant,nfreq));
    }
    //
    columnLabel.seekp(0, ios::beg);
  }

}

// =============================================================================

/*!
  \brief Extract antenna gaincurves from a set of auto-correlation functions

  See DataFrequency::setFrequencyBands for how to split the frequency range into
  sub-bands; using array shape indices instead of frequencies provides the
  indices required below to extract sub-bands of the auto-correlation spectra.

  \param gainValues
  \param gainFrequencies
  \param datasets    - A list of datasets.
  \param antennafile - Path to file with 3-dim antenna positions.
  \param nofBands    - Number of sub-bands for which a value is extracted.
  \param division    - Method to divide the frequency range into sub-bands:
                       <i>lin</i> (linear; this also is the default,
		       <i>log</i> (logarithmic).
*/
void do_extractGaincurves (Matrix<Double>& gainValues,
			   Vector<Double>& gainFrequencies,
			   const Vector<String>& datasets,
			   const String antennafile,
			   Int nofBands,
			   String division)
{
  Bool verboseON(False);
  Int nofAntennas(0);
  Int nofFrequencies(0);
  Int nofDatasets(0);
  String metafile;
  Matrix<Int> indices;
  Vector<Double> frequencyValues;
  Double valueBand;

  nofDatasets = datasets.nelements();
  metafile = datasets(0) + "/experiment.meta";

  //------------------------------------------------------------------
  // Get some basic information for the computation:
  //  - number of antennas
  //  - the frequency values
  //  - set up DataFrequency object (if required)

  try {
    //
    ITSCorrelation cc (metafile);
    //
    Vector<uint> antennas;    // Array with available antenna numbers
    Vector<Double> spectrum;  // Auto-correlation spectrum
    Vector<Int> range(2);     // Range (i.e. shape) of the spectrum
    //
    antennas = cc.antennas();
    nofAntennas = antennas.nelements();
    //
    valueBand = max(spectrum);
    nofFrequencies = spectrum.nelements();
    range(0) = 0;
    range(1) = nofFrequencies-1;
    //
    indices = CR::splitRange(range,nofBands,division);
  } catch (AipsError x) {
    cerr << "[do_extractGaincurves]" << x.getMesg() << endl;
  }

  //------------------------------------------------------------------
  // Provide some feedback (if enabled)
  
  if (verboseON) {
    cout << "[extractGaincurves]" << endl;
    cout << " - Number of frequencies : " << nofFrequencies << endl;
    cout << " - Frequency range ..... : [" << min(frequencyValues) << ","
	 << max(frequencyValues) << "]" << endl;
    cout << " - Array access indices  : " << indices << endl;
  }

  //------------------------------------------------------------------
  // Extract center frequencies for the subbands

  IPosition blc(1);
  IPosition trc(1);
  gainFrequencies.resize(nofBands);

  for (int band=0; band<nofBands; band++) {
    blc = indices(0,band);
    trc = indices(0,band+1);
    // 
    gainFrequencies(band) = mean(frequencyValues(blc,trc));
  }

  //------------------------------------------------------------------
  // Go through the datasets and extract the gain values
  
  gainValues.resize(nofAntennas,nofBands);
  gainValues = valueBand;

  for (int n=0; n<nofDatasets; n++) {
    //
    metafile = datasets(n) + "/experiment.meta";
    //
    ITSCorrelation cc (metafile);
    //
    for (int ant=0; ant<nofAntennas; ant++) {
      // get the auto-correlation spectrum
      Vector<Double> spectrum;
      // go through the frequency sub-bands
      for (int band=0; band<nofBands; band++) {
	blc = indices(ant,band);
	trc = indices(ant,band+1);
	// 
	valueBand = min(spectrum(blc,trc));
	//
	if (valueBand < gainValues(ant,band)) {
	  gainValues(ant,band) = valueBand;
	}
      }
    }
    
  }
  
}

// =============================================================================

Bool extractGaincurves (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;

  //------------------------------------------------------------------
  // Extract input parameters from GlishRecord

  Vector<String> datasets;  // Path to the antenna datasets
  String antennafile;       // Path to the antenna position file
  Int nofBands;
  String division;
  String tablename;
  Bool verboseON (False);

  if (glishRec.exists("datasets")) {
    gtmp = glishRec.get("datasets");
    gtmp.get(datasets);
  } else glishBus->reply(GlishArray(False));

  if (glishRec.exists("antennafile")) {
    gtmp = glishRec.get("antennafile");
    gtmp.get(antennafile);
  } else glishBus->reply(GlishArray(False));

  if (glishRec.exists("nofBands")) {
    gtmp = glishRec.get("nofBands");
    gtmp.get(nofBands);
  } else glishBus->reply(GlishArray(False));

  if (glishRec.exists("division")) {
    gtmp = glishRec.get("division");
    gtmp.get(division);
  } else glishBus->reply(GlishArray(False));

  if (glishRec.exists("tablename")) {
    gtmp = glishRec.get("tablename");
    gtmp.get(tablename);
  } else {
    tablename = "";
  }

  if (glishRec.exists("verboseON")) {
    gtmp = glishRec.get("verboseON");
    gtmp.get(verboseON);
  } else {
    verboseON = False;
  }

  //------------------------------------------------------------------
  // Provide some feedback
  
  if (verboseON) {
    cout << "[extractGaincurves]" << endl;
    cout << " - Number of datasets .......... : " << datasets.nelements() << endl;
    cout << " - Antenna position file ....... : " << antennafile << endl;
    cout << " - Number of frequency sub-bands : " << nofBands << endl;
    cout << " - Division of the frequencies . : " << division << endl;
    cout << " - Export of gaincurves ........ : " << tablename << endl;
  }

  //------------------------------------------------------------------
  // Perform the actual computation

  Matrix<Double> gainValues;
  Vector<Double> gainFrequencies;

  do_extractGaincurves (gainValues,
			gainFrequencies,
			datasets,
			antennafile,
			nofBands,
			division);

  //------------------------------------------------------------------
  // Export the gaincurves as table
  
  if (tablename != "") {
    exportGaintable (gainValues,
		     gainFrequencies,
		     tablename);
  }

  //------------------------------------------------------------------
  // Return the computation results

  GlishRecord record;

  record.add("gainValues",gainValues);
  record.add("gainFrequencies",gainFrequencies);

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[ccData2Gaincurve]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }

  return True;
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main(int argc, char **argv)
{  

  GlishSysEventSource glishStream(argc, argv);
  
  glishStream.addTarget(extractGaincurves, "extractGaincurves");
  
  glishStream.loop();

}
