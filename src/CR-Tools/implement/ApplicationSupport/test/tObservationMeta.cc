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

#include <ApplicationSupport/ObservationMeta.h>

using CR::ObservationMeta;

/*!
  \file tObservationMeta.cc
  
  \brief A collection of tests for the ObservationMeta class

  \author Lars B&auml;hren
*/

/*!
  \fn void readMeta (ObservationMeta&)
  \brief Read in the metadata file
 */
void readMeta (ObservationMeta&);
/*!
  \fn void printAntpos (ObservationMeta&)
  \brief Show the 3dim antenna positions
 */
void printAntpos (ObservationMeta&);
/*!
  \fn void plugins (ObservationMeta&)
  \brief Manipulation of the plugin options
 */
void plugins (ObservationMeta&);
/*!
  \fn void timestamps (ObservationMeta&)
  \brief Obtain and display the various time information in different formats
  \verbatim
  [tObservationMeta::timestamps]

  (a) Experiment submission time
  - as string ..... : 2004-11-04T14:24:17.00Z
  - as quantity ... : 53313.6002 
  - as measure .... : Epoch: 53313::14:24:17.00000008
  
  (b) Experiment start time
  - as string ..... : now
  
  (c) Experiment trigger time(s)
  0       2004-11-04T14:24:23.88Z 53313.60028     Epoch: 53313::14:24:23.88000010
  \endverbatim
 */
void timestamps (ObservationMeta&);

int main () {

  String metafile = "../submit-experiment.meta";
  String antposFile = "antpos-its.dat";
  ObservationMeta meta (metafile,antposFile);

  /* Read in the contents of the metafile and provide some basic feedback
     on its contents */
  readMeta(meta);
  
  /* Provide some control output of the array element positions.  */
  printAntpos(meta);

  /* Manipulation of the settings for the plugins. */
  plugins(meta);

  /* Obtain the experiment triggertime in various formats.  */
  timestamps(meta);

  return 0;
  
}

// =============================================================================

void readMeta (ObservationMeta& obsMeta)
{
  cout << "\n[tObservationMeta::readMeta]\n" << endl;
  // read in the metafile
  obsMeta.readMetafile();
  // Get the stored data entries
  obsMeta.metadata(std::cout);
}

// =============================================================================

void printAntpos (ObservationMeta& obsMeta)
{
  cout << "\n[tObservationMeta::printAntpos]\n" << endl;

  Matrix<double> antpos = obsMeta.antennaPositions();
  IPosition shape = antpos.shape();

  cout << "Shape of the antenna positions matrix : " << shape << endl;

  cout.precision(10);
  for (int row=0; row<shape(0); row++) {
    cout << "\t" << row << "\t" << antpos.row(row) << endl;
  }
  
}

// =============================================================================

void plugins (ObservationMeta& obsMeta)
{
  cout << "\n[tObservationMeta::plugins]\n" << endl;

  Vector<String> pluginOptions(2);

  // Print the original plugin options
  cout << "Options for plugin 1 : " << obsMeta.plugin1() << endl;
  cout << "Options for plugin 2 : " << obsMeta.plugin2() << endl;

  // Set new plugin options
  pluginOptions(0) = "fftPlugin --daqinput --nchannels=8192 --startchannel=0 "
    "--stopchannel=4096 --window=hanning";
  pluginOptions(1) = "correlatorPlugin --nchannels=4096 --integrations=65535";
  obsMeta.plugins(pluginOptions);

  // print the newly stored plugin options
  cout << "Options for plugin 1 : " << obsMeta.plugin1() << endl;
  cout << "Options for plugin 2 : " << obsMeta.plugin2() << endl;

}

// =============================================================================

void timestamps (ObservationMeta& obsMeta)
{
  cout << "\n[tObservationMeta::timestamps]\n" << endl;

  // (a) Submit time

  Quantity submittimeQuantity;
  MEpoch submittimeMEpoch;

  String submittimeString = obsMeta.submittime();
  obsMeta.submittime(submittimeQuantity);
  obsMeta.submittime(submittimeMEpoch);

  cout << "(a) Experiment submission time" << endl;
  cout << " - as string ..... : " << submittimeString << endl;
  cout << " - as quantity ... : " << submittimeQuantity << endl;
  cout << " - as measure .... : " << submittimeMEpoch << endl;
  cout << endl;

  // (b) Start time

  String starttimeString = obsMeta.starttime();

  cout << "(b) Experiment start time" << endl;
  cout << " - as string ..... : " << starttimeString << endl;
  cout << endl;

  // (c) Trigger time

  Vector<Quantity> triggerQuantity;
  Vector<MEpoch> triggerMEpoch;

  Vector<String> triggerString = obsMeta.triggertime();
  obsMeta.triggertime(triggerQuantity);
  obsMeta.triggertime(triggerMEpoch);

  IPosition shape = triggerString.shape();

  cout << "(c) Experiment trigger time(s)" << endl;
  for (int i=0; i<shape(0); i++) {
    cout << i << "\t" << triggerString(i) << "\t" << triggerQuantity(i)
	 << "\t" << triggerMEpoch(i) << endl;
  }
  cout << endl;
}
