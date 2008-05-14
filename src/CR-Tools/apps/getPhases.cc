/***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schröder (<mail>)                                               *
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

#include <iostream>
#include <fstream>

#include <Analysis/FirstStagePipeline.h>
#include <Data/LopesEventIn.h>

/*!
  \file getPhases.cc

  \brief Gets the phases of a LOPES event at a fixed frequency
	 
  \author Frank Schröder
 
  \date 2008/04/08
    
  <h3>Motivation</h3>

  For the development of a new time calibration method for LOPES we
  need a tool which can read out the phase at a fixed frequency.
  As the frequency axis is discrete, the program will search for the 
  frequency which is the nearest to the specifed frequency.
 
  <h3>Usage</h3>
  \verbatim
  ./getPhases eventfile frequency output-file [path]
  \endverbatim
  
  <ul>
    <li> eventfile = LOPES event file (.event)
    <li> frequency = frequency in Hz to look at
    <li> output-file = a file to which the output will be appended(!)
    <li> path = Path to LOPES calibration tables (if the default doesn't work)
  </ul>
   
  <h3>Examples</h3>

  \verbatim
  ./getPhases test.event 63500000 output.txt
  (reads the phases for the spectra of test.event at 63.5 MHz and appends it to output.txt)
  ./getPhases test.event 63500000 output.txt /home/schroeder/usg/data/lopes/LOPES-CalTable/
  (same, but using the calibration table at the specified path)
  \endverbatim  
*/


// -----------------------------------------------------------------------------

using namespace CR;
/*
    Vector<Double> tmpvec;
    tmpvec= lev->frequencyValues();
    pcal.calcWeights(lev->fft());
*/
// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  try
  {
    // Check correct number of arguments (3 or 4 + program name = 4 or 5)
    if ((argc < 4) || (argc > 5))
    {
      std::cerr << "Wrong number of arguments in call of \"getPhases\".\n The correct format is:\n";
      std::cerr << "getPhases LOPES-event freqeuency output-file [Path to Caltables]\n";
      std::cerr << "for example:\n";
      std::cerr << "./getPhases example.event 57000000 output.txt\n";
      std::cerr << "If no CalTable-path is given, the default will be used.\n" << std::endl;
      return 1;				// Exit the program
    }

    // assign the arguments:

    // name of the event
    string eventfile(argv[1]);

    // name of the output file
    string outputfile(argv[3]);

    // the frequency argument must be a positive double 
    double frequency = -1;
    string tempString(argv[2]);
    stringstream(tempString) >> frequency; 
    if (frequency <= 0)
    {
      std::cerr << "Error: Frequency must by a positive double\n";
      std::cerr << "Please specify the frequency in Hz.\n";
      return 1;				// Exit the program
    }

    // Default CalTable-Path
    string CalTablePath="/home/schroeder/usg/data/lopes/LOPES-CalTable";
    // Check if there are 4 arguments: The last one is the path to the Caltables
    if (argc == 5) CalTablePath.assign(argv[4]);

    cout << "Intialising event: " << eventfile << std::endl;

    // initialise the event and check if it works
    FirstStagePipeline pipeline;
    LopesEventIn dr;
    Record obsrec;
    obsrec.define("LOPES",CalTablePath);
    pipeline.SetObsRecord(obsrec);

    if (! dr.attachFile(eventfile) )
    {
      std::cerr << "Failed to attach file: " << eventfile << std::endl;
      return 1;
    };

    if (! pipeline.InitEvent(&dr))
    {
      std::cerr << "Failed to initialize the DataReader!" << std::endl;
      return 1;
    };

    // searching for the nearest frequency

    // get the available frequencies
    Vector<Double> freqValues = dr.frequencyValues();	
    // start at frequency index zero
    int freqIndex = 0;

    // loop through frequencies and look for the lowest distance to the specified frequency
    for (unsigned int i = 0; i < freqValues.size(); i++)
      if ( abs(frequency - freqValues(i)) < abs(freqValues(freqIndex) - freqValues(i)) )
        freqIndex = i;

    cout << "Nearest frequency: " << freqValues(freqIndex)/1e6 << " MHz at index " << freqIndex << endl;


    // get phases of the spectra
    Matrix<DComplex> spectra = dr.fft();
    Vector<Double> phases = phase(spectra.row(freqIndex))*180./3.1415926;


    // open output file in append-mode
    ofstream outputfile_stream;
    outputfile_stream.open(outputfile.c_str(), ofstream::app);

    // check if file could be opened
    if (!(outputfile_stream.is_open()))
    {
      std::cerr << "Failed to open file \"" << outputfile <<"\" in append-mode." << std::endl;
    } else 
    {
      cout << "Appending phases to file: " << outputfile << endl;

      // Write the output (name of eventfile and phases in columns)
      outputfile_stream << "\"" << eventfile << "\"";

      // loop through antennas and write the pases (seperated by space)
      for (unsigned int i = 0; i < phases.size(); i++)
        outputfile_stream << " " << phases(i);

      // end the output file with a new line and close it
      outputfile_stream << endl;
      outputfile_stream.close();
    }
  } catch (AipsError x) 
  {
      cerr << "getPhases: " << x.getMesg() << endl;
      return 1;
  };

  return 0;
}
