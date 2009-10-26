/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <Math/Statistics.h>

namespace CR { // Namespace CR -- begin
  
  // =============================================================================
  //
  //  Statistical properties of data
  //
  // =============================================================================

  // --------------------------------------------------------------------- NRerfcc
  
  double NRerfcc (double const &x)
  {
    double t (0.0);
    double z (0.0);
    double ans (0.0);
    
    z=fabs(x);
    t=1.0/(1.0+0.5*z);
    ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
	    t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
            t*(-0.82215223+t*0.17087277)))))))));
    return x >= 0.0 ? ans : 2.0-ans;
  }
  
  // -------------------------------------------------------------------- NRgammln

  double NRgammln (double const &xx)
  {
    int j (0);
    double x (0.0);
    double y (0.0);
    double tmp (0.0);
    double ser (0.0);
    static double cof[6]={76.18009172947146,-86.50532032941677,
			  24.01409824083091,-1.231739572450155,
			  0.1208650973866179e-2,-0.5395239384953e-5};  
    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for (j=0;j<=5;j++) ser += cof[j]/++y;
    return -tmp+log(2.5066282746310005*ser/x);
  }
  
  // -------------------------------------------------------------------- NRbetacf

  double NRbetacf (double const &a,
		   double const &b,
		   double const &x)
  {
    int m,m2;
    double aa,c,d,del,h,qab,qam,qap;
    
    qab=a+b;
    qap=a+1.0;
    qam=a-1.0;
    c=1.0;
    d=1.0-qab*x/qap;
    if (fabs(d) < FPMIN) d=FPMIN;
    d=1.0/d;
    h=d;
    for (m=1;m<=MAXIT;m++) {
      m2=2*m;
      aa=m*(b-m)*x/((qam+m2)*(a+m2));
      d=1.0+aa*d;
      if (fabs(d) < FPMIN) d=FPMIN;
      c=1.0+aa/c;
      if (fabs(c) < FPMIN) c=FPMIN;
      d=1.0/d;
      h *= d*c;
      aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
      d=1.0+aa*d;
      if (fabs(d) < FPMIN) d=FPMIN;
      c=1.0+aa/c;
      if (fabs(c) < FPMIN) c=FPMIN;
      d=1.0/d;
      del=d*c;
      h *= del;
      if (fabs(del-1.0) < EPS) break;
    }
    if (m > MAXIT) {
      std::cerr << "[betacf] ";
      std::cerr << "a or b too big, or MAXIT too small in betacf" << std::endl;
    }
    return h;
  }
  
  double NRbetai(double const &a,
		 double const &b,
		 double const &x)
  {
    double bt;
    
    if (x < 0.0 || x > 1.0) {
      std::cerr << "[NRbetai] Bad x in routine betai" << std::endl;
    }
    if (x == 0.0 || x == 1.0) bt=0.0;
    else
      bt=exp(NRgammln(a+b)-NRgammln(a)-NRgammln(b)+a*log(x)+b*log(1.0-x));
    if (x < (a+1.0)/(a+b+2.0))
      return bt*NRbetacf(a,b,x)/a;
    else
      return 1.0-bt*NRbetacf(b,a,1.0-x)/b;
  }
  
  // ===========================================================================
  //
  //  Correlations
  //
  // ===========================================================================
  
  void LinearCorrelation (const Vector<double>& x,
			  const Vector<double>& y,
			  double *r,
			  double *prob,
			  double *z)
  {
    int n (0);
    int j (0);
    double yt(0.0);
    double xt (0.0);
    double t (0.0);
    double df (0.0);
    double syy (0.0);
    double sxy (0.0), sxx=0.0,ay=0.0,ax=0.0;
    
    x.shape(n);
    
    // -------------------------------------------------------
    // Find the means.
    
    for (j=1;j<=n;j++) {
      ax += x(j);
      ay += y(j);
    }
    ax /= n;
    ay /= n;
    
    // -------------------------------------------------------
    // Compute the correlation coefficient.
    
    for (j=1;j<=n;j++) {
      xt=x(j)-ax;
      yt=y(j)-ay;
      sxx += xt*xt;
      syy += yt*yt;
      sxy += xt*yt;
    }
    *r=sxy/(sqrt(sxx*syy)+TINY);
    
    // -------------------------------------------------------
    // Fisher's z transformation
    
    *z=0.5*log((1.0+(*r)+TINY)/(1.0-(*r)+TINY));
    df=n-2;
    
    // -------------------------------------------------------
    // Student's t probability.
    
    t = (*r)*sqrt(df/((1.0-(*r)+TINY)*(1.0+(*r)+TINY)));
    *prob = NRbetai(0.5*df,0.5,df/(df+t*t));
    
  }
  
  
  // ===========================================================================
  //
  //  Cross-Correlation function (CCF)
  //
  // ===========================================================================

  // ---------------------------------------------------------- CrossCorrelation
  
  void CrossCorrelation (Vector<double>& cc,
			 const Vector<double>& f,
			 const Vector<double>& g)
  {
    int fShape,gShape,ccShape;
    double ff,gg,fMean,gMean,fDeviation,gDeviation,normalization;
    
    f.shape(fShape); fMean = mean(f); fDeviation = stddev(f);
    g.shape(gShape); gMean = mean(g); gDeviation = stddev(g);
    
    normalization = (fShape-1)*fDeviation*gDeviation;
    
    ccShape = fShape +gShape + 1;
    cc.resize(ccShape);
    cc = 0.0;
    
    for (int m=0; m<ccShape; ++m) {
      for (int n=0; n<ccShape; ++n) {
	ff = ((n<fShape) ? f(n)-fMean : 0);
	gg = (((m+n)<0 || (m+n) >= gShape) ? (g(m+n)-gMean) : 0);
	cc(m) += ff*gg;
      }
      cc(m) /= normalization;
    }
    
  }
  
  // ---------------------------------------------------------- CrossCorrelation
  
  void CrossCorrelation (Vector<Complex>& cc,
			 const Vector<Complex>& DataI,
			 const Vector<Complex>& DataJ)
  {  
    int nelem;
    Vector<Complex> conjDataI;
    
    DataI.shape(nelem);
    
    conjDataI.resize(nelem);
    conjDataI = conj(DataI);
    
    cc.resize(nelem);
    cc = 0.0;
    
    for (int n=0; n<nelem; ++n) {
      for (int m=0; m<=n; ++m) {
	cc(n) += DataI(n)*DataJ(n-m);
      }
    } 
    
  }
  
  // ---------------------------------------------------------- CrossCorrelation
  
  void CrossCorrelation (Matrix<Complex>& cc,
			 const Matrix<Complex>& f,
			 const Matrix<Complex>& g,
			 int axis)
  {
    int naxes;
    Vector<int> fshape(2),gshape(2);
    Matrix<Complex> conjf;
    
    // -------------------------------------------------------
    // Check if the shapes of the input arrays match
    
    f.shape(fshape(0),fshape(1));
    g.shape(gshape(0),gshape(1));
    
    if (fshape(0) != gshape(0)) {
      std::cerr << "[CrossCorrelation] ";
      std::cerr << "Number of columns in the input arrays do not match!" << std::endl;
    }
    
    if (fshape(1) != gshape(1)) {
      std::cerr << "[CrossCorrelation] ";
      std::cerr << "Number of rows in the input arrays do not match!" << std::endl;
    }
    
    // -------------------------------------------------------
    // Check the index for the axis along which to cross-correlate
    
    naxes = f.ndim();
    if (axis >= naxes || axis < 0) axis = 0;
    
    // -------------------------------------------------------
    // Carry out the cross-correlation
    
    conjf.resize (fshape(0),fshape(1));
    conjf = conj(f);
    
    cc.resize (fshape(0),fshape(1));
    cc = 0.0;
    
    if (axis ==0) { // cross-correlation along the x-axis
      for (int i=0; i<fshape(0); ++i) {
	for (int n=0; n<fshape(1); ++n) {
	  for (int m=0; m<=n; ++m) cc(i,n) += conjf(i,n)*g(i,n-m);
	} 
      }
    }
    else if (axis == 1) { // cross-correlation along the y-axis
      for (int j=0; j<fshape(1); ++j) {
	for (int n=0; n<fshape(0); ++n) {
	  for (int m=0; m<=n; ++m) cc(n,j) += conjf(n,j)*g(n-m,j);
	} 
      }
    }
    else {
      std::cerr << "[AutoCorrelation] Warning, illegal axis index." << std::endl;
    }
    
  }
  
  // ---------------------------------------------------------- CrossCorrelation
  
  void CrossCorrelation (Matrix<double>& cc,
			 const Matrix<double>& f,
			 const Matrix<double>& g,
			 int axis)
  {
    int naxes (0);
    Vector<int> fshape(2);
    Vector<int> gshape(2);
    Vector<int> naxis(2);
    
    // -------------------------------------------------------
    // Get the shapes of the input arrays
    
    f.shape(fshape(0),fshape(1));
    g.shape(gshape(0),gshape(1));
    
    // -------------------------------------------------------
    // Check the index for the axis along which to cross-correlate
    
    naxes = f.ndim();
    if (axis >= naxes || axis < 0) axis = 0;
    
      // Additional local variables
    int i (0);
    int j (0);
    int k (0);
    int m (0);
    int n (0);
    
    // -------------------------------------------------------
    // Carry out the cross-correlation
    
    if (axis == 1) { // cross-correlation along the y-axis
      std::cout << "[CrossCorrelation] Starting correlation ..." << std::endl;
      cc.resize (fshape(0)+gshape(0),fshape(1)+gshape(1));
      cc = 0.0;
      for (i=0; i<fshape(0); ++i) {
	for (n=0; n<fshape(1); ++n) {
	  for (m=0; m<=n; ++m) cc(i,n) += f(i,n)*g(i,n-m);
	}
      }
    }
    else if (axis == 0) { // cross-correlation along the x-axis
      std::cout << "[CrossCorrelation] Starting correlation ..." << std::endl;
      cc.resize (fshape(0)+gshape(0),fshape(1)+gshape(1));
      cc = 0.0;
      for (j=0; j<fshape(1); ++j) {
	for (n=0; n<fshape(0); ++n) {
	  for (m=0; m<=n; ++m) cc(n,j) += f(n,j)*g(n-m,j);
	}
      }
    }
    else if (axis == -1) { // 2-dim cross-correlation
      std::cout << "[CrossCorrelation] 2-dim cross-correlation ..." << std::endl;
      //
      naxis = -1;
      naxis += fshape + gshape;
      cc.resize (naxis);
      cc = 0.0;
      for (j=0; j<naxis(0); ++j) {
	for (n=0; n<naxis(1); ++n) {
	  for (k=0; k<=j; ++k) {
	    for (m=0; m<=n; ++m) cc(j,n) += f(k,m)*g(j-k,n-m);
	  }
	}
      }
    }
    else {
      std::cerr << "[CrossCorrelation] Warning, illegal axis index." << std::endl;
    }
    
    std::cout << "[CrossCorrelation] Correlation completed." << std::endl;
    
  }
  
  // =============================================================================
  //
  //  Auto-Correlation function (ACF)
  //
  // =============================================================================
  
  
  void AutoCorrelation (Vector<double>& cc,
			const Vector<double>& f)
  {
    CrossCorrelation (cc,f,f);
  }
  
  void AutoCorrelation (Vector<Complex>& cc,
			const Vector<Complex>& f)
  {
    CrossCorrelation (cc,f,f);
  }
  
  void AutoCorrelation (Matrix<Complex>& cc,
			const Matrix<Complex>& f,
			int axis)
  {
    CrossCorrelation (cc,f,f,axis);
  }
  
  
  // =============================================================================
  //
  //  Averaging
  //
  // =============================================================================
  
  void RunningAverage (Vector<double>& ary,
		       const int& blocksize)
  {
    int aryLen,maryLen;
    double sumElements;
    Vector<double> Mean;
    Vector<double> mary;
    
    ary.shape(aryLen);
    maryLen = aryLen+blocksize-1;
    
    mary.resize(maryLen);
    
    if (blocksize < aryLen && blocksize > 1) {
      for (int i=0; i<blocksize; ++i) {
	sumElements = 0.0;
	for (int j=0; j<=i; ++j) sumElements += ary(j);
	mary(i) = sumElements*blocksize/i;
      }
      for (int i=blocksize; i<aryLen; ++i) {
	mary(i) = mary(i-1)+ary(i)-ary(i-blocksize);
      }
      for (int i=aryLen; i<maryLen; ++i) {
	sumElements = 0.0;
	for (int j=i-blocksize; j<aryLen; ++j) sumElements += ary(j);
	mary(i) = sumElements*blocksize/(maryLen-i+1);
      }
      Mean.resize(aryLen);
      for (int i=0; i<aryLen; ++i) Mean(i) = mary(blocksize/2+i)/blocksize;
    } else if (blocksize >= aryLen) {
      Mean.resize(1);
      Mean = sum(ary)/aryLen;
    } else if (blocksize == 1) {
      Mean.resize(aryLen);
      Mean = ary;
    } else {
      std::cerr << "[RunningAverage] ";
      std::cerr << "Failed to calculate running average: ";
      std::cerr << "blocksize = " << blocksize << "out of range" << std::endl;
    }
    
    Mean.shape(aryLen);
    ary.resize(aryLen);
    ary = Mean;
    
  }
  
  void NormalizeData (Matrix<double>& data,
		      Vector<double>& norm)
  {
    int nx,ndata,nelem;
    
    data.shape(nx,ndata);
    norm.shape(nelem);
    
    if (nelem == nx) {
      for (int n=0; n<ndata; ++n) {
	data.row(n) = data.row(n)/norm;
	//       for (int i=0; i<nx; ++i) data(i,n) = data(i,n)/norm(i); 
      }
    }
    
  }

  // ------------------------------------------------------------- WeightedCenter
  
  Vector<double> WeightedCenter (const Matrix<double>& x,
				 const Vector<double>& f)
  {
    int nelem (0);
    int ndim (0);
    int dim (0);
    Vector<double> cms;
    
    x.shape(nelem,ndim);
    cms.resize(ndim);
    cms = 0.0;
    
    for (int n=0; n<nelem; ++n) {
      for (dim=0; dim<ndim; ++dim) {
	cms(dim) += x(n,dim)*f(n);
      }
    }
    cms /= sum(f);
    
    return cms;    
  }

  template <class T> T WeightedCenter (const Vector<T>& f)
  {
    casa::IPosition shape = f.shape();
    Vector<T> x(shape);
    
    for (int n=0; n<shape(0); ++n) {
      x(n) = n;
    }
    
    return WeightedCenter (x,f);
  }

  template <class T> T WeightedCenter (const Vector<T>& w,
				       const Vector<T>& f)
  {
    int nelem;
    T cms;
    
    f.shape(nelem);
    cms = 0.0;
    
    for (int n=0; n<nelem; ++n) {
      cms += w(n)*f(n);
    }
    
    return cms/sum(f);
  }

  // ---------------------------------------------------------------- averageData
  
  Vector<double> averageData (const Vector<double>& dataIN,
			      const int lenOUT)
  {
    int lenIN (dataIN.nelements());
    
    if (lenIN == lenOUT) {
      return dataIN;
    } else {
      int nofBins (0);
      Vector<double> dataOUT (lenOUT,0.0);
      //
      Matrix<int> indexValues = binningIndices (lenIN, lenOUT);
      // regrid the data
      for (int bin=0; bin<lenOUT; bin++) {
	nofBins = 0;
	for (int i=indexValues(bin,0); i<=indexValues(bin,1); i++) {
	  dataOUT(bin) += dataIN(i);
	  ++nofBins;
      }
	dataOUT(bin) /= nofBins;
      }
      // return the result
      return dataOUT;
    }
  }
  
  Matrix<int> binningIndices (const int lenData,
			      const int nofBins)
  {
    Matrix<int> indices;
    
    if (nofBins < lenData) {
      // adjust size of returned array
      indices.resize(nofBins,2);
      //
      double values_per_bin;
      values_per_bin = 1.0*(lenData-1)/nofBins;    
      //
      for (int k=0; k<nofBins; ++k) {
	indices(k,0) = (int)floor(values_per_bin*k+0.5);
	indices(k,1) = (int)floor(values_per_bin*(k+1)+0.5);
      }
    } else {
      indices.resize(lenData,2);
      for (int k=0; k<lenData; ++k) {
	indices(k,0) = k;
	indices(k,1) = k;
      }
    }
    
    //
    return indices;
  }

  // ============================================================================
  //
  //  Template instantiations
  //
  // ============================================================================

  template float WeightedCenter (const Vector<float>& f);
  template double WeightedCenter (const Vector<double>& f);
  
  template float WeightedCenter (const Vector<float>& w,
				 const Vector<float>& f);
  template double WeightedCenter (const Vector<double>& w,
				  const Vector<double>& f);
  
  
} // Namespace CR -- end
