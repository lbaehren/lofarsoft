/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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
#include <popt.h>

#include <crtools.h>
#include <Imaging/Skymapper.h>

using std::cout;
using std::endl;

/*!
  \file skymapper.cc

  \ingroup CR_Applications
  
  \brief Application front-end to the Skymapper class

  \author Lars B&auml;hren

  \date 2008/09/08

  <h3>Prerequisite</h3>
  
  <ul>
    <li>CR::Skymapper
    <li>CR::Beamformer
  </ul>

  <h3>Synopsis</h3>
  
  While the main functionality is implemented through the classes collected in
  \e Coordinates and \e Imaging, this is the executable expected to be run by
  the prospective user. 
*/

int main (int argc,
	  const char **argv)
{
  int status (0);

  // -----------------------------------------------------------------
  // Parsing of the command-line parameters

  std::string infile     = "";
  std::string outfile    = "skymap.h5";
  int blocksize          = 1024;
  int arg_blocksPerFrame     = 1;
  int nofFrames          = 1;
  std::string refcode    = "AZEL";
  std::string projection = "STG";

  static const struct poptOption params[] = {
    {	"infile", 'i', POPT_ARG_STRING,  &infile,    0,
	"Input data file from which to generate the image.",
	NULL },
    {	"outfile", 'o', POPT_ARG_STRING,  &outfile,    0,
	"Output image file.",
	NULL },
    {	"blocksize", 'b', POPT_ARG_INT,  &blocksize,    0,
	"Size of an individual block of data read from file, [samples].",
	NULL },
    {	"blocks_per_frame", 'f', POPT_ARG_INT,  &arg_blocksPerFrame,    0,
	"Number of input data blocks combined into an output time-frame.",
	NULL },
    {	"frames", 't', POPT_ARG_INT,  &nofFrames,    0,
	"Number of time-frames in generated image.",
	NULL },
    {	"refcode", 'r', POPT_ARG_STRING,  &refcode,    0,
	"Reference code for the celestial coordinate system.",
	NULL },
    {	"projection", 'p', POPT_ARG_STRING,  &projection,    0,
	"Reference code for the spherical map projection.",
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
  
  // -----------------------------------------------------------------
  // Report control parameters

  cout << "-- blocksize                 = " << blocksize      << endl;
  cout << "-- Blocks per time-frame     = " << arg_blocksPerFrame << endl;
  cout << "-- nof. time frames          = " << nofFrames      << endl;
  cout << "-- Celestial reference frame = " << refcode        << endl;
  cout << "-- Spherical map projection  = " << projection     << endl;

  return status;
}
