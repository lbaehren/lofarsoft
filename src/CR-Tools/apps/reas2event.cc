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

using CR::simulation2event;  // Namespace usage

using namespace CR;

int main(int argc, char *argv[]){

  int nofFailed=0;
  String CalTablePath("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
  String outfile("reas2event.event");
  String SimName("30deg1e17_shift");
  String AntListName("lopes30");
  String SimPath("/home/horneff/testreas");
  uInt date=1136116800;
  Int presync=32768;
  Bool AddTV=True, AddNoise=True;
    
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
