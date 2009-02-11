/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Frank Schröder                                                       *
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
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <Analysis/analyseLOPESevent2.h>

using CR::analyseLOPESevent2;
using CR::LopesEventIn;

/*!
  \file call_pipeline.cc

  \ingroup CR_Applications

  \brief Calls the LOPES analysis pipeline in "analyseLOPESevent2"
 
  \author Frank Schr&ouml;der
 
  \date 2008/20/05

  <h3>Motivation</h3>

  The aim of this short application is to provide a tool which
  makes the LOPES-analysis-pipeline usable without changing any
  code. Any information about the LOPES-events you want to analyse
  and any configurations how to analyse the events should be provide
  be external, human readable files.

  <h3>Usage</h3>

  \verbatim
  ./call_pipeline arguments
  \endverbatim
  
  <ul>
    Required arguments:
    <li> --in         eventfile list      list with LOPES event files (.event)
    Optional argumens:
    <li> --config     textfile            a file with configuration settings for the pipeline
    <li> --help                           prints a help message
  </ul>

  <h3>Prerequisites</h3>

  You need at least one event list file (textfile) like the example files below:<br>

  \verbatim
  Example event list
  some lines of comments here
  filename(event) azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]
  =========================================================================================
  /home/schroeder/data/lopesstar/first.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/second.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/third.event 354.55215 63.882182 2750 8.6060886 -368.0933
  \endverbatim
  <br>
  Warning: Make sure you provide the elevation not the zenith of the event!<br>
  <br>
  If you don't want to process the events in the default manner<br>
  you can also provide a config file (textfile), e.g. config.txt:<br>
  \verbatim
  example configuration file:
  some lines of text above a line of '='
  =================================================================  
  caltablepath           = /home/schroeder/usg/data/lopes/LOPES-CalTable
  path                   = /home/schroeder/lopes/events
  RotatePos              = true
  GeneratePlots          = true
  GenerateSpectra        = true
  SinglePlots            = true
  PlotRawData            = false
  CalculateMaxima        = false
  listCalcMaxima         = false
  printShowerCoordinates = false
  verbose                = true
  ignoreDistance         = true
  simplexFit             = true
  doTVcal                = default
  doGainCal              = true
  doDispersionCal        = true
  doDelayCal             = true
  doRFImitgation         = true
  polarization           = ANY
  plotStart              = -2.05e-6
  plotStop               = -1.60e-6
  spectrumStart          = 40e6
  spectrumStop           = 80e-6
  freqStart              = 40e6
  freqStop               = 80e-6
  upsamplingRate         = 320e6
  upsamplingExponent     = 1
  summaryColumns         = 3
  ccWindowWidth          = 0.045e-6
  flagged                = 10101
  flagged                = 10102
  rootfilename           = output.root
  rootfilemode           = recreate
  wirteBadEvents         = true
  caliabration           = false
  lateralDistribution    = false
  lateralOutputFile      = false
  \endverbatim
  This example means:
  <ul>
    <li>the caltables are in /home/horneff/lopescasa/data/LOPES/LOPES-CalTable,
    <li>the events are stored at a certain location,
    <li>the given positions are in the Kascade coordinate system and must be
    rotated to the LOPES system,
    <li>there will be generated plots of the analaysed event,
    <li>a spectrum will be plotted for every antenna
    <li>produces a plot for each antenna,
    <li>the raw ADC values (FX) are not plotted,
    <li>calcutlates the time and the height of the maximum and the minimum in the plot range 
    <li>prints a list of the absolut maxima in a more user friendly was
    <li>prints the distance form the core to the antennas in shower coordinates
    <li>there will be more text output during the analysis,
    <li>the distance value will be estimateda and the value in the eventlist ignored
    <li>the simplex fit of the arrival direction and radius of curvature is done,
    <li>the TV calibration will be done by default,
    <li>the electrical gain calibration (fieldstrength) will be done,
    <li>correction of the dispersion (frequency dependent delay) is enabled,
    <li>correction of the general delay is enabled,
    <li>supresses narrow band noise (RFI),
    <li>the analysis does not take care of the polarization of the antennas
        (select EW, NS or BOTH if you want to results for one (both) polarizations),
    <li>the plots start at -2.05 micro seconds and end at -1.60 micro seconds,
    <li>the plotted spectra start at 40 MHz seconds and end 80 MHz,
    <li>frequencies between 40 MHz and 80 MHz will be used for analysis,
    <li>the Lopes data will be upsampled to a sampling rate of 320 MHz (see below),
    <li>the upsampling of the calibrated antenna fieldstrengthes will be done by
    a factor of 2^1 = 2,
    <li>there will by a summary postscript of all created plots,
    <li>time range to search for CC-beam-peak in lateral distribution studies is +/- 45 ns,
    <li>the antennas 10101 and 10102 are not considered in the analysis,
    <li>the output of the pipeline is written to a root tree in the file "output.root".
    <li>an old root file with the same name will be overwritten
    <li>events with a bad reconstruction (e.g. simplex fit crashed) 
        will be written to the root file.
    <li>the analysis is run normally, not in calibration mode
    <li>the lateral distribution will not be analysed
    <li>there will be no file created, which has a special format for the lateral distribtion analysis
  </ul>

  <h3>Example</h3>

  \verbatim
  ./call_pipeline --in eventlist.txt --config config.txt
  \endverbatim

  <h3>Upsampling</h3>

  Currently there are two upsampling methods implemented:
  <ol>
    <li>The one called by setting an upsampling exponent uses the Upsampling
    routines of LOPES-Star and has an effect only to the traces of single
    antennas.
    <li>The upsampling due to setting an upsamplingRate greater than 160 MHz
    effects the whole analysis chain, including the CC-Beam. This method is
    recommended if no upsampling of the raw data is needed.
  </ol>
  You should prefer 2^n * 80 MHz rates (e.g. 160, 320, 640, 1280, ...).

  WARNING:
  The two upsampling methods should not be mixed!
  Otherwise wrong traces are obtained.

  <h3>Calibration</h3>

  The calibration mode can be switched on by calibration = true in the config file.
  It can be used for processing LOPES events for the delay calibration.
  In calibration mode the eventlist must contain only the event files but not additional
  information like Az, El, core and curvature.

*/

// ------------- Global variables ----------------

