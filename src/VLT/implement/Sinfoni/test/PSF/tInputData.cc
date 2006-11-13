//#  tInputData.cc: Test program for the InputData(Rep) classes.
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
//#  $Id: tInputData.cc,v 1.9 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/Exception.h>
#include <iostream>

using namespace ASTRON::SINFONI;
using namespace ASTRON::SINFONI::PSF;
using namespace std;

int read(const string& filename, InputData::DataFormat filetype)
{
  try {
    cerr << "\n********************************************************\n"
         << "Reading file " << filename << " ..." << endl;
    InputData& data = theInputData();
    data.openFile(filename, filetype);
    cout << setprecision(8);
    cout << "\nsumCountsMean :\n"       << data.sumCountsMean()       << endl;
    cout << "\nsumCountsVariance :\n"   << data.sumCountsVariance()   << endl;
    cout << "\ndiffCountsMean :\n"      << data.diffCountsMean()      << endl;
    cout << "\ndiffCountsVariance :\n"  << data.diffCountsVariance()  << endl;
    cout << "\ncurvatureMean :\n"       << data.curvatureMean()       << endl;
    cout << "\ncurvatureCovariance :\n" << data.curvatureCovariance() << endl;
    cout << "\nmirrorMean :\n"          << data.mirrorMean()          << endl;
    cout << "\nmirrorCovariance :\n"    << data.mirrorCovariance()    << endl;
    cout << "\ngainMean :\n"            << data.gainMean()            << endl;
    cout << "\ngainVariance :\n"        << data.gainVariance()        << endl;
    cout << endl;
  }
  catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  return 0;
}

int main()
{
  bool fail(false);
//   fail |= read("SINFONI_IFS_DARK093_0001.fits", InputData::FITS);
  fail |= read("target3_1.dat", InputData::ASCII);
  return fail;
}
