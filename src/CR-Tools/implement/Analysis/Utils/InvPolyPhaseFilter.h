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

#ifndef INVPOLYPHASEFILTER_H
#define INVPOLYPHASEFILTER_H


// Standard library header files
#include <iostream>
#include <assert.h>
#include <complex>
#include "fftw3.h" 
#include <math.h>
//CR-Tools

#include <crtools.h>
#include <Analysis/Utils/ToTimeDomain.h>

using namespace std;



namespace CR { // Namespace CR -- begin
  
  /*!
    \class InvPolyPhaseFilter
    
    \ingroup Analysis
    
    \brief Another class to filter timeseries data using PPF
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tInvPolyPhaseFilter.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Another PPF implementation
    <h3>Example(s)</h3>
    See tInvPolyPhaseFilter.cc

  */  
  class InvPolyPhaseFilter:public ToTimeDomain{
  public:
    InvPolyPhaseFilter(uint size=1024,uint nrtaps=16);
    ~InvPolyPhaseFilter();
    void setWeights(const vector<double> &weights){itsWeights= weights;}
    void Convert(const vector<mydcomplex> &in);
    void Convert();
    void resetFreq();//init to 0
    void setFreqBin(uint bin,mydcomplex value);
    double GetAmp(uint itime);
    const vector<double> GetBlock();
    vector<double> GetEBlock();
 
 private:
    void init();
    void init_fft();
    void get_invppf();
    void AddBlock();
    vector<double> itsWeights;
    uint nrTaps;
    fftw_complex *incomplex;
    double *outreal;
    fftw_plan pc2r;
    uint startpos;
    vector<double> itsData;
    vector<double> itsTimeData;
    
  };


}//namespace CR


#endif
