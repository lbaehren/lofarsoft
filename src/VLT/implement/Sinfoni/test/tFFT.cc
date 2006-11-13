//#  tFFT.cc: Test program for the FFT/Blitz++ methods.
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
//#  $Id: tFFT.cc,v 1.5 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/FFT.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Assert.h>
#include <fstream>

using namespace ASTRON::SINFONI;
using namespace blitz;
using namespace std;

const Real eps(8*std::numeric_limits<Real>::epsilon());

int fftshift()
{
  cout << endl << "======== fftshift ========" << endl;
  try {
    VectorInteger v(5);
    v = tensor::i + 1;
#if 0
    cout << endl << "v              = " << v;
    cout << endl << "fftshift(v)    = " << fftshift(v);
    cout << endl << "ifftshift(v)   = " << ifftshift(v);
#endif
    Assert(isEqual(v, ifftshift(fftshift(v))));

    MatrixInteger m(5,4);
    m = tensor::i * m.cols() + tensor::j + 1;
#if 0
    cout << endl << "m              = " << m;
    cout << endl << "fftshift(m,0)  = " << fftshift(m,0);
    cout << endl << "fftshift(m,1)  = " << fftshift(m,1);
    cout << endl << "fftshift(m)    = " << fftshift(m);
    cout << endl << "ifftshift(m,0) = " << ifftshift(m,0);
    cout << endl << "ifftshift(m,1) = " << ifftshift(m,1);
    cout << endl << "ifftshift(m)   = " << ifftshift(m);
#endif
    Assert(isEqual(m, ifftshift(fftshift(m))));
    Assert(isEqual(m, ifftshift(fftshift(m,0),0)));
    Assert(isEqual(m, ifftshift(fftshift(m,1),1)));
  }
  catch (Exception& e) {
    cerr << endl << e << endl;
    return 1;
  }
  return 0;  
}


int fft_c2c_1d()
{
  cout << endl << "======== fft_c2c_1d ========" << endl;
  try {

    ifstream ifs;

#if 1
    VectorComplex x;
    ifs.open("fft_c2c_1d.in");
    if (!ifs) 
      THROW (IOException, "Failed to open file `fft_c2c_1d.in' for reading");
    if (!(ifs >> x))
      THROW (IOException, "Error reading file `fft_c2c_1d.in'");
    ifs.close();
#else
    VectorReal t(601);
    VectorComplex x(601);
    t = 0.001 * tensor::i;
    x = sin(2*pi*50*t) + sin(2*pi*120*t);
    ofstream ofs("fft_c2c_1d.in");
    ofs << setprecision(2*(sizeof(Real)+1)) << x << endl;
    ofs.close();
#endif

    VectorComplex X = fftshift(fft(x));
    Assert(compare(x, ifft(ifftshift(X)), eps));

    VectorComplex Xref;
    ifs.open("fft_c2c_1d.ref");
    if (!ifs) 
      THROW (IOException, "Failed to open file `fft_c2c_1d.ref' for reading");
    if (!(ifs >> Xref))
      THROW (IOException, "Error while reading file `fft_c2c_1d.ref'");
    ifs.close();

    Assert(compare(X, Xref, 4*sizeof(Real)*eps));

  } 
  catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  return 0;
}


int fft2_c2c_2d()
{
  cout << endl << "======== fft2_c2c_2d ========" << endl;
  try {

    ifstream ifs;

#if 1
    MatrixComplex x;
    ifs.open("fft2_c2c_2d.in");
    if (!ifs)
      THROW (IOException, "Failed to open file `fft2_c2c_2d.in' for reading");
    if (!(ifs >> x))
      THROW (IOException, "Error reading file `fft2_c2c_2d.in'");
    ifs.close();
#else
    VectorReal t(101);
    VectorReal u(101);
    VectorReal v(101);
    t = -0.5 + 0.01 * tensor::i;
    u = (cos(2*pi*5*t) + sin(2*pi*12*t)) * exp(-t*t);
    v = (sin(2*pi*5*t) + cos(2*pi*12*t)) * exp(-t*t);
    MatrixComplex x(101, 101);
    x = u(tensor::i) * v(tensor::j);
    ofstream ofs("fft2_c2c_2d.in");
    ofs << setprecision(2*(sizeof(Real)+1)) << x << endl;
    ofs.close();
#endif

    MatrixComplex X = fftshift(fft2(x));
    Assert(compare(x, ifft2(ifftshift(X)), sizeof(Real)*eps));

    MatrixComplex Xref;
    ifs.open("fft2_c2c_2d.ref");
    if (!ifs) 
      THROW (IOException, "Failed to open file `fft2_c2c_2d.ref' for reading");
    if (!(ifs >> Xref))
      THROW (IOException, "Error while reading file `fft2_c2c_2d.ref'");
    ifs.close();

    Assert(compare(X, Xref, 8*sizeof(Real)*sizeof(Real)*eps));

  }
  catch (Exception& e) {
    cerr << e << endl;
    return 1;
  }
  return 0;
}


int main()
{
  int fail(0);

  fail += fftshift();
  fail += fft_c2c_1d();
  fail += fft2_c2c_2d();

  if (fail != 0) {
    cerr << "---------------------" << endl
         << fail << " FFT test(s) failed." << endl
         << "---------------------" << endl;
    return 1;
  }
  cout << endl << "All FFT tests passed." << endl << endl;
  return 0;
}
