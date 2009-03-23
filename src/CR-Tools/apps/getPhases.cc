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

#include <crtools.h>
#include <Analysis/FirstStagePipeline.h>
#include <Data/LopesEventIn.h>

/*!
  \file getPhases.cc

  \ingroup CR_Applications

  \brief Gets the phases of a LOPES event at a certain frequency (range)
	 
  \author Frank Schröder
 
  \date 2008/11/03
    
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
  ./getPhases arguments
  \endverbatim
  
  <ul>
    Required arguments:
    <li> --in         eventfile           LOPES event file (.event)
    <li> --freq       start_frequency     frequency in Hz to look at
    Optional argumens:
    <li> --out        textfile            a file to which the output will be appended(!)
    <li> --stopfreq   stop_frequency      end of the frequency range to look at
    <li> --cal        CalTablePath        path to LOPES calibration tables (if the default doesn't work)
    <li> --refant     reference antenna   if no refAnt is given, phases instead of phase diffs will be calculated
    <li> --cutSample  number of samples   samples not used at the beginning and the end of the trace
    <li> --inter                          linerly interpolates to the phase at the exact frequency
    <li> --help                           prints short help message
  </ul>
   
  <h3>Examples</h3>

  \verbatim
  ./getPhases --in test.event --out output.txt --freq 63500000
  (reads the phases for the spectra of test.event at 63.5 MHz and appends it to output.txt)
  ./getPhases --in test.event --out output.txt 63500000 --cal /home/schroeder/usg/data/lopes/LOPES-CalTable/
  (same, but using the calibration table at the specified path)
  ./getPhases --in test.event --out output.txt --freq 63000000 --stopfreq 64000000 --refant 1
  (calculates the phase differences to antenna 1 at the frequency with the heighest peak between 63 and 64 MHz)
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
    // Process the arguments
    string eventfile("");	// event file for input
    string outputfile("");	// test file for output
    double startFreq = -1;	// start frequency
    double stopFreq = 0;	// stop frequency if a peak in a frequency range should be searched for
    unsigned int refAnt = 0;    // reference antenna for calculation of phase differences
    string CalTablePath (caltable_lopes); // default
    unsigned int cutSamples = 0;// unused samples at the beginning and the end of the trace
    bool interpolate = false;	// interpolate phase to the exact frequency
    int i = 1;		//counter for arguments
    while (i < argc)
    {
      // get option and argument
      string option(argv[i]);
      i++;
      // look for keywords which require no argument
      if ( (option == "--help") || (option == "-help")
           || (option == "--h") || (option == "-h")) {
        cout << "getPhases calculates the phase of a event at a certain frequency.\n"
             << "Possible options and arguments:\n" 
             << "option        argument\n"
             << "--help                           Displays this help message\n"
             << "--inter                          Interpolate phase to the exact frequency (optional)\n"
             << "--in, -i      eventfile          The name of the input file (required)\n"
             << "--out, -o     outputfile         The name of the output file (optional)\n"
             << "--freq, -f    frequency in Hz    (Start)frequency to look at (required)\n"
             << "--stopfreq    frequency in Hz    (Stop)frequency for peak search in interval (optional)\n"
             << "--cal         path               Path to calibration tables (optional)\n"
             << "--refant      antenna number     Calculates phase differences to this antenna instead of phases (optional)\n"
             << "--cutsamples  number of samples  Unused samples at the beginning and the end of the trace (optional)\n"
             << endl;
        continue;
      }

      if ( (option == "--inter") || (option == "-inter") ) {
        interpolate = true;
        continue;
      }

      // now look for options which require an argument and get the argument
      if (i >= argc) {
        cerr << "ERROR: No argument given for option or unknown option: \"" << option << "\"\n";
        cerr << "Use --help for more information." << endl;
        return 1;			// Exit the program if argument is missing
      }
      string argument(argv[i]);
      i++;

      // look for keywords which require an option
      if ( (option == "--in") || (option == "-in")
           || (option == "--i") || (option == "-i")) {
        eventfile = argument;
        continue;
      }

      if ( (option == "--out") || (option == "-out")
           || (option == "--o") || (option == "-o")) {
        outputfile = argument;
        continue;
      }

      if ( (option == "--cal") || (option == "-cal")
           || (option == "--c") || (option == "-c")) {
        CalTablePath = argument;
        continue;
      }

      if ( (option == "--freq") || (option == "-freq")
           || (option == "--f") || (option == "-f")) {
        stringstream(argument) >> startFreq;
        continue;
      }

      if ( (option == "--stopfreq") || (option == "-stopfreq")) {
        stringstream(argument) >> stopFreq;
        continue;
      }

      if ( (option == "--refant") || (option == "-refant")
           || (option == "--r") || (option == "-r")) {
        stringstream(argument) >> refAnt;
        continue;
      }

       if ( (option == "--cutsamples") || (option == "-cutsamples")
           || (option == "--cs") || (option == "-cs")) {
        stringstream(argument) >> cutSamples;
        continue;
      }

      // if loop comes here, the option was not found
      cerr << "ERROR: Invalid option: \"" << option << "\"\n";
      cerr << "Use --help for more information." << endl;
      return 1;			// Exit the program if argument is missing
    }

     // Check if options are set correctly
     if (eventfile == "") {
       cerr << "ERROR: Please set an input file with the --in option!\n";
       cerr << "Use --help for more information." << endl;
       return 1;
     }
     if (startFreq <= 0) {
      std::cerr << "ERROR: startfreq must by a positive double.\n";
       cerr << "Please set a frequency with the --freq option.\n";
       cerr << "Use --help for more information." << endl;
       return 1;
     }
     if (stopFreq < 0) {
       std::cerr << "ERROR: stoptfreq must by a non-negative double.\n";
       return 1;
     }

    cout << "Intialising event: " << eventfile << std::endl;

    // initialise the event and check if it works
    FirstStagePipeline pipeline;
    LopesEventIn dr;
    Record obsrec;
    obsrec.define("LOPES",CalTablePath);
    pipeline.SetObsRecord(obsrec);

    if (! dr.attachFile(eventfile) ) {
      std::cerr << "Failed to attach file: " << eventfile << std::endl;
      return 1;
    }

    // switch off certain calibration steps:
    pipeline.doGainCal(false);			// has no effect on phases of the calFFT
    pipeline.doDispersionCal(false);		// application of dispersion correction
    pipeline.doDelayCal(false);		// correction of antenna delays

    if (! pipeline.InitEvent(&dr)) {
      std::cerr << "Failed to initialize the DataReader!" << std::endl;
      return 1;
    }

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

    startsample += cutSamples;
    stopsample -= cutSamples;

    if (startsample >= stopsample) {
       cerr << "ERROR: cutSamples must be < 1/2 of the total samples, which are: " << dr.timeValues().size() << endl;
       return 1;
    }
    // WARNING:
    // Phasedifferences seem to depend dramatically on the exact number of samples and frequency
    // (Problem does not persist anymore if one uses a fixed number of samples)

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

    // get phases of the specta
    // if dispersion and delay correction are switched off (see above)
    // then phase(calfft) = phase(fft) (means raw data FFT)
    Matrix<DComplex> spectra = dr.fft(); // use fft() instead of calfft() to get faster results
    Vector<Double> phases = phase(spectra.row(freqIndex))*180./3.1415926;

    // use frequency bin before and after the exact frequency and interpolate linearly
    if (interpolate) {
      cout << "Using linear interpolation between "
           << freqValues(freqIndex-1)/1e6 << " MHz at index " << freqIndex-1 << " and "
           << freqValues(freqIndex)/1e6   << " MHz at index " << freqIndex   << endl;

      Vector<Double> phases2 = phase(spectra.row(freqIndex-1))*180./3.1415926;
      double freqbin = freqValues(freqIndex-1) - freqValues(freqIndex);

      // loop through the antennas and interpolate lineraly antenna by antenna
      for (unsigned int i = 0; i < phases.size(); i++) {
        double lowphase = phases2(i);
        double highphase = phases(i);
        //cout << "Lower phase / higher phase of ant " << i+1  << ": \t" << lowphase << " \t" << highphase << endl;
        // add +/- 360° if the difference between two phases is > 180°
        // to bring the two phases closer together for interpolation
        if (abs(lowphase - highphase) > 180) lowphase += 360 * ((highphase > 0) - (highphase < 0));
        //cout << "Lower phase / higher phase of ant " << i+1  << ": \t" << lowphase << " \t" << highphase << endl;

        // print warning if phase difference is still big
        if ( abs(lowphase - highphase) > 90)
          cout << "WARNING: big phase difference between neighbour bins during interpolation. Ant: " << i+1 << endl;

        // linear interpolation: 
        // phase_int = phase1 - ( delta phase * delta freq / freq bin)
        double interphase = lowphase - (highphase-lowphase) * (startFreq - freqValues(freqIndex-1)) / freqbin;
        //cout << "Interpolated phase: " << interphase << endl;
        phases(i) = interphase;
      }
    } else {
      cout << "Used frequency (without interpolation): " 
           << freqValues(freqIndex)/1e6 << " MHz at index " << freqIndex << endl;
    }

    // substract reference phase if refAnt is given (and not too big)
    if (refAnt > phases.size() ) {
      cout << "WARNING: Reference antenna must be <= " << phases.size() << ".\n"
           << "No phase differences will be calculated" << endl;
    } else
    if (refAnt > 0) {
      double refPhase = phases(refAnt -1);
      phases = phases - refPhase;
    }

    // reduce phases to range of -180° until +180°
    for (unsigned int i = 0; i < phases.size(); i++) {
      while (phases(i) > 180) phases(i) -= 360;
      while (phases(i) < -180) phases(i) += 360;
    }

    // if an output filename was given, write output
    if (outputfile == "") {
      cout << "Output will not be written to a file." << endl;
    } else {
      // open output file in append-mode
      ofstream outputfile_stream;
      outputfile_stream.open(outputfile.c_str(), ofstream::app);

      // check if file could be opened
      if (!(outputfile_stream.is_open()))
      {
        std::cerr << "Failed to open file \"" << outputfile <<"\" in append-mode." << std::endl;
      }   else 
      {
        cout << "Appending phases to file: " << outputfile << endl;

        // Write the output (name of eventfile and phases in columns)
        outputfile_stream << "\"" << eventfile << "\"";

        // loop through antennas and write the pases (seperated by space)
        const unsigned int nAntennas = phases.size();
        for (unsigned int i = 0; i < nAntennas; i++)
          outputfile_stream << " " << phases(i);

        // end the output file with a new line and close it
        outputfile_stream << endl;
        outputfile_stream.close();
      }
    }
  } catch (AipsError x) 
  {
      cerr << "getPhases: " << x.getMesg() << endl;
      return 1;
  };

  return 0;
}
