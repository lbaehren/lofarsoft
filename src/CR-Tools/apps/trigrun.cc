/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Sven Lafebre ()                                                       *
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
  \file trigrun.cc

  \ingroup CR_Applications

  \brief Search for peaks in data

  \author Sven Lafebre
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
    <li>LopesEvent
    <li>PeakSearch
    <li>DataReader
  </ul>
*/

#include <iostream>
#include <sstream>
#include <popt.h>

#include <scimath/Mathematics/FFTServer.h>

#include <IO/DataReader.h>
#include <IO/LopesEvent.h>
#include <Analysis/PeakSearch.h>

using namespace std;

int main (int argc, const char** argv)
 {
  int arg_blocksize =  6;
  int arg_threshold =  6;
  int arg_delay     =  0;
  int arg_window    = 50;
  int arg_antennas  =  6;
  
  static const struct poptOption params[] = {
    {	"blocksize", 'b', POPT_ARG_INT,  &arg_blocksize,    0,
	"Block size for calculation of average.",
	NULL },
    {	"threshold", 't', POPT_ARG_INT,  &arg_threshold,    0,
	"Trigger level over which to report peaks.",
	NULL },
    {	"delay",     'd', POPT_ARG_INT,  &arg_delay,        0,
	"The required number of antennas reporting a detection to"
	"produce a trigger.",
	NULL },
    {	"window",    'w', POPT_ARG_INT,  &arg_window,       0,
	"The window width (in samples) within which the detection"
	"may occur.",
	NULL },
    {	"antennas",  'a', POPT_ARG_INT,  &arg_antennas,     0,
	"The required number of antennas reporting a detection to"
	"produce a trigger.",
	NULL },
    POPT_TABLEEND
  };
  
  poptContext optcon = poptGetContext(NULL,
				      argc, argv,
				      params,
				      POPT_CONTEXT_NO_EXEC |
				      POPT_CONTEXT_KEEP_FIRST);
  
  int rc = poptGetNextOpt(optcon);
  
  if (rc < -1) {
    cerr << "  WARNING: " << poptStrerror(rc)
	 << " (" << poptBadOption(optcon, POPT_BADOPTION_NOALIAS) << ")." << endl;
  }
  
  const char **args = poptGetArgs(optcon);
  uint file = 1;
  if (args == NULL || args[file] == NULL) {
    cout << "Usage: trigrun [options] file1 [file2 [...]]" << endl
	 << "       where file<n> is a LOPES event file." << endl;
    return 0;
  } else {
    while (args[file] != NULL) {
      cout << args[file] << endl;
      
      try {
	CR::LopesEvent l(args[file]);
	CR::PeakSearch s(arg_blocksize, arg_threshold, arg_delay);
	CR::PeakList p[l.length()];

	for (uint i = 0; i < l.length(); i ++) {
	  p[i] = s.findPeaks(l[i]);
	}
	s.corrPeaks(p, l.length(), arg_window, arg_antennas);
      } catch (string str) {
	cerr << "  ERROR: " << str << endl;
      }
      file ++;
    }
  }
  poptFreeContext(optcon); // Release optcon.
  cerr << "Done." << endl;
  return 0;
}

