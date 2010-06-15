/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Frank Schröder                                                        *
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

// #include <iostream>
#include <string>
// #include <fstream>
// #include <iomanip>
// #include <sstream>

#include <crtools.h>
#include <Analysis/checkNoiseInfluence.h>
// #include <Analysis/PulseProperties.h>

using CR::checkNoiseInfluence;


/*!
  \file checkNoiseInfluence.cc

  \ingroup Analysis

  \brief Checks the influence of noise on the pulse height

  \author Frank Schr&ouml;der

  \date 2010/Jan/11

  <h3>Motivation</h3>

  The aim of this application is to resolve the question how typical
  LOPES radio noise affects the height of a radio pulse.

  <h3>Usage</h3>

  \verbatim
  ./tcheckNoiseInfluence -p pulseEvent.event -n noiseEvent.event
  \endverbatim

  <h3>Prerequisites</h3>

  one event containing a test pulse
  noise events
*/

string pulseEvent="";
string noiseEvent="";
const int pulseAnt = 1;
const double pulseStart = -4e-6;
// const double pulseStart = -1e-5;
// const double pulseStop  = 1e-5;
const int upsamplingExponent = 5;
const int NnoiseIntervals = 20; // number of noise intervals (do not use more than 20)
string resultFileName = "summary.dat";

//_______________________________________________________________________________
//                                                                           main

int main (int argc, char *argv[])
{
  try {
    cout << "\nStarting tcheckNoiseInfluence.\n" << endl;
    
    // check arguments
    for (int i=1; i < argc; ++i) {
      // get option and argument
      string option(argv[i]);
      i++;
      // look for keywords which require no argument
      if ( (option == "--help") || (option == "-help")
           || (option == "--h") || (option == "-h")) {
        cout << "Use: ./tcheckNoiseInfluence -p pulseEvent.event -n noiseEvent.event"
             << endl;
        return 0;       // exit here
      }

      // now look for options which require an argument and get the argument
      if (i >= argc) {
        cerr << "ERROR: No argument given for option or unknown option: \"" << option << "\"\n";
        cerr << "Use --help for more information." << endl;
        return 1;                       // Exit the program if argument is missing
      }
      string argument(argv[i]);

      // look for keywords which require an option
      if ( (option == "--pulse") || (option == "-pulse")
           || (option == "--p") || (option == "-p")) {
        pulseEvent = argument;
        continue;
      }

      // look for keywords which require an option
      if ( (option == "--noise") || (option == "-noise")
           || (option == "--n") || (option == "-n")) {
        noiseEvent = argument;
        continue;
      }
      // if loop comes here, the option was not found
      cerr << "ERROR: Invalid option: \"" << option << "\"\n";
      cerr << "Use --help for more information." << endl;
      return 1;                 // Exit the program if argument is missing
    }

    // Check if options are set correctly
    if (noiseEvent == "") {
      cerr << "ERROR: Please specify noise event with -n option!\n";
      cerr << "Use --help for more information." << endl;
      return 1;
    }

    // create instance of checkNoiseInfluence and intialize it for LOPES
    checkNoiseInfluence noisetest;
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);
    noisetest.initPipeline(obsrec);
    noisetest.setUpsamplingExponent(upsamplingExponent);
    noisetest.setCCWindowWidth(1e-7);
    noisetest.setNnoiseIntervals(NnoiseIntervals);

    if (pulseEvent == "") {
      cout << "Looking to signal-to-noise ratio of pure noise..." << endl;
      
      noisetest.SNRofNoise(noiseEvent);
      //noisetest.loadNoiseEvent(noiseEvent);    
      //noisetest.plotNoise("noiseSteps");
    } else { 
      cout << "Looking how pulse is changed by noise..." << endl;
    
      noisetest.loadPulsePattern(pulseEvent, pulseAnt, pulseStart);
      noisetest.loadNoiseEvent(noiseEvent);
      noisetest.addPulseToNoise(0, 6, "", "exponential");
    }  

    cout << "\nEnd of program.\n" << endl;
  } catch (AipsError x) {
    cerr << "tcheckNoiseInfluence: " << x.getMesg() << endl;
  }

  return 0;
}

