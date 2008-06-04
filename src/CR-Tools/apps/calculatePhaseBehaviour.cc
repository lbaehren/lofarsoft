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
  ./calculatePhaseBehaviour Antenna
  \endverbatim
  
  <ul>
    <li> Number of Antenna (if 0, current PhaseCalValues are read from CalTables).
  </ul>
   
  <h3>Examples</h3>
  ./calculatePhaseBehaviour 1

  \verbatim
  \endverbatim  
*/


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
  \brief analyses one event for one antenna

  \param antenna -- Number of the antenna to analyse
*/

void analyseEvent(int antenna, string eventName)
{
  try
  {
    // open event
    LopesEventIn dr;

    cout << "Attaching file: " << eventName << endl;
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
    Vector<Double> upYvalues;			// upsampled y-values
    int color = 9;				// starting color

    // Get the (not upsampled) time axis
    xaxis = static_cast< Vector<Double> >(dr.timeValues());
    yValues = dataFX.column(antenna-1).copy();
    upYvalues = dataFX.column(antenna-1).copy();

    // Upsampled x-axis
    Vector<Double> upxaxis = xaxis;

    // check length of time axis and yValues traces for consistency
    if (upxaxis.size() != upYvalues.size())
      std::cerr << " WARNING: Length of time axis differs from length of the antenna traces!\n"
           << std::endl;

    // Define plotrange for not upsampled and upsampled data
    const double plotStart = -2e-6;
    const double plotStop = 0e-6;
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
    string plotfilename = eventName;
    // delete ending
    if (plotfilename.find(".event") != string::npos)
        plotfilename.erase(plotfilename.find_last_of('.'));	
    // deletes the file path, if it exists	
    if (plotfilename.find("/") != string::npos)
        plotfilename.erase(0,plotfilename.find_last_of('/')+1);


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
    //  plotter.PlotSymbols(xaxis(plotRange),yValues(plotRange),empty, empty, color, 2, 5);


    // plot the FFT
    yValues.resize(dataFFT.column(antenna-1).size());
    xaxis.resize(frequencies.size());
    xaxis = frequencies.copy();

    // check length of time axis and yValues traces for consistency
    if (xaxis.size() != yValues.size())
      std::cerr << " WARNING: Length of frequency axis differs from length of the FFT!\n"
           << std::endl;

    // FFT-Power ~ square of absolute value; use log-plotting
    for (unsigned int i=0; i < yValues.size(); i++)
     yValues (i) = log( pow( abs(dataFFT.column(antenna-1)(i)), 2) );
 

    // define Plotrange
    xmin = min(xaxis);
    xmax = max(xaxis);

    // find the minimal and maximal y values for the plot and take 105% to have some space
    ymin = min(yValues) * 1.05;
    ymax = max(yValues) * 1.05;


    // create the plot filename
    plotfilename = eventName;
    // delete ending
    if (plotfilename.find(".event") != string::npos)
        plotfilename.erase(plotfilename.find_last_of('.'));	
    // deletes the file path, if it exists	
    if (plotfilename.find("/") != string::npos)
        plotfilename.erase(0,plotfilename.find_last_of('/')+1);

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
    plotter.AddLabels("Frequency [MHz]", "log Power",label);

    // Plot FFT
    plotter.PlotLine(xaxis,yValues,color,1);
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
    if (argc != 2)
    {
      std::cerr << "Wrong number of arguments in call of \"calculatePhaseBehaviour\". The correct format is:\n";
      std::cerr << "calculatePhaseBehaviour Antenna\n";
      std::cerr << "for example:\n";
      std::cerr << "./calculatePhaseBehaviour 1\n" << std::endl;
      return 1;				// Exit the program
    }

    // assign the arguments (antenna)
    int antenna = -1;
    string antennaString(argv[1]);
    stringstream(antennaString) >> antenna;

    if ((antenna < 0) || (antenna > 30))
    {
      std::cerr << "\nError: Antenna number has to be inbetween 1 and 30!" << endl;
      return 1;
    }

    // read the current values
    if (antenna == 0)
    {
      readCalTableValues();
      return 0;
    }

    // process event
    analyseEvent(antenna, "/data/calibration/newCalib/080528/2008.05.28.08:19:39.599.event");

  }  catch (AipsError x) 
  {
      cerr << "calculatePhaseBehaviour: " << x.getMesg() << endl;
      return 1;
  }

  return 0;
}

