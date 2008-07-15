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
  
  \ingroup CR_Applications

  \brief calculation of PhaseCal values in CalTables
 
  \author Frank Schröder
 
  \date 2008/06/02
    
  <h3>Motivation</h3>

  This application should calculate new PhaseCalValues for the CalTables.

  The PhaseCal values are complex numbers, one for each frequency.
  The absolute of the values is 1, thus there is no amplitude corretion done.

  First Idea: Use the same measurements as for the amplitude calibration.
  This data contain pulses consisting of combinded sine waves with 1 MHz spacing.
  - Apply digital filter to each frequency
  - Determine the position of the pulse
  --> This idea did not work, as there is no pulse left after applying a filter
      with a small band width

  Second Idea: Use measurement of the phases of the LOPES filter box.
  This works in principle, but the filter box is probably responsible for
  only a small part of dispersion of the system.

  <h3>Usage</h3>
  \verbatim
  ./calculatePhaseBehaviour [file] [delay]
  \endverbatim
  
  \param file  - file containing phases (if not supplied, current PhaseCalValues
         are read from CalTables).
  \param delay - approximate delay [in ns] of the box at 60 MHz (fine tuning is
         done automatically).
   
  <h3>Examples</h3>

  \verbatim
  ./calculatePhaseBehaviour phase_measurement.dat
  \endverbatim  
*/

const int upsampling_exp = 5;		// upsampling of trace will be done by 2^upsampling_exp
const double plotStart = -0.9e-6;	// start time for plotting of FX values
const double plotStop = -0.5e-6;        // stop time for plotting of FX values
const double peakWidth  = 20e3;		// width of spectral peaks
const int startfreq = 4e7;		// start frequency for plots and output
const int stopfreq = 8e7;              // stop frequency for plots and output


/*!
  \brief reduces phase to -180° - +180°

  \param phase         -- input phase

  \return reducedPhase -- output phase
*/


double reducePhase(double phase)
{
  double reducedPhase = phase;
  while (reducedPhase < -180) reducedPhase += 360;
  while (reducedPhase >  180) reducedPhase -= 360;
  return reducedPhase;
}

/*!
  \brief calculates the group delay as - d phi / d omega

  \param frequencies    -- frequency values in Hz
  \param phases         -- phase values in degree

  \return groupDelay    -- d (phi/360) / d f
*/

vector<double> calcGroupDelay(const vector<double> &frequencies, const vector<double> &phases)
{
  vector<double> groupDelay; // return value for upsampled trace

  try
  {
    // check for consistency
    if (frequencies.size() != phases.size() )
    {
      cerr << "calculatePhaseBehaviour:calcGroupDelay: " << "arguments must have same length" << endl;
      return groupDelay;
    }
    // check for minimal size of 3
    if (frequencies.size() < 3)
    {
      cerr << "calculatePhaseBehaviour:calcGroupDelay: " << "length of argument < 3" << endl;
      return groupDelay;
    }

    // calculate first group delay
    double deltaFreq = frequencies[1] - frequencies[0];
    groupDelay.push_back( reducePhase(phases[0]-phases[1])/deltaFreq /360);

    // calculate group delays: use d phi = 1/2 (phi(i+1) - phi(i-1)
    for(unsigned int i = 1; i < frequencies.size()-1; i++)
    {
      deltaFreq = frequencies[i+1] - frequencies[i-1];
      groupDelay.push_back( reducePhase(phases[i-1]-phases[i+1])/deltaFreq /360);
    }

    // calculate last group delay
    deltaFreq = frequencies[frequencies.size()-1] - frequencies[frequencies.size()-2];
    groupDelay.push_back( reducePhase(phases[phases.size()-2]-phases[phases.size()-1])/deltaFreq /360);

  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:calcGroupDelay: " << x.getMesg() << endl;
  }

  return groupDelay;
}



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
  \brief returns the phases of the filter measurement which Tim Huege got from Andreas Horneffer

  \param  filename    -- file with filter measurements (ASCII-table with frequency, real and imag. trace)
  \param  freqValues  -- reference to a vector for storing the frequency values
  \param  phaseValues -- reference to a vector for storing the phase values

  \return bool        -- true if successfull
