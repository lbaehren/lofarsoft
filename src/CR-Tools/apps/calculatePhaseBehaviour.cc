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

#include <time.h>
#include <iostream>
#include <string>
#include <Calibration/CalTableReader.h>
#include <Data/LopesEventIn.h>
#include <Display/SimplePlot.h>
#include <startools/reconstruction.hh>


using namespace CR;
using CR::CalTableReader;

/*!
  \file calculatePhaseBehaviour.cc
  
  \ingroup apps

  \brief calculation of PhaseCal values in CalTables
 
  \author Frank Schröder
 
  \date 2008/06/02
    
  <h3>Motivation</h3>

  This application should calculate new PhaseCalValues for the CalTables.

  The PhaseCal values are complex numbers, one for each frequency.
  The absolute of the values is 1, thus there is no amplitude corretion done.

  Idea: Use the same measurements as for the amplitude calibration.
  This data contain pulses consisting of combinded sine waves with 1 MHz spacing.
  - Apply digital filter to each frequency
  - Determine the position of the pulse

  <h3>Usage</h3>
  \verbatim
  ./calculatePhaseBehaviour [eventfile]
  \endverbatim
  
  <ul>
    <li> eventfile - Lopes event file (if not supplied, current PhaseCalValues are read from CalTables).
  </ul>
   
  <h3>Examples</h3>
  ./calculatePhaseBehaviour example.event

  \verbatim
  \endverbatim  
*/

const int upsampling_exp = 5;		// upsampling of trace will be done by 2^upsampling_exp
const double plotStart = -0.9e-6;		// start time for plotting of FX values
const double plotStop = -0.5e-6;           // stop time for plotting of FX values

/*!
  \brief reads the PhaseCal values stored in the CalTables
*/
void readCalTableValues(void)
{
  // define constants for the start date of LOPES 10 and LOPES 30
  // const unsigned int LOPES10_start = 978350400;
  // const unsigned int LOPES30_start = 1104580800;
  const int maxAntennas = 30;

  // Create Vector of antenna IDs
  vector<int> antennaIDs;

  // First Data for Antenna 1-10 at time 978350400
  antennaIDs.push_back(10101);
  antennaIDs.push_back(10102);
  antennaIDs.push_back(10201);
  antennaIDs.push_back(10202);
  antennaIDs.push_back(20101);
  antennaIDs.push_back(20102);
  antennaIDs.push_back(20201);
  antennaIDs.push_back(20202);
  antennaIDs.push_back(30101);
  antennaIDs.push_back(30102);

  // First Data for Antenna 11-30 at time 1104580800
  antennaIDs.push_back(40101);
  antennaIDs.push_back(40102);
  antennaIDs.push_back(40201);
  antennaIDs.push_back(40202);
  antennaIDs.push_back(50101);
  antennaIDs.push_back(50102);
  antennaIDs.push_back(50201);
  antennaIDs.push_back(50202);
  antennaIDs.push_back(60101);
  antennaIDs.push_back(60102);

  antennaIDs.push_back(70101);
  antennaIDs.push_back(70102);
  antennaIDs.push_back(70201);
  antennaIDs.push_back(70202);
  antennaIDs.push_back(80101);
  antennaIDs.push_back(80102);
  antennaIDs.push_back(80201);
  antennaIDs.push_back(80202);
  antennaIDs.push_back(90101);
  antennaIDs.push_back(90102);

  try
  {
    // Default CalTable-Path
    string CalTablePath="/home/schroeder/usg/data/lopes/LOPES-CalTable"; 	

    // Initialise CalTableReader    
    CalTableReader table;

    // open the table
    table.AttachTable(CalTablePath);

    Vector<Double> frequencies;
    Vector<DComplex> PhaseCalValues; 

    // set date for reading the values
    unsigned int date = time(NULL);	// default time: now
 
    // Reading the frequency axis
    cout << "\nThe value of the field \"" << "PhaseCalFreq" << "\" for the time " << date << " is: " << endl;
    if (!table.GetData(date, antennaIDs[0], "PhaseCalFreq", &frequencies))
    {
       cerr << "Error while retrieving data" << endl;
    } else
    {
      cout << "\nFrequency axis for antenna " << antennaIDs[0] << ":\n" << frequencies << endl;
    }

    // Reading the values  
    cout << "\nThe value of the field \"" << "PhaseCal" << "\" for the time " << date << " is: " << endl;

    // Loop through antennas and get the data (depending of the type)
    for (int j = 0; j < maxAntennas ; j++)
    {
       if (!table.GetData(date, antennaIDs[j], "PhaseCal", &PhaseCalValues))
       {
         cerr << "Error while retrieving data" << endl;
       } else
       {
           cout << "\nfor antenna " << antennaIDs[j] << ":\n" << PhaseCalValues << endl;
           cout << "Absolute value:\n" << abs(PhaseCalValues) << endl;
           cout << "Phase:\n" << phase(PhaseCalValues)*180./3.1415926 << endl;
       }
    }
  }
  catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour:readCalTableValues: " << x.getMesg() << endl;
      return;
  }

}

