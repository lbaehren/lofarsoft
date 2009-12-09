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

#ifndef SIMULATOR_H
#define SIMULATOR_H

// Standard library header files
#include <iostream>
#include <assert.h>


// CR-Tools header files
#include <crtools.h>
#include <Analysis/Utils/SimulFunction.h>
#include <Analysis/Utils/TimeSeriesData.h>

// AIPS++/CASA header files

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Simulator
    
    \ingroup Analysis
    
    \brief Simulates timeseries data for LOFAR
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tSimulator.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Simulates LOFAR data including timeshifts and beamshapes. The original data is either pulse or conintuous source, or is definedby the user.
    
    <h3>Example(s)</h3>
    See tSimulator.cc

  */  
  class Simulator:public TimeSeriesData{
  public:

    typedef enum {
      /*!
	Short pulse in time
      */
      PULSE,
      /*!
	Continuous source, user can define number of frequencies and range
      */
      CONTINUOUS_SOURCE
    }Function_Type ;

    
    /*!
      \brief Default constructor
    */
    Simulator ();
    /*!
      \brief Destructor
    */
    ~Simulator ();
   /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Simulator.
    */
    std::string className () const {
      return "SIMULATOR";
    }

    /*!
      \brief Provide a summary of the internal status

      (Not implemented yet.)
    */
    void summary(){};    
    /*!
      \brief Set the function (time -> value)
    */
    void setFunction(double (* myFunc)(double));
    /*!
      \brief Set the function to own SimulFunction
    */
    void setFunction(SimulFunction &myFuncClass);
    /*!
      \brief Set the function to standard function
    */
    void setFunction(Function_Type function= PULSE);
    /*!
      \brief Set the antenna positions(x,y,z in m, sizeof vector=3*nofAntennas)
    */
    void setAntennas(const vector<double> & positions);
    /*!
      \brief Set the viewing direction 
    */
    void setAzEl(double az,double el){Az=az;El=el;};
    /*!
      \brief Get the viewing direction 
    */
    double getAz(){return Az;}
    /*!
      \brief Get the viewing direction 
    */
    double getEl(){return El;}
;
    /*!
      \brief Set the SampleFrequency
    */
    void setSampleFreq(double freq=200.e6){SampleFreq=freq; TimeBin = 1./SampleFreq;}
    /*!
      \brief Get the SampleFreq 
    */
    double getSampleFreq(){return SampleFreq;}
    /*!
      \brief Get the TimeBinSize 
    */
    double getTimeBin(){return TimeBin;}
    /*!
      \brief Get the timeseries data for direction az, el, antenna number ant
      \return vector of size blockize with timeseries data
    */
    void getTimeSeries(vector<double>& out,uint blocksize,double az,double el, int ant,uint starttime,double timestep) const;
    /*!
      \brief Get the timeseries data for direction az, el, antenna number ant
      \return vector of size blockize with timeseries data
    */
    void getTimeSeries(vector<double>& out,uint blocksize,int ant, uint startSample) const;
    
  protected:
    void init();
    
  private:
    SimulFunction * thisFunctionClassp;
    SimulFunction * myFunction;
    double (* thisFunction)(double);
    vector<double> antPos;
    double Az,El;
    double SampleFreq,TimeBin;
    int nofAntennas;
    bool use_user_function;
  };
} // Namespace CR -- end

#endif /* SIMULATOR_H */
  
