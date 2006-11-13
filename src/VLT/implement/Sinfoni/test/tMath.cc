//#  tMath.cc: Test program for the Sinfoni math extensions.
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
//#  $Id: tMath.cc,v 1.7 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/Math.h>
#include <Sinfoni/Types.h>
#include <Sinfoni/Assert.h>
#include <iostream>

using namespace ASTRON::SINFONI;
using namespace std;

int main()
{
  Real eps(2*numeric_limits<Real>::epsilon());

  cout << boolalpha << setprecision(2*(sizeof(Real)-1));

  cout << "Real::epsilon()      = " << eps << endl;
  cout << "pi                   = " << pi << endl;
  cout << "sqr(pi)              = " << pi*pi << endl;
  cout << "sqrt(pi)             = " << sqrt(pi) << endl;
  cout << "sqrt(pi*pi)          = " << sqrt(pi*pi) << endl;

  try {

//     eps *= 2;

    cout << "sqrt(pi)-pi/sqrt(pi) = " << sqrt(pi)-pi/sqrt(pi) << endl;
    Assert(compare(0, sqrt(pi)-pi/sqrt(pi), eps));
         
    cout << "pi-sqrt(pi)*sqrt(pi) = " << pi-sqrt(pi)*sqrt(pi) << endl;
    Assert(compare(0, pi-sqrt(pi)*sqrt(pi), eps));

    cout << "sinc(-2.0)           = " << sinc(-2.0) << endl;
    Assert(compare(0, sinc(-2.0), eps));
  
    cout << "sinc(-1.0)           = " << sinc(-1.0) << endl;
    Assert(compare(0, sinc(-1.0), eps));

    cout << "sinc(0.0)            = " << sinc(0.0) << endl;
    Assert(compare(1, sinc(0.0), eps));

    cout << "sinc(1.0)            = " << sinc(1.0) << endl;
    Assert(compare(0, sinc(1.0), eps));

    cout << "sinc(2.0)            = " << sinc(2.0) << endl;
    Assert(compare(0, sinc(2.0), eps));

  } catch (AssertError& e) {
    cerr << e << endl;
    return 1;
  }

  return 0;
}
