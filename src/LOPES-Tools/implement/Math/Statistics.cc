/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (<bahren@astron.nl>)                                     *
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

/* $Id: Statistics.cc,v 1.5 2006/10/31 18:24:08 bahren Exp $*/

#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

#include <Math/Algebra.h>
#include <Math/NR.h>
#include <Math/Statistics.h>

namespace LOPES {

  // ------------------------------------------------------------------------- cc

  template <class T>
  Vector<T> cc (const Matrix<T>& data,
		const bool& positiveLags=true,
		const bool& circular=false)
  {
    IPosition shape (data.shape());
    int nofLags (shape(0));
    int nofPairs (0);
    int ant1 (0);
    int ant2 (0);

    if (!positiveLags && !circular) {
      nofLags += shape(0);
    }

    Vector<T> crosscorr (nofLags,0);
    Vector<T> tmp;

    for (ant1=0; ant1<shape(1); ant1++) {
      tmp = data.column(ant1);
      for (ant2=ant1+1; ant2<shape(1); ant2++) {
	crosscorr += cc (tmp,
			 data.column(ant2),
			 positiveLags,
			 circular);
	// book-keeping
	nofPairs++;
      }
    }

    return crosscorr/static_cast<T>(nofPairs);
  }

  // ------------------------------------------------------------------------- cc
  
  template <class T>
  Vector<T> cc (const Vector<T>& f,
		const Vector<T>& g,
		const bool& positiveLags=true,
		const bool& circular=false)
  {
    int nelem_f (f.nelements());
    int nelem_g (g.nelements());
    
    /* Check if the vectors are consistent in length */
    if (nelem_f != nelem_g) {
      cerr << "[MathStatistics::cc] Mismatching array sizes." << endl;
      return casa::Vector<T> (1,0.0);
    } else {
      // compute the maximum lag between the two series
      int nofLags (nelem_g);
      int minimumLag (0);

      // Check wether or not we only consider positive lags
      if (!positiveLags && !circular) {
	minimumLag = -nelem_f;
	nofLags += nelem_f;
      }

      Vector<T> cc (nofLags,0);
      int shift (0);
      int k(0);
      int n (0);

      if (circular) {
	// circular cross-correlation (J. O. Smith, CCRMA, Stanford)
	for (int lag(minimumLag); lag<nelem_g; lag++) {
	  for (k=0; k<nelem_g; k++) {
	    shift = (lag+k)%nelem_f;
	    cc(n) += f(shift)*g(k);
	  }
	  n++;
	}
	// normalization
	cc /= static_cast<T>(nofLags);
      } else {
	// unbiased sample cross-correlation (J. O. Smith, CCRMA, Stanford)
	for (int lag(minimumLag); lag<nelem_g; lag++) {
	  for (k=0; k<nelem_f-lag; k++) {
	    shift = (lag+k);
	    cc(n) += f(shift)*g(k);
	  }
	  cc(n) /= nelem_g-lag;
	  n++;
	}
      }      
      
      return cc;
    }
    
  }
  
  // ------------------------------------------------------------- WeightedCenter

  template <class T>
  T WeightedCenter (const Vector<T>& f)
  {
    IPosition shape = f.shape();
    Vector<T> x(shape);
    
    for (int n=0; n<shape(0); ++n) {
      x(n) = n;
    }
    
    return WeightedCenter (x,f);
  }

  // ------------------------------------------------------------- WeightedCenter

