/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef CONTSOURCEHBATILE_H
#define CONTSOURCEHBATILE_H

// Standard library header files
#include <iostream>
#include <vector>
#include <math.h>

// CR-Tools header files
#include <crtools.h>
#include <Analysis/Utils/SimulFunction.h>
// AIPS++/CASA header files

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class CONTSOURCEHBATILE
    
    \ingroup Analysis
    
    \brief The function to simulate time data of a continuous source at position az,el. with tile beam potining upwards
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tSimulator.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    See tSimulator.cc

  */  
  class ContSourceHBATile:public SimulContSource{
  public:
    ContSourceHBATile():az(0.),el(0.)
      {
	SimulContSource::SimulContSource();
	init();
      }
    
    ~ContSourceHBATile(){};
      
    std::string className () const {
      return "CONTSOURCEHBATILE";
    }
  

  protected:
    void initTileWeights(){
      
      double source_pos_x = cos(az)*cos(el) ;
      double source_pos_y = sin(az)*cos(el) ;
      for(uint ifreq=0;ifreq<nr_simfreq;ifreq++)
	{
	  tile_offsets[ifreq]=0;
	  for(int ix=0;ix<4;ix++)
	    for(int iy=0;iy<2;iy++)
	      {
		//sin(a+b)+sin(a-b)=sin(a)*2*cos(b);
		double delay = cos((1./speed_light)*(source_pos_x*(1.5-ix)*1.25+source_pos_y*(1.5-iy)*1.25)*2*pi*simFreq[ifreq]) ;
		//	    cout<<"offset "<<ix<<":"<<iy<<" = "<<delay<<endl;
		tile_offsets[ifreq]+=delay/8.;
	      }
	}

    }
    
    void init(){
      SimulContSource::init();
      tile_offsets.resize(nr_simfreq);
      initTileWeights();
    }
    
  public:
    void setAzEl(double a,double e){
      az=a;el=e;
      initTileWeights();
     }
    virtual double evaluate(double time){
      double val=0.;
      for(uint ifreq=0;ifreq<nr_simfreq;ifreq++)
	{
	  val+=simAmp[ifreq]*sin((time)*2*pi*simFreq[ifreq])*tile_offsets[ifreq];
	}
      return val;
    }


  private:
    double az,el;
    vector<double> tile_offsets;

  };

}//namespace CR

#endif