// Set default configuration values for the pipeline
string caltablepath = "/home/schroeder/usg/data/lopes/LOPES-CalTable";
string path = "";
bool generatePlots = true;	      // the plot prefix will be the name of the event file
bool generateSpectra = false;	      // by default no spectra are plotted
bool singlePlots = false;	      // by default there are no single plots for each antenna
bool PlotRawData = false;	      // by default there the raw data are not plotted
bool CalculateMaxima = false;	      // by default the maxima are not calculated
bool listCalcMaxima=false;    	      // print calculated maxima in more user friendly way
bool printShowerCoordinates=false;    // print the distance between antenna and shower core
bool RotatePos = true; 	              // should be true if coordinates are given in KASKADE frame
bool verbose = true;
bool ignoreDistance = true;           // distance value of the eventlist will be ignored
bool simplexFit = true;
int doTVcal = -1;		      // 1: yes, 0: no, -1: use default	
bool doGainCal = true;		      // calibration of the electrical fieldstrength
bool doDispersionCal = true;	      // application of the CalTable PhaseCal values	
bool doDelayCal = true;		      // correction for the general delay of each antenna
bool doRFImitigation = true;	      // supresses narrow band noise (RFI)
string polarization = "ANY";	      // polarization: ANY, EW, NS or BOTH
bool both_pol = false;		      // Should both polarizations be processed?
double plotStart = -2.05e-6;	      // in seconds
double plotStop = -1.60e-6;	      // in seconds
double spectrumStart = 40e6;          // for plotting, in Hz
double spectrumStop= 80e6;	      // for plotting, in Hz
double freqStart = 40e6;              // for analysis, in Hz
double freqStop= 80e6;	              // for analysis, in Hz
double upsamplingRate = 0.;	      // Upsampling Rate for new upsampling
unsigned int upsamplingExponent = 0;  // by default no upsampling will be done
vector<Int> flagged;		      // use of STL-vector instead of CASA-vector due to support of push_back()
unsigned int summaryColumns = 0;      // be default no summary of all plots
double ccWindowWidth = 0.045e-6;      // width of window for CC-beam
string rootFileName = "";             // Name of root file for output
string rootFileMode = "RECREATE";     // Mode, how to access root file
bool writeBadEvents = false;         // also bad events are written into the root file (if possible)
bool calibrationMode = false;	      // Calibration mode is off by default
bool lateralDistribution = false;    // the lateral distribution will not be generated
bool lateralOutputFile = false;      // no file for the lateral distribution will be created

// Event parameters for calling the pipeline
string eventname ="";
double azimuth=0, elevation=0, radiusOfCurvature=0, core_x=0, core_y=0;   // basic input parameters (e.g. from Kascade)

// ------------- Functions ----------------

/*!
  \brief reads text file which contains configuration information

  \param filenname     -- 

  \return nothing (If an error occurs, the program will continue with the default configuration.)
*/

