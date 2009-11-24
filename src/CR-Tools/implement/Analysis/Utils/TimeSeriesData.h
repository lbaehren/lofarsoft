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

#ifndef TIMESERIESDATA_H
#define TIMESERIESDATA_H

// Standard library header files
#include <iostream>
#include <assert.h>


// CR-Tools header files
#include <crtools.h>
using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TimeSeriesData
    
    \ingroup Analysis
    
    \brief Abstract class for reading or simulating  timeseries
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tTimeSeriesData.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Reads or simulates timeseries
    
    <h3>Example(s)</h3>
    See tTimeSeriesData.cc

  */  
  class TimeSeriesData{
  public:
    TimeSeriesData(){};
    ~TimeSeriesData(){};
    
    virtual  void getTimeSeries(vector<double>& out,uint blocksize,int ant, uint startSample) const =0 ;
      
  };

}//namespace CR
#endif