/*!
  \brief upsamples a trace (with non-zero baseline substraction)

  \param trace         -- Trace (time series)
  \param upsamplingExp -- umsampling factor will be 2^upsamplingExp

  \return upTrace	     -- upsampled trace
*/

Vector<double> upsampleTrace(const Vector<double> &trace, int upsamplingExp)
{
  Vector<double> upTrace;		// return value for upsampled trace

  try
  {
    // create upsampling factor by upsampling exponent
    unsigned int upsampled = pow(2,upsampling_exp);

    // get length of trace
    unsigned int tracelength = trace.size();

    // allocate memory for original and upsampled traces
    double* originalTrace = new double[tracelength];
    double* upsampledTrace = new double[tracelength * upsampled];

    // Resize Vector for usampled values
    upTrace.resize(tracelength * upsampled);

    // copy the trace into the array
    for (unsigned int i = 0; i < tracelength; i++) 
      originalTrace[i] = trace(i);

    // do upsampling by factor #upsampled (--> NoZeros = upsampled -1)
    ZeroPaddingFFT(tracelength, originalTrace, upsampled-1, upsampledTrace);

    // copy upsampled trace into Matrix with antenna traces
    // remember: ZeroPaddingFFT removes the offset automatically
    for (unsigned int i = 0; i < tracelength*upsampled; i++)
      upTrace(i) = upsampledTrace[i];

    // delete arrays
    delete[] originalTrace;
    delete[] upsampledTrace;
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:upsampleTrace: " << x.getMesg() << endl;
    return Vector<double>();
  }

  return upTrace;
}


/*!
  \brief interpolates time axis for upsampled traces

  \param dr            -- DataReader (for not interpolated time axis)
  \param upsamplingExp -- umsampling factor will be 2^upsamplingExp

  \return upTimeAxis   -- interpolated time axis
*/

Vector<double> upsampleTimeAxis(DataReader &dr, int upsamplingExp)
{
  Vector<double> upTimeAxis;		// return value for upsampled trace

  try 
  {
    // create upsampling factor by upsampling exponent
    unsigned int upsampled = pow(2,upsampling_exp);

    // Get time between samples
    double sampleTime = 1/(dr.sampleFrequency() * upsampled);

    // Get the not interpolated time axis
    Vector<double> timeAxis = static_cast< Vector<double> >(dr.timeValues()); 

    // get the length of the x-axis 
    long int timeLength = timeAxis.size();

    // Resize the time Vector for the new upsampled xaxis
    upTimeAxis.resize(timeLength*upsampled, false);

    // copy old values to the right place and fill space inbetween 
    for (long int i = timeLength-1; i >= 0; i--)
    {
      // move existing time value to the right place
      upTimeAxis(i*upsampled) = timeAxis(i);
      // create new values
      for (unsigned int j = 1; j < upsampled; j++) 
        upTimeAxis(i*upsampled+j) = upTimeAxis(i*upsampled) + j*sampleTime;
    }
  } catch (AipsError x) 
  {
    std::cerr << "calculatePhaseBehaviour:upsampleTimeAxis: " << x.getMesg() << std::endl;
    return Vector<double>();
  }

  return upTimeAxis;
}

