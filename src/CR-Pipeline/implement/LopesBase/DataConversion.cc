
#include <LopesBase/DataConversion.h>

DataConversion::DataConversion (const Int nofAntennas,
				const Int blocksize)
  : DataTIM40(),
    nofAntennas_p(nofAntennas), blocksize_p(blocksize)
{
  fftlength_p = blocksize/2+1;
}

DataConversion::DataConversion (const Int nofAntennas,
				const Int blocksize,
				const Int fftlength)
  : DataTIM40(),
    nofAntennas_p (nofAntennas), blocksize_p (blocksize), fftlength_p (fftlength)
{;}

DataConversion::~DataConversion () {;}


// =============================================================================
//
// Access to the conversion buffers
//
// =============================================================================

Vector<Double> DataConversion::Fx2Voltage (const Vector<Double>& dataFx,
					   const Bool invert)
{
  Double conversionFactor (0.0);

  if (invert) {
    conversionFactor = DataTIM40::ADCMaxChannel() / DataTIM40::maxVoltage();
  } else {
    conversionFactor = DataTIM40::maxVoltage() / DataTIM40::ADCMaxChannel();
  }

  Vector<Double> voltage (dataFx*conversionFactor);

  return voltage;
}

Vector<Double> DataConversion::Fx2PowerT (const Vector<Double>& dataFx)
{
  Vector<Double> powerT (dataFx);
  powerT *= dataFx;
  return powerT;
}

Matrix<Complex> DataConversion::FFT2CalFFT ()
{
  return fft2calfft_p;
}

Bool DataConversion::setFFT2CalFFT (const Matrix<Complex>& fft2calfft)
{
  Bool ok (True);

  try {
    fft2calfft_p.resize (fft2calfft.shape());
    fft2calfft_p = fft2calfft;
  } catch (AipsError x) {
    cerr << "[DataConversion::setFFT2CalFFT] " << x.getMesg() << endl;
    ok = False;
  }

  return ok;
}

// =============================================================================
//
//  Conversion between data fields
//
// =============================================================================

Vector<Complex> DataConversion::FFT2CalFFT (const Vector<Complex>& dataFFT,
					    const Int antenna)
{
  Int nofFrequencies (dataFFT.nelements());
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



// Vector<Double> DataConversion::FFT2Power (const Vector<Double>& dataFFT)
// {
//   IPosition shape (dataFFT.shape());
//   Vector<Double> dataPower (shape);
//   Double fftsize2 = (2*FFTSize_p)*(2*FFTSize_p);
  
//   dataPower = 1.0/(50*fftsize2);
//   for (int freq=0; freq<shape(0); freq++) {
//     dataPower(freq) *= real(dataFFT(freq)*conj(dataFFT(freq)));
//   }
  
//   return dataPower;
// }


Vector<Double> DataConversion::Power2NoiseT (const Vector<Double>& dataPower)
{
  // Boltzmann's constant
  Quantity k (QC::k);
  // Conversion factor from power to noise temperature
  Double conversionFactor (k.getValue()*DataConversion::frequencyBin());
  Vector<Double> noiseT (dataPower.shape(),1/conversionFactor);

  noiseT *= dataPower;

  return noiseT;
}

// =============================================================================
//
//  Auxiliary routines
//
// =============================================================================

Double DataConversion::frequencyBin ()
{
  Double frequencyBin (1/blocksize_p);
  //
  frequencyBin *= DataTIM40::samplerate()*DataTIM40::samplerateUnit();
  //
  return frequencyBin;
}
