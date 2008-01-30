/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
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

#include "Analysis/analyseLOPESevent.h"

using CR::analyseLOPESevent;
using CR::LopesEventIn;

/*!
  \file call_pipeline.cc

  \ingroup apps

  \brief Calls the LOPES analysis pipeline in "analyseLOPESevent"
 
  \author Frank Schröder
 
  \date 2008/29/01

  <h3>Motivation</h3>

  The aim of this short application is to provide a tool which
  makes the LOPES-analysis-pipeline usable without changing any
  code. Any information about the LOPES-events you want to analyse
  and any configurations how to analyse the should be provide
  be external, human readable files.

  <h3>Prerequisites</h3>

  You need at least one event list file of the following format:


  Example event list

  filename(event) azimuth[°] zenith[°] distance(radius of curvature)[m] core_x[m] core_y[m]
  =========================================================================================
  /home/schroeder/data/lopesstar/first.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/second.event 354.55215 63.882182 2750 8.6060886 -368.0933
  /home/schroeder/data/lopesstar/third.event 354.55215 63.882182 2750 8.6060886 -368.0933


  If you don't want to process the events in the default manner
  you can also provide a config file of the following format:


  example configuration file

  =================================================================
  caltablepath = /home/horneff/lopescasa/data/LOPES/LOPES-CalTable
  RotatePos = true
  GeneratePlots = true
  verbose = true
  simplexFit = true
  doTVcal = default
  flagged = 10101
  flagged = 10102

  <h3>Example</h3>

  \verbatim
  call_pipeline eventlist.txt configs.txt
  \endverbatim

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
    bool RotatePos = true; 		// should be true if coordinates are given in KASKADE frame
    bool verbose = true;
    bool simplexFit = true;
    int doTVcal = -1;			// 1: yes, 0: no, -1: use default	
    vector<Int> flagged;		// use of STL-vector instead of CASA-vector due to support of push_back()
    string caltablepath = "/home/horneff/lopescasa/data/LOPES/LOPES-CalTable";


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
        std::cerr << "caltablepath = /home/horneff/lopescasa/data/LOPES/LOPES-CalTable\n";
        std::cerr << "RotatePos = true\n";
        std::cerr << "GeneratePlots = true\n";
        std::cerr << "verbose = true\n";
        std::cerr << "simplexFit = true\n";
        std::cerr << "doTVcal = default\n";
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
      std::cerr << "eventfile1 azimuth[°] zenith[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
      std::cerr << "eventfile2 azimuth[°] zenith[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
      std::cerr << "... \n" << std::endl;
      return 1;		// exit program
    }


    // Initialize the pipeline
    analyseLOPESevent eventPipeline;
    Record obsrec,results;
    obsrec.define("LOPES",caltablepath);
    eventPipeline.initPipeline(obsrec);


    // Process events from event file list
    string filename, plotprefix;
    double azimuth, zenith, distance, core_x, core_y;
    bool read_in_error = false;

    while (eventfilelist.good())
    {	
      // read in filename, azimuth, zenith, distance and log_energy	
      if (eventfilelist.good()) eventfilelist >> filename;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> azimuth;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> zenith;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> distance;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> core_x;
	else read_in_error = true;	
      if (eventfilelist.good()) eventfilelist >> core_y;
	else read_in_error = true;	

      // exit program if error occured
      if (read_in_error)		
      {
        eventfilelist.close();  // close file
	std::cerr << "\nError processing file \"" << eventfilelistname <<"\".\n" ;
	std::cerr << "Use the following file format: \n\n";
	std::cerr << "some lines of text\n";
	std::cerr << "===================================\n";
	std::cerr << "eventfile1 azimuth[°] zenith[°] distance(radius of curvature)[m] core_x[m] core_y[m]\n";
	std::cerr << "eventfile2 azimuth[°] zenith[°] distance(radius of curvature)[m] core_x[m] core_y[m]]\n";
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
      std::cout << "\nProcessing event \"" << filename << "\"\nwith azimuth " << azimuth << " °, zenith " << zenith
	<< " °, distance (radius of curvature) " << distance << " m, core position X " << core_x
	<< " m and core position Y " << core_y << " m.\n" << std::endl;

      // call the pipeline with an extra delay = 0.
      results = eventPipeline.ProcessEvent(filename, azimuth, zenith, distance, core_x, core_y, RotatePos, 
		plotprefix, generatePlots, (Vector<int>)flagged, verbose, simplexFit, 0., doTVcal); 
    }

    // close file
    eventfilelist.close();
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
 
  return 0;
}