  template <class T>
  T WeightedCenter (const Vector<T>& w,
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
  
  // ===========================================================================
  //
  //  Correlations
  //
  // ===========================================================================
  
  template <class T>
  void LinearCorrelation (const Vector<T>& x,
			  const Vector<T>& y,
			  T *r,
			  T *prob,
			  T *z)
  {
    int n (0);
    int j (0);
    T yt(0.0);
    T xt (0.0);
    T t (0.0);
    T df (0.0);
    T syy(0.0), sxy(0.0), sxx(0.0), ay(0.0), ax(0.0);
    
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
  
  void CrossCorrelation (Vector<double>& cc,
			 const Vector<double>& f,
			 const Vector<double>& g)
  {
    int fShape (0);
    int gShape (0);
    int ccShape (0);
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
      cerr << "[CrossCorrelation] ";
      cerr << "Number of columns in the input arrays do not match!" << endl;
    }
    
    if (fshape(1) != gshape(1)) {
      cerr << "[CrossCorrelation] ";
      cerr << "Number of rows in the input arrays do not match!" << endl;
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
      cerr << "[AutoCorrelation] Warning, illegal axis index." << endl;
    }
    
  }
  
  
  void CrossCorrelation (Matrix<double>& cc,
			 const Matrix<double>& f,
			 const Matrix<double>& g,
			 int axis)
  {
    int naxes;
    Vector<int> fshape(2),gshape(2),naxis(2);
    
    // -------------------------------------------------------
    // Get the shapes of the input arrays
    
    f.shape(fshape(0),fshape(1));
    g.shape(gshape(0),gshape(1));
    
    // -------------------------------------------------------
    // Check the index for the axis along which to cross-correlate
    
    naxes = f.ndim();
    if (axis >= naxes || axis < 0) axis = 0;
    
    // -------------------------------------------------------
    // Carry out the cross-correlation
    
    if (axis == 1) { // cross-correlation along the y-axis
      cout << "[CrossCorrelation] Starting correlation ..." << endl;
      cc.resize (fshape(0)+gshape(0),fshape(1)+gshape(1));
      cc = 0.0;
      for (int i=0; i<fshape(0); ++i) {
	for (int n=0; n<fshape(1); ++n) {
	  for (int m=0; m<=n; ++m) cc(i,n) += f(i,n)*g(i,n-m);
	}
      }
    }
    else if (axis == 0) { // cross-correlation along the x-axis
      cout << "[CrossCorrelation] Starting correlation ..." << endl;
      cc.resize (fshape(0)+gshape(0),fshape(1)+gshape(1));
      cc = 0.0;
      for (int j=0; j<fshape(1); ++j) {
	for (int n=0; n<fshape(0); ++n) {
	  for (int m=0; m<=n; ++m) cc(n,j) += f(n,j)*g(n-m,j);
	}
      }
    }
    else if (axis == -1) { // 2-dim cross-correlation
      cout << "[CrossCorrelation] 2-dim cross-correlation ..." << endl;
      naxis = -1;
      naxis += fshape + gshape;
      cc.resize (naxis);
      cc = 0.0;
      for (int j=0; j<naxis(0); ++j) {
	for (int n=0; n<naxis(1); ++n) {
	  for (int k=0; k<=j; ++k) {
	    for (int m=0; m<=n; ++m) cc(j,n) += f(k,m)*g(j-k,n-m);
	  }
	}
      }
    }
    else {
      cerr << "[CrossCorrelation] Warning, illegal axis index." << endl;
    }
    
    cout << "[CrossCorrelation] Correlation completed." << endl;
    
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
      cerr << "[RunningAverage] ";
      cerr << "Failed to calculate running average: ";
      cerr << "blocksize = " << blocksize << "out of range" << endl;
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
  
  Vector<double> WeightedCenter (const Matrix<double>& x,
				 const Vector<double>& f)
  {
    int nelem (0);
    int ndim (0);
    Vector<double> cms;
    
    x.shape(nelem,ndim);
    cms.resize(ndim);
    cms = 0.0;
    
    for (int n=0; n<nelem; ++n) {
      for (int dim=0; dim<ndim; ++dim) {
	cms(dim) += x(n,dim)*f(n);
      }
    }
    cms /= sum(f);
    
    return cms;    
  }
  
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
  //  Template instantiation
  //
  // ============================================================================
  
  template Vector<float> cc (const Matrix<float>& data,
			     const bool&,
			     const bool&);
  template Vector<double> cc (const Matrix<double>& data,
			      const bool&,
			      const bool&);
  
  template Vector<float> cc (const Vector<float>&,
			     const Vector<float>&,
			     const bool&,
			     const bool&);
  template Vector<double> cc (const Vector<double>&,
			      const Vector<double>&,
			      const bool&,
			      const bool&);
  
  template float WeightedCenter (const Vector<float>& f);
  template double WeightedCenter (const Vector<double>& f);
  
  template float WeightedCenter (const Vector<float>& w,
				 const Vector<float>& f);
  template double WeightedCenter (const Vector<double>& w,
				  const Vector<double>& f);
  

  template void LinearCorrelation (const Vector<float>& x,
				   const Vector<float>& y,
				   float *r,
				   float *prob,
				   float *z);
  template void LinearCorrelation (const Vector<double>& x,
				   const Vector<double>& y,
				   double *r,
				   double *prob,
				   double *z);
}

