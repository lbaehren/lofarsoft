/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
 *   maaijke mevius (<mail>)                                                     *
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

#include <Calibration/RFIMitigation.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  RFIMitigation::RFIMitigation ()
  {;}
  
  RFIMitigation::RFIMitigation (RFIMitigation const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  RFIMitigation::~RFIMitigation ()
  {
    destroy();
  }
  
  void RFIMitigation::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    operator=
  
  RFIMitigation& RFIMitigation::operator= (RFIMitigation const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void RFIMitigation::copy (RFIMitigation const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                      summary
  
  void RFIMitigation::summary (std::ostream &os)
  {
    os << "[RFIMitigation] Summary of internal parameters." << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  void RFIMitigation::doDownsampling(const Vector<Double> &spectrumVec,
				    const uint nr_samples,
				    Vector<Double> &amplVec,
				    Vector<Double> &rmsVec)
  {
    
    if(!amplVec.size()==nr_samples){
      std::cerr << "[RFIMitigation] Warning, amplVec size wrong, resizing." << std::endl;
    //warning
      amplVec.resize(nr_samples);
    }
    if(!rmsVec.size()==nr_samples){
      //warning
       std::cerr << "[RFIMitigation] Warning, rmsVec size wrong, resizing." << std::endl;
       rmsVec.resize(nr_samples);
    }
    uint nr_channels = spectrumVec.size();
    uint nr_channels_per_segment = nr_channels/nr_samples;
    int extra_channels= (nr_channels%nr_samples); 
//     if(extra_channels > 0){
//       std::cerr << "[RFIMitigation] Warning, nr_samples does not match. Putting extra channels in last bin" << std::endl;
//       ;//warning: add extra channels to last bin
//     }
    Vector<Double> tempVec(nr_channels_per_segment);
    for(uint isamp=0;isamp<nr_samples;isamp++)
      {
	for(uint ichan=0;ichan<nr_channels_per_segment;ichan++){
	  tempVec(ichan)=spectrumVec(isamp*nr_channels_per_segment+ichan);
	  
	}
	if(isamp==(nr_samples-1))
	  {//add extra channels to last bin
	    tempVec.resize(nr_channels_per_segment+extra_channels);
	    for(uint ichan=nr_channels_per_segment;ichan<extra_channels+nr_channels_per_segment;ichan++){
	      tempVec(ichan)=spectrumVec(isamp*nr_channels_per_segment+ichan);
	      
	    }
	  }
	//now calculate RMS and mean
	amplVec(isamp)= median(tempVec);
	rmsVec(isamp) = rms(tempVec);
	
      }
    
    
  }
  
  
  void RFIMitigation::doBaselineFitting(const Vector<Double> &amplVec,
				       const Vector<Double> &rmsVec,
				       const Double rmsThresholdValue,
				       Vector<Double> &fitVec,
				       int method){
    assert(amplVec.size() == rmsVec.size());
    uint nr_channels = fitVec.size();
    uint nr_samples = amplVec.size();
    uint nr_channels_per_segment = nr_channels/nr_samples;
    assert(nr_channels_per_segment>0);
    int extra_channels= (nr_channels%nr_samples); 
//     if(extra_channels > 0){
//       std::cerr << "[RFIMitigation] Warning, nr_samples does not match. Assuming extra channels in last bin" << std::endl;
//       //warning: assume extra channels in last bin
//     }
    uint eff_nr_samples=0;
    //first get effective nr of samples
    for( uint isamp=0;isamp<nr_samples;isamp++)
      if(rmsVec(isamp)<rmsThresholdValue)
	eff_nr_samples++;
    
    Vector<Double> xin(eff_nr_samples);
    Vector<Double> yin(eff_nr_samples);
    Vector<Double> xout(nr_channels);
    
    for( uint ichan=0;ichan<nr_channels;ichan++)
      xout(ichan)= Double(ichan);
    uint eff_isamp=0;
    for( uint isamp=0;isamp<nr_samples-1;isamp++){
      if(rmsVec(isamp)<rmsThresholdValue){//only add data point if rms below treshold
	xin(eff_isamp)=isamp*nr_channels_per_segment+nr_channels_per_segment*0.5;
	yin(eff_isamp) = amplVec(isamp);
	eff_isamp++;
      }
    }
    //treat last channel separately
    if(rmsVec(nr_samples-1)<rmsThresholdValue){//only add data point if rms below treshold
      xin(eff_isamp)=nr_samples-1+(nr_channels_per_segment+extra_channels)*0.5;
      yin(eff_isamp) = amplVec(nr_samples-1);
    }
    
    int casamethod;
    switch(method){
    case RFIMitigation::linear:
      casamethod=  InterpolateArray1D<Double,Double>::linear;
      break;
    case RFIMitigation::cubic:
      casamethod=  InterpolateArray1D<Double,Double>::cubic;
      break;
    case RFIMitigation::spline:
      casamethod=  InterpolateArray1D<Double,Double>::spline;
      break;
    default:
      casamethod=  InterpolateArray1D<Double,Double>::linear;
      break;
    }
    //now do the interpolation
    
    InterpolateArray1D<Double,Double>::interpolate(fitVec,xout,xin,yin,casamethod);
    
    
    
  }
  
  void RFIMitigation::doBaselineFitting(const Vector<Double> &spectrumVec,
				       const uint nr_samples,
				       const Double rmsThresholdValue,
				       Vector<Double> &fitVec,
				       int method){
    Vector<Double> amplVec(nr_samples);
    Vector<Double> rmsVec(nr_samples);
    RFIMitigation::doDownsampling(spectrumVec,
				 nr_samples,
				 amplVec,
				 rmsVec);
    
    RFIMitigation::doBaselineFitting(amplVec,
				    rmsVec,
				    rmsThresholdValue,
				    fitVec,
				    method);
  }



  
  void RFIMitigation::doRFIFlagging(const Vector<Double> &spectrumVec,
				   const Vector<Double> &fitVec,
				   const Double flagThresholdValue,
				   Vector<Int> &flagVec){

    assert(spectrumVec.size()==fitVec.size());
    uint nr_channels=fitVec.size();
    
    if(!(flagVec.size()==nr_channels)){
      std::cerr << "[RFIMitigation] Warning, flagVec size wrong, resizing." << std::endl;
      //warning
      flagVec.resize(nr_channels);
    }
    for(uint ichan=0;ichan<nr_channels;ichan++){
      if(fitVec(ichan)!=0)   flagVec(ichan) = (spectrumVec(ichan)/fitVec(ichan))>flagThresholdValue;
      else flagVec(ichan)=1;
    }
    
  }

  

  void RFIMitigation::doRFIFlagging(const Vector<Double> &spectrumVec,
				   const uint nr_samples,
				   const Double rmsThresholdValue,
				   const Double flagThresholdValue,
				   Vector<Int> &flagVec,
				   int method){
    Vector<Double> amplVec(nr_samples);
    Vector<Double> rmsVec(nr_samples);
    RFIMitigation::doDownsampling(spectrumVec,
				 nr_samples,
				 amplVec,
				 rmsVec);
 
    uint nr_channels= spectrumVec.size();
    Vector<Double> fitVec(nr_channels);
    RFIMitigation::doBaselineFitting(amplVec,
				    rmsVec,
				    rmsThresholdValue,
				    fitVec,
				    method);
    
    RFIMitigation::doRFIFlagging(spectrumVec,
				fitVec,
				flagThresholdValue,
				flagVec);
  }
      

  

  void RFIMitigation::doRFIMitigation(const Vector<Double> &spectrumVec,
				     const Vector<Double> &fitVec,
				     const Vector<Int> &flagVec,
				     Vector<Double> &mitigatedSpectrumVec){
    assert(spectrumVec.size()==fitVec.size());
    assert(spectrumVec.size()==flagVec.size());
    uint nr_channels=fitVec.size();
    
    if(!(mitigatedSpectrumVec.size()==nr_channels)){
      std::cerr << "[RFIMitigation] Warning, mitigatedSpectrumVec size wrong, resizing." << std::endl;
      //warning
      mitigatedSpectrumVec.resize(nr_channels);
    }
    
    for(uint ichan=0;ichan<nr_channels;ichan++)
      {
	if(flagVec(ichan))
	  mitigatedSpectrumVec(ichan) = fitVec(ichan);
	else
	  mitigatedSpectrumVec(ichan) = spectrumVec(ichan);
      }
    
  }

  void RFIMitigation::doRFIMitigation(const Vector<Double> &spectrumVec,
				     const Vector<Int> &flagVec,
				     const Double fitValue,
				     Vector<Double> &mitigatedSpectrumVec){
    assert(spectrumVec.size()==flagVec.size());
    uint nr_channels=spectrumVec.size();
    
    if(!(mitigatedSpectrumVec.size()==nr_channels)){
      std::cerr << "[RFIMitigation] Warning, mitigatedSpectrumVec size wrong, resizing." << std::endl;
      //warning
      mitigatedSpectrumVec.resize(nr_channels);
    }
    
    for(uint ichan=0;ichan<nr_channels;ichan++)
      {
	if(flagVec(ichan))
	  mitigatedSpectrumVec(ichan) = fitValue;
	else
	  mitigatedSpectrumVec(ichan) = spectrumVec(ichan);
      }
    
  }

  


  void RFIMitigation::doRFIMitigation(const Vector<Double> &spectrumVec,
				     const uint nr_samples,
				     const Double rmsThresholdValue,
				     const Double flagThresholdValue,
				     Vector<Double> &mitigatedSpectrumVec,
				     int method){
     
    uint nr_channels= spectrumVec.size();
    Vector<Double> fitVec(nr_channels);
    RFIMitigation::doBaselineFitting(spectrumVec,
				    nr_samples,
				    rmsThresholdValue,
				    fitVec,
				    method);
    Vector<Int> flagVec(nr_channels);
    RFIMitigation::doRFIFlagging(spectrumVec,
				fitVec,
				flagThresholdValue,
				flagVec);
    
    RFIMitigation::doRFIMitigation(spectrumVec,
				  fitVec,
				  flagVec,
				  mitigatedSpectrumVec);
    
				  
  }

  
  void RFIMitigation::doRFIMitigation(const Vector<Double> &spectrumVec,
				     const uint nr_samples,
				     const Double rmsThresholdValue,
				     const Double flagThresholdValue,
				     const Double fitValue,
				     Vector<Double> &mitigatedSpectrumVec,
				     int method){
     
    uint nr_channels= spectrumVec.size();
    Vector<Double> fitVec(nr_channels);
    RFIMitigation::doBaselineFitting(spectrumVec,
				    nr_samples,
				    rmsThresholdValue,
				    fitVec,
				    method);
    Vector<Int> flagVec(nr_channels);
    RFIMitigation::doRFIFlagging(spectrumVec,
				fitVec,
				flagThresholdValue,
				flagVec);
    
    RFIMitigation::doRFIMitigation(spectrumVec,
				  flagVec,
				  fitValue,
				  mitigatedSpectrumVec);
    
				  
  }



  

 

} // Namespace CR -- end
