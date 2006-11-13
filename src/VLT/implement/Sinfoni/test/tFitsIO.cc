//#  tFitsIO.cc: one line description
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
//#  $Id: tFitsIO.cc,v 1.4 2006/05/19 16:03:24 loose Exp $

#include <CCfits/CCfits>

using namespace CCfits;
using namespace std;

void readHeader(const string& filename, const string& extension)
{
  FITS file(filename, Read, extension);
  ExtHDU& table = file.extension(extension);
  table.readAllKeys();
  cout << table << endl;
  size_t nrAxes(table.axes());
  cout << "This extension has " << nrAxes << (nrAxes == 1 ? " axis" : " axes")
       << endl;
  for(size_t i=0; i<nrAxes; i++) {
    cout << "Dimension of axis(" << i << ") = " << table.axis(i) << endl;
  }
}

int main(int argc, const char* argv[])
{
  string filename("SINFONI_IFS_DARK093_0001.fits");
  string extension("PSF Data");
  if (argc > 1) filename = argv[1];
  if (argc > 2) extension = argv[2];

  FITS::setVerboseMode(true);
  try {
    readHeader(filename, extension);
  } catch (FitsException& e) {
    cerr << "Caught FitsException" << endl;
    return 1;
  }
  return 0;
}
