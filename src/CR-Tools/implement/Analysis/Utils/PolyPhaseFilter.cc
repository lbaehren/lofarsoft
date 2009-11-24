/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Maaijke Mevius (<mail>)                                            *
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

#include <Analysis/Utils/PolyPhaseFilter.h>


namespace CR { // Namespace CR -- begin

  PolyPhaseFilter::PolyPhaseFilter(uint size,uint nrtaps):nrTaps(nrtaps){
    blocksize=size;
    inreal=NULL;
    outcomplex=NULL;
    init();
  }

  PolyPhaseFilter::~PolyPhaseFilter(){
    free(outcomplex);
    free(inreal);
    fftw_destroy_plan(pr2c);
}

  void PolyPhaseFilter::init(){
    
    norm=1./blocksize;
    init_fft();
  }

  void PolyPhaseFilter::init_fft(){
    outcomplex = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (blocksize/2+1));
    inreal = (double*) fftw_malloc(sizeof(double) * (blocksize));
    
    pr2c= fftw_plan_dft_r2c_1d(blocksize, inreal, outcomplex, FFTW_MEASURE );
  }


  void PolyPhaseFilter::get_ppf(const vector<double> & timedata){
    assert(itsWeights.size() >= nrTaps*blocksize);
    assert(timedata.size() >=nrTaps*blocksize);
    for(uint ib=0;ib<blocksize;ib++){
      double val=0;
      for(uint ifilt=0;ifilt<nrTaps;ifilt++){
	val+=itsWeights[ifilt*blocksize+ib]*timedata[ifilt*blocksize+ib];
	
      }
      //    cout<<"val["<<ib<<"]="<<val<<endl;
      inreal[ib]=val;
    }
  }


  void PolyPhaseFilter::Convert(const vector<double> & in){
    get_ppf(in);
    fftw_execute(pr2c);

  }
  
  double PolyPhaseFilter::GetPhase(uint channel){
    return carg(outcomplex[channel]);
  }
  double PolyPhaseFilter::GetAmp(uint channel){
    double re = norm*creal(outcomplex[channel]);
    double im = norm*cimag(outcomplex[channel]);
    return sqrt(re*re+im*im);

  }
  double PolyPhaseFilter::GetReal(uint channel){
    return norm*creal(outcomplex[channel]);
  }
  double PolyPhaseFilter::GetImag(uint channel){
    return norm*cimag(outcomplex[channel]);
  }
  
  complex double PolyPhaseFilter::GetComplex(uint channel){
    return norm*outcomplex[channel];
  }
  
  
  
  



}//namespace CR