void readConfigFile (const string &filename)
{
  try
  {
      ifstream configfile;
      configfile.open (filename.c_str(), ifstream::in);

      // check if file could be opened
      if (!(configfile.is_open())) {
        cerr << "Failed to open file \"" << filename <<"\".\n";
        cerr << "Program will continue with default configuration." << endl;
        return;		// exit function
      }

      // look for the beginnig of the config data (after a line containing only and at least three '-' or '='	
      string temp_read;
      bool header_found= false;
      while ((header_found== false) && (configfile.good())) {
	configfile >> temp_read;
        if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos))
	  header_found= true;
      }
	
      // print warning if no header was found and assume there is none
      if (header_found== false) {
        configfile.close();  // close file
        cout << "\nWarning: No header was found in configuration file \"" << filename <<"\".\n" ;
        cout << "Program will continue normally.\n" << endl;
        configfile.open (filename.c_str(), ifstream::in);  //reopen file to start at the beginning
      }

      while(configfile.good()) { // read configurations if configs_found
        string keyword, value, equal_token;

        // read in first keyword, then a token that should be '=' and afterward the value for the keyword
        if (configfile.good()) configfile >> keyword;
        if (configfile.good()) configfile >> equal_token;
        if (configfile.good()) configfile >> value;

        // check if end of file occured:
        // keyword should contain "", if file is terminated be a new line
        if (keyword == "") continue;	// go back to begin of while-loop 
					// configfile.good() should be false now.

        // check if syntax ("=") is correct		
        if (equal_token.compare("=") != 0) {
          cerr << "\nError processing file \"" << filename <<"\".\n" ;
          cerr << "No '=' was found after \"" << keyword << "\".\n";
          cerr << "\nProgram will continue skipping the problem." << endl;
        }

        // check keywords an set appropriate configurations

        if ( (keyword.compare("path")==0) || (keyword.compare("Path")==0) ) {
	  path = value;
          // add final "/" if not allready there
          if ( path[path.length()-1] != '/') path += "/";

	  cout << "Path set to \"" << path << "\".\n";
	}

        if ( (keyword.compare("generateplots")==0) || (keyword.compare("GeneratePlots")==0) ||
             (keyword.compare("generatePlots")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    generatePlots = true;
	    cout << "GeneratePlots set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    generatePlots = false;
	    cout << "GeneratePlots set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "GeneratePlots must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("singleplots")==0) || (keyword.compare("SinglePlots")==0) ||
             (keyword.compare("singlePlots")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    singlePlots = true;
	    cout << "SinglePlots set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    singlePlots = false;
	    cout << "SinglePlots set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "SinglePlots must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("generateSpectra")==0) || (keyword.compare("GenerateSpectra")==0) ||
             (keyword.compare("generatespectra")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    generateSpectra = true;
	    cout << "GenerateSpectra set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    generateSpectra = false;
	    cout << "GenerateSpectra set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "GenerateSpectra must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("plotrawdata")==0) || (keyword.compare("PlotRawData")==0) ||
             (keyword.compare("plotRawdata")==0) || (keyword.compare("plotRawData")==0) ) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    PlotRawData = true;
	    cout << "PlotRawData set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    PlotRawData = false;
	    cout << "PlotRawData set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "PlotRawData must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("calculatemaxima")==0) || (keyword.compare("CalculateMaxima")==0) ||
             (keyword.compare("Calculatemaxima")==0) || (keyword.compare("calculateMaxima")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    CalculateMaxima = true;
	    cout << "CalculateMaxima set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    CalculateMaxima = false;
	    cout << "CalculateMaxima set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "CalculateMaxima must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("listcalcmaxima")==0) || (keyword.compare("ListCalcMaxima")==0) ||
             (keyword.compare("listCalcMaxima")==0) ) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    listCalcMaxima = true;
	    cout << "listCalcMaxima set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    listCalcMaxima = false;
	    cout << "listCalcMaxima set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "listCalcMaxima must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("printShowerCoordinates")==0) || (keyword.compare("printShowerCoordinates")==0) ||
             (keyword.compare("printShowerCoordinates")==0) ) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    printShowerCoordinates = true;
	    cout << "printShowerCoordinates set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    printShowerCoordinates = false;
	    cout << "printShowerCoordinates set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "printShowerCoordinates must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("rotatepos")==0) || (keyword.compare("RotatePos")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    RotatePos = true;
	    cout << "RotatePos set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    RotatePos = false;
	    cout << "RotatePos set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "RotatePos must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("verbose")==0) || (keyword.compare("Verbose")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    verbose = true;
	    cout << "Verbose set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    verbose = false;
	    cout << "Verbose set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "Verbose must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("ignoredistance")==0) || (keyword.compare("ignoreDistance")==0) ||
             (keyword.compare("IgnoreDistance")==0) || (keyword.compare("IgnoreDistance")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    ignoreDistance = true;
	    cout << "ignoreDistance set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    ignoreDistance = false;
	    cout << "ignoreDistance set to 'false'.\n";
          } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "ignoreDistance must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("simplexfit")==0) || (keyword.compare("simplexFit")==0) || (keyword.compare("SimplexFit")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    simplexFit = true;
	    cout << "SimplexFit set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    simplexFit = false;
	    cout << "SimplexFit set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "SimplexFit must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("dotvcal")==0) || (keyword.compare("doTVcal")==0) 
             || (keyword.compare("DoTVCal")==0) || (keyword.compare("doTVCal")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    doTVcal = 1;
	    cout << "doTVcal set to 1 (TV calibration will be done.).\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    doTVcal = 0;
	    cout << "doTVcal set to 0 (TV calibration won't be done.).\n";
	  } else
	  if ( (value.compare("default")==0) || (value.compare("Default")==0) || (value.compare("-1")==0) ) {
	    doTVcal = -1;
	    cout << "doTVcal set to -1 (default will be used).\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "doTVcal must be either -1 ('default'), 0 ('false') or 1 ('true').\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("doGaincal")==0) || (keyword.compare("doGainCal")==0)
             || (keyword.compare("DoGainCal")==0) || (keyword.compare("dogaincal")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    doGainCal = true;
	    cout << "doGainCal set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    doGainCal = false;
	    cout << "doGainCal set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "doGainCal must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("doDispersioncal")==0) || (keyword.compare("doDispersionCal")==0)
             || (keyword.compare("DoDispersionCal")==0) || (keyword.compare("dodispersioncal")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    doDispersionCal = true;
	    cout << "doDispersionCal set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    doDispersionCal = false;
	    cout << "doDispersionCal set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "doDispersionCal must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("doDelaycal")==0) || (keyword.compare("doDelayCal")==0)
             || (keyword.compare("DoDelayCal")==0) || (keyword.compare("dodelaycal")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    doDelayCal = true;
	    cout << "doDelayCal set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    doDelayCal = false;
	    cout << "doDelayCal set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "doDelayCal must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}


        if ( (keyword.compare("doRFImitigation")==0) || (keyword.compare("DoRFIMitigation")==0)
             || (keyword.compare("DoRFImitigation")==0) || (keyword.compare("dorfimitigation")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    doRFImitigation = true;
	    cout << "doRFImitigation set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    doRFImitigation = false;
	    cout << "doRFImitigation set to 'false'.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "doRFImitigation must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("polarization")==0) || (keyword.compare("Polarization")==0)
             || (keyword.compare("polarisation")==0) || (keyword.compare("Polarisation")==0)) {
          if ( (value.compare("ANY")==0) || (value.compare("any")==0) ) {
	    polarization = "ANY";
            both_pol = false;
	    cout << "polarization set to ANY (polarization won't be considered during analysis).\n";
	  } else
          if ( (value.compare("EW")==0) || (value.compare("ew")==0) ) {
	    polarization = "EW";
            both_pol = false;
	    cout << "polarization set to EW (only EW antennas will be beamformed).\n";
	  } else
	  if ( (value.compare("NS")==0) || (value.compare("ns")==0) ) {
	    polarization = "NS";
            both_pol = false;
	    cout << "polarization set to NS (only NS antennas will be beamformed).\n";
	  } else
	  if ( (value.compare("Both")==0) || (value.compare("BOTH")==0) || (value.compare("both")==0) ) {
	    polarization = "BOTH";
            both_pol = true;
	    cout << "polarization set to BOTH (EW and NS antennas will be beamformed seperatly).\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "Polarization must be either ANY, EW, NS or BOTH.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("plotStart")==0) || (keyword.compare("PlotStart")==0) || (keyword.compare("plotstart")==0)) {
          double temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) { // will be false, if value is not of typ "double"
            plotStart = temp;
	    cout << "PlotStart set to " << plotStart << " seconds.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "PlotStart must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("plotStop")==0) || (keyword.compare("PlotStop")==0) || (keyword.compare("plotstop")==0)
            || (keyword.compare("plotEnd")==0) || (keyword.compare("PlotEnd")==0) || (keyword.compare("plotend")==0)) {
          double temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) { // will be false, if value is not of typ "double"
            plotStop = temp;
	    cout << "PlotStop set to " << plotStop << " seconds.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "PlotStop must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("spectrumStart")==0) || (keyword.compare("SpectrumStart")==0) 
            || (keyword.compare("spectrumstart")==0)) {
          double temp = -9999999;
          stringstream(value) >> temp;

          if (temp != -9999999) { // will be false, if value is not of typ "double"
            spectrumStart = temp;
	    cout << "SpectrumStart set to " << spectrumStart << " Hz.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "SpectrumStart must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

       if ( (keyword.compare("spectrumStop")==0) || (keyword.compare("SpectrumStop")==0) 
            || (keyword.compare("spectrumstop")==0)) {
          double temp = -9999999;
          stringstream(value) >> temp;

          if (temp != -9999999) { // will be false, if value is not of typ "double"
            spectrumStop = temp;
	    cout << "SpectrumStop set to " << spectrumStop << " Hz.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "SpectrumStop must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }


        if ( (keyword.compare("freqStart")==0) || (keyword.compare("FreqStart")==0) 
            || (keyword.compare("freqstart")==0)) {
          double temp = -9999999;
          stringstream(value) >> temp;

          if (temp != -9999999) { // will be false, if value is not of typ "double"
            freqStart = temp;
	    cout << "FreqStart set to " << freqStart << " Hz.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "FreqStart must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

       if ( (keyword.compare("freqStop")==0) || (keyword.compare("FreqStop")==0) 
            || (keyword.compare("freqstop")==0)) {
          double temp = -9999999;
          stringstream(value) >> temp;

          if (temp != -9999999) { // will be false, if value is not of typ "double"
            freqStop = temp;
	    cout << "FreqStop set to " << freqStop << " Hz.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "FreqStop must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("upsamplingRate")==0) || (keyword.compare("UpsamplingRate")==0) 
	  || (keyword.compare("upsamplingrate")==0)) {
          double temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) {  // will be false, if value is not of typ "double"
            upsamplingRate = temp;
	    cout << "UpsamplingRate set to " << upsamplingRate << " Hz.\n";
            // For upsampling rates < 80 MHz no upsampling will be done,
            // but for upsampling rates between 80 and 160 MHz strange results
            // may occur. So show a warning in this case:
	    if ((upsamplingRate >= 80e6) && (upsamplingRate < 160e6))
              cerr << "WARNING: UpsamplingRate should be larger than 160 MHz to obtain useful results.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "UpsamplingRate must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("upsamplingExponent")==0) || (keyword.compare("UpsamplingExponent")==0) 
	  || (keyword.compare("upsamplingexponent")==0)) {
          unsigned int temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) {  // will be false, if value is not of typ "int"
            upsamplingExponent = temp;
	    cout << "UpsamplingExponent set to " << upsamplingExponent<< ".\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "UpsamplingExponent must be of typ 'unsignend int'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("summarycolumns")==0) || (keyword.compare("summaryColumns")==0)
	  || (keyword.compare("SummaryColumns")==0)) {
          unsigned int temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) {  // will be false, if value is not of typ "int"
            summaryColumns = temp;
	    cout << "SummaryColumns set to " << summaryColumns << ".\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "SummaryColumns must be of typ 'unsignend int'. \n";
            cerr << "Use 'summaryColumns = 0' if you don't want to hava a summary postscrict.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("ccwindowwidth")==0) || (keyword.compare("CCWindowWidth")==0) ||
              (keyword.compare("CCwindowwidth")==0) || (keyword.compare("ccWindowWidth")==0))
        {
          double temp = 9999999;
          stringstream(value) >> temp;

          if (temp != 9999999) { // will be false, if value is not of typ "double"
            ccWindowWidth = temp;
	    cout << "ccWindowWidth set to " << ccWindowWidth << " seconds.\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "ccWindowWidth must be of typ 'double'. \n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

	// flagg antennas
        if ( (keyword.compare("flagged")==0) || (keyword.compare("Flagged")==0))
        {
          if ( (value.compare("10101")==0) || (value.compare("1")==0) ) {
	    flagged.push_back(10101);
	    cout << "Flagged antenna 1 (id = 10101).\n";
	  } else
          if ( (value.compare("10102")==0) || (value.compare("2")==0) ) {
	    flagged.push_back(10102);
	    cout << "Flagged antenna 2 (id = 10102).\n";
	  } else
          if ( (value.compare("10201")==0) || (value.compare("3")==0) ) {
	    flagged.push_back(10201);
	    cout << "Flagged antenna 3 (id = 10201).\n";
	  } else
          if ( (value.compare("10202")==0) || (value.compare("4")==0) ) {
	    flagged.push_back(10202);
	    cout << "Flagged antenna 4 (id = 10202).\n";
	  } else
          if ( (value.compare("20101")==0) || (value.compare("5")==0) ) {
	    flagged.push_back(20101);
	    cout << "Flagged antenna 5 (id = 20101).\n";
	  } else
          if ( (value.compare("20102")==0) || (value.compare("6")==0) ) {
	    flagged.push_back(20102);
	    cout << "Flagged antenna 6 (id = 20102).\n";
	  } else
          if ( (value.compare("20201")==0) || (value.compare("7")==0) ) {
	    flagged.push_back(20201);
	    cout << "Flagged antenna 7 (id = 20201).\n";
	  } else
          if ( (value.compare("20202")==0) || (value.compare("8")==0) ) {
	    flagged.push_back(20202);
	    cout << "Flagged antenna 8 (id = 20202).\n";
	  } else
          if ( (value.compare("30101")==0) || (value.compare("9")==0) ) {
	    flagged.push_back(30101);
	    cout << "Flagged antenna 9 (id = 30101).\n";
	  } else
          if ( (value.compare("30102")==0) || (value.compare("10")==0) ) {
	    flagged.push_back(30102);
	    cout << "Flagged antenna 10 (id = 30102).\n";
	  } else
          if ( (value.compare("40101")==0) || (value.compare("11")==0) ) {
	    flagged.push_back(40101);
	    cout << "Flagged antenna 11 (id = 40101).\n";
	  } else
          if ( (value.compare("40102")==0) || (value.compare("12")==0) ) {
	    flagged.push_back(40102);
	    cout << "Flagged antenna 12 (id = 40102).\n";
	  } else
          if ( (value.compare("40201")==0) || (value.compare("13")==0) ) {
	    flagged.push_back(40201);
	    cout << "Flagged antenna 13 (id = 40201).\n";
	  } else
          if ( (value.compare("40202")==0) || (value.compare("14")==0) ) {
	    flagged.push_back(40202);
	    cout << "Flagged antenna 14 (id = 40202).\n";
	  } else
          if ( (value.compare("50101")==0) || (value.compare("15")==0) ) {
	    flagged.push_back(50101);
	    cout << "Flagged antenna 15 (id = 50101).\n";
	  } else
          if ( (value.compare("50102")==0) || (value.compare("16")==0) ) {
	    flagged.push_back(50102);
	    cout << "Flagged antenna 16 (id = 50102).\n";
	  } else
          if ( (value.compare("50201")==0) || (value.compare("17")==0) ) {
	    flagged.push_back(50201);
	    cout << "Flagged antenna 17 (id = 50201).\n";
	  } else
          if ( (value.compare("50202")==0) || (value.compare("18")==0) ) {
	    flagged.push_back(50202);
	    cout << "Flagged antenna 18 (id = 50202).\n";
	  } else
          if ( (value.compare("60101")==0) || (value.compare("19")==0) ) {
	    flagged.push_back(60101);
	    cout << "Flagged antenna 19 (id = 60101).\n";
	  } else
          if ( (value.compare("60102")==0) || (value.compare("20")==0) ) {
	    flagged.push_back(60102);
	    cout << "Flagged antenna 20 (id = 60102).\n";
	  } else
          if ( (value.compare("70101")==0) || (value.compare("21")==0) ) {
	    flagged.push_back(70101);
	    cout << "Flagged antenna 21 (id = 70101).\n";
	  } else
          if ( (value.compare("70102")==0) || (value.compare("22")==0) ) {
	    flagged.push_back(70102);
	    cout << "Flagged antenna 22 (id = 70102).\n";
	  } else
          if ( (value.compare("70201")==0) || (value.compare("23")==0) ) {
	    flagged.push_back(70201);
	    cout << "Flagged antenna 23 (id = 70201).\n";
	  } else
          if ( (value.compare("70202")==0) || (value.compare("24")==0) ) {
	    flagged.push_back(70202);
	    cout << "Flagged antenna 24 (id = 70202).\n";
	  } else
          if ( (value.compare("80101")==0) || (value.compare("25")==0) ) {
	    flagged.push_back(80101);
	    cout << "Flagged antenna 25 (id = 80101).\n";
	  } else
          if ( (value.compare("80102")==0) || (value.compare("26")==0) ) {
	    flagged.push_back(80102);
	    cout << "Flagged antenna 26 (id = 80102).\n";
	  } else
          if ( (value.compare("80201")==0) || (value.compare("27")==0) ) {
	    flagged.push_back(80201);
	    cout << "Flagged antenna 27 (id = 80201).\n";
	  } else
          if ( (value.compare("80202")==0) || (value.compare("28")==0) ) {
	    flagged.push_back(80202);
	    cout << "Flagged antenna 28 (id = 80202).\n";
	  } else
          if ( (value.compare("90101")==0) || (value.compare("29")==0) ) {
	    flagged.push_back(90101);
	    cout << "Flagged antenna 29 (id = 90101).\n";
	  } else
          if ( (value.compare("90102")==0) || (value.compare("30")==0) ) {
	    flagged.push_back(90102);
	    cout << "Flagged antenna 30 (id = 90102).\n";
	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "'Flagged = ' must be followed by a valid antenna id.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }

        if ( (keyword.compare("caltablepath")==0) || (keyword.compare("CalTablePath")==0) ) {
	  caltablepath = value;
	  cout << "CalTablePath set to \"" << caltablepath << "\".\n";
	}

        if ( (keyword.compare("rootfilename")==0) || (keyword.compare("Rootfilename")==0)
           || (keyword.compare("rootFileName")==0) || (keyword.compare("RootFilename")==0)) {
	  rootFileName = value;
	  cout << "RootFilename set to \"" << rootFileName << "\".\n";
	}

        if ( (keyword.compare("rootfilemode")==0) || (keyword.compare("Rootfilemode")==0)
           || (keyword.compare("rootFileMode")==0) || (keyword.compare("RootFileMode")==0)) {
          if ( (value.compare("overwrite")==0) || (value.compare("OVERWRITE")==0)
             || (value.compare("recreate")==0) || (value.compare("RECREATE")==0)) {
	    rootFileMode = "RECREATE";
	    cout << "RootFileMode set to RECREATE (existing root file will be overwritten).\n";
   	  } else {
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "RootFileMode must be RECREATE.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
        }
 
        if ( (keyword.compare("writeBadEvents")==0) || (keyword.compare("writebadevents")==0)
             || (keyword.compare("Writebadevents")==0) || (keyword.compare("WriteBadEvents")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    writeBadEvents = true;
	    cout << "writeBadEvents set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    calibrationMode = false;
	    cout << "writeBadEvents set to 'false'.\n";
	  } else{
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "writeBadEvents must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("calibration")==0) || (keyword.compare("calibration")==0)
             || (keyword.compare("calibrationmode")==0) || (keyword.compare("CalibrationMode")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    calibrationMode = true;
	    cout << "calibration set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    calibrationMode = false;
	    cout << "calibration set to 'false'.\n";
	  } else{
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "calibration must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("lateralOutputFile")==0) || (keyword.compare("lateraloutputfile")==0)
             || (keyword.compare("Lateraloutputfile")==0) || (keyword.compare("LateralOutputFile")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    lateralOutputFile = true;
	    cout << "lateralOutputFile set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    calibrationMode = false;
	    cout << "lateralOutputFile set to 'false'.\n";
	  } else{
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "lateralOutputFile must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

        if ( (keyword.compare("lateralDistribution")==0) || (keyword.compare("lateraldistribution")==0)
             || (keyword.compare("Lateraldistribution")==0) || (keyword.compare("LateralDistribution")==0)) {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) ) {
	    lateralDistribution = true;
	    cout << "lateralOutputFile set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) ) {
	    calibrationMode = false;
	    cout << "lateralDistribution set to 'false'.\n";
	  } else{
            cerr << "\nError processing file \"" << filename <<"\".\n" ;
            cerr << "lateralDistribution must be either 'true' or 'false'.\n";
            cerr << "\nProgram will continue skipping the problem." << endl;
          }
	}

      }	// while(configfile.good())

      // close config file
      configfile.close();
  } catch (AipsError x) {
    cerr << "call_pipeline:readConfigFile: " << x.getMesg() << endl;
  }
}


/*!
  \brief reads the next event from an eventlist file

  \param eventfilelistname     -- name of eventlist

  \return true if an event could be read in, false in case of EOF or error
*/

bool getEventFromEventlist (const string &eventfilelistname)
{
  static bool fileOpen = false;		// will be set to true, once an eventlist was openend
  static ifstream eventfilelist;

  try {
    // reset pipeline parameters before readin
    eventname ="";
    azimuth=0, elevation=0, radiusOfCurvature=0, core_x=0, core_y=0;

    // if this function is called for the first time, then try to open the file
    if (!fileOpen) {
      // open event file list
      eventfilelist.open (eventfilelistname.c_str(), ifstream::in);

      // check if file could be opened
      if (!(eventfilelist.is_open())) {
        cerr << "Failed to open file \"" << eventfilelistname <<"\"." << endl;
        return false;
      }

      cout << "Opened eventlist: " << eventfilelistname << endl;

      // store the file is open and read in the header
      fileOpen = true;
      // look for the end of the header (after a line containing only and at least three '-' or '=')
      string temp_read;
      bool header_found = false;
      while ((header_found == false) && (eventfilelist.good())) {
        eventfilelist >> temp_read;
        if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos))
        header_found = true;
      }
      //  if no header was found, assume that the file begins with an event and reopen it.
      if (header_found == false) {
        eventfilelist.close();  // close file
        cout << "\nWarning: No header found in file \"" << eventfilelistname <<"\".\n" ;
        cout << "Program will continue normally.\n" << endl;
        eventfilelist.open (eventfilelistname.c_str(), ifstream::in); // reopen file to start at the beginning
      }
    }

    // check if file is still good
    if (!(eventfilelist.good())) {
      cout << "\nFile \"" << eventfilelistname 
           << "\"is no longer good. Do not worry if this message appears after processing the last event."
           << endl;
      // close file and return to main()
      eventfilelist.close();
      fileOpen = false;
      return false;
    }

    // if the file is open, then try to get the next event
    bool read_in_error = false;
    if (fileOpen) {
      // read in event name
      if (eventfilelist.good()) eventfilelist >> eventname;
        else read_in_error = true;

      // check if end of file occured:
      // eventname should contain "", if file is terminated by a new line
      if (eventname == "") {
        cout << "\nFile \"" << eventfilelistname 
             << "\"seems to contain no more events."
             << endl;
        // close file and return to main()
        eventfilelist.close();
        fileOpen = false;
        return false;
      }

      // in calibration mode the eventfile list contains only the event name
      // otherwise read azimuth, elevation, radiusOfCurvature and shower core
      if ( !calibrationMode ) {
        if (eventfilelist.good()) eventfilelist >> azimuth;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> elevation;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> radiusOfCurvature;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> core_x;
          else read_in_error = true;
        if (eventfilelist.good()) eventfilelist >> core_y;
          else read_in_error = true;
      }
    }

    // return false if error occured and print error message
    if (read_in_error) {
      eventfilelist.close();  // close file
      fileOpen = false;

      cerr << "\nError processing file \"" << eventfilelistname <<"\".\n" ;
      cerr << "Use the following file format: \n\n";
      cerr << "some lines of description (any text)\n";
      cerr << "==================================\n";
      if (calibrationMode) {
        cerr << "eventfile1\n";
        cerr << "eventfile2\n";
      } else {
        cerr << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
        cerr << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]]\n";
      }
      cerr << "... \n" << endl;
      return false;  // go back to main
    } else {
      // if no error occured the pipeline parameters should contain the new values now
      return true;
    }
  } catch (AipsError x) {
    cerr << "call_pipeline:getFileFromEventlist: " << x.getMesg() << endl;
  }

  // normally should not get here, if so return false
  return false;
}


// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  string eventfilelistname;			         // Files to be read in
  Record results;					 // results of the pipeline

  // variables for reconstruction information (output of pipeline)
  unsigned int gt = 0;
  double CCheight, CCheight_NS;                          // CCheight will be used for EW polarization or ANY polarization
  double CCheight_error, CCheight_error_NS;
  bool CCconverged, CCconvergedNS;                       // is true if the Gaussian fit to the CCbeam converged
  double AzL, ElL, AzL_NS, ElL_NS;                       // Azimuth and Elevation
  double distanceResult = 0, distanceResultNS = 0;       // distance = radius of curvature
  double R_0 = 0, sigR_0 = 0, R_0_NS = 0, sigR_0_NS = 0; // R_0 from lateral distribution fit
  double eps = 0, sigeps = 0, eps_NS = 0, sigeps_NS = 0; // Epsilon from lateral distribution fit
  map <int,PulseProperties> rawPulsesMap;                // pulse properties of pules in raw data traces
  map <int,PulseProperties> calibPulsesMap;              // pulse properties of pules in calibrated data traces
  PulseProperties* rawPulses[MAX_NUM_ANTENNAS];          // use array of pointers to store pulse properties in root tree
  PulseProperties* calibPulses[MAX_NUM_ANTENNAS];        // use array of pointers to store pulse properties in root tree
  bool goodEW = false, goodNS = false;                   // true if reconstruction worked
  double rmsCCbeam, rmsCCbeam_NS;			 // rms values of the beams in remote region
  double rmsXbeam, rmsXbeam_NS;
  double rmsPbeam, rmsPbeam_NS;

  try {
    // allocate space for arrays with pulse properties
    for (int i=0; i < MAX_NUM_ANTENNAS; i++) {
      rawPulses[i] = new PulseProperties();
      calibPulses[i] = new PulseProperties();
    }


    cout << "\nStarting Program \"call_pipline\".\n\n" << endl;

    // check arguments of call_pipeline
    int i = 1;		//counter for arguments
    while (i < argc) {
      // get option and argument
      string option(argv[i]);
      i++;
      // look for keywords which require no argument
      if ( (option == "--help") || (option == "-help")
           || (option == "--h") || (option == "-h")) {
        cout << "call_pipeline runs the CR-Tools pipeline for LOPES events.\n"
             << "Possible options and arguments:\n"
             << "option        argument\n"
             << "--help                           Displays this help message\n"
             << "--in, -i      eventfilelist      A test file which contains a list of events to process (required)\n"
             << "--config, -c  configfile         A file containing configuration parameters how to process the events (optional)\n"
             << "\n\nUse the following file format for the eventlist: \n\n"
             << "some lines of text\n"
             << "===================================\n"
             << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
             << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n"
             << "... \n"
             << "\n\nUse the following file format for the config file\n"
             << "('=' must be seperated by spaces): \n\n"
             << "some lines of text\n"
             << "===================================\n"
             << "caltablepath = /home/schroeder/usg/data/lopes/LOPES-CalTable\n"
             << "path = /home/schroeder/lopes/events\n"
             << "RotatePos = true\n"
             << "GeneratePlots = true\n"
             << "SinglePlots = true\n"
             << "PlotRawData = false\n"
             << "GenerateSpectra = true\n"
             << "CalculateMaxima = false\n"
             << "listCalcMaxima=false\n"
             << "printShowerCoordinates=false\n"
             << "verbose = true\n"
             << "ignoredistance = true\n"
             << "simplexFit = true\n"
             << "doTVcal = default\n"
             << "doGainCal = true\n"
             << "doDispersionCal = true\n"
             << "doDelayCal = true\n"
             << "doRFImitigation = true\n"
             << "polarization = ANY\n"
             << "plotStart = -2.05e-6\n"
             << "plotStop = -1.60e-6\n"
             << "spectrumStart = 40e6\n"
             << "spectrumEnd = 80e6\n"
             << "upsamplingRate = 320e6\n"
             << "upsamplingExponent = 1\n"
             << "summaryColumns = 3\n"
             << "ccWindowWidth = 0.045e-6\n"
             << "flagged = 10101\n"
             << "flagged = 10102\n"
             << "rootfilename = output.root\n"
             << "rootfilemode = recreate\n"
             << "writebadevents = false\n"
             << "calibration = false\n"
             << "lateralDistribution = false\n"
             << "lateralOutputFile = false\n"
             << "... \n"
             << endl;
        return 0;	// exit here
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
        eventfilelistname = argument;
        continue;
      }

      if ( (option == "--config") || (option == "-config")
           || (option == "--c") || (option == "-c")) {
        cout << "Reading config data from file: " << argument << endl;
        readConfigFile(argument);
        continue;
      }

      // if loop comes here, the option was not found
      cerr << "ERROR: Invalid option: \"" << option << "\"\n";
      cerr << "Use --help for more information." << endl;
      return 1;			// Exit the program if argument is missing
    }

    // Check if options are set correctly
    if (eventfilelistname == "") {
      cerr << "ERROR: Please set an eventlist file with the --in option!\n";
      cerr << "Use --help for more information." << endl;
      return 1;
    }
    cout << "Processing eventlist: " << eventfilelistname << endl;

    // prepare output in root file
    TFile *rootfile=NULL;

    if (rootFileName != "") {
      // open root file and create tree structure
      rootfile = new TFile(rootFileName.c_str(),"RECREATE","Resulst of CR-Tools pipeline");

      // check if file is open
      if (rootfile->IsZombie()) {
        cerr << "\nError: Could not create file: " << rootFileName << "\n" << endl;
        return 1;		// exit program
      }
    }

    // create tree and tree structure (depends on chosen polarization)
    TTree roottree("T","LOPES");
    roottree.Branch("Gt",&gt,"Gt/i");	// GT as unsigned int

    // the following branches are not used in the calibration mode
    if ( !calibrationMode ) {
      roottree.Branch("Xc",&core_x,"Xc/D");
      roottree.Branch("Yc",&core_y,"Yc/D");
      roottree.Branch("AzIn",&azimuth,"AzIn/D");
      roottree.Branch("ElIn",&elevation,"ElIn/D");
      roottree.Branch("DistanceIn",&radiusOfCurvature,"DistanceIn/D");

      // one result, if polarization = ANY
      if (polarization == "ANY") {
        roottree.Branch("AzL",&azimuth,"AzL/D");
        roottree.Branch("ElL",&elevation,"ElL/D");
        roottree.Branch("Distance",&distanceResult,"Distance/D");	// radius of curvature
        roottree.Branch("CCheight",&CCheight,"CCheight/D");
        roottree.Branch("CCheight_error",&CCheight_error,"CCheight_error/D");
        roottree.Branch("CCconverged",&CCconverged,"CCconverged/B");
        roottree.Branch("goodReconstructed",&goodEW,"goodReconstructed/B");
        roottree.Branch("rmsCCbeam",&rmsCCbeam,"rmsCCbeam/D");
        roottree.Branch("rmsXbeam",&rmsXbeam,"rmsXbeam/D");
	roottree.Branch("rmsPbeam",&rmsPbeam,"rmsPbeam/D");
        if (lateralDistribution) {
          roottree.Branch("R_0",&R_0,"R_0/D");
          roottree.Branch("sigR_0",&sigR_0,"sigR_0/D");
          roottree.Branch("eps",&eps,"eps/D");
          roottree.Branch("sigeps",&sigeps,"sigeps/D");
        }
      }
      if ( (polarization == "EW") || (polarization == "BOTH")) {
        roottree.Branch("AzL_EW",&AzL,"AzL_EW/D");
        roottree.Branch("ElL_EW",&ElL,"ElL_EW/D");
        roottree.Branch("Distance_EW",&distanceResult,"Distance_EW/D");	// radius of curvature
        roottree.Branch("CCheight_EW",&CCheight,"CCheight_EW/D");
        roottree.Branch("CCheight_error_EW",&CCheight_error,"CCheight_error_EW/D");
        roottree.Branch("CCconverged_EW",&CCconverged,"CCconverged_EW/B");
        roottree.Branch("goodReconstructed_EW",&goodEW,"goodReconstructed_EW/B");
        roottree.Branch("rmsCCbeam_EW",&rmsCCbeam,"rmsCCbeam_EW/D");
	roottree.Branch("rmsXbeam_EW",&rmsXbeam,"rmsXbeam_EW/D");
	roottree.Branch("rmsPbeam_EW",&rmsPbeam,"rmsPbeam_EW/D");
        if (lateralDistribution) {
          roottree.Branch("R_0_EW",&R_0,"R_0_EW/D");
          roottree.Branch("sigR_0_EW",&sigR_0,"sigR_0_EW/D");
          roottree.Branch("eps_EW",&eps,"eps_EW/D");
          roottree.Branch("sigeps_EW",&sigeps,"sigeps_EW/D");
        }
      }
      if ( (polarization == "NS") || (polarization == "BOTH")) {
        roottree.Branch("AzL_NS",&AzL_NS,"AzL_NS/D");
        roottree.Branch("ElL_NS",&ElL_NS,"ElL_NS/D");
        roottree.Branch("Distance_NS",&distanceResultNS,"Distance_NS/D");	// radius of curvature
        roottree.Branch("CCheight_NS",&CCheight_NS,"CCheight_NS/D");
        roottree.Branch("CCheight_error_NS",&CCheight_error_NS,"CCheight_error_NS/D");
        roottree.Branch("CCconverged_NS",&CCconverged,"CCconverged_NS/B");
        roottree.Branch("goodReconstructed_NS",&goodNS,"goodReconstructed_NS/B");
        roottree.Branch("rmsCCbeam_NS",&rmsCCbeam_NS,"rmsCCbeam_NS/D");
	roottree.Branch("rmsXbeam_NS",&rmsXbeam_NS,"rmsXbeam_NS/D");
	roottree.Branch("rmsPbeam_NS",&rmsPbeam_NS,"rmsPbeam_NS/D");
        if (lateralDistribution) {
          roottree.Branch("R_0_NS",&R_0_NS,"R_0_NS/D");
          roottree.Branch("sigR_0_NS",&sigR_0_NS,"sigR_0_NS/D");
          roottree.Branch("eps_NS",&eps_NS,"eps_NS/D");
          roottree.Branch("sigeps_NS",&sigeps_NS,"sigeps_NS/D");
        }
      }
    } //if

    // Process events from event file list
    while ( getEventFromEventlist(eventfilelistname) ) {

      // Check if file exists
      fstream ftest( (path+eventname).c_str());
      if(!ftest.is_open()) {
        cerr << "Unable to open "<<path+eventname<<endl;
	continue;
      }
      ftest.close();

      // Create a plotprefix using the eventname
      string plotprefix = eventname;

      // delete the file ending
      if (plotprefix.find(".event") != string::npos)
        plotprefix.erase(plotprefix.find_last_of('.'));	
      // deletes the file path, if it exists	
      if (plotprefix.find("/") != string::npos)
       plotprefix.erase(0,plotprefix.find_last_of('/')+1);

      // set reconstruction flags to bad
      // if they are not set to true during reconstruction the event is not written into the root file
      goodEW = false;
      goodNS = false;

      // reset all resuls to 0
      gt = 0;
      CCheight = 0, CCheight_NS = 0;
      CCheight_error = 0, CCheight_error_NS = 0;
      CCconverged = 0, CCconvergedNS = 0;
      AzL = 0, ElL = 0, AzL_NS = 0, ElL_NS = 0;
      distanceResult = 0, distanceResultNS = 0;
      R_0 = 0, sigR_0 = 0, R_0_NS = 0, sigR_0_NS = 0;
      eps = 0, sigeps = 0, eps_NS = 0, sigeps_NS = 0;
      rmsCCbeam = 0, rmsXbeam = 0, rmsPbeam = 0;
      rmsCCbeam_NS = 0, rmsXbeam_NS = 0, rmsPbeam_NS = 0;


      // print information and process the event
      if (calibrationMode) {
        cout << "\nProcessing calibration event \"" << eventname << "\".\n" << endl;
      } else {
        cout << "\nProcessing event \"" << eventname << "\"\nwith azimuth " << azimuth << " °, elevation " << elevation
                  << " °, distance (radius of curvature) " << radiusOfCurvature << " m, core position X " << core_x
                  << " m and core position Y " << core_y << " m.\n" << endl;
      }

      // Set observatory record to LOPES
      Record obsrec;
      obsrec.define("LOPES",caltablepath);

      // check for the polarizations, if both polarizations are required
      // then the pipeline has to be called twice (once for EW and once for NS);
      // in this case an additional plotprefix is used
      string polPlotPrefix = "";

      if ( calibrationMode )
      {
        // initialize the pipeline
        analyseLOPESevent2 eventPipeline;
        eventPipeline.initPipeline(obsrec);

        // set parameters of pipeline
        eventPipeline.setPlotInterval(plotStart,plotStop);
        eventPipeline.setSpectrumInterval(spectrumStart,spectrumStop);
        eventPipeline.setFreqInterval(freqStart,freqStop);
        eventPipeline.setUpsamplingExponent(upsamplingExponent);

        // call the pipeline with an extra delay = 0.
        results = eventPipeline.CalibrationPipeline (path+eventname,
                                                     plotprefix,
                                                     generatePlots,
                                                     generateSpectra,
                                                     static_cast< Vector<int> >(flagged),
                                                     verbose,
                                                     doGainCal,
                                                     doDispersionCal,
                                                     false,		// never correct delays in calibration mode
                                                     doRFImitigation,
                                                     singlePlots,
                                                     PlotRawData,
                                                     CalculateMaxima);

        // make a postscript with a summary of all plots
        // if summaryColumns = 0 the method does not create a summary.
        eventPipeline.summaryPlot(plotprefix+"-summary",summaryColumns);

        // get the pulse properties
        rawPulsesMap = eventPipeline.getRawPulseProperties();
        calibPulsesMap = eventPipeline.getCalibPulseProperties();

        // adding results to variables (needed to fill them into the root tree)
        goodEW = results.asBool("goodReconstructed");
        gt = results.asuInt("Date");
      } else {
        if ( (polarization == "ANY") || (polarization == "EW") || both_pol) {
          if (both_pol) {
            cout << "Pipeline is started for East-West Polarization.\n" << endl;
            polPlotPrefix = "-EW";
            polarization = "EW";	// do EW here
          }

          // initialize the pipeline
          analyseLOPESevent2 eventPipeline;
          eventPipeline.initPipeline(obsrec);

          // set parameters of pipeline
          eventPipeline.setPlotInterval(plotStart,plotStop);
          eventPipeline.setSpectrumInterval(spectrumStart,spectrumStop);
          eventPipeline.setFreqInterval(freqStart,freqStop);
          eventPipeline.setCCWindowWidth(ccWindowWidth);
          eventPipeline.setUpsamplingExponent(upsamplingExponent);

          // call the pipeline with an extra delay = 0.
          results = eventPipeline.RunPipeline (path+eventname,
                                               azimuth,
                                               elevation,
                                               radiusOfCurvature,
                                               core_x,
                                               core_y,
                                               RotatePos,
                                               plotprefix+polPlotPrefix,
                                               generatePlots,
                                               generateSpectra,
                                               static_cast< Vector<int> >(flagged),
                                               verbose,
                                               simplexFit,
                                               0.,
                                               doTVcal,
                                               doGainCal,
                                               doDispersionCal,
                                               doDelayCal,
                                               doRFImitigation,
                                               upsamplingRate,
                                               polarization,
                                               singlePlots,
                                               PlotRawData,
                                               CalculateMaxima,
                                               listCalcMaxima,
                                               printShowerCoordinates,
                                               ignoreDistance);

          // make a postscript with a summary of all plots
          // if summaryColumns = 0 the method does not create a summary.
          eventPipeline.summaryPlot(plotprefix+polPlotPrefix+"-summary",summaryColumns);

          // create a special file for the lateral distribution output
          if (lateralOutputFile)
            eventPipeline.createLateralOutput("lateral"+polPlotPrefix+"-",results, core_x, core_y);

          // generate the lateral distribution and get resutls
          if (lateralDistribution) {
            eventPipeline.fitLateralDistribution("lateral"+polPlotPrefix+"-",results, core_x, core_y);
            R_0 = results.asDouble("R_0");
            sigR_0 = results.asDouble("sigR_0");
            eps = results.asDouble("eps");
            sigeps = results.asDouble("sigeps");
          }

          // get the pulse properties
          rawPulsesMap = eventPipeline.getRawPulseProperties();
          calibPulsesMap = eventPipeline.getCalibPulseProperties();

          // adding results to variables (needed to fill them into the root tree)
          goodEW = results.asBool("goodReconstructed");
          AzL = results.asDouble("Azimuth");
          ElL = results.asDouble("Elevation");
          distanceResult = results.asDouble("Distance");
          CCheight = results.asDouble("CCheight");
          CCheight_error = results.asDouble("CCheight_error");
          CCconverged = results.asBool("CCconverged");
	  rmsCCbeam = results.asDouble("rmsCCbeam");
	  rmsXbeam = results.asDouble("rmsXbeam");
	  rmsPbeam = results.asDouble("rmsPbeam");
          gt = results.asuInt("Date");
         }

        if ( (polarization == "NS") || both_pol) {
          if (both_pol) {
            cout << "Pipeline is started for North-South Polarization.\n" << endl;
            polPlotPrefix = "-NS";
            polarization = "NS";	// do NS here
          }

          // initialize the pipeline
          analyseLOPESevent2 eventPipeline;
          eventPipeline.initPipeline(obsrec);

          // set parameters of pipeline
          eventPipeline.setPlotInterval(plotStart,plotStop);
          eventPipeline.setSpectrumInterval(spectrumStart,spectrumStop);
          eventPipeline.setFreqInterval(freqStart,freqStop);
          eventPipeline.setCCWindowWidth(ccWindowWidth);
          eventPipeline.setUpsamplingExponent(upsamplingExponent);

          // call the pipeline with an extra delay = 0.
          results = eventPipeline.RunPipeline (path+eventname,
                                               azimuth,
                                               elevation,
                                               radiusOfCurvature,
                                               core_x,
                                               core_y,
                                               RotatePos,
                                               plotprefix+polPlotPrefix,
                                               generatePlots,
                                               generateSpectra,
                                               static_cast< Vector<int> >(flagged),
                                               verbose,
                                               simplexFit,
                                               0.,
                                               doTVcal,
                                               doGainCal,
                                               doDispersionCal,
                                               doDelayCal,
                                               doRFImitigation,
                                               upsamplingRate,
                                               polarization,
                                               singlePlots,
                                               PlotRawData,
                                               CalculateMaxima,
                                               listCalcMaxima,
                                               printShowerCoordinates,
					       ignoreDistance);

          /* make a postscript with a summary of all plots
           if summaryColumns = 0 the method does not create a summary. */
          eventPipeline.summaryPlot(plotprefix+polPlotPrefix+"-summary",summaryColumns);

          // create a special file for the lateral distribution output
          if (lateralOutputFile)
            eventPipeline.createLateralOutput("lateral"+polPlotPrefix+"-",results, core_x, core_y);

          // generate the lateral distribution
          if (lateralDistribution) {
            eventPipeline.fitLateralDistribution("lateral"+polPlotPrefix+"-",results, core_x, core_y);
            R_0_NS = results.asDouble("R_0");
            sigR_0_NS = results.asDouble("sigR_0");
            eps_NS = results.asDouble("eps");
            sigeps_NS = results.asDouble("sigeps");
          }

          // get the pulse properties and insert them into allready existing EW map
          // (which will be empty, if polarization = EW, but still existing
          map <int,PulseProperties> newPulses(eventPipeline.getRawPulseProperties());
          rawPulsesMap.insert(newPulses.begin(), newPulses.end()) ;
          newPulses = eventPipeline.getCalibPulseProperties();
          calibPulsesMap.insert(newPulses.begin(), newPulses.end()) ;

          // adding results to variables (needed to fill them into the root tree)
          goodNS = results.asBool("goodReconstructed");
          AzL_NS = results.asDouble("Azimuth");
          ElL_NS = results.asDouble("Elevation");
          distanceResultNS = results.asDouble("Distance");
          CCheight_NS = results.asDouble("CCheight");
          CCheight_error_NS = results.asDouble("CCheight_error");
          CCconvergedNS = results.asBool("CCconverged");
          rmsCCbeam_NS = results.asDouble("rmsCCbeam");
	  rmsXbeam_NS = results.asDouble("rmsXbeam");
	  rmsPbeam_NS = results.asDouble("rmsPbeam");
          gt = results.asuInt("Date");
        }
      }  // if...else (calibrationMode)


      // process pulse properties
      // delete arrays
      for (int i=0; i < MAX_NUM_ANTENNAS; i++) {
        *rawPulses[i] = PulseProperties();
        *calibPulses[i] = PulseProperties();
      }

      // fill information in array for raw pulses
      for ( map<int,PulseProperties>::iterator it=rawPulsesMap.begin() ; it != rawPulsesMap.end(); it++ ) {
        // check if antenna number lies in valid range
        if ( (it->second.antenna < 1) || (it->second.antenna >= MAX_NUM_ANTENNAS) ) {
          cerr << "\nWARNING: Antenna number in rawPulsesMap is out of range!" << endl;
        } else {
          *rawPulses[it->second.antenna-1] = it->second;
          // create branch name
          stringstream antNumber(""); 
          antNumber << it->second.antenna;
          string branchname = "Ant_" + antNumber.str() + "_raw.";
          // check if branch allready exists and if not, create it
          if (! roottree.GetBranchStatus(branchname.c_str()))
            roottree.Branch(branchname.c_str(),"PulseProperties",&rawPulses[it->second.antenna-1]);
        }
      }
      for ( map<int,PulseProperties>::iterator it=calibPulsesMap.begin() ; it != calibPulsesMap.end(); it++ )

      // fill information in array for calibrated pulses
      for ( map<int,PulseProperties>::iterator it=calibPulsesMap.begin() ; it != calibPulsesMap.end(); it++ ) {
        // check if antenna number lies in valid range
        if ( (it->second.antenna < 1) || (it->second.antenna >= MAX_NUM_ANTENNAS) ) {
          cerr << "\nWARNING: Antenna number in rawPulsesMap is out of range!" << endl;
        } else {
          *calibPulses[it->second.antenna-1] = it->second;
          // create branch name
          stringstream antNumber(""); 
          antNumber << it->second.antenna;
          string branchname = "Ant_" + antNumber.str() + "_cal.";
          // check if branch allready exists and if not, create it
          if (! roottree.GetBranchStatus(branchname.c_str()))
            roottree.Branch(branchname.c_str(),"PulseProperties",&calibPulses[it->second.antenna-1]);
        }
      }

      // write output to root tree
      if (rootFileName != "") {
        // check if event was reconstructed or if also bad events shall be written to the root file
        if (goodEW || goodNS || writeBadEvents) {
          cout << "Adding results to root tree and saving root file \"" << rootFileName << "\"\n" << endl;
          roottree.Fill();
          rootfile->Write("",TObject::kOverwrite);
        } else {
          cout << "WARNING: Event is not written into root file because as badly reconstructed." << endl;
        }
      }
    }

    // write and close root file
    if (rootFileName != "") {
      cout << "Closing root file: " << rootFileName << endl;
      rootfile->Close();
    }

    // free space
    for (int i=0; i < MAX_NUM_ANTENNAS; i++) {
      delete rawPulses[i];
      delete calibPulses[i];
    }

    cout << "\nPipeline finished successfully.\n" << endl;

  } catch (AipsError x) {
    cerr << "call_pipeline: " << x.getMesg() << endl;
  }

  return 0;
}

