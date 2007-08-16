/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *   Andreas Horneffer (a.horneffer@astro.ru.nl)                           *
 *   Sven Lafebre (s.lafebre@astro.ru.nl)                                  *
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

#include <LopesLegacy/DataConversion.h>

using std::cerr;
using std::endl;

namespace CR {  // Namespace CR -- begin
  
  DataConversion::DataConversion (const int nofAntennas,
				  const int blocksize)
    : DataTIM40(),
      nofAntennas_p(nofAntennas), blocksize_p(blocksize)
  {
    fftlength_p = blocksize/2+1;
  }
  
  DataConversion::DataConversion (const int nofAntennas,
				  const int blocksize,
				  const int fftlength)
    : DataTIM40(),
      nofAntennas_p (nofAntennas), blocksize_p (blocksize), fftlength_p (fftlength)
  {;}
  
  DataConversion::~DataConversion () {;}
  
  
  // =============================================================================
  //
  // Access to the conversion buffers
  //
  // =============================================================================
  
  Vector<double> DataConversion::Fx2Voltage (const Vector<double>& dataFx,
					     const bool invert)
  {
    double conversionFactor (0.0);
    
    if (invert) {
      conversionFactor = DataTIM40::ADCMaxChannel() / DataTIM40::maxVoltage();
    } else {
      conversionFactor = DataTIM40::maxVoltage() / DataTIM40::ADCMaxChannel();
    }
    
    Vector<double> voltage (dataFx*conversionFactor);
    
    return voltage;
  }
  
  Vector<double> DataConversion::Fx2PowerT (const Vector<double>& dataFx)
  {
    Vector<double> powerT (dataFx);
    powerT *= dataFx;
    return powerT;
  }
  
  Matrix<Complex> DataConversion::FFT2CalFFT ()
  {
    return fft2calfft_p;
  }
  
  bool DataConversion::setFFT2CalFFT (const Matrix<Complex>& fft2calfft)
  {
    bool ok (true);
    
    try {
      fft2calfft_p.resize (fft2calfft.shape());
      fft2calfft_p = fft2calfft;
    } catch (casa::AipsError x) {
      cerr << "[DataConversion::setFFT2CalFFT] " << x.getMesg() << endl;
      ok = false;
    }
    
    return ok;
  }
  
  // =============================================================================
  //
  //  Conversion between data fields
  //
  // =============================================================================
  
  Vector<Complex> DataConversion::FFT2CalFFT (const Vector<Complex>& dataFFT,
					      const int antenna)
  {
    int nofFrequencies (dataFFT.nelements());
    Vector<Complex> dataCalFFT (dataFFT.shape());
    
    for (int freq(0); freq<nofFrequencies; freq++) {
      dataCalFFT(freq) = dataFFT(freq)*fft2calfft_p(freq,antenna);
    }
    
    return dataCalFFT;
  }
  
  Matrix<Complex> DataConversion::FFT2CalFFT (const Matrix<Complex>& dataFFT)
  {
    IPosition shapeConv (fft2calfft_p.shape());
    IPosition shapeData (dataFFT.shape());
    Matrix<Complex> dataCalFFT;
    
    if (shapeData(0) == shapeConv(0)) {
      dataCalFFT.resize(shapeData);
      //
      for (int ant(0); ant<shapeData(0); ant++) {
	dataCalFFT.row(ant) = DataConversion::FFT2CalFFT (dataFFT.row(ant),ant);
      }
    } else {
      cerr << "[DataConversion::FFT2CalFFT] Mismatch in array shapes." << endl;
    }
    
    return dataCalFFT;
  }
  
  
  
  // Vector<double> DataConversion::FFT2Power (const Vector<double>& dataFFT)
  // {
  //   IPosition shape (dataFFT.shape());
  //   Vector<double> dataPower (shape);
  //   double fftsize2 = (2*FFTSize_p)*(2*FFTSize_p);
  
  //   dataPower = 1.0/(50*fftsize2);
  //   for (int freq=0; freq<shape(0); freq++) {
  //     dataPower(freq) *= real(dataFFT(freq)*conj(dataFFT(freq)));
  //   }
  
  //   return dataPower;
  // }
  
  
  Vector<double> DataConversion::Power2NoiseT (const Vector<double>& dataPower)
  {
    // Boltzmann's constant
    casa::Quantity k (casa::QC::k);
    // Conversion factor from power to noise temperature
    double conversionFactor (k.getValue()*DataConversion::frequencyBin());
    Vector<double> noiseT (dataPower.shape(),1/conversionFactor);
    
    noiseT *= dataPower;
    
    return noiseT;
  }
  
  // =============================================================================
  //
  //  Auxiliary routines
  //
  // =============================================================================
  
  double DataConversion::frequencyBin ()
  {
    double frequencyBin (1/blocksize_p);
    //
    frequencyBin *= DataTIM40::samplerate()*DataTIM40::samplerateUnit();
    //
    return frequencyBin;
  }
  
}  // Namespace CR -- end
