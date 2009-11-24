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

#ifndef SIMPLEBEAMFORMER_H
#define SIMPLEBEAMFORMER_H


// Standard library header files
#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>
#include <complex.h>
//CR-Tools

#include <crtools.h>

using namespace std;



namespace CR { // Namespace CR -- begin
  
  /*!
    \class SimpleBeamFormer
    
    \ingroup Analysis
    
    \brief Simple BeamForming
    
    \author Maaijke Mevius

    \date 2009/11/13

    \test tSimpleBeamFormer.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>SecondStagePipeline
    </ul>
    
    <h3>Synopsis</h3>
    Forming Beam in given direction
    <h3>Example(s)</h3>
    See tSimpleBeamFormer.cc

  */  
  class SimpleBeamFormer{
  public:
    static const double pi = 3.141592653589793238462643383279502884197169 ;
    static  const double speed_light = 299792458;

    SimpleBeamFormer(uint nr_chan=1);
    ~SimpleBeamFormer();
    
    void Add(double Amp, double Phase, int iant, double freq,uint pixelnr,uint channelnr=0,bool real_imag=false);
    void Add(complex double data, int iant, double freq,uint pixelnr,uint channelnr=0);
    
    double GetPhase(uint pixelnr,uint channelnr=0);
    double GetAmp(uint pixelnr,uint channelnr=0);
    double GetReal(uint pixelnr,uint channelnr=0);
    double GetImag(uint pixelnr,uint channelnr=0);
    complex double GetComplex(uint pixelnr,uint channelnr=0);
    const vector<complex double> GetComplexBlock(){return itsBeam;}
    /*!
      \brief Set the antenna positions(x,y,z in m, sizeof vector=3*nofAntennas)
    */
    void setAntennas(const vector<double> & positions);
    void setAzEl(const vector<double> &az,const vector <double> &el);
    void Clear();
  private:
    void setDelays();
    void init();
    vector<complex double> itsBeam;
    int nofAntennas;
    uint pixels;
    uint nofChannels;
    vector<double> antPos;
    vector<double> Az,El;
    vector<double> delays;
  };


}//namespace CR


#endif