/*!
  \brief plots the data

  \param dr           -- DataReader
  \param PlotPrefix   -- Base of filename
  \param antenna      -- Number of the antenna to plot
*/

void plotEvent(DataReader &dr, const string &PlotPrefix, int antenna)
{
  try
  {
    // get fx() data
    Matrix<Double> dataFX;
    Matrix<DComplex> dataFFT;
    Vector<Double> frequencies;
    dataFX = dr.fx();
    dataFFT = dr.fft();
    frequencies = dr.frequencyValues();

    // cout << "Date: " << dr.header().asuInt("Date") << endl;
    // cout << "IDs: " << dr.header().asArrayInt("AntennaIDs") << endl;

    // plot the data
    SimplePlot plotter;    			// define plotter
    Vector<Double> xaxis;			// xaxis
    double xmax,xmin,ymin=0,ymax=0;		// Plotrange
    Vector<Double> yValues;			// y-values
    int color = 9;				// starting color

    // Get the (not upsampled) time axis
    xaxis = static_cast< Vector<Double> >(dr.timeValues());
    yValues = dataFX.column(antenna-1).copy();

    // Get upsampled values
    Vector<double> upYvalues = upsampleTrace(yValues,upsampling_exp);
    Vector<double> upxaxis = upsampleTimeAxis(dr, upsampling_exp);

    // check length of time axis and yValues traces for consistency
    if (upxaxis.size() != upYvalues.size())
      std::cerr << " WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;

    // Define plotrange for not upsampled and upsampled data
    int startsample = ntrue(xaxis<plotStart);			//number of elements smaller then starting value of plot range
    int stopsample = ntrue(xaxis<plotStop);			//number of elements smaller then end of plot range
    Slice plotRange(startsample,(stopsample-startsample));	// create Slice with plotRange
    startsample = ntrue(upxaxis<plotStart);               	//number of elements smaller then starting value of plot range
    stopsample = ntrue(upxaxis<plotStop);                 	//number of elements smaller then end of plot range
    Slice upplotRange(startsample,(stopsample-startsample));	// create Slice with plotRange

    // conversion to micro (no conversion for height of raw data)
    xaxis *= 1e6;
    upxaxis *= 1e6;

    // define Plotrange
    xmin = min(xaxis(plotRange));
    xmax = max(xaxis(plotRange));

    // find the minimal and maximal y values for the plot and take 105% to have some space
    // do it with the upsampled data only as they are as least as heigh as the original ones
    ymin = min(upYvalues(upplotRange)) * 1.05;
    ymax = max(upYvalues(upplotRange)) * 1.05;


    // create the plot filename
    string plotfilename = PlotPrefix;

    stringstream antennanumber;
    antennanumber << antenna;
 
    //set the plotfilename to filename + "-" + antennanumber.str() + ".ps";
    if ( (antenna+1) < 10 )
    {
      plotfilename += "-FX-0" + antennanumber.str() + ".ps";
    } else
    {
      plotfilename += "-FX-" + antennanumber.str() + ".ps";
    }
    cout << "Plotfilename: " << plotfilename << endl;

    //set label "GT - Ant.Nr"
    stringstream gtlabel;
    uInt gtdate;
    dr.header().get("Date",gtdate);
    gtlabel << gtdate;
    string label = "GT " + gtlabel.str() + " - Antenna " + antennanumber.str();

    // Make the plot

    // Create empty vector for not existing error bars 
    Vector<Double> empty;

    // Initialize the plot giving xmin, xmax, ymin and ymax
    plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

    // Add labels
    plotter.AddLabels("Time t [#gmsec]", "Counts",label);

    // Plot (upsampled) trace
    plotter.PlotLine(upxaxis(upplotRange),upYvalues(upplotRange),color,1);
    // Plot original data points (if upsampling was done).
    //plotter.PlotSymbols(xaxis(plotRange),yValues(plotRange),empty, empty, color, 2, 5);


    // plot the FFT
    yValues.resize(dataFFT.column(antenna-1).size());
    xaxis.resize(frequencies.size());
    xaxis = frequencies.copy();

    // check length of time axis and yValues traces for consistency
    if (xaxis.size() != yValues.size())
      std::cerr << " WARNING: Length of frequency axis differs from length of the FFT!\n" << std::endl;

    // FFT-Power ~ square of absolute value; use log-plotting
    for (unsigned int i=0; i < yValues.size(); i++)
     yValues (i) = log10( pow( abs(dataFFT.column(antenna-1)(i)), 2) );
 

    // define Plotrange
    xmin = min(xaxis);
    xmax = max(xaxis);

    // find the minimal and maximal y values for the plot and take 105% to have some space
    ymin = min(yValues) * 1.05;
    ymax = max(yValues) * 1.05;


    // create the plot filename
    plotfilename = PlotPrefix;

    //set the plotfilename to filename + "-" + antennanumber.str() + ".ps";
    if ( (antenna+1) < 10 )
    {
      plotfilename += "-FFT-0" + antennanumber.str() + ".ps";
    } else
    {
      plotfilename += "-FFT-" + antennanumber.str() + ".ps";
    }
    cout << "Plotfilename: " << plotfilename << endl;


    // Make the plot

    // Initialize the plot giving xmin, xmax, ymin and ymax
    plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

    // Add labels
    plotter.AddLabels("Frequency [MHz]", "lg Power",label);

    // Plot FFT
    plotter.PlotLine(xaxis,yValues,color,1);
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:plotEvent: " << x.getMesg() << endl;
  }
}


