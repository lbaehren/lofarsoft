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

#ifndef TBBDATA_H
#define TBBDATA_H

// Standard library header files
#include <iostream>
#include <assert.h>

//DAL
#include <crtools.h>
#include <dal/TBB_Timeseries.h>
#include <dal/TBB_StationGroup.h>

// CR-Tools header files
#include <Analysis/Utils/TimeSeriesData.h>



// AIPS++/CASA header files

using namespace std;
using namespace DAL;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TBBData
    
    \ingroup Analysis
    
    \brief Another class to read TBB data, derived from timeseriesdata
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tTBBData.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Reads TBB data. Select either even or odd dipoles or both.
    
    <h3>Example(s)</h3>
    See tTBBData.cc

  */  
  class TBBData:public TimeSeriesData{
  public:
    TBBData();
    ~TBBData();
    void getTimeSeries(vector<double>& out,uint blocksize, int ant, uint startSample) const;
    void getTimeSeries(casa::Vector<double>& out,uint blocksize, int ant, uint startSample) const;
    void initData(vector<string> filenames,bool even=true,bool odd=false); //even, odd is dipole selection within a file (x an y repsectively) 
 
    int getLength(){return minlength;}
    int getNofAntennas(){return nofAntennas;}
 private:
    void  initOffsets();
    vector<TBB_Timeseries *> daldata; //The data files
    vector<uint> filenr;
    vector<uint> dipnr;
    vector<uint> offsets;
    int nofAntennas;
    uint minlength;
    uint dipstart;
    uint dipstep;
  };

}//namespace CR
#endif
