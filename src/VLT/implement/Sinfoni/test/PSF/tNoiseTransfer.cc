//#  tNoiseTransfer.cc: Check calculation of the noise transfer function.
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
//#  $Id: tNoiseTransfer.cc,v 1.6 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/NoiseTransfer.h>
#include <Sinfoni/Exception.h>
#include <Sinfoni/Types.h>
#include <iostream>
#include <iomanip>

using namespace ASTRON::SINFONI::PSF;
using namespace ASTRON::SINFONI;
using namespace std;

int main()
{
  for(Real g=-0.01; g<1.18; g+=0.02) {
    try {
      Real Hn(noiseTransfer(g));
      cout << fixed << setprecision(2) << "noiseTransfer(" << g << ") = "
           << setw(12) << setprecision(8) << right << Hn << endl;
    } catch (Exception& e) {
      cerr << "Warning: " << e.what() << endl;
    }
  }
  return 0;
}