/*!
  \brief finds the calibrated antenna:
         The calibrated antenna must have the frquency comb in the spectrum:
         That means one peak with SNR > 10 at each full MHz - frequency from
         41 to 79 MHz.
         If there are several antennas containing a frequency comb, the one
         with the highest SNR sum will be chosen.

  \return antenna -- number of antenna (starting with 1),
                      -1 if calibrated antenna was not found
*/

int calibratedAntenna(DataReader &dr)
{
  int antenna = -1;

  try
  {
    // find the frequency comb
    Matrix<DComplex> dataFFT;
    Vector<Double> frequencies;
    dataFFT = dr.fft();
    frequencies = dr.frequencyValues();

    // create Matrix with FFT power (= abs^2(FFT) ) 
    Matrix<Double> FFTpower(dataFFT.shape());
    for (unsigned int i=0; i < FFTpower.ncolumn(); i++)
      for (unsigned int j=0; j < FFTpower.nrow(); j++)
        FFTpower(j,i) = pow( abs(dataFFT(j,i)), 2);

    // variables for interval creation 
    const double peakWidth  = 20e3;		// peak width <= 20 kHz
    const double noiseWidth = 400e3;		// peak width <= 400 kHz
    int startsample, stopsample;
    Slice peakInterval, noiseInterval;

    // relative peak to noise ratio, for each peak
    double peakSNR;		// must be at leat 10 for all frequency peaks
    double sumSNR;		// sum of all peakheights
    bool foundcomb; 		// set to false, if one frequency peak is missing
    double maxSum=0;		// maximal sum

    // loop through all antennas and find frequency comb
    for (unsigned int i = 0; i < FFTpower.ncolumn(); i++)
    {
      // reset SNR and foundcomb variables
      foundcomb = true;
      sumSNR = 0;

      // loop through frequencies (from 41 MHz to 79 MHz)
      // and compare peak at full MHz with mean at following interval
      for (int f = 41e6; f < 79e6; f += 1e6)
      {
        // create peak interval (f +/- peakWidth)
        startsample = ntrue(frequencies < f-peakWidth);			//number of elements smaller then starting
        stopsample  = ntrue(frequencies < f+peakWidth);			//number of elements smaller then end point
        Slice peakInterval (startsample,(stopsample-startsample));	// create Slice with interval
        // create peak interval (f + 0.5 MHz +/- noiseWidth)
        startsample = ntrue(frequencies < f+0.5e6-noiseWidth);		//number of elements smaller then starting
        stopsample  = ntrue(frequencies < f+0.5e6+noiseWidth);		//number of elements smaller then end point
        Slice noiseInterval(startsample,(stopsample-startsample));	// create Slice with interval

        peakSNR = max(FFTpower.column(i)(peakInterval)) / median(FFTpower.column(i)(noiseInterval));
        //cout << "relative peak height (SNR) at f = " << f/1e6 << " MHz : " << peakSNR << endl;

        // check if SNR is to small
        if (peakSNR < 10) foundcomb = false;

        // sum SNR
        sumSNR += peakSNR;
      }

      // check if comb was found (SNR > 10 for all frequencies)
      if (foundcomb)
      {
        // store antenna with maximal SNR in comb frequencies
        if (sumSNR > maxSum)
        {
           maxSum = sumSNR;
           antenna = i+1;
        }
        //cout << "Frequency comb found for antenna: " << i+1 << " \t SNR(sum): " << sumSNR << endl;
      }
    }

  }  catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour:calibratedAntenna: " << x.getMesg() << endl;
      return -1;
  }

  return antenna;
}


