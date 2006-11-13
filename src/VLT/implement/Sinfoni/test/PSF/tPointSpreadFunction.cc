//#  tPointSpreadFunction.cc: Test program for the PointSpreadFunction class.
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
//#  $Id: tPointSpreadFunction.cc,v 1.6 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/PointSpreadFunction.h>
#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/Exception.h>
#include <Sinfoni/Assert.h>
#include <Sinfoni/Debug.h>
#include <iostream>

using namespace ASTRON::SINFONI;
using namespace ASTRON::SINFONI::PSF;
using namespace std;

int main()
{
  try {
    InputData& data = theInputData();
    data.openFile("target3_1.dat", InputData::ASCII);

    DBG_TRACE("Calculating psf (1st time).");
    const MatrixReal& psf1 = thePointSpreadFunction();

    DBG_TRACE("Calculating psf (2nd time).");
    const MatrixReal& psf2 = thePointSpreadFunction();

    Assert(isEqual(psf1,psf2));
    cout << setprecision(8) << thePointSpreadFunction() << endl;
  }
  catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  return 0;
}
