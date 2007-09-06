/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: xBeam.cc,v 1.4 2007/06/19 08:12:26 horneff Exp $*/

#include <scimath/Mathematics/FFTServer.h>

#include <LopesLegacy/xBeam.h>
#include <Math/MathAlgebra.h>
#include <Math/VectorNorms.h>
#include <Math/StatisticsFilter.h>


namespace LOPES { // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T, class S> xBeam<T,S>::xBeam (){
    start = 1./8.;
    stop = 3./8.;
    nsigma = 1.;
    navg = 3;
  }
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  template <class T, class S> xBeam<T,S>::~xBeam ()
  {
    destroy();
  }
  
  template <class T, class S>  void xBeam<T,S>::destroy ()
  {;}
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ----------------------------------------------------- xbeam from time-series

  template <class T, class S>
  Vector<T> xBeam<T,S>::xbeam (const Matrix<T> &data) {
    Vector<T> xbeamS;
    try {
      IPosition shape (data.shape());
      Vector<T> corr(shape(0),0.), pow, tmpvec, pown, corrn;
      Int i,j,nants=shape(1);
      Int nc (0), np(0);

      pow = square(data.column(nants-1));
      nc=0;
      np=1;

      for (i=0; i<(nants-1); i++){
	tmpvec = data.column(i);
	for (j=i+1; j<nants; j++) {
	  corr += tmpvec*data.column(j);
	  nc++;
	};
	pow += square(data.column(i));
	np++;
      };
      pow /= (T)np;
      corr /= (T)nc;

      i = ifloor(start*shape(0));
      j = ifloor(stop*shape(0));
      Slice sli(i,(j-i));
      T mp, sigma, mc;
      mp = mean(pow(sli));
      sigma = stddev(pow(sli))*nsigma;
      mc = mean(corr(sli));
      StatisticsFilter<T> mf(navg,FilterType::MEAN);
      pown = mf.filter(pow)-mp;
      corrn = mf.filter(corr)-mc;
      pown(pown<sigma) = sigma; //implicit MaskedArray
      tmpvec = abs(corrn/pown);
      xbeamS = tmpvec*corr;

      return CR::sign(xbeamS)*sqrt(abs(xbeamS));
    } catch (AipsError x) {
      cerr << "xBeam:xbeam: " << x.getMesg() << endl;
      return Vector<T>();
    }    
  }



  // -------------------------------------------------------- xbeam from FFT-Data

  template <class T, class S> 
  Vector<T> xBeam<T,S>::xbeam (const Matrix<S>& data,
			       Int const &blocksize)
  {
    // get the shape of the input data
    IPosition shape (data.shape());
    // matrix with intermediate data which will be forwarded for processing
    Matrix<T> dataTime (blocksize,shape(1),static_cast<T>(0));
    // set up FFT server
    FFTServer<T,S> server(IPosition(1,blocksize),
			  FFTEnums::REALTOCOMPLEX);
    try {
      Vector<T> tmp(blocksize);
      // inverse FFT on the input data
      for (int antenna(0); antenna<shape(1); antenna++) {
	server.fft(tmp,data.column(antenna));
	// .. and copy the result
	dataTime.column(antenna) = tmp;
      }
      return xbeam(dataTime);
    } catch (AipsError x) {
      cerr << "xBeam:xbeam_from_FFT: " << x.getMesg() << endl;
      return Vector<T>();
    }
  }
  
  // ==============================================================================
  //
  //  Template instantiations
  //
  // ==============================================================================
  
  template class xBeam<casa::Float,casa::Complex>;
  template class xBeam<casa::Double,casa::DComplex>;
  
} // Namespace LOPES -- end

