#ifndef DATACONVERSION_H
#define DATACONVERSION_H

/* $Id: DataConversion.h,v 1.2 2006/08/01 15:03:25 bahren Exp $ */

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta.h>
#include <measures/Measures.h>

#include <Data/TIM40.h>

using casa::AipsError;
using casa::Complex;
using casa::IPosition;
using casa::Matrix;
using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class DataConversion
    
    \ingroup LopesBase
    
    \brief Conversion of LOPES-Tools data fields
    
    \author Lars B&auml;hren
    
    \date 2005/05
    
    \test tDataConversion.cc
  */
  
  class DataConversion : public TIM40 {
    
    int nofAntennas_p;
    int blocksize_p;
    int fftlength_p;
    
    Matrix<Complex> fft2calfft_p;
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Argumented constructor
      
      \param nofAntennas -- Number of antennas.
      \param blocksize   -- Blocksize by which data are read; this is the input 
      size of the FFT.
    */
    DataConversion (const int nofAntennas,
		    const int blocksize);
    
    /*!
      \brief Argumented constructor
      
      \param nofAntennas -- Number of antennas.
      \param blocksize   -- Blocksize by which data are read; this is the input 
      size of the FFT.
      \param fftlength   -- Output length of the FFT.
    */
    DataConversion (const int nofAntennas,
		    const int blocksize,
		    const int fftlength);
    
    // --- Destruction -----------------------------------------------------------
    
    ~DataConversion ();
    
  // --- Access to the conversion buffers --------------------------------------

    /*!
      \brief 

      \return 
    */
    Matrix<Complex> FFT2CalFFT ();
    
    bool setFFT2CalFFT (const Matrix<Complex>& fft2calfft);
    
    // --- Conversion between data fields ----------------------------------------

  Vector<double> Fx2Voltage (const Vector<double>& dataFx,
			     const bool invert);

  Vector<double> Fx2PowerT (const Vector<double>& dataFx);

  /*!
    \brief Convert FFT data to calibrated FFT data

    \param dataFFT -- Array with the FFT of an antenna signal
    \param antenna -- Identifier of the antenna, for which we want to run the
                      conversion.

    \return dataCalFFT -- 
   */
  Vector<Complex> FFT2CalFFT (const Vector<Complex>& dataFFT,
			      const int antenna);

  Matrix<Complex> FFT2CalFFT (const Matrix<Complex>& dataFFT);

  Vector<double> FFT2Power (const Vector<Complex>& dataFFT);

  /*!
    \brief Convert calibrated power to noise temperature.

    \f[ T (\nu) = \frac{P (\nu)}{\delta \nu \cdot k} \f]

    where \f$ P (\nu) \f$ is the calibrated power at a frequency \f$ \nu \f$,
    \f$ \delta \nu \f$ the frequency span of a data bin and \f$ k \f$ is 
    Boltzmann's constant.

    \param dataPower -- 

    \return noiseT -- 
  */
  Vector<double> Power2NoiseT (const Vector<double>& dataPower);

 private:
  
  double frequencyBin ();

};

}  // Namespace LOPES -- end

#endif