/*!
  \brief analyses one event

  \param eventName -- Lopes event file
*/

void analyseEvent(const string &eventName)
{
  try
  {
    // open event
    LopesEventIn dr;

    cout << "\nAttaching file: " << eventName << endl;
    if (! dr.attachFile(eventName))
    {
      cerr << "Failed to attach file!" << endl;
      return;
    }

    // consistency check
    if (dr.header().asString("Observatory") != "LOPES")
    {
      cerr << "File does not contain LOPES data." << endl;
      return;
    }

    // get calibrated antenna
    int antenna = calibratedAntenna(dr);

    // check if calibrated antenna was found
    if (antenna == -1)
    {
       cerr << "ERROR: calibrated antenna was not found. Program will exit now." << endl;
       return;
    }
    else 
    {
      cout << "Calibrated antenna: " << antenna << endl;
    }

    // create plot prefix
    string plotPrefix = eventName;

    // delete ending
    if (plotPrefix.find(".event") != string::npos)
        plotPrefix.erase(plotPrefix.find_last_of('.'));	
    // deletes the file path, if it exists	
    if (plotPrefix.find("/") != string::npos)
        plotPrefix.erase(0,plotPrefix.find_last_of('/')+1);

    // Plot FX and FFT
    plotEvent(dr, plotPrefix, antenna);

  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:analyseEvent: " << x.getMesg() << endl;
  }

}

int main (int argc, char *argv[])
{
  try
  {
    // Check correct number of arguments (1 + program name = 2)
    if (argc > 2)
    {
      std::cerr << "Wrong number of arguments in call of \"calculatePhaseBehaviour\". The correct format is:\n";
      std::cerr << "calculatePhaseBehaviour [eventfile]\n";
      std::cerr << "for example:\n";
      std::cerr << "./calculatePhaseBehaviour example.event\n" << std::endl;
      return 1;				// Exit the program
    }

    // read the current PhaseCal values if not eventfile was specified
    if (argc == 1)
    {
      readCalTableValues();
      return 0;
    }

    // assign the arguments (event filename)
    string eventfile(argv[1]);

    // process event
    analyseEvent(eventfile);

  }  catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour: " << x.getMesg() << endl;
      return 1;
  }

  return 0;
}

