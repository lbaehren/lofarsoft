
#include <LopesBase/DataFrequency.h>

// =============================================================================
//
//  Construction & Destruction
//
// =============================================================================

// Empty constructor
DataFrequency::DataFrequency ()
{
  // Double variables
  FrequencyUnit_p = SamplerateUnit_p = Samplerate_p = -1;
  // Integer variables
  NyquistZone_p = -1;
}

// Construction from Glish record
DataFrequency::DataFrequency (GlishRecord& record)
{
  DataFrequency::setDataFrequency (record);
}

// DataFrequency::~DataFrequency () {
//   Bool deleteIt = True;
//   const Double* ptrFrequencyRange = FrequencyRange_p.getStorage (deleteIt);
//   FrequencyRange_p.freeStorage(ptrFrequencyRange,deleteIt);
// }

// =============================================================================
//
//  Initialization
//
// =============================================================================

// Set the data from a Glish record
void DataFrequency::setDataFrequency (GlishRecord& record)
{
  GlishArray gtmp;
  Int itmp;
  Double dtmp;
  String stmp;
  Vector<Double> vdtmp;

  FrequencyRange_p.resize(2);

  if (record.exists("FrequencyUnit")) {
    gtmp = record.get("FrequencyUnit");
    gtmp.get(dtmp);
    DataFrequency::setFrequencyUnit (dtmp);
  }
  if (record.exists("SamplerateUnit")) {
    gtmp = record.get("SamplerateUnit");
    gtmp.get(dtmp);
    DataFrequency::setSamplerateUnit (dtmp);
  }
  if (record.exists("Samplerate")) {
    gtmp = record.get("Samplerate");
    gtmp.get(dtmp);
    DataFrequency::setSamplerate (dtmp);
  }
  if (record.exists("NyquistZone")) {
    gtmp = record.get("NyquistZone");
    gtmp.get(itmp);
    DataFrequency::setNyquistZone (itmp);
  }
  if (record.exists("FrequencyRange")) {
    gtmp = record.get("FrequencyRange");
    gtmp.get(vdtmp);
    DataFrequency::setFrequencyRange (vdtmp);
  }
  if (record.exists("FrequencyLow")) {
    gtmp = record.get("FrequencyLow");
    gtmp.get(dtmp);
    DataFrequency::setFrequencyLow (dtmp);
  }
  if (record.exists("FrequencyHigh")) {
    gtmp = record.get("FrequencyHigh");
    gtmp.get(dtmp);
    DataFrequency::setFrequencyHigh (dtmp);
  }
  if (record.exists("FFTSize")) {
    gtmp = record.get("FFTSize");
    gtmp.get(itmp);
    DataFrequency::setFFTSize (itmp);
  }
  if (record.exists("FFTlen")) {
    gtmp = record.get("FFTlen");
    gtmp.get(itmp);
    DataFrequency::setFFTlen (itmp);
  }
  if (record.exists("FrequencyMask")) {
    Vector<Bool> mask;
    gtmp = record.get("FrequencyMask");
    gtmp.get(mask);
    DataFrequency::setFrequencyMask (mask);
  }
  if (record.exists("FrequencyBand")) {
    Vector<Double> frequencyBand;
    gtmp = record.get("FrequencyBand");
    gtmp.get(frequencyBand);
    DataFrequency::setFrequencyBands (frequencyBand);
  }

}

// =============================================================================
//
//  Rates and units
//
// =============================================================================

void DataFrequency::setFrequencyUnit (Double FrequencyUnit) {
  FrequencyUnit_p = FrequencyUnit;
}

Double DataFrequency::FrequencyUnit () { return FrequencyUnit_p; }

void DataFrequency::setSamplerateUnit (Double SamplerateUnit) {
  SamplerateUnit_p = SamplerateUnit;
}

Double DataFrequency::SamplerateUnit () { return SamplerateUnit_p; }

void DataFrequency::setSamplerate (Double Samplerate) {
  Samplerate_p = Samplerate;
}

Double DataFrequency::Samplerate () { return Samplerate_p; }

void DataFrequency::setFrequencyLow (Double FrequencyLow) {
  FrequencyRange_p(0) = FrequencyLow;
}

void DataFrequency::setFrequencyHigh (Double FrequencyHigh) {
  FrequencyRange_p(1) = FrequencyHigh;
}

