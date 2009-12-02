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

#ifndef TOTIMEDOMAIN_H
#define TOTIMEDOMAIN_H

// Standard library header files
#include <iostream>
#include <assert.h>
#include <vector>
#include <complex>



// CR-Tools header files
#include <crtools.h>
#include <Analysis/Utils/ToFreqDomain.h>

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class ToTimeDomain
    
    \ingroup Analysis
    
    \brief Abstract class for converting timeseries to the frequency domain (either ppf or fft or....)
    
    \author Maaijke Mevius

    \date 2009/11/17

    \test tToTimeDomain.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Converts timeseries data to freq domain
    
    <h3>Example(s)</h3>
    See tToTimeDomain.cc

  */  
  class ToTimeDomain{
  public:
    ToTimeDomain(){};
    ~ToTimeDomain(){};
    virtual void Convert(const vector<mydcomplex> &in)=0;

    virtual double GetAmp(uint channel) =0;
    virtual const vector<double> GetBlock() =0;
    virtual vector<double> GetEBlock() =0;
    
/*     void setNyquistZone(uint nzone){nyquistZone=nzone;} */
/*     void setSampleFrequency(double freq){SampleFrequency=freq;} */
  protected:
/*     uint nyquistZone; */
/*     double SampleFrequency; */
    uint blocksize;
  };

}//namespace CR
#endif