*/

bool getLopes10Filter (const string &filename,
                       vector<double> &freqValues,
                       vector<double> &phaseValues)
{
  // temporarly store vectors in local variables
  // copy them into the given vectors at the end (on success only)
  vector<double> phases;
  vector<double> frequencies; 

  try
  {
     // open phase file
    ifstream phasefile;
    phasefile.open (filename.c_str(), ifstream::in);

    // check if file could be opened
    if (!(phasefile.is_open()))
    {
      std::cerr << "Failed to open file \"" << filename <<"\"." << std::endl;
      return false;      // exit
    }

    cout << "\nReading file: " << filename << endl;
    // look for the beginnig of the data (after a line containing only and at least three '-' or '='	
    string temp_read;
    bool data_found = false;
    while ((data_found == false) && (phasefile.good()))
    {
      phasefile >> temp_read;
      if (temp_read.find("Imag\"") != string::npos)	// header ends with: Imag"
      {
        data_found = true;
      }
    }

    // exit program if no data are found	
    if (data_found == false)
    {
      phasefile.close();  // close file
      std::cerr << "Failure while reading file \"" << filename <<"\":\n" ;
      std::cerr << "Wrong format! Please use a file containing phase measurements" << endl;
      return false;           // exit
    }

    // Process events from event file list
    while (phasefile.good())
    {	
      double frequency = -1, real_part = -1, imag_part = -99999999;
      bool read_in_error = false;

      // read in frequencies, phases, and imaginary part	
      if (phasefile.good()) phasefile >> frequency;
        else read_in_error = true;
      if (phasefile.good()) phasefile >> real_part;
        else read_in_error = true;	
      if (phasefile.good()) phasefile >> imag_part;
        else read_in_error = true;	

      // check if end of file occured:
      // imag_part should be -1, if file is terminated be a new line
      if (imag_part == -99999999) continue;	// go back to begin of while-loop 
                                                // phasefile.good() should be false now.

      // exit program if error occured
      if (read_in_error)		
      {
        phasefile.close();  // close file
        std::cerr << "Failure while reading file \"" << filename <<"\":\n" ;
        std::cerr << "Wrong format! Please use a file containing phase measurements" << endl;
        return false;           // exit
      }

      // store values if they are in the correct frequency interval
      if ( (startfreq <= frequency) && ( frequency <= stopfreq) )
      {
        phases.push_back( atan2(imag_part, real_part) / 3.1415926 * 180 ); // atan2 does the same as the casacore funciton phase()
        frequencies.push_back(frequency);
      }
    } // while

    // close file
    phasefile.close();

    // store vectors
    freqValues = frequencies;
    phaseValues = phases;
  }
  catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:getLopes10Filter: " << x.getMesg() << endl;
    return false;
  }
  return true;
}



/*!
  \brief applies a band filter to a trace (using Star Tools library)

  \param trace          -- Trace (time series)
  \param freq           -- frequency to filter on (in MHz)
  \param widht          -- width of the band pass (in MHz)
 
  \return filteredTrace -- trace after applying the band filter
*/

