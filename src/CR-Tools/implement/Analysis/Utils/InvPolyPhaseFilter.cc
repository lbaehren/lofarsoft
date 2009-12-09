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

#include <Analysis/Utils/InvPolyPhaseFilter.h>


namespace CR { // Namespace CR -- begin

  InvPolyPhaseFilter::InvPolyPhaseFilter(uint size,uint nrtaps):nrTaps(nrtaps),startpos(0){
    blocksize=size;
    outreal=NULL;
    incomplex=NULL;
    init();
  }

  InvPolyPhaseFilter::~InvPolyPhaseFilter(){
    free(incomplex);
    free(outreal);
    
}

  void InvPolyPhaseFilter::init(){
    
    itsTimeData.resize(blocksize);
    itsData.resize(nrTaps*blocksize);
    init_fft();
  }

  void InvPolyPhaseFilter::init_fft(){
    incomplex = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (blocksize/2+1));
    outreal = (double*) fftw_malloc(sizeof(double) * (blocksize));
    
    pc2r= fftw_plan_dft_c2r_1d(blocksize, incomplex, outreal, FFTW_MEASURE|FFTW_PRESERVE_INPUT );
    resetFreq();
  }


  void InvPolyPhaseFilter::get_invppf(){
    assert(itsWeights.size() >= nrTaps*blocksize);
    for(uint ib=0;ib<blocksize;ib++){
      double val=0;
      for(uint ifilt=0;ifilt<nrTaps;ifilt++){
	uint pos = ifilt*blocksize+ib +startpos;
	if(pos>=nrTaps*blocksize) pos-=nrTaps*blocksize;
	//val+=itsWeights[ifilt*blocksize+ib]*itsData[pos];
	val+=itsWeights[ifilt+ib*nrTaps]*itsData[pos];
	
      }
      //    cout<<"val["<<ib<<"]="<<val<<endl;
      itsTimeData[ib]=val;
    }
  }
  
  void InvPolyPhaseFilter::AddBlock(){
    for(uint i=0;i<blocksize;i++){
      itsData[startpos+i]=outreal[i]; //normalized
    }
    startpos+=blocksize;
    if(startpos>=nrTaps*blocksize) startpos=0;
  }
  
  void InvPolyPhaseFilter::setFreqBin(uint bin,mydcomplex value){
    assert(bin<blocksize/2+1);
    incomplex[bin][0] = real(value);
    incomplex[bin][1] = imag(value);
  }
 
  void InvPolyPhaseFilter::resetFreq(){
     for(uint i=0;i<blocksize/2+1;i++){
      incomplex[i][0] = 0.;
      incomplex[i][1] = 0.;
     }
  }
  
  void InvPolyPhaseFilter::Convert(const vector<mydcomplex > & in){
    assert(in.size()==blocksize/2+1);
    for(uint i=0;i<blocksize/2+1;i++){
      incomplex[i][0] = real(in[i]);
      incomplex[i][1] = imag(in[i]);
    }
    Convert();
  }
  
  void InvPolyPhaseFilter::Convert(){
    fftw_execute(pc2r);
    AddBlock();
    get_invppf();

  }


  double InvPolyPhaseFilter::GetAmp(uint itime){
     return itsTimeData[itime];

  }

  const vector<double> InvPolyPhaseFilter::GetBlock(){
    return itsTimeData;
  }
  
  vector<double> InvPolyPhaseFilter::GetEBlock(){
    return itsTimeData;
  }
  
  



}//namespace CR
