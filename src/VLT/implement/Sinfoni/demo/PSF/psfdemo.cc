//#  psfdemo.cc: Demo program for the Sinfoni/PSF software library
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: psfdemo.cc,v 1.5 2006/05/19 16:03:23 loose Exp $

#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/PointSpreadFunction.h>
#include <Sinfoni/PSF/QualityMetrics.h>
#include <Sinfoni/Exception.h>
#include <iostream>

using namespace ASTRON::SINFONI;
using namespace ASTRON::SINFONI::PSF;
using namespace std;

void intro()
{
  cerr << endl
       << "**********************************************************\n" 
       << "*  This program demonstrates the use of the Sinfoni/PSF  *\n"
       << "*                 C++ software library.                  *\n"
       << "**********************************************************\n" 
       << endl;
}


void usage()
{
  cerr << "Usage: psfdemo <filename> <filetype>\n"
       << "  where <filename> is the name of the input datafile,\n"
       << "  and <filetype> is either ASCII or FITS.\n"
       << endl;
}


int main(int argc, const char* const argv[])
{
  intro();
  if (argc < 3) {
    usage();
    return 1;
  }
  string filename(argv[1]);
  
  //# Starting from here, Sinfoni/PSF specific exceptions may be thrown.
  try {

    //# Create an input data object. InputData acts like a singleton; we use
    //# theInputData() to get a reference to it.
    InputData& data = theInputData();

    //# Input data can either be in ASCII or FITS format.
    InputData::DataFormat format(InputData::UNKNOWN);

    if (!strcmp(argv[2], "ASCII") || !strcmp(argv[2], "ascii")) {
      format = InputData::ASCII;
    }
    if (!strcmp(argv[2], "FITS")  || !strcmp(argv[2], "fits")) {
      format = InputData::FITS;
    }

    //# Open an input data file. In a future release of the Sinfoni/PSF
    //# library it should be possible to call openFile() multiple times with
    //# different arguments. This is currently not supported.
    data.openFile(filename, format);

    //# Calculate the estimated normalized point spread function and dump the
    //# contents of the psf matrix to stdout.
    cout << thePointSpreadFunction() << endl;

    //# Calculate the quality metrics for the PSF estimation and dump them to
    //# stdout.
    cout << theQualityMetrics() << endl;

  }

  //# Catch any Sinfoni/PSF specific exception and show the message on stderr.
  catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }

  return 0;
}
