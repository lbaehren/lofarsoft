/***************************************************************************
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

/* $Id: ccBeam.cc,v 1.5 2006/10/31 21:07:59 bahren Exp $*/

/*!
  \class ccBeam
*/

#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

#include <Beamforming/ccBeam.h>
#include <Math/Algebra.h>

using std::cout;
using std::cerr;
using std::endl;

namespace LOPES {  // namespace LOPES -- begin
  
  // ==============================================================================
  //
  //  Construction
  //
  // ==============================================================================
  
  // ----------------------------------------------------------------------- ccBeam
  
  template <class T, class S>
  ccBeam<T,S>::ccBeam ()
  {
    blocksize_p = 1;
  }
  
  // ----------------------------------------------------------------------- ccBeam
  
  template <class T, class S>
  ccBeam<T,S>::ccBeam (int const &blocksize)
  {
    blocksize_p = blocksize;
  }
  
  // ---------------------------------------------------------------------- ~ccBeam
  
  template <class T, class S>
  ccBeam<T,S>::~ccBeam ()
  {;}
  
  
  // ==============================================================================
  //
  //  Processing
  //
  // ==============================================================================
  
  // ----------------------------------------------------------------------- ccbeam
  
  template <class T, class S>
  Vector<T> ccBeam<T,S>::ccbeam (const Matrix<T>& data)
  {
    int ant1 (0);
    int ant2 (0);
    int nofPairs (0);
    IPosition shape (data.shape());
    Vector<T> dataAnt1 (shape(0));
    Vector<T> cc (shape(0),static_cast<T>(0));
    
    for (ant1=0; ant1<shape(1); ant1++) {
      dataAnt1 = data.column(ant1);
      for (ant2=ant1+1; ant2<shape(1); ant2++) {
	cc += dataAnt1*data.column(ant2);
	//
	nofPairs += 1;
      }
    }
    
    // normalization by number of antenna pairs
    cc /= static_cast<T>(nofPairs);
    
    // we need to extract the sign at this point
    
    return LOPES::sign(cc)*sqrt(abs(cc));
  }
  
  // ----------------------------------------------------------------------- ccbeam
  
  template <class T, class S>
  Vector<T> ccBeam<T,S>::ccbeam (const Matrix<S>& data,
				 int const &blocksize)
  {
    IPosition shape (data.shape());
    Matrix<T> dataTime (blocksize,shape(1),static_cast<T>(0));
    
    casa::FFTServer<T,S> server(IPosition(1,blocksize),
			  casa::FFTEnums::REALTOCOMPLEX);
    
    try {
      Vector<T> tmp;
      // inverse FFT on the input data
      for (int antenna(0); antenna<shape(1); antenna++) {
	server.fft(tmp,data.column(antenna));
	// .. and copy the result
	dataTime.column(antenna) = tmp;
      }
    } catch (AipsError x) {
      cerr << "[Statistics::ccBeam] " << x.getMesg() << endl;
    }
    
    return ccBeam::ccbeam(dataTime);
  }
  
  // ==============================================================================
  //
  //  Template instantiations
  //
  // ==============================================================================
  
  template class ccBeam<float,Complex>;
  template class ccBeam<double,DComplex>;
  
}  // namespace LOPES -- end