Vector<Double> DataFrequency::frequencyRange () {
  return FrequencyRange_p;
}

void DataFrequency::setFrequencyRange (const Vector<Double>& frequencyRange)
{
  FrequencyRange_p.resize(2);
  //
  FrequencyRange_p(0) = min(frequencyRange);
  FrequencyRange_p(1) = max(frequencyRange);
  // 
  if (FrequencyRange_p(0) == FrequencyRange_p(1)) {
    cerr << "[DataFrequency::setFrequencyRange] Invalid range definition!"
	 << endl;
    cerr << "\t" << FrequencyRange_p << endl;
  }
}

// =============================================================================
//
//  Frequency selection
//
// =============================================================================

void DataFrequency::setFrequencyBands (const Int nofBands,
				       const String division="lin")
{
  Vector<Double> freqRange = FrequencyRange_p;
  //
  DataFrequency::setFrequencyBands (freqRange,nofBands,division);
}

void DataFrequency::setFrequencyBands (const Vector<Double>& freqRange,
				       const Int nofBands,
				       const String division="lin")
{
  Bool verboseON = True;
  Vector<Double> bands(nofBands+1);
  Double step;

  if (division == "log") {
    // Stepwidth in frequency; make shure we avoid division by zero!
    Vector<Double> tmpFreqRange = freqRange;
    if (freqRange(0) == 0) {
      tmpFreqRange += freqRange(1);
    }
    step = log10(tmpFreqRange(1)/tmpFreqRange(0))/nofBands;
    // generate limits of sub-bands
    for (int i=0; i<=nofBands; i++) {
      bands(i) = freqRange(0)*pow(10.0,step*i);
    }
  } else {
    // Stepwidth in frequency
    step = ((freqRange(1)-freqRange(0))/(nofBands));
    // generate limits of sub-bands
    for (int i=0; i<=nofBands; i++) {
      bands(i) = freqRange(0)+i*step;
    }
  }

  // set up the frequency bands

  Matrix<Double> frequencyBands (nofBands,2);

  for (int i=0; i<nofBands; i++) {
    frequencyBands(i,0) = bands(i);
    frequencyBands(i,1) = bands(i+1);
  }
  
  DataFrequency::setFrequencyBands (frequencyBands);

  // provide some feedback

  if (verboseON) {
    cout << "[DataFrequency::setFrequencyBands]" << endl;
    cout << " - Frequency range for sub-bands : " << freqRange << endl;
    cout << " - Number of frequency sub-bands : " << nofBands << endl;
    cout << " - Division method ............. : " << division << endl;
    cout << " - Frequency stepwidth ......... : " << step << endl;
    cout << " - Sub-band limits ............. : " << bands << endl;
  }
  
}

void DataFrequency::setFrequencyBands (const Double& freqMIN,
				       const Double& freqMAX) {

  Vector<Double> freqBand(2);
  freqBand(0) = freqMIN;
  freqBand(1) = freqMAX;
  DataFrequency::setFrequencyBands (freqBand); 
}

void DataFrequency::setFrequencyBands (const Vector<Double>& freqBand)
{
  IPosition shape = freqBand.shape();
  Matrix<Double> freqBands(1,shape(0));
  freqBands.row(0) = freqBand;
  DataFrequency::setFrequencyBands (freqBands);
}

void DataFrequency::setFrequencyBands (const Matrix<Double>& freqBands)
{
  IPosition shape = freqBands.shape();
  FrequencyBands_p.resize(shape);
  FrequencyBands_p = freqBands;
}

Matrix<Double> DataFrequency::FrequencyBands () { return FrequencyBands_p; }

void DataFrequency::setFrequencyMask (Vector<Bool>& mask)
{
  Int nelem;
  mask.shape(nelem);
  if (nelem != FFTlen_p) {
    cerr << "[DataFrequency::setFrequencyMask] Warning, data inconsistent" << endl;
    cerr << " - FFT length ... : " << FFTlen_p << endl;
    cerr << " - Frequency mask : " << nelem << endl;
  }
  FrequencyMask_p.resize(nelem);
  FrequencyMask_p = mask;
}

