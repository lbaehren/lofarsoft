/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (horneff@astro.ru.nl)                               *
 *   Tim Huege (tim.huege@ik.fzk.de)                                       *
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

/*!
  \file reas2event.cc
  
  \ingroup CR_Applications
  
  \brief Convert reas simulation data into LOPES event files
  
  \author Andreas Horneffer, Tim Huege
  
  \date 2007/11/01

  <h3>Motivation</h3>

  <h3>Prerequisites</h3>

  <h3>Example</h3>

*/

#include <IO/simulation2event.h>
#include <sstream>

using CR::simulation2event;  // Namespace usage

using namespace CR;

int main(int argc, char** argv){

  cout << "\nreas2event V1.0 by Andreas Horneffer & Tim Huege\n";
  if ((argc < 6) || (argc >10))
  {
    cout << "\nUsage: reas2event <outfile> <simpath> <simname> <antlist> <date> [<caltabpath> <presync> <addnoise> <addtv>]\n\n" 
         << "outfile:     filename to write event to, e.g. newevent.event\n"
         << "simpath:     path where REAS simulation lies, e.g. /home/huege/sims\n"
         << "simname:     name of REAS parameter file (without extension), e.g. event15\n"
         << "antlist:     name of antenna list (without extension), e.g. lopes30\n"
	 << "date:        unique event date, e.g. 1136116800\n"
	 << "caltabpath:  path to caltables, defaults to /home/huege/lopescasa/data/LOPES/LOPES-CalTable\n"
	 << "presync:     number of samples in presync, defaults to 32768\n"
	 << "addnoise:    set to 0 to switch off Galactic noise, defaults to 1\n"
	 << "addtv:       set to 0 to switch off TV transmitter insertion, defaults to 1\n"
         << endl;
    return 10;
  }

  int nofFailed=0;
  String outfile(argv[1]);
  String SimPath(argv[2]);
  String SimName(argv[3]);
  String AntListName(argv[4]);
  uInt date;
  { // block to invalidate local variables
    stringstream ss;
    ss << argv[5];
    ss >> date;
  } // block to invalidate local variables
  String CalTablePath("/home/huege/lopescasa/data/LOPES/LOPES-CalTable");
  if (argc>6) { CalTablePath=argv[6]; }
  Int presync=32768;
  if (argc>7)
  {
    stringstream ss;
    ss << argv[7];
    ss >> presync;
  }
  Bool AddNoise=True;
  if (argc>8)
  {
    stringstream ss;
    ss << argv[8];
    ss >> AddNoise;
  }
  Bool AddTV=True;
  if (argc>9)
  {
    stringstream ss;
    ss << argv[9];
    ss >> AddTV;
  }
  try {
    simulation2event newsimulation2event;
    //
    
    CalTableReader CTread(CalTablePath);
    newsimulation2event.setCalTable(&CTread);
    
    if ( !newsimulation2event.generateEvent(outfile, SimName, AntListName, SimPath,
					    date, presync, AddTV, AddNoise) ){
      nofFailed++;
    };
				      
				      
  } catch (std::string message) {
    std::cerr << "reas2event: " << message << std::endl;
    nofFailed++;
  }

  return nofFailed;
}
