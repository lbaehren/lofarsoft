//#  tMath.cc: Test program for the Blitz++ math extensions.
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

#include <Sinfoni/Blitz/Math.h>
#include <Sinfoni/Blitz/Types.h>
#include <Sinfoni/Assert.h>
#include <iostream>

using namespace ASTRON::SINFONI;
using namespace blitz;
using namespace std;

int main()
{
  firstIndex i;
  secondIndex j;

  VectorReal v(4);
  v = i-2;

  MatrixInteger m(4,3);
  m = j*4 + i;
  m *= m;

  cout << boolalpha << setprecision(2*(sizeof(Real)-1));


  //# Test elementary matrix operations.
  try {

    cout << endl << "---- Testing diag() ----" << endl << endl;
  
    cout << "v          = " << v << endl;
    cout << "diag(v)    = " << diag(v) << endl;
    cout << "diag(v,-1) = " << diag(v,-1) << endl;
    cout << "diag(v,1)  = " << diag(v,1) << endl;
    cout << endl;

    Assert (all(v          == diag(diag(v))));
    Assert (all(diag(v,-1) == trans(diag(v,1))));

    cout << "m          = " << m << endl;
    cout << "diag(m)    = " << diag(m) << endl;
    cout << "diag(m,-1) = " << diag(m,-1) << endl;
    cout << "diag(m,-3) = " << diag(m,-3) << endl;
    cout << "diag(m,1)  = " << diag(m,1) << endl;
    cout << "diag(m,3)  = " << diag(m,3) << endl;
    cout << endl;

    VectorInteger vv(3);
    vv(0) = m(0,0); vv(1) = m(1,1); vv(2) = m(2,2);
    Assert(all(diag(m) == vv));

    vv(0) = m(1,0); vv(1) = m(2,1); vv(2) = m(3,2);
    Assert(all(diag(m,-1) == vv));

    vv.resize(2);
    vv(0) = m(0,1); vv(1) = m(1,2);
    Assert(all(diag(m,1) == vv));

    vv.resize(1);
    vv(0) = m(3,0);
    Assert(all(diag(m,-3) == vv));

    vv.resize(0);
    Assert(all(diag(m,3) == vv));
  }

  catch (AssertError& e) {
    cerr << e << endl;
    return 1;
  }


  //# Testing statistical operations
  try {

    cout << endl << "---- Testing statistics ----" << endl << endl;

    cout << "v          = " << v << endl;
    cout << "mean(v)    = " << mean(v) << endl;
    cout << "median(v)  = " << median(v) << endl;
    cout << "var(v)     = " << var(v) << endl;
    cout << "cov(v)     = " << cov(v) << endl;
    cout << endl;

    Assert(compare(Real(-0.5), mean(v)));
    Assert(compare(Real(-0.5), median(v)));
    Assert(compare(Real(5.0/3.0), var(v)));
    Assert(compare(Real(5.0/3.0), cov(v)));
    
    cout << "m          = " << m << endl;
    cout << "median(m)  = " << median(m) << endl;
    cout << "mean(m)    = " << mean(m) << endl;
    cout << "var(m)     = " << var(m) << endl;
    cout << "cov(m)     = " << cov(m) << endl;
    cout << endl;

    VectorReal vv(3);

    //# Calculate the medians by hand.
    vv(0) = Real((m(1,0)+m(2,0))/2.0);
    vv(1) = Real((m(1,1)+m(2,1))/2.0);
    vv(2) = Real((m(1,2)+m(2,2))/2.0);
    Assert(compare(vv, median(m)));

    //# Calculate the means by hand.
    vv(0) = Real((m(0,0)+m(1,0)+m(2,0)+m(3,0))/4.0);
    vv(1) = Real((m(0,1)+m(1,1)+m(2,1)+m(3,1))/4.0);
    vv(2) = Real((m(0,2)+m(1,2)+m(2,2)+m(3,2))/4.0);
    Assert(compare(vv, mean(m)));

    //# Remove the means form the matrix mm.
    MatrixReal mm(cast<Real>(m));
    mm(0,0) -= vv(0); mm(1,0) -= vv(0); mm(2,0) -= vv(0); mm(3,0) -= vv(0);
    mm(0,1) -= vv(1); mm(1,1) -= vv(1); mm(2,1) -= vv(1); mm(3,1) -= vv(1);
    mm(0,2) -= vv(2); mm(1,2) -= vv(2); mm(2,2) -= vv(2); mm(3,2) -= vv(2);

    //# Calculate the variances by hand.
    vv(0) = Real((mm(0,0)*mm(0,0)+mm(1,0)*mm(1,0)+
                  mm(2,0)*mm(2,0)+mm(3,0)*mm(3,0))/3.0);
    vv(1) = Real((mm(0,1)*mm(0,1)+mm(1,1)*mm(1,1)+
                  mm(2,1)*mm(2,1)+mm(3,1)*mm(3,1))/3.0);
    vv(2) = Real((mm(0,2)*mm(0,2)+mm(1,2)*mm(1,2)+
                  mm(2,2)*mm(2,2)+mm(3,2)*mm(3,2))/3.0);
    Assert(compare(vv, var(m)));

    //# Calculate the covariances by hand.
    MatrixReal cm(m.cols());
    cm(0,0) = vv(0);
    cm(1,1) = vv(1);
    cm(2,2) = vv(2);
    cm(0,1) = Real((mm(0,0)*mm(0,1)+mm(1,0)*mm(1,1)+
                    mm(2,0)*mm(2,1)+mm(3,0)*mm(3,1))/3.0);
    cm(0,2) = Real((mm(0,0)*mm(0,2)+mm(1,0)*mm(1,2)+
                    mm(2,0)*mm(2,2)+mm(3,0)*mm(3,2))/3.0);
    cm(1,2) = Real((mm(0,1)*mm(0,2)+mm(1,1)*mm(1,2)+
                    mm(2,1)*mm(2,2)+mm(3,1)*mm(3,2))/3.0);
    cm(1,0) = cm(0,1);
    cm(2,0) = cm(0,2);
    cm(2,1) = cm(1,2);
    Assert(compare(cm, cov(m)));

  }
  
  catch (AssertError& e) {
    cerr << e << endl;
    return 1;
  }

  
  //# Testing floating point comparison
  try {
    
    cout << endl << "---- Testing compare() ----" << endl << endl;

    Real eps(1000*numeric_limits<Real>::epsilon());

    VectorReal r(10);
    VectorReal z(10);

    //# Initialize vector r so that all elements contain pi. However, the
    //# number of manipulations on each element increases with increasing
    //# index. Next, pi is subtracted from all elements of r; hence r should
    //# be equal to zero. That's what we're going to test, using compare().
    r(0) = pi;
    for (int i=1; i<r.rows(); i++) {
      r(i) = sqrt(r(i-1));
    }
    for (int i=0; i<r.rows(); i++) {
      for (int j=0; j<i; j++) {
        r(i) *= r(i);
      }
    }
    r -= pi;
    z = 0;
    
    cout << "r = " << r << endl;
    Assert(compare(z, r, eps));
  }

  catch (AssertError& e) {
    cerr << e << endl;
    return 1;
  }

  return 0;

}