Vector<Bool> DataFrequency::FrequencyMask (const Double freqMin,
					   const Double freqMax)
{
  Vector<Double> freq = DataFrequency::calcFrequencies ();
  IPosition freqShape = freq.shape();
  Vector<Bool> mask(freqShape);
  
  /* Step through all the frequency values and check if they lie within
     the valid range of frequencies. */
  for (int k=0; k<freqShape(0); ++k) {
    if (freq(k) < freqMin) mask(k) = False;
    else if (freq(k) > freqMax) mask(k) = False;
    else mask(k) = True;
  }

  return mask;
}

Vector<Bool> DataFrequency::FrequencyMask (const Vector<Double>& band)
{
  IPosition shape = band.shape();

  if (shape(0) == 2) {
    return DataFrequency::FrequencyMask (band(0),band(1));
  } else {
    return DataFrequency::maskFromBands ();
  }
}

Vector<Bool> DataFrequency::FrequencyMask ()
{
  return DataFrequency::maskFromBands ();
}

Vector<Bool> DataFrequency::maskFromBands ()
{
  IPosition shapeBands = FrequencyBands_p.shape();
  Vector<Bool> frequencyMask(FFTlen_p);
  
  frequencyMask = False;

  if (shapeBands(1) == 2) {
    for (int band=0; band<shapeBands(0); band++) {
      Vector<Double> range = FrequencyBands_p.row(band);
      Vector<Bool> mask = DataFrequency::FrequencyMask(range);
      for (int freq=0; freq<FFTlen_p; freq++) frequencyMask(freq) += mask(freq);
    }
  } else {
    cerr << "[DataFrequency::maskFromBands] Illegal array shape "
	 << shapeBands << endl;
  } 

  return frequencyMask;
}

Int DataFrequency::channelsInMask (const Vector<Bool>& mask,
				   const Bool selected)
{
  IPosition shape = mask.shape();
  Int nofElements = 0;

  for (int i=0; i<shape(0); i++) if (mask(i)) ++nofElements;

  if (selected) return nofElements;
  else return shape(0)-nofElements;
  
}

// =============================================================================
//
//  Fourier transform settings
//
// =============================================================================

void DataFrequency::setNyquistZone (Int NyquistZone) {
  NyquistZone_p = NyquistZone;
}

Int DataFrequency::NyquistZone () { return NyquistZone_p; }

void DataFrequency::setFFTSize (Int FFTSize) {
  FFTSize_p = FFTSize;
}

Int DataFrequency::FFTSize () { return FFTSize_p; }

void DataFrequency::setFFTlen (Int FFTlen) {
  FFTlen_p = FFTlen;
}

Int DataFrequency::FFTlen () { return FFTlen_p; }


// =============================================================================
//
//  Frequency values
//
// =============================================================================

Vector<Double> DataFrequency::Frequencies ()
{
  Vector<Double> freq = DataFrequency::calcFrequencies ();
  return freq;
}

Vector<Double> DataFrequency::Frequencies (Bool selected=True)
{
  // Step 1: get all available frequency values
  Vector<Double> freq = DataFrequency::calcFrequencies ();
  // Step 2: get the masking array
  Vector<Bool> mask = DataFrequency::maskFromBands ();
  // Step 3: return the frequency values based on the created mask
  return DataFrequency::Frequencies (mask);
}

Vector<Double> DataFrequency::Frequencies (Double freqMin, Double freqMax)
{
  // Step 1: check the values for the frequency band
  if (freqMin < FrequencyUnit_p || freqMax < FrequencyUnit_p) {
    freqMin *= FrequencyUnit_p;
    freqMax *= FrequencyUnit_p;
  }
  // Step 2: create the frequency selection mask
  Vector<Bool> mask = DataFrequency::FrequencyMask (freqMin,freqMax);
  // Step 3: use the generated mask to perform the actual frequency selection
  return DataFrequency::Frequencies (mask);
}

Vector<Double> DataFrequency::Frequencies (Vector<Double>& range)
{
  IPosition shape = range.shape();

  /* Proceed only if the vector providing the frequency range selection is
     of valid shape, i.e. it has two entries. */
  if (IPosition(0) == 2) {
    return DataFrequency::Frequencies (range(0),range(1));
  } else {
    cerr << "[DataFrequency::Frequencies]"
      " Invalid specification of frequency range" << endl;
    Vector<Double> frequencies(1);
    frequencies = -1;
    return frequencies;
  }
}

