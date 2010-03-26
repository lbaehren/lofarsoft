/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
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
#include <crtools.h>

#include <Analysis/lateralDistribution.h>
#include <Analysis/PulseProperties.h>

/*!
  \file compareLOPES2sim.cc

  \ingroup CR_Applications

  \brief Compares lateral distribution of LOPES events to REAS simulation

  \author Frank Schr&ouml;der

  \date 2008/20/05

  <h3>Motivation</h3>

 
  <h3>Usage</h3>

  \verbatim
  ./compareLOPES2sim arguments
  \endverbatim

  <ul>
    Optional argumens:
    <li> --help                           prints a help message
  </ul>

  <h3>Prerequisites</h3>

  ROOT
  
  <h3>Examples</h3>

  \verbatim
  ./compareLOPES2sim --help
  \endverbatim
*/


// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
  try {
    cout << "\nStarting compareLOPES2sim.\n" << endl;
    

    // check arguments
    for (int i=1; i < argc; ++i) {
      // get option and argument
      string option(argv[i]);
      i++;
      // look for keywords which require no argument
      if ( (option == "--help") || (option == "-help")
           || (option == "--h") || (option == "-h")) {
        cout << "Use: ./compareLOPES2sim --help"
             << endl;
        return 0;       // exit here
      }
    }  
  } catch (AipsError x) {
    cerr << "tcheckNoiseInfluence: " << x.getMesg() << endl;
  }
  
  return 0;
}

