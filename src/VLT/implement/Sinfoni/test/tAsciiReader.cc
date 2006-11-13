//#  tAsciiReader.cc: Test program for the ASCII reader class.
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
//#  $Id: tAsciiReader.cc,v 1.4 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Exception.h>
#include <Sinfoni/Types.h>

using namespace std;
using namespace ASTRON::SINFONI;

int main()
{
  try {
    MatrixReal m(60,60);
    AsciiReader reader("tAsciiReader.in");
    reader.read(m);
    cout << m << endl;
  } catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  try {
    VectorReal v(60);
    AsciiReader reader("tAsciiReader.in");
    for (int i=0; i<60; i++) {
      reader.read(v);
      cout << v << endl;
    }
  } catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  return 0;
}
