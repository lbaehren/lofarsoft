/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Frank Schröder                                            *
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

  <h3>Prerequisites</h3>
  <br>
  You need at least one event list file (textfile) like the following example:<br>
  <br>
  <br>
  \verbatim
  Example event list<br>
  some lines of comments here<br>
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
  RotatePos              = true
  GeneratePlots          = true
  SinglePlots            = true
  PlotRawData            = false
  CalculateMaxima        = false
  listCalcMaxima         = false
  printShowerCoordinates = false
  verbose                = true
  simplexFit             = true
  doTVcal                = default
  plotStart              = -2.05e-6
  plotEnd                = -1.60e-6
  upsamplingRate         = 320e6
  upsamplingExponent     = 1
  summaryColumns         = 3
  ccWindowWidth          = 0.045e-6
  flagged                = 10101
  flagged                = 10102
  \endverbatim
  This example means:
  <ul>
    <li>the caltables are in /home/horneff/lopescasa/data/LOPES/LOPES-CalTable,
    <li>the given positions are in the Kascade coordinate system and must be
    rotated to the LOPES system,
    <li>there will be generated plots of the analaysed event,
    <li>produces a plot for each antenna,
    <li>the raw ADC values (FX) are not plotted,
    <li>calcutlates the time and the height of the maximum and the minimum in the plot range 
    <li>prints a list of the absolut maxima in a more user friendly was
    <li>prints the distance form the core to the antennas in shower coordinates
    <li>there will be more text output during the analysis,
    <li>the simplex fit of the arrival direction and radius of curvature is
    done,
    <li>the TV calibration will be done by default,
    <li>the plots start at -2.05 micro seconds and end at -1.60 micro seconds,
    <li>the Lopes data will be upsampled to a sampling rate of 320 MHz (see below),
    <li>the upsampling of the calibrated antenna fieldstrengthes will be done by
    a factor of 2^1 = 2,
    <li>there will by a summary postscript of all created plots,
    <li>time range to search for CC-beam-peak in lateral distribution studies is +/- 45 ns
    <li>the antennas 10101 and 10102 are not considered in the analysis.
  </ul>

  <h3>Example</h3>

  \verbatim
  ./call_pipeline eventlist.txt configs.txt
  \endverbatim

  <h3>Upsampling</h3>

  Currently there are to upsampling methods implemented:
  The one called by setting an upsampling exponent uses the Upsampling routines of LOPES-Star
  and has an effect only to the traces of single antennas.
  The upsampling due to setting an upsamplingRate greater than 160 MHz effects the whole analysis
  chain, including the CC-Beam. At a later point this method might be the only one used anymore.
*/



// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  string eventfilelistname, configfilename;	// Files to be read in
  bool configfile_exists = false;		// Set to true if name of config file was passed as parameter

  try {
    std::cout << "\nStarting Program \"call_pipline\".\n\n" << std::endl;

    // Check correct number of arguments (1 or 2 + program name = 2 or 3)
    if ((argc < 2) || (argc >3))
    {
      std::cerr << "Wrong number of arguments in call of \"call_pipeline\". The correct format is:\n";
      std::cerr << "call_pipeline eventfilelist [configfile]\n" << std::endl;
      return 1;				// Exit the program
    }

    // First argument should be the name of the file containing the list of event files
    eventfilelistname.assign(argv[1]);
    std::cout << "File containing list of event files: " << eventfilelistname << std::endl;

    // If a second argument is available it sould contain the name of the config file
    if (argc == 3) 
    {
      configfilename.assign(argv[2]);
      configfile_exists = true;
      std::cout << "Reading config data from file: " << configfilename << std::endl;
    }


    // Set default configuration values for the pipeline
    bool generatePlots = true;		// the plot prefix will be the name of the event file
    bool singlePlots = false;		// by default there are no single plots for each antenna
    bool PlotRawData = false;		// by default there the raw data are not plotted
    bool CalculateMaxima = false;	// by default the maxima are not calculated
    bool listCalcMaxima=false;    	// print calculated maxima in more user friendly way
    bool printShowerCoordinates=false;	// print the distance between antenna and shower core
    bool RotatePos = true; 		// should be true if coordinates are given in KASKADE frame
    bool verbose = true;
    bool simplexFit = true;
    int doTVcal = -1;			// 1: yes, 0: no, -1: use default	
    double plotStart = -2.05e-6;	// in seconds
    double plotEnd = -1.60e-6;		// in seconds
    double upsamplingRate = 0.;		// Upsampling Rate for new upsampling
    unsigned int upsamplingExponent = 0;// by default no upsampling will be done
    vector<Int> flagged;		// use of STL-vector instead of CASA-vector due to support of push_back()
    string caltablepath = "/home/schroeder/usg/data/lopes/LOPES-CalTable";
    unsigned int summaryColumns = 0;	// be default no summary of all plots
    double ccWindowWidth = 0.045e-6;	// width of window for CC-beam


    // Open config file if passed by programm call
    if (configfile_exists)
    {
      ifstream configfile;
      configfile.open (configfilename.c_str(), ifstream::in);

      // check if file could be opened
      if (!(configfile.is_open()))
      {
        std::cerr << "Failed to open file \"" << configfilename <<"\"." << std::endl;
        return 1;		// exit program
      }

      // look for the beginnig of the config data (after a line containing only and at least three '-' or '='	
      string temp_read;
      bool configs_found = false;
      while ((configs_found == false) && (configfile.good()))
      {
	configfile >> temp_read;
        if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos))
        {
	  configs_found = true;
        }
      }
	
      // print warning if no configs were found and countinue program using default values
      if (configs_found == false)
      {
        configfile.close();  // close file
        std::cerr << "\nWarning!";
        std::cerr << "\nNo config information was foung in file \"" << configfilename <<"\".\n" ;
        std::cerr << "Use the following file format ('=' seperated by spaces): \n\n";
        std::cerr << "some lines of text\n";
        std::cerr << "===================================\n";
        std::cerr << "caltablepath = /home/schroeder/usg/data/lopes/LOPES-CalTable\n";
        std::cerr << "RotatePos = true\n";
        std::cerr << "GeneratePlots = true\n";
        std::cerr << "SinglePlots = true\n";
        std::cerr << "PlotRawData = false\n";
        std::cerr << "CalculateMaxima = false\n";
        std::cerr << "listCalcMaxima=false\n";
        std::cerr << "printShowerCoordinates=false\n";
        std::cerr << "verbose = true\n";
        std::cerr << "simplexFit = true\n";
        std::cerr << "doTVcal = default\n";
        std::cerr << "plotBegin = -2.05e-6\n";
        std::cerr << "plotEnd = -1.60e-6\n";
        std::cerr << "upsamplingRate = 320e6\n";
        std::cerr << "upsamplingExponent = 1\n";
        std::cerr << "summaryColumns = 3\n";
        std::cerr << "ccWindowWidth = 0.045e-6\n";
        std::cerr << "flagged = 10101\n";
        std::cerr << "flagged = 10102\n";
        std::cerr << "... \n";
        std::cerr << "\nProgram will continue using default configuration values." << std::endl;
      }
      else while(configfile.good()) // read configurations if configs_found
      {
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
        if (equal_token.compare("=") != 0)
        {
          std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
          std::cerr << "No '=' was found after \"" << keyword << "\".\n";
          std::cerr << "\nProgram will continue skipping the problem." << std::endl;
        }

        // check keywords an set appropriate configurations

        if ( (keyword.compare("generateplots")==0) || (keyword.compare("GeneratePlots")==0) ||
             (keyword.compare("generatePlots")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    generatePlots = true;
	    std::cout << "GeneratePlots set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    generatePlots = false;
	    std::cout << "GeneratePlots set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "GeneratePlots must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("singleplots")==0) || (keyword.compare("SinglePlots")==0) ||
             (keyword.compare("singlePlots")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    singlePlots = true;
	    std::cout << "SinglePlots set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    singlePlots = false;
	    std::cout << "SinglePlots set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "SinglePlots must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("plotrawdata")==0) || (keyword.compare("PlotRawData")==0) ||
             (keyword.compare("plotRawdata")==0) || (keyword.compare("plotRawData")==0) )
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    PlotRawData = true;
	    std::cout << "PlotRawData set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    PlotRawData = false;
	    std::cout << "PlotRawData set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "PlotRawData must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("calculatemaxima")==0) || (keyword.compare("CalculateMaxima")==0) ||
             (keyword.compare("Calculatemaxima")==0) || (keyword.compare("calculateMaxima")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    CalculateMaxima = true;
	    std::cout << "CalculateMaxima set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    CalculateMaxima = false;
	    std::cout << "CalculateMaxima set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "CalculateMaxima must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("listcalcmaxima")==0) || (keyword.compare("ListCalcMaxima")==0) ||
             (keyword.compare("listCalcMaxima")==0) )
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    listCalcMaxima = true;
	    std::cout << "listCalcMaxima set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    listCalcMaxima = false;
	    std::cout << "listCalcMaxima set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "listCalcMaxima must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("printShowerCoordinates")==0) || (keyword.compare("printShowerCoordinates")==0) ||
             (keyword.compare("printShowerCoordinates")==0) )
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    printShowerCoordinates = true;
	    std::cout << "printShowerCoordinates set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    printShowerCoordinates = false;
	    std::cout << "printShowerCoordinates set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "printShowerCoordinates must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("rotatepos")==0) || (keyword.compare("RotatePos")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    RotatePos = true;
	    std::cout << "RotatePos set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    RotatePos = false;
	    std::cout << "RotatePos set to 'false'.\n";
          } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "RotatePos must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("verbose")==0) || (keyword.compare("Verbose")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    verbose = true;
	    std::cout << "Verbose set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    verbose = false;
	    std::cout << "Verbose set to 'false'.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "Verbose must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }	

        if ( (keyword.compare("simplexfit")==0) || (keyword.compare("simplexFit")==0) || (keyword.compare("SimplexFit")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    simplexFit = true;
	    std::cout << "SimplexFit set to 'true'.\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    simplexFit = false;
	    std::cout << "SimplexFit set to 'false'.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "SimplexFit must be either 'true' or 'false'.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
	}
      
        if ( (keyword.compare("dotvcal")==0) || (keyword.compare("doTVcal")==0) || (keyword.compare("DoTVCal")==0))
        {
          if ( (value.compare("true")==0) || (value.compare("True")==0) || (value.compare("1")==0) )
	  {
	    doTVcal = 1;
	    std::cout << "doTVcal set to 1 (TV calibration will be done.).\n";
	  } else
          if ( (value.compare("false")==0) || (value.compare("False")==0) || (value.compare("0")==0) )
	  {
	    doTVcal = 0;
	    std::cout << "doTVcal set to 0 (TV calibration won't be done.).\n";
	  } else
	  if ( (value.compare("default")==0) || (value.compare("Default")==0) || (value.compare("-1")==0) )
	  {
	    doTVcal = -1;
	    std::cout << "doTVcal set to -1 (default will be used).\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "doTVcal must be either -1 ('default'), 0 ('false') or 1 ('true').\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("plotStart")==0) || (keyword.compare("PlotStart")==0) || (keyword.compare("plotstart")==0))
        {
          double temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "double"
	  {
            plotStart = temp; 
	    std::cout << "PlotStart set to " << plotStart << " seconds.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "PlotStart must be of typ 'double'. \n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("plotEnd")==0) || (keyword.compare("PlotEnd")==0) || (keyword.compare("plotend")==0))
        {
          double temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "double"
	  {
            plotEnd = temp; 
	    std::cout << "PlotEnd set to " << plotEnd << " seconds.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "PlotEnd must be of typ 'double'. \n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("upsamplingRate")==0) || (keyword.compare("UpsamplingRate")==0) 
	  || (keyword.compare("upsamplingrate")==0))
        {
          double temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "double"
	  {
            upsamplingRate = temp; 
	    std::cout << "UpsamplingRate set to " << upsamplingRate << " Hz.\n";
            // For upsampling rates < 80 MHz no upsampling will be done,
            // but for upsampling rates between 80 and 160 MHz strange results
            // may occur. So show a warning in this case:
	    if ((upsamplingRate >= 80e6) && (upsamplingRate < 160e6))
              std::cerr << "WARNING: UpsamplingRate should be larger than 160 MHz to obtain useful results.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "UpsamplingRate must be of typ 'double'. \n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("upsamplingExponent")==0) || (keyword.compare("UpsamplingExponent")==0) 
	  || (keyword.compare("upsamplingexponent")==0))
        {
          unsigned int temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "int"
	  {
            upsamplingExponent = temp; 
	    std::cout << "UpsamplingExponent set to " << upsamplingExponent<< ".\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "UpsamplingExponent must be of typ 'unsignend int'. \n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("summarycolumns")==0) || (keyword.compare("summaryColumns")==0) 
	  || (keyword.compare("SummaryColumns")==0))
        {
          unsigned int temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "int"
	  {
            summaryColumns = temp; 
	    std::cout << "SummaryColumns set to " << summaryColumns << ".\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "SummaryColumns must be of typ 'unsignend int'. \n";
            std::cerr << "Use 'summaryColumns = 0' if you don't want to hava a summary postscrict.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("ccwindowwidth")==0) || (keyword.compare("CCWindowWidth")==0) || 
              (keyword.compare("CCwindowwidth")==0) || (keyword.compare("ccWindowWidth")==0))
        {
          double temp = 9999999;
          stringstream(value) >> temp; 

          if (temp != 9999999)  // will be false, if value is not of typ "double"
	  {
            ccWindowWidth = temp; 
	    std::cout << "ccWindowWidth set to " << ccWindowWidth << " seconds.\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "ccWindowWidth must be of typ 'double'. \n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }


	// flagg antennas
        if ( (keyword.compare("flagged")==0) || (keyword.compare("Flagged")==0))
        {
          if ( (value.compare("10101")==0) || (value.compare("1")==0) )
	  {
	    flagged.push_back(10101);
	    std::cout << "Flagged antenna 1 (id = 10101).\n";
	  } else
          if ( (value.compare("10102")==0) || (value.compare("2")==0) )
	  {
	    flagged.push_back(10102);
	    std::cout << "Flagged antenna 2 (id = 10102).\n";
	  } else
          if ( (value.compare("10201")==0) || (value.compare("3")==0) )
	  {
	    flagged.push_back(10201);
	    std::cout << "Flagged antenna 3 (id = 10201).\n";
	  } else
          if ( (value.compare("10202")==0) || (value.compare("4")==0) )
	  {
	    flagged.push_back(10202);
	    std::cout << "Flagged antenna 4 (id = 10202).\n";
	  } else
          if ( (value.compare("20101")==0) || (value.compare("5")==0) )
	  {
	    flagged.push_back(20101);
	    std::cout << "Flagged antenna 5 (id = 20101).\n";
	  } else
          if ( (value.compare("20102")==0) || (value.compare("6")==0) )
	  {
	    flagged.push_back(20102);
	    std::cout << "Flagged antenna 6 (id = 20102).\n";
	  } else
          if ( (value.compare("20201")==0) || (value.compare("7")==0) )
	  {
	    flagged.push_back(20201);
	    std::cout << "Flagged antenna 7 (id = 20201).\n";
	  } else
          if ( (value.compare("20202")==0) || (value.compare("8")==0) )
	  {
	    flagged.push_back(20202);
	    std::cout << "Flagged antenna 8 (id = 20202).\n";
	  } else
          if ( (value.compare("30101")==0) || (value.compare("9")==0) )
	  {
	    flagged.push_back(30101);
	    std::cout << "Flagged antenna 9 (id = 30101).\n";
	  } else
          if ( (value.compare("30102")==0) || (value.compare("10")==0) )
	  {
	    flagged.push_back(30102);
	    std::cout << "Flagged antenna 10 (id = 30102).\n";
	  } else
          if ( (value.compare("40101")==0) || (value.compare("11")==0) )
	  {
	    flagged.push_back(40101);
	    std::cout << "Flagged antenna 11 (id = 40101).\n";
	  } else
          if ( (value.compare("40102")==0) || (value.compare("12")==0) )
	  {
	    flagged.push_back(40102);
	    std::cout << "Flagged antenna 12 (id = 40102).\n";
	  } else
          if ( (value.compare("40201")==0) || (value.compare("13")==0) )
	  {
	    flagged.push_back(40201);
	    std::cout << "Flagged antenna 13 (id = 40201).\n";
	  } else
          if ( (value.compare("40202")==0) || (value.compare("14")==0) )
	  {
	    flagged.push_back(40202);
	    std::cout << "Flagged antenna 14 (id = 40202).\n";
	  } else
          if ( (value.compare("50101")==0) || (value.compare("15")==0) )
	  {
	    flagged.push_back(50101);
	    std::cout << "Flagged antenna 15 (id = 50101).\n";
	  } else
          if ( (value.compare("50102")==0) || (value.compare("16")==0) )
	  {
	    flagged.push_back(50102);
	    std::cout << "Flagged antenna 16 (id = 50102).\n";
	  } else
          if ( (value.compare("50201")==0) || (value.compare("17")==0) )
	  {
	    flagged.push_back(50201);
	    std::cout << "Flagged antenna 17 (id = 50201).\n";
	  } else
          if ( (value.compare("50202")==0) || (value.compare("18")==0) )
	  {
	    flagged.push_back(50202);
	    std::cout << "Flagged antenna 18 (id = 50202).\n";
	  } else
          if ( (value.compare("60101")==0) || (value.compare("19")==0) )
	  {
	    flagged.push_back(60101);
	    std::cout << "Flagged antenna 19 (id = 60101).\n";
	  } else
          if ( (value.compare("60102")==0) || (value.compare("20")==0) )
	  {
	    flagged.push_back(60102);
	    std::cout << "Flagged antenna 20 (id = 60102).\n";
	  } else
          if ( (value.compare("70101")==0) || (value.compare("21")==0) )
	  {
	    flagged.push_back(70101);
	    std::cout << "Flagged antenna 21 (id = 70101).\n";
	  } else
          if ( (value.compare("70102")==0) || (value.compare("22")==0) )
	  {
	    flagged.push_back(70102);
	    std::cout << "Flagged antenna 22 (id = 70102).\n";
	  } else
          if ( (value.compare("70201")==0) || (value.compare("23")==0) )
	  {
	    flagged.push_back(70201);
	    std::cout << "Flagged antenna 23 (id = 70201).\n";
	  } else
          if ( (value.compare("70202")==0) || (value.compare("24")==0) )
	  {
	    flagged.push_back(70202);
	    std::cout << "Flagged antenna 24 (id = 70202).\n";
	  } else
          if ( (value.compare("80101")==0) || (value.compare("25")==0) )
	  {
	    flagged.push_back(80101);
	    std::cout << "Flagged antenna 25 (id = 80101).\n";
	  } else
          if ( (value.compare("80102")==0) || (value.compare("26")==0) )
	  {
	    flagged.push_back(80102);
	    std::cout << "Flagged antenna 26 (id = 80102).\n";
	  } else
          if ( (value.compare("80201")==0) || (value.compare("27")==0) )
	  {
	    flagged.push_back(80201);
	    std::cout << "Flagged antenna 27 (id = 80201).\n";
	  } else
          if ( (value.compare("80202")==0) || (value.compare("28")==0) )
	  {
	    flagged.push_back(80202);
	    std::cout << "Flagged antenna 28 (id = 80202).\n";
	  } else
          if ( (value.compare("90101")==0) || (value.compare("29")==0) )
	  {
	    flagged.push_back(90101);
	    std::cout << "Flagged antenna 29 (id = 90101).\n";
	  } else
          if ( (value.compare("90102")==0) || (value.compare("30")==0) )
	  {
	    flagged.push_back(90102);
	    std::cout << "Flagged antenna 30 (id = 90102).\n";
	  } else
          {
            std::cerr << "\nError processing file \"" << configfilename <<"\".\n" ;
            std::cerr << "'Flagged = ' must be followed by a valid antenna id.\n";
            std::cerr << "\nProgram will continue skipping the problem." << std::endl;
          }
        }

        if ( (keyword.compare("caltablepath")==0) || (keyword.compare("CalTablePath")==0) )
        {
	  caltablepath = value;
	  std::cout << "CalTablePath set to \"" << caltablepath << "\".\n";
	}
 
      }	// while(configfile.good())

      // close config file
      configfile.close();
    } //if (configfile_exists)


    // open event file list
    ifstream eventfilelist;
    eventfilelist.open (eventfilelistname.c_str(), ifstream::in);

    // check if file could be opened
    if (!(eventfilelist.is_open()))
    {
      std::cerr << "Failed to open file \"" << eventfilelistname <<"\"." << std::endl;
      return 1;		// exit program
    }

    // look for the beginnig of the data (after a line containing only and at least three '-' or '='	
    string temp_read;
    bool data_found = false;
    while ((data_found == false) && (eventfilelist.good()))
    {
      eventfilelist >> temp_read;
      if ((temp_read.find("---") != string::npos) || (temp_read.find("===") != string::npos))
      {
	data_found = true;
      }
    }
	
    // exit program if no data are found	
    if (data_found == false)
    {
      eventfilelist.close();  // close file
      std::cerr << "\nNo list of event files found in file \"" << eventfilelistname <<"\".\n" ;
      std::cerr << "Use the following file format: \n\n";
      std::cerr << "some lines of text\n";
      std::cerr << "===================================\n";
      std::cerr << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
      std::cerr << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
      std::cerr << "... \n" << std::endl;
      return 1;		// exit program
    }

    // Process events from event file list
    while (eventfilelist.good())
    {	
      string filename, plotprefix;
      double azimuth, elevation, distance, core_x, core_y;
      bool read_in_error = false;
      
      // read in filename, azimuth, elevation, distance and log_energy	
      if (eventfilelist.good()) eventfilelist >> filename;
	else read_in_error = true;
      if (eventfilelist.good()) eventfilelist >> azimuth;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> elevation;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> distance;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> core_x;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> core_y;
	else read_in_error = true;	

      // check if end of file occured:
      // filename should contain "", if file is terminated be a new line
      if (filename == "") continue;	// go back to begin of while-loop 
					// eventfilelist.good() should be false now.

      // exit program if error occured
      if (read_in_error)		
      {
        eventfilelist.close();  // close file
	std::cerr << "\nError processing file \"" << eventfilelistname <<"\".\n" ;
	std::cerr << "Use the following file format: \n\n";
	std::cerr << "some lines of text\n";
	std::cerr << "===================================\n";
	std::cerr << "eventfile1 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
	std::cerr << "eventfile2 azimuth[°] elevation[°] distance(radius of curvature)[m] core_x[m] core_y[m]]\n";
	std::cerr << "... \n" << std::endl;
	return 1;		// exit program
      }
		
      // Create a plotprefix using the filename
      plotprefix = filename;
      // delete the file ending
      if (plotprefix.find(".event") != string::npos)
        plotprefix.erase(plotprefix.find_last_of('.'));	
      // deletes the file path, if it exists	
      if (plotprefix.find("/") != string::npos)
       plotprefix.erase(0,plotprefix.find_last_of('/')+1);

      // print information and process the event
      std::cout << "\nProcessing event \"" << filename << "\"\nwith azimuth " << azimuth << " °, elevation " << elevation
	<< " °, distance (radius of curvature) " << distance << " m, core position X " << core_x
	<< " m and core position Y " << core_y << " m.\n" << std::endl;

      // Initialize the pipeline
      analyseLOPESevent2 eventPipeline;
      Record obsrec,results;
      obsrec.define("LOPES",caltablepath);
      eventPipeline.initPipeline(obsrec);

      // set plot range and ccWindowWidth
      eventPipeline.setPlotInterval(plotStart,plotEnd);
      eventPipeline.setCCWindowWidth(ccWindowWidth);
   
      // set the upsampling coefficient (upsampling factor = 2^upsamplingExponent
      eventPipeline.setUpsamplingExponent(upsamplingExponent);

      // call the pipeline with an extra delay = 0.
verbose = true;
      results = eventPipeline.RunPipeline (filename, azimuth, elevation, distance, core_x, core_y, RotatePos,
                                           plotprefix, generatePlots, static_cast< Vector<int> >(flagged), verbose, 
                                           simplexFit, 0., doTVcal, upsamplingRate, singlePlots, PlotRawData, CalculateMaxima,
                                           listCalcMaxima, printShowerCoordinates); 

      // make a postscript with a summary of all plots
      // if summaryColumns = 0 the method does not create a summary.
      eventPipeline.summaryPlot(plotprefix+"-summary",summaryColumns);
    }

    // close file
    eventfilelist.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
 
  return 0;
}

