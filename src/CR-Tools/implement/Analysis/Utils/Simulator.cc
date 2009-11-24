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

#include <Analysis/Utils/Simulator.h>


namespace CR { // Namespace CR -- begin


  Simulator::Simulator():myFunction(0),Az(0.),El(0.5*pi),SampleFreq(200.e6),TimeBin(5e-9),nofAntennas(-1),use_user_function(false)
  {
    init();
  }
 
 Simulator::~Simulator(){
   if(myFunction)
     delete myFunction;

  }
  void Simulator::init(){
    setFunction();
  }

  void Simulator::setFunction(double (* myFunc)(double)){
    use_user_function=true;
    thisFunction=myFunc;
    
  }
  
  void Simulator::setFunction(SimulFunction & myFuncClass){
    thisFunctionClassp=&myFuncClass;
    
  }
  
  void Simulator::setFunction(Function_Type function){
    if(myFunction)
      delete myFunction;
    use_user_function=false;
    
    switch(function){
    case Simulator::PULSE:
      myFunction=new SimulPulse();
      thisFunctionClassp=myFunction;
      break;
    case Simulator::CONTINUOUS_SOURCE:
      myFunction=new SimulContSource();
      thisFunctionClassp=myFunction;
     //     thisFunctionClassp->SetFreq(lowend,highend,1500);
	break;
    default:
      myFunction=new SimulPulse();
      thisFunctionClassp=myFunction;
      break;
    
    }    
  }
 
  void Simulator::setAntennas(const vector<double> & positions){
    nofAntennas = positions.size()/3.;
    antPos = positions;
    
  }

  void Simulator::getTimeSeries(vector<double>& out,uint blocksize,double az,double el, int ant,uint starttime,double timestep)const{
 
    assert(nofAntennas>ant);
    out.resize(blocksize);
    //calculate delay;
    double source_pos_x = cos(az)*cos(el) ;
    double source_pos_y = sin(az)*cos(el) ;
    double source_pos_z = sin(el) ; 
    
    double delay = (1./speed_light)*(source_pos_x*antPos[ant*3+0]+source_pos_y*antPos[ant*3+1]+source_pos_z*antPos[ant*3+2])  ;
   
    double time = (starttime)*timestep-delay;


    if(use_user_function)
      for(uint it =0;it<blocksize;it++){
	
	out[it]=thisFunction(time);
	time+=timestep;
      }
    else
      for(uint it =0;it<blocksize;it++){
	//	cout<<"time "<<time<<endl;
	out[it]=thisFunctionClassp->evaluate(time);
	time+=timestep;
      }
 
  }
 
 void Simulator::getTimeSeries(vector<double>& out,uint blocksize,int ant,uint startSample)const {
   getTimeSeries(out,blocksize,Az,El,ant,startSample,TimeBin);
 }
  
}
