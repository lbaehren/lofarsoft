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

#ifndef SIMULFUNCTION_H
#define SIMULFUNCTION_H

// Standard library header files
#include <iostream>
#include <vector>
#include <math.h>

// CR-Tools header files
#include <crtools.h>
// AIPS++/CASA header files

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Simulator
    
    \ingroup Analysis
    
    \brief The function to simulated time data
    
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
  class SimulFunction{

  public:
    /*!
      \brief Default constructor
    */
    SimulFunction (){}
    /*!
      \brief Destructor
    */
    ~SimulFunction (){}
   /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Simulator.
    */
    virtual std::string className () const {
      return "SIMULFUNCTION";
    }

    /*!
      \brief Provide a summary of the internal status

      (Not implemented yet.)
    */
    virtual void summary(){};    
    virtual double evaluate(double time){
      return 0.;
    }
    
  protected:
    virtual void init(){};
    
  };


  class SimulPulse: public SimulFunction{
  public:
    SimulPulse():pulse_time_min(0.),pulse_time_max(1.)
      {}
      
    ~SimulPulse(){}
    std::string className () const {
      return "SIMULPULSE";
    }
   
    void setMinTime(double mintime =0.){
      pulse_time_min=mintime;
    }
    void setMaxTime(double maxtime =1.){
      pulse_time_max=maxtime;
    }
    void setTime(double min =0.,double max=1.){
      pulse_time_min=min;
      pulse_time_max=max;
    }
    
    double evaluate(double time){
      if(time<pulse_time_min) return 0;
      if(time>pulse_time_max) return 0;
      return 1;
    }
  private:
    double pulse_time_min;
    double pulse_time_max;
  };




  class SimulContSource: public SimulFunction{
  public:
    static const  double pi = 3.141592653589793238462643383279502884197169 ;
    static  const double speed_light = 299792458;


  public:
    SimulContSource():
      lowend(100.e6),highend(200.e6),nr_simfreq(1000)
      {
	init();
      }
      
    ~SimulContSource(){}
    
    virtual std::string className () const {
      return "SIMULCONTSOURCE";
    }
   
    void SetFreq(double low,double high,uint nrfreq){
      lowend=low;
      highend=high;
      nr_simfreq=nrfreq;
      init();
    }

    

    virtual double evaluate(double time){
      double val=0.;
      for(uint ifreq=0;ifreq<nr_simfreq;ifreq++)
	{
	  val+=simAmp[ifreq]*sin((time)*2*pi*simFreq[ifreq]);
	}
      return val;
    }

  protected:
    virtual void init(){
      simAmp.resize(nr_simfreq);
      simFreq.resize(nr_simfreq);
      double freqbin2=(highend-lowend)/nr_simfreq;
      for(uint ifreq=0;ifreq<nr_simfreq;ifreq++){
	double random = double(rand())/RAND_MAX; 
	double random2 = double(rand())/RAND_MAX; 
       
	simAmp[ifreq]=(1.+random)/nr_simfreq;
	simFreq[ifreq]=lowend+(ifreq+random2)*freqbin2;
      }
      
    }
    
  protected:
   double lowend;
   double highend;
   uint nr_simfreq;
   vector<double> simAmp;
   vector<double> simFreq;

  };
 
    
} // Namespace CR -- end

#endif /* SIMULFUNCTION_H */
  
 





