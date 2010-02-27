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
  ./tcheckNoiseInfluence
  \endverbatim

  <h3>Prerequisites</h3>

  one event containing a test pulse
  noise events
*/

const string pulseEvent="2009.08.10.12:18:07.598.event";
const string noiseEvent="2009.06.15.19:42:11.987.event";
const int pulseAnt = 1;
const double pulseStart = 0.9e-6;
const double pulseStop  = 1.5e-6;
// const double pulseStart = -1e-5;
// const double pulseStop  = 1e-5;
const int upsamplingExponent = 3;

int main (int argc, char *argv[])
{
  try {
    cout << "\nStarting tcheckNoiseInfluence.\n" << endl;
    
    // create instance of checkNoiseInfluence and intialize it for LOPES
    checkNoiseInfluence noisetest;
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);
    noisetest.initPipeline(obsrec);
    
    // load pulse pattern from calibration event
    noisetest.setPlotInterval(pulseStart, pulseStop);
    noisetest.setUpsamplingExponent(upsamplingExponent);
    noisetest.setCCWindowWidth(1e-7);
    noisetest.loadPulsePattern(pulseEvent, pulseAnt, pulseStart, pulseStop);
    
    // load noise event
    noisetest.loadNoiseEvent(noiseEvent);

    cout << "\nEnd of program.\n" << endl;
  } catch (AipsError x) {
    cerr << "tcheckNoiseInfluence: " << x.getMesg() << endl;
  }

  return 0;
}

