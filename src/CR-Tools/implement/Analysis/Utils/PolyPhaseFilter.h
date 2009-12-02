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

#ifndef POLYPHASEFILTER_H
#define POLYPHASEFILTER_H


// Standard library header files
#include <iostream>
#include <assert.h>
#include <complex>
#include "fftw3.h" 
#include <math.h>
//CR-Tools

#include <crtools.h>
#include <Analysis/Utils/ToFreqDomain.h>

using namespace std;



namespace CR { // Namespace CR -- begin
  
  /*!
    \class PolyPhaseFilter
    
    \ingroup Analysis
    
    \brief Another class to filter timeseries data using PPF
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tPolyPhaseFilter.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Another PPF implementation
    <h3>Example(s)</h3>
    See tPolyPhaseFilter.cc

  */  
  class PolyPhaseFilter:public ToFreqDomain{
  public:
    PolyPhaseFilter(uint size=1024,uint nrtaps=16);
    ~PolyPhaseFilter();
    void setWeights(const vector<double> &weights){itsWeights= weights;}
    void Convert(const vector<double> &in);
    
    double GetPhase(uint channel);
    double GetAmp(uint channel);
    double GetReal(uint channel);
    double GetImag(uint channel);
    mydcomplex GetComplex(uint channel);
    const fftw_complex * GetComplexP(){return outcomplex;}
 
 private:
    void init();
    void init_fft();
    void get_ppf(const vector<double> & timedata);
    vector<double> itsWeights;
    uint nrTaps;
    fftw_complex *outcomplex;
    double *inreal;
    fftw_plan pr2c;
    
  };


}//namespace CR


#endif