Vector<Double> DataFrequency::Frequencies (Vector<Bool>& mask)
{
  Vector<Double> freqALL = DataFrequency::calcFrequencies ();
  Vector<Double> freqOUT;
  IPosition freqShape = freqALL.shape();
  IPosition maskShape = mask.shape();
  

  /* Check if the shape of the masking array matches that of the frequency
     values. */
  if (freqShape == maskShape) {
    Int chan=0;
    Int nofChannels = DataFrequency::channelsInMask (mask,True);
    freqOUT.resize(nofChannels);
    /* Go through all possible frequency values and apply selection */
    for (int k=0; k<freqShape(0); ++k) {
      if (mask(k)) {
	freqOUT(chan) = freqALL(k);
	++chan;
      } 
    }
  }
  else if (maskShape(0) > freqShape(0)) {
    cerr << "[DataFrequency] Warning, masking array has more elements ("
	 << maskShape << ") than the frequency array (" << freqShape << ")." << endl;
  }
  else if (maskShape(0) < freqShape(0)) {
    cerr << "[DataFrequency] Warning, masking array has less elements ("
	 << maskShape << ") than the frequency array (" << freqShape << ")." << endl;
  }

  return freqOUT;
}

Double DataFrequency::FrequencyBin () {
  return Samplerate_p*SamplerateUnit_p/FFTSize_p;
}

Vector<Double> DataFrequency::calcFrequencies ()
{
  Double freqBin = DataFrequency::FrequencyBin ();
  Vector<Double> frequencies(FFTlen_p);

  /* Compute the individual frequency values from the sampling frequency
     and the number of frequency bins; note that (k-1) -> k, since in C++
     arrays are 0-based instead of 1-based as in Glish. */
  frequencies = FrequencyRange_p(0);
  for (int k=0; k<FFTlen_p; ++k) {
    frequencies(k) += k*freqBin;
  }

  return frequencies;
}


// =============================================================================
//
//  Conversion of quantities in the frequency domain
//
// =============================================================================

Vector<Double> DataFrequency::Power2NoiseT (const Vector<Double>& calpow)
{ 
  IPosition shape = calpow.shape();
  Vector<Double> noiseTemp(shape);
  const Double k = QC::k.getValue();
  Double freqBin = DataFrequency::FrequencyBin ();
  Vector<Double> normalization(shape);

  normalization = 1/freqBin/k;

  noiseTemp = normalization*calpow;
  
  return noiseTemp;
}

Vector<Double> DataFrequency::FFT2Power (Vector<DComplex> const &fftData) 
{
  IPosition shape = fftData.shape();
  Vector<Double> fftPower(shape);
  Double fftsize2 = (2*FFTSize_p)*(2*FFTSize_p);

  fftPower = 1.0/(50*fftsize2);
  for (int freq=0; freq<shape(0); freq++) {
    fftPower(freq) *= real(fftData(freq)*conj(fftData(freq)));
  }

  return fftPower;
}


// =============================================================================
//
//  Output to a data stream
//
// =============================================================================

void DataFrequency::show (ostream& s = std::cout)
{
  s.precision(10);
  s << "\n[DataFrequency]\n" << endl;
  s << " - Unit for the frequency values . [Hz] : " << FrequencyUnit_p << endl;
  s << " - Unit for the sample rate ...... [Hz] : " << SamplerateUnit_p << endl;
  s << " - Samplerate ......................... : " << Samplerate_p << endl;
  s << " - Nyquist Zone ....................... : " << NyquistZone_p << endl;
  s << " - Frequency range ............... [Hz] : " << FrequencyRange_p << endl;
  s << " - Time domain size of FFT ............ : " << FFTSize_p << endl;
  s << " - Frequency domain size of FFT ....... : " << FFTlen_p << endl;
  s << " - Frequency resolution (bin size) [Hz] : "
    << DataFrequency::FrequencyBin() << endl;
  s << " - Shape of the band selection array .. : "
    << FrequencyBands_p.shape() << endl;
  s << " - Frequency bands .................... : ";
  for (uInt i=0; i<FrequencyBands_p.nrow(); i++) {
    s << FrequencyBands_p.row(i) << " ";
  }
  s << endl;
  s << " - Shape of the frequency mask ........ : "
    << FrequencyMask_p.shape() << endl;
}