Vector<double> bandFilter(const Vector<double> &trace, double freq, double width)
{
  Vector<double> filteredTrace;	 // return value for upsampled trace

  try
  {
    // get length of trace
    unsigned int tracelength = trace.size();

    // change size of return vector
    filteredTrace.resize(tracelength);

    // allocate memory for original and upsampled traces
    float* originalTrace = new float[tracelength];
    float* bandTrace = new float[tracelength];		// for trace after band filter

    // copy the trace into the array
    for (unsigned int i = 0; i < tracelength; i++) 
      originalTrace[i] = trace(i);

    // do filtering
    BandFilterFFT(tracelength, originalTrace, freq-width, freq+width , bandTrace);

    // copy upsampled trace into Matrix with antenna traces
    // remember: ZeroPaddingFFT removes the offset automatically
    for (unsigned int i = 0; i < tracelength; i++)
      filteredTrace(i) = bandTrace[i];

    // delete arrays
    delete[] originalTrace;
    delete[] bandTrace;
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:bandFilter: " << x.getMesg() << endl;
    return Vector<double>();
  }

  return filteredTrace;
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
  \brief Caclulates an envelope of an (upsampled) trace (uses the LOPES-Star library)

  \param trace            -- any trace to calculate the envelope
  \param upsamplingExp    -- umsampling factor will be 2^upsamplingExp (default = 0)

  \return envelope        -- envelope of the given trace (has the same lenght)
*/


Vector<double> envelope(const Vector<double> &trace, unsigned int upsamplingExp = 0)
{
  Vector<double> envelope;		// create vector to return envelope

  try 
  {
    // create upsampling factor by upsampling exponent
    unsigned int upsampled = pow(2,upsamplingExp);

    // get length of trace
    unsigned int tracelength = trace.size();

    // allocate memory for original and (upsampled) envelope trace
    float* originalTrace = new float[tracelength];
    float* envelopeTrace = new float[tracelength * upsampled];

    // Resize Vector for usampled values
    envelope.resize(tracelength * upsampled);

    // copy the trace into the array
    for (unsigned int i = 0; i < tracelength; i++) 
      originalTrace[i] = trace(i);


    // do upsampling by factor #upsampled (--> NoZeros = upsampled -1)
    ZeroPaddingFFT(tracelength, originalTrace, upsampled-1, envelopeTrace);

    // do envelope calculation (given trace will be overwritten!)
    RectifierHilbert(tracelength * upsampled, envelopeTrace);

    // copy envelope from array to Vector
    for (unsigned int i = 0; i < tracelength* upsampled ; i++)
      envelope(i) = envelopeTrace[i];

    // delete arrays
    delete[] originalTrace;
    delete[] envelopeTrace;
  } catch (AipsError x) 
  {
      std::cerr << "calculatePhaseBehaviour:envelope: " << x.getMesg() << std::endl;
  }

  return envelope;
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
  \brief plots the FX trace

  \param dr           -- DataReader
  \param PlotPrefix   -- Base of filename
  \param antenna      -- Number of the antenna to plot
*/

void plotTrace(DataReader &dr, const string &PlotPrefix, int antenna)
{
  try
  {
    // get fx() data
    Matrix<Double> dataFX;
    dataFX = dr.fx();

    // plot the data
    SimplePlot plotter;    			// define plotter
    Vector<Double> xaxis;			// xaxis
    double xmax,xmin,ymin=0,ymax=0;		// Plotrange
    Vector<Double> yValues;			// y-values
    int color = 9;				// starting color

    // Get the (not upsampled) time axis
    xaxis = static_cast< Vector<Double> >(dr.timeValues());
    // yValues = bandFilter(dataFX.column(antenna-1),50,10);
    // applying the band pass filter was not successfull
    yValues = dataFX.column(antenna-1);

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
    ymax = max( envelope(upYvalues(upplotRange)) ) * 1.05;
    ymin = -ymax;

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
    // plot envelope
    plotter.PlotLine(upxaxis(upplotRange),envelope(upYvalues(upplotRange)),color+3,1);
    // Plot original data points (if upsampling was done).
    //plotter.PlotSymbols(xaxis(plotRange),yValues(plotRange),empty, empty, color, 2, 5);
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:plotTrace: " << x.getMesg() << endl;
  }
}


/*!
  \brief plots the amplitude of the FFT

  \param dr           -- DataReader
  \param PlotPrefix   -- Base of filename
  \param antenna      -- Number of the antenna to plot
*/

void plotFFT(DataReader &dr, const string &PlotPrefix, int antenna)
{
  try
  {
    // get FFT data
    Matrix<DComplex> dataFFT;
    Vector<Double> frequencies;
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

    // Create empty vector for not existing error bars 
    Vector<Double> empty;

    // plot the FFT
    yValues.resize(dataFFT.column(antenna-1).size());
    xaxis.resize(frequencies.size());
    xaxis = frequencies.copy();

    // check length of time axis and yValues traces for consistency
    if (xaxis.size() != yValues.size())
      std::cerr << " WARNING: Length of frequency axis differs from length of the FFT!\n" << std::endl;

    // FFT-Power ~ amplitude = absolute value; use log-plotting
    for (unsigned int i=0; i < yValues.size(); i++)
      yValues(i) = log10( abs(dataFFT.column(antenna-1)(i)) );
    // yValues = phase(dataFFT.column(antenna-1));
 

    // define Plotrange
    xmin = min(xaxis);
    xmax = max(xaxis);

    // find the minimal and maximal y values for the plot and take 105% to have some space
    ymin = min(yValues) * 1.05;
    ymax = max(yValues) * 1.05;


    // create the plot filename
    string plotfilename = PlotPrefix;

    stringstream antennanumber;
    antennanumber << antenna;

    //set the plotfilename to filename + "-" + antennanumber.str() + ".ps";
    if ( (antenna+1) < 10 )
    {
      plotfilename += "-FFT-0" + antennanumber.str() + ".ps";
    } else
    {
      plotfilename += "-FFT-" + antennanumber.str() + ".ps";
    }
    cout << "Plotfilename: " << plotfilename << endl;

    //set label "GT - Ant.Nr"
    stringstream gtlabel;
    uInt gtdate;
    dr.header().get("Date",gtdate);
    gtlabel << gtdate;
    string label = "GT " + gtlabel.str() + " - Antenna " + antennanumber.str();


    // Make the plot

    // Initialize the plot giving xmin, xmax, ymin and ymax
    plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

    // Add labels
    plotter.AddLabels("Frequency [MHz]", "lg Amplitude",label);

    // Plot FFT
    plotter.PlotLine(xaxis,yValues,color,1);
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:plotFFT: " << x.getMesg() << endl;
  }
}


/*!
  \brief plots the phase

  \param frequencies  -- frequency axis
  \param phases       -- phase values
  \param PlotPrefix   -- Base of filename
  \param phases2      -- second set of phase values
*/

void plotPhase (Vector<double> frequencies,
                const Vector<double> phases,
                const string &PlotPrefix,
                const Vector<double> phases2 = Vector<double> ())
{
  try
  {
    SimplePlot plotter;    			// define plotter
    double xmax,xmin,ymin=0,ymax=0;		// Plotrange
    int color = 9;				// starting color

    // Create empty vector for not existing error bars 
    Vector<Double> empty;

    // check length of frequency axis and phase valuse
    if (frequencies.size() != phases.size())
      std::cerr << " WARNING: Length of frequency axis differs from length of the FFT!\n" << std::endl;

    // convert frequencies to MHz
    frequencies /= 1e6;

    // define Plotrange
    xmin = min(frequencies);
    xmax = max(frequencies);

    // find the minimal and maximal y values for the plot and take 105% to have some space
    ymin = min(phases) * 1.05;
    ymax = max(phases) * 1.05;


    // create the plot filename
    string plotfilename = PlotPrefix + ".ps";

    cout << "Plotfilename: " << plotfilename << endl;

    string label = "Phases";


    // Make the plot

    // Initialize the plot giving xmin, xmax, ymin and ymax
    plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

    // Add labels
    plotter.AddLabels("Frequency [MHz]", "Phase [degree]",label);

    // Plot phases
    plotter.PlotLine(frequencies,phases,color,1);

    // Plot second set of phase values (if given and if of correct length)
    if ( phases2.size() == frequencies.size() )
      plotter.PlotLine(frequencies, phases2, color+1, 1); 
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:plotPhase: " << x.getMesg() << endl;
  }
}


/*!
  \brief plots the delay

  \param frequencies  -- frequency axis
  \param delay        -- delay values in ns
  \param PlotPrefix   -- Base of filename
  \param delay2       -- second set of delay values
*/

void plotDelay (Vector<double> frequencies,
                Vector<double> delay,
                const string &PlotPrefix,
                Vector<double> delay2 = Vector<double> ())
{
  try
  {
    SimplePlot plotter;    			// define plotter
    double xmax,xmin,ymin=0,ymax=0;		// Plotrange
    int color = 9;				// starting color

    // Create empty vector for not existing error bars 
    Vector<Double> empty;

    // check length of frequency axis and phase valuse
    if (frequencies.size() != delay.size())
      std::cerr << " WARNING: Length of frequency axis differs from length of the FFT!\n" << std::endl;

    // convert frequencies to MHz
    frequencies /= 1e6;
    // convert delays to ns
    delay  *= 1e9;
    delay2 *= 1e9;

    // define Plotrange
    xmin = min(frequencies);
    xmax = max(frequencies);

    // find the minimal and maximal y values for the plot and take 105% to have some space
    ymin = min(delay) * 1.05;
    ymax = max(delay) * 1.05;


    // create the plot filename
    string plotfilename = PlotPrefix + "-delay" + ".ps";

    cout << "Plotfilename: " << plotfilename << endl;

    string label = "Delay";


    // Make the plot

    // Initialize the plot giving xmin, xmax, ymin and ymax
    plotter.InitPlot(plotfilename, xmin, xmax, ymin, ymax);

    // Add labels
    plotter.AddLabels("Frequency [MHz]", "Delay [ns]",label);

    // Plot phases
    plotter.PlotLine(frequencies,delay,color,1);

    // Plot second set of phase values (if given and if of correct length)
    if ( delay2.size() == frequencies.size() )
      plotter.PlotLine(frequencies, delay2, color+1, 1); 
  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:plotDelay: " << x.getMesg() << endl;
  }
}



/*!
  \brief searches the frequency index for a peak in a spectrum
         If no stopFreq is given or if stopFreq is <= starFreq,
         then it returns the first frequency index after startFreq.

  \param spectrum     -- spectrum (FFT) of one antenna
  \param frequencies  -- frequency axis
  \param startFreq    -- start of frequency interval
  \param stopFreq     -- stop of frequency interval

  \return peakIndex   -- frequency index of peak
*/


unsigned int getPeakPos(const Vector<DComplex> &spectrum,
                        const Vector<double> &frequencies,
                        const double startFreq,
                        const double stopFreq = 0.)
{
  unsigned int peakIndex = 0;		// return value
  try 
  {
    unsigned int maxIndex = frequencies.nelements(); // maximum frequency possible

    // look for first frequency after the begin of the frequency range
    while (frequencies(peakIndex) < startFreq)
    {
      peakIndex++;
      if (peakIndex >= maxIndex) 	// check if frequency is out of range
      {
	cerr << "calculatePhaseBehaviour:getPeakPos: " << "startFreq is out of range!" << endl;
	return 0;
      }
    }

    // if this frequency is already greater or equal than the end of the frequency range,
    // then take it is automatically taken, 
    // otherwise it searches for the peak (= frequency with maximal amplitude)

    unsigned int freq = peakIndex;	// frequency index to loop through frequency range

    // loop through all frequency values in the given interval
    // (only if current frequency is not allready greater then stopFreq)
    while (frequencies(freq) < stopFreq)
    {
      // look if current amplitude is large then the last one
      if ( abs(spectrum(freq)) > abs(spectrum(peakIndex)) ) 
      {
        peakIndex = freq;
      }

      // increase frequency
      freq++;
      if (freq > maxIndex) 	// check if frequency is out of range
      {
	return peakIndex;
      }
    }
  } catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour:getPeakPos: " << x.getMesg() << endl;
  } 

  return peakIndex;
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
	// remark: correctly the power is not abs(FFT)^2
        //         , but abs(FFT(fieldstrengt^2))

    // variables for interval creation 
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
    plotTrace(dr, plotPrefix, antenna);
    //plotFFT(dr, plotPrefix, antenna);

    // calculate phases
    Vector<DComplex> spectrum = dr.fft().column(antenna-1).copy();
    Vector<Double> phases = phase(dr.fft().column(antenna-1));
    Vector<Double> frequencies = dr.frequencyValues().copy();

    // Store Phases of 41 comb frequencies
    Vector<Double> combFreqs(41,0);     // comb frequencies
    Vector<Double> combPhases(41,0);    // phases of the combfrequencies
    Vector<Double> phaseDiffs(41,0);
    Vector<Double> neighbourDiffs(41,0);
    double freq = 0;
    double freqIndex = 0;

    // Phase of reference frequency
    double referencePhase 
      = phases(getPeakPos(spectrum, frequencies, 60e6-peakWidth, 60e6+peakWidth))*180/3.1415926;		

    // loop through frequencies
    for (int i = 0; i <= 40; i++)
    {
      // calculate frequency and frequency index
      freq = i*1e6 + 40e6;
      freqIndex = getPeakPos(spectrum, frequencies, freq-peakWidth, freq+peakWidth);
      // store found frequency:
      combFreqs(i) = frequencies(freqIndex);
      //cout << "Frequency: " << frequencies(freqIndex)/1e6 << endl;

      // calculate phases
      combPhases(i) = phases(freqIndex)*180/3.1415926;
      phaseDiffs(i) = reducePhase(combPhases(i) - referencePhase);
      if (i>0) neighbourDiffs(i) = reducePhase(combPhases(i)-combPhases(i-1));
    }

    // calculate phase differences
    cout << "combPhases :\n" << combPhases << endl;
    cout << "PhaseDiffs :\n" << phaseDiffs << endl;
    cout << "neighbourDiffs :\n" << neighbourDiffs << endl;

    // plot phase differences
    plotPhase(combFreqs, phaseDiffs, plotPrefix, neighbourDiffs);

  } catch (AipsError x) 
  {
    cerr << "calculatePhaseBehaviour:analyseEvent: " << x.getMesg() << endl;
  }
}


