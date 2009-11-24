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

#include <Analysis/Utils/SimpleBeamFormer.h>


namespace CR { // Namespace CR -- begin

  SimpleBeamFormer::SimpleBeamFormer(uint nr_chan):pixels(0),nofChannels(nr_chan){
    Clear();
  }

  SimpleBeamFormer::~SimpleBeamFormer(){
  }


  void SimpleBeamFormer::Clear(){
    itsBeam.assign(pixels*nofChannels,0+0*I);
  }
  
  double SimpleBeamFormer::GetPhase(uint pixelnr,uint channelnr){
    return carg(itsBeam[channelnr*pixels+pixelnr]);
  }

  double SimpleBeamFormer::GetAmp(uint pixelnr,uint channelnr)
  {
    double re = creal(itsBeam[channelnr*pixels+pixelnr]);
    double im = cimag(itsBeam[channelnr*pixels+pixelnr]);
    return sqrt(re*re+im*im);
  }
  double SimpleBeamFormer::GetReal(uint pixelnr,uint channelnr){
    return creal(itsBeam[channelnr*pixels+pixelnr]);
  }
  double SimpleBeamFormer::GetImag(uint pixelnr,uint channelnr){
    return cimag(itsBeam[channelnr*pixels+pixelnr]);
  }
  
  complex double SimpleBeamFormer::GetComplex(uint pixelnr,uint channelnr){
    return itsBeam[channelnr*pixels+pixelnr];
  }
  
  
  void SimpleBeamFormer::setAntennas(const vector<double> & positions){
    nofAntennas = positions.size()/3.;
    antPos = positions;
  }

  void SimpleBeamFormer::setAzEl(const vector<double> &az,const vector<double> &el){
    Az=az;El=el;
    assert(az.size()==el.size());
    pixels = az.size();
    itsBeam.resize(pixels);
    setDelays();

  }

  void SimpleBeamFormer::setDelays(){
    if(nofAntennas<=0) return;
    delays.resize(pixels*nofAntennas);
    
    for(uint ix=0;ix<pixels;ix++){
      double az=Az[ix];
      double el=El[ix];
      
      double source_pos_x = cos(az)*cos(el) ;
      double source_pos_y = sin(az)*cos(el) ;
      double source_pos_z = sin(el) ; 
      
      for(int iant=0;iant<nofAntennas;iant++){
	double delay = (-1./speed_light)*(source_pos_x*antPos[iant*3]+source_pos_y*antPos[iant*3+1]+source_pos_z*antPos[iant*3+2])  ;
	delays[ix*nofAntennas +iant]=delay*2*pi;
      }
    
      //   cout<<"delay "<<ix<<": "<<delays[ix*nofAntennas+3]<<endl;
    }
    
    
  }


  void SimpleBeamFormer::Add(double Amp, double Phase, int iant, double freq,uint pixelnr,uint channel_nr,bool real_imag){
    complex double data = Amp*cos(Phase)+Amp*sin(Phase)*I;
    Add(data,iant,freq,pixelnr);
  }

  void SimpleBeamFormer::Add(complex double data,int iant, double freq,uint pixelnr,uint channelnr){
    double delay = delays[pixelnr*nofAntennas+iant]*freq;
    itsBeam[channelnr*pixels+pixelnr]+=data*(cos(delay)+sin(delay)*I);
  }
  



}//namespace CR
