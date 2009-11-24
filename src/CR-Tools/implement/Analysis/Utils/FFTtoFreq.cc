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

#include <Analysis/Utils/FFTtoFreq.h>


namespace CR { // Namespace CR -- begin

  FFTtoFreq::FFTtoFreq(uint size){
    blocksize=size;
    inreal=NULL;
    outcomplex=NULL;
    init();
  }

  FFTtoFreq::~FFTtoFreq(){
    free(outcomplex);
    free(inreal);

}

  void FFTtoFreq::init(){
    
    norm=1./blocksize;

    init_fft();
  }

  void FFTtoFreq::init_fft(){
    outcomplex = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (blocksize/2+1));
    inreal = (double*) fftw_malloc(sizeof(double) * (blocksize));
    
    pr2c= fftw_plan_dft_r2c_1d(blocksize, inreal, outcomplex, FFTW_MEASURE );
  }




  void FFTtoFreq::Convert(const vector<double> & in){
    assert(in.size()>=blocksize);
    for(uint i=0;i<blocksize;i++)
      inreal[i]=in[i];
    fftw_execute(pr2c);

  }

  
  double FFTtoFreq::GetPhase(uint channel){
    return carg(outcomplex[channel]);
  }
  double FFTtoFreq::GetAmp(uint channel){
    double re = creal(outcomplex[channel]);
    double im = cimag(outcomplex[channel]);
    return sqrt(re*re+im*im);

  }
  double FFTtoFreq::GetReal(uint channel){
    return creal(outcomplex[channel]);
  }
  double FFTtoFreq::GetImag(uint channel){
    return cimag(outcomplex[channel]);
  }
  
   complex double FFTtoFreq::GetComplex(uint channel){
    return outcomplex[channel];
  }
  
  
  
  



}//namespace CR