/*!
  \brief reads and processes a file which contains phase measurements

  \param filename      -- name of phase file
  \param approxDelay   -- approximate delay at 60 MHz
*/


void readPhaseFile(const string &filename, const double approxDelay)
{
  try
  {
    double delay = 0;			// remaining delay at 60 MHz

    // vectors to store measurements
    vector<double> frequencies;
    vector<double> phases;
    vector<double> groupdelay;

     // open phase file
    ifstream phasefile;
    phasefile.open (filename.c_str(), ifstream::in);

    // check if file could be opened
    if (!(phasefile.is_open()))
    {
      std::cerr << "Failed to open file \"" << filename <<"\"." << std::endl;
      return;		// exit
    }

    cout << "\nReading file: " << filename << endl;
    // look for the beginnig of the data (after a line containing only and at least three '-' or '='	
    string temp_read;
    bool data_found = false;
    while ((data_found == false) && (phasefile.good()))
    {
      phasefile >> temp_read;
      if (temp_read.find("Imag\"") != string::npos)	// header ends with: Imag"
      {
        data_found = true;
      }
    }

    // exit program if no data are found	
    if (data_found == false)
    {
      phasefile.close();  // close file
      std::cerr << "Failure while reading file \"" << filename <<"\":\n" ;
      std::cerr << "Wrong format! Please use a file containing phase measurements" << endl;
      return;           // exit
    }

    //cout << "\nBeginning to read values:" << endl;
    //cout << "Frequency \t Phase" << endl;

    // Process events from event file list
    while (phasefile.good())
    {	
      double frequency = -1, phase = -1, imag_part = -1;
      bool read_in_error = false;

      // read in frequencies, phases, and imaginary part	
      if (phasefile.good()) phasefile >> frequency;
        else read_in_error = true;
      if (phasefile.good()) phasefile >> phase;
        else read_in_error = true;	
      if (phasefile.good()) phasefile >> imag_part;
        else read_in_error = true;	

      // check if end of file occured:
      // imag_part should be -1, if file is terminated be a new line
      if (imag_part == -1) continue;	// go back to begin of while-loop 
                                        // phasefile.good() should be false now.

      // normally imag_part should be 0, else show a warning
      if (imag_part != 0)
        std::cerr << "Warning: Imaginary part of the phase is not zero!" << endl;

      // exit program if error occured
      if (read_in_error)		
      {
        phasefile.close();  // close file
        std::cerr << "Failure while reading file \"" << filename <<"\":\n" ;
        std::cerr << "Wrong format! Please use a file containing phase measurements" << endl;
        return;           // exit
      }

      // store values in the range between startfreq and stopfreq
      if ( (frequency >= startfreq) && (frequency <= stopfreq) )
      {
        frequencies.push_back(frequency);
        phases.push_back(phase);

        // calculate remaining delay at 60 MHz, will be removed later
        if (frequency == 60e6)
        {
          // substract approximate delay:  periods to remove = delay * frequency
          double redPhase = reducePhase(phase + approxDelay*frequency * 360);
          // calculate remaining delay
          delay = -redPhase / frequency / 360;
          // add approximate delay to get total delay
          delay += approxDelay;
          cout << "Delay at 60 MHz: " << delay*1e9 << " ns" << endl;
        }
        //cout << frequency << "   \t" << phase << endl;
      }
    } // while

    // close file
    phasefile.close();

    // read in the values for the LOPES 10 filter instead (overwrites vectors)
    if (false)	// only if desired
    {
      getLopes10Filter("/home/schroeder/lopes/PhaseCal/lopes10filt_horneffer.dat", frequencies, phases);
      delay = approxDelay;
    }

    groupdelay = calcGroupDelay(frequencies, phases);

    // remove remaining delay: periods to remove = delay * frequency
    for (unsigned int i = 0; i < phases.size(); i++)
    {
      phases[i] = reducePhase(phases[i] + delay*frequencies[i] * 360);
    }

    // create plot prefix
    string plotPrefix = filename;

    // delete ending
    if (plotPrefix.find(".dat") != string::npos)
        plotPrefix.erase(plotPrefix.find_last_of('.'));	
    // delete the file path, if it exists	
    if (plotPrefix.find("/") != string::npos)
        plotPrefix.erase(0,plotPrefix.find_last_of('/')+1);

    // plot data
    plotPhase(frequencies, phases, plotPrefix);
    plotDelay(frequencies, calcGroupDelay(frequencies, phases), plotPrefix, groupdelay);

    if (false) 	// give additional output if desired
    {
      // output of frequencies
      cout << "\nFrequencies:";
      for (unsigned int i = 0; i < frequencies.size(); i++)
      {
        if (i==0) cout << "{";
          else cout << ",";
        cout << frequencies[i];
      }
      cout << "}" << endl;

      // output of phases
      cout << "\nPhasecorrection (=phase *-1):";
      for (unsigned int i = 0; i < phases.size(); i++)
      {
        if (i==0) cout << "{";
          else cout << ",";
        cout << -phases[i];
      }
      cout << "}" << endl;

      // output of group delay
      cout << "\nGroup delay:";
      for (unsigned int i = 0; i < groupdelay.size(); i++)
      {
        if (i==0) cout << "{";
          else cout << ",";
        cout << groupdelay[i];
      }
      cout << "}" << endl;
    } // if
  } catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour:readPhaseFile " << x.getMesg() << endl;
  }
}



int main (int argc, char *argv[])
{
  try
  {
    // Check correct number of arguments (1 + program name = 2)
    if ((argc != 1) && (argc !=2) && (argc !=3))
    {
      std::cerr << "Wrong number of arguments in call of \"calculatePhaseBehaviour\". The correct format is:\n";
      std::cerr << "calculatePhaseBehaviour [file] [delay]\n";
      std::cerr << "for example:\n";
      std::cerr << "./calculatePhaseBehaviour file.dat 1\n" << std::endl;
      return 1;				// Exit the program
    }

    // read the current PhaseCal values if not eventfile was specified
    if (argc == 1)
    {
      readCalTableValues();
      return 0;
    }

    // assign the arguments (filename and antenna)
    string filename(argv[1]);

    // process event (for first idea, which does not work)
    if (argc == 2)
    {
      analyseEvent(filename);
      return 0;
    }

    // if the delay argument is not a doble, default (0) will be used
    double delay = 0;
    string delayString(argv[2]);
    stringstream(delayString) >> delay; 

    // process file containing phase measurements
    readPhaseFile(filename, delay/1e9);

  } catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour: " << x.getMesg() << endl;
      return 1;
  }

  return 0;
}

