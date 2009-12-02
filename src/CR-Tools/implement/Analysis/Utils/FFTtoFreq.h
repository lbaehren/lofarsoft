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

#ifndef FFTTOFREQ_H
#define FFTTOFREQ_H


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
    \class FFTtoFreq
    
    \ingroup Analysis
    
    \brief FFT of timeseries
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tFFTtoFreq.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Another FFT implementation
    <h3>Example(s)</h3>
    See tFFTtoFreq.cc

  */  
   class FFTtoFreq:public ToFreqDomain{
  public:
    FFTtoFreq(uint size=1024);
    ~FFTtoFreq();
    void Convert(const vector<double> &in);
    
    double GetPhase(uint channel);
    double GetAmp(uint channel);
    double GetReal(uint channel);
    double GetImag(uint channel);
    mydcomplex GetComplex(uint channel);
    
  private:
    void init();
    void init_fft();
    fftw_complex *outcomplex;
    double *inreal;
    fftw_plan pr2c;
    
  };


}//namespace CR


#endif
