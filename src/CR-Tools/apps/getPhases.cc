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

  \brief Gets the phases of a LOPES event at a certain frequency (range)
	 
  \author Frank Schröder
 
  \date 2008/05/19
    
  <h3>Motivation</h3>

  For the development of a new time calibration method for LOPES and for the
  calculation of reference phase differences for the TV phase calibration
  weneed a tool which can read out the phase at either a fixed frequency or
  at the position of a peak in the frequency spectrum.
  As the frequency axis is discrete, the program will search for the 
  frequency which is the nearest (greater or equal) to the specifed frequency.
  If a stop frequency is given then the program searches instead for
  the frequency which has the heighest amplitude in this range.
 
  <h3>Usage</h3>
  \verbatim
  ./getPhases eventfile output-file start_frequency [stop_frequency [path]]
  \endverbatim
  
  <ul>
    <li> eventfile = LOPES event file (.event)
    <li> output-file = a file to which the output will be appended(!)
    <li> start_frequency = frequency in Hz to look at
    <li> stop_frequency = end of the frequency range to look at
    <li> path = Path to LOPES calibration tables (if the default doesn't work)
  </ul>
   
  <h3>Examples</h3>

  \verbatim
  ./getPhases test.event output.txt 63500000
  (reads the phases for the spectra of test.event at 63.5 MHz and appends it to output.txt)
  ./getPhases test.event output.txt 63500000 0 /home/schroeder/usg/data/lopes/LOPES-CalTable/
  (same, but using the calibration table at the specified path)
  ./getPhases test.event output.txt 63000000 64000000
  (reads the phases for the spectra of test.event at the heighest peak between 63 and 64 MHz)
  \endverbatim  
*/


// -----------------------------------------------------------------------------

using namespace CR;

const unsigned int PeakRefAnt = 0;	// define antenna 1 as reference for the peak search
                                        // (because antenna 1 is used as refernce for the TV phase calibration)

// returns the index of the peak in a certain range of a frequency spectrum
// the spectrum is taken as a FFT of the DataReader
// If no stopFreq is given or if stopFreq is <= starFreq,
// then it returns the first frequency index after startFreq.
// The algorithm is the same as in DelayCorrectionPlugin.

unsigned int getPeakPos(DataReader& dr, const double startFreq, const double stopFreq = 0.)
{
  unsigned int peakIndex = 0;		// return value
  try 
  {
    Vector<DComplex> spectrum = dr.fft().column(PeakRefAnt);
    Vector<Double> frequencyValues = dr.frequencyValues();
 
    unsigned int maxIndex = frequencyValues.nelements(); // maximum frequency possible

    // look for first frequency after the begin of the frequency range
    while (frequencyValues(peakIndex) < startFreq)
    {
      peakIndex++;
      if (peakIndex >= maxIndex) 	// check if frequency is out of range
      {
	cerr << "getPhases:getPeakPos: " << "startFreq is out of range!" << endl;
	return 0;
      }
    }
    // if this frequency is already greater or equal than the end of the frequency range,
    // then take it is automatically taken, 
    // otherwise it searches for the peak (= frequency with maximal amplitude)

    unsigned int freq = peakIndex;	// frequency index to loop through frequency range

    // loop through all frequency values in the given interval
    // (only if current frequency is not allready greater then stopFreq
    while (frequencyValues(freq) < stopFreq)
    {
      // look if current amplitude is large then the last one
      if ( fabs(spectrum(freq)) > fabs(spectrum(peakIndex)) ) 
      {
        peakIndex = freq;
      }

      // increase frequency
      freq++;
      if (freq >= maxIndex) 	// check if frequency is out of range
      {
	cerr << "getPhases:getPeakPos: " << "stopFreq is out of range!" << endl;
	return 0;
      }
    }
  } catch (AipsError x) 
  {
      cerr << "getPhases:getPeakPos: " << x.getMesg() << endl;
  } 

  return peakIndex;
}

// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  try
  {
    // Check correct number of arguments (3 or 4 + program name = 4 or 5)
    if ((argc < 4) || (argc > 6))
    {
      std::cerr << "Wrong number of arguments in call of \"getPhases\".\n The correct format is:\n";
      std::cerr << "getPhases LOPES-event output-file start_frequency [stop_frequency [Path to Caltables]]\n";
      std::cerr << "for example:\n";
      std::cerr << "./getPhases example.event output.txt 63500000\n";
      std::cerr << "If no CalTable-path is given, the default will be used.\n" << std::endl;
      return 1;				// Exit the program
    }

    // assign the arguments:

    // name of the event
    string eventfile(argv[1]);

    // name of the output file
    string outputfile(argv[2]);

    // the startFreq argument must be a positive double 
    double startFreq = -1;
    string tempString(argv[3]);
    stringstream(tempString) >> startFreq; 
    if (startFreq <= 0)
    {
      std::cerr << "Error: startFreq must by a positive double\n";
      std::cerr << "Please specify the frequency in Hz.\n";
      return 1;				// Exit the program
    }

    // the stopFreq argument must be a positive double if supplied, if not set it to zero
    double stopFreq = -1;
    if (argc >= 5)
    {
      string tempString2(argv[4]);
      stringstream(tempString2) >> stopFreq; 
      if (stopFreq < 0)
      {
        std::cerr << "Error: stopFreq must by a non-negative double\n";
        std::cerr << "Please specify the frequency in Hz.\n";
        return 1;				// Exit the program
      }
    } else
      stopFreq = 0;


    // Default CalTable-Path
    string CalTablePath="/home/schroeder/usg/data/lopes/LOPES-CalTable";
    // Check if there are 4 arguments: The last one is the path to the Caltables
    if (argc >= 6) CalTablePath.assign(argv[5]);

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

    // set a time range in the data reader
    // this is useful, to consider only a part of the trace for the FFT
    //double start_time = -300e-6; // start time in s
    //double stop_time = 100e-6; // interval size in s
    double start_time = -1; // start time in s
    double stop_time = 1; // interval size in s
    //number of elements smaller then start time
    unsigned int startsample = ntrue(dr.timeValues()<start_time);
    //number of elements smaller then end time
    unsigned int stopsample = ntrue(dr.timeValues()<stop_time);

    // WARNING:
    // Phasedifferences seem to depend dramatically on the exact number of samples and frequency

    // print information and set it in the datareader if only an interval of the time series is used
    if ( (startsample!=0) || (stopsample != dr.timeValues().size()) )
    {
      cout << "Use Trace between sample " << startsample << " and sample " << stopsample << " ." << endl;
      dr.setShift(startsample);
      dr.setBlocksize(stopsample-startsample); 
    }

    // searching for the nearest frequency

    // get the available frequencies
    Vector<Double> freqValues = dr.frequencyValues();	
    // get the frequency Index of the nearest / peak frequency
    unsigned int freqIndex = getPeakPos(dr,startFreq, stopFreq);

    /* old way:
     // loop through frequencies and look for the next after the specified frequeny
     for (unsigned int i = 0; i < freqValues.size(); i++)
       if ( abs(startFreq - freqValues(i)) < abs(freqValues(freqIndex) - freqValues(i)) )
       {
         freqIndex = i;
         break;
       }
   */

    cout << "Used frequency: " 
         << freqValues(freqIndex)/1e6 << " MHz at index " << freqIndex << endl;

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
