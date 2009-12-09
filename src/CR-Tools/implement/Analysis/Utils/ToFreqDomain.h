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

#ifndef TOFREQDOMAIN_H
#define TOFREQDOMAIN_H

// Standard library header files
#include <complex>
#include <iostream>
#include <assert.h>
#include <vector>





// CR-Tools header files
#include <crtools.h>

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class ToFreqDomain
    
    \ingroup Analysis
    
    \brief Abstract class for converting timeseries to the frequency domain (either ppf or fft or....)
    
    \author Maaijke Mevius

    \date 2009/11/17

    \test tToFreqDomain.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Converts timeseries data to freq domain
    
    <h3>Example(s)</h3>
    See tToFreqDomain.cc

  */  
  typedef complex<double> mydcomplex;

  class ToFreqDomain{
  public:
    ToFreqDomain(){};
    virtual ~ToFreqDomain(){};
    virtual void Convert(const vector<double> &in)=0;

    virtual double GetPhase(uint channel) =0;
    virtual double GetAmp(uint channel) =0;
    virtual double GetReal(uint channel) =0;
    virtual double GetImag(uint channel) =0;

    virtual mydcomplex GetComplex(uint channel) =0;

    
/*     void setNyquistZone(uint nzone){nyquistZone=nzone;} */
/*     void setSampleFrequency(double freq){SampleFrequency=freq;} */
  protected:
/*     uint nyquistZone; */
/*     double SampleFrequency; */
    uint blocksize;
    double norm;
  };

}//namespace CR
#endif
