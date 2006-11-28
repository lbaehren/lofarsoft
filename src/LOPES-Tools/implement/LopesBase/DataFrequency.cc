
#include <LopesBase/DataFrequency.h>

using std::cerr;
using std::cout;
using std::endl;

namespace LOPES {  // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction & Destruction
  //
  // ============================================================================
  
  // Empty constructor
  DataFrequency::DataFrequency ()
  {
    // double variables
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
  //   bool deleteIt = true;
  //   const double* ptrFrequencyRange = FrequencyRange_p.getStorage (deleteIt);
  //   FrequencyRange_p.freeStorage(ptrFrequencyRange,deleteIt);
  // }
  
  // ============================================================================
  //
  //  Initialization
  //
  // ============================================================================
  
// Set the data from a Glish record
void DataFrequency::setDataFrequency (GlishRecord& record)
{
  GlishArray gtmp;
  int itmp;
  double dtmp;
  String stmp;
  Vector<double> vdtmp;

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
    Vector<bool> mask;
    gtmp = record.get("FrequencyMask");
    gtmp.get(mask);
    DataFrequency::setFrequencyMask (mask);
  }
  if (record.exists("FrequencyBand")) {
    Vector<double> frequencyBand;
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

void DataFrequency::setFrequencyUnit (double FrequencyUnit) {
  FrequencyUnit_p = FrequencyUnit;
}

double DataFrequency::FrequencyUnit () { return FrequencyUnit_p; }

void DataFrequency::setSamplerateUnit (double SamplerateUnit) {
  SamplerateUnit_p = SamplerateUnit;
}

double DataFrequency::SamplerateUnit () { return SamplerateUnit_p; }

void DataFrequency::setSamplerate (double Samplerate) {
  Samplerate_p = Samplerate;
}

double DataFrequency::Samplerate () { return Samplerate_p; }

void DataFrequency::setFrequencyLow (double FrequencyLow) {
  FrequencyRange_p(0) = FrequencyLow;
}

void DataFrequency::setFrequencyHigh (double FrequencyHigh) {
  FrequencyRange_p(1) = FrequencyHigh;
}

Vector<double> DataFrequency::frequencyRange () {
  return FrequencyRange_p;
}

void DataFrequency::setFrequencyRange (const Vector<double>& frequencyRange)
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

void DataFrequency::setFrequencyBands (const int nofBands,
				       const String division="lin")
{
  Vector<double> freqRange = FrequencyRange_p;
  //
  DataFrequency::setFrequencyBands (freqRange,nofBands,division);
}

void DataFrequency::setFrequencyBands (const Vector<double>& freqRange,
				       const int nofBands,
				       const String division="lin")
{
  bool verboseON = true;
  Vector<double> bands(nofBands+1);
  double step;

  if (division == "log") {
    // Stepwidth in frequency; make shure we avoid division by zero!
    Vector<double> tmpFreqRange = freqRange;
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

  Matrix<double> frequencyBands (nofBands,2);

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

void DataFrequency::setFrequencyBands (double const &freqMIN,
				       double const &freqMAX) {

  Vector<double> freqBand(2);
  freqBand(0) = freqMIN;
  freqBand(1) = freqMAX;
  DataFrequency::setFrequencyBands (freqBand); 
}

void DataFrequency::setFrequencyBands (const Vector<double>& freqBand)
{
  IPosition shape = freqBand.shape();
  Matrix<double> freqBands(1,shape(0));
  freqBands.row(0) = freqBand;
  DataFrequency::setFrequencyBands (freqBands);
}

void DataFrequency::setFrequencyBands (const Matrix<double>& freqBands)
{
  IPosition shape = freqBands.shape();
  FrequencyBands_p.resize(shape);
  FrequencyBands_p = freqBands;
}

Matrix<double> DataFrequency::FrequencyBands () { return FrequencyBands_p; }

void DataFrequency::setFrequencyMask (Vector<bool>& mask)
{
  int nelem;
  mask.shape(nelem);
  if (nelem != FFTlen_p) {
    cerr << "[DataFrequency::setFrequencyMask] Warning, data inconsistent" << endl;
    cerr << " - FFT length ... : " << FFTlen_p << endl;
    cerr << " - Frequency mask : " << nelem << endl;
  }
  FrequencyMask_p.resize(nelem);
  FrequencyMask_p = mask;
}

Vector<bool> DataFrequency::FrequencyMask (const double freqMin,
					   const double freqMax)
{
  Vector<double> freq = DataFrequency::calcFrequencies ();
  IPosition freqShape = freq.shape();
  Vector<bool> mask(freqShape);
  
  /* Step through all the frequency values and check if they lie within
     the valid range of frequencies. */
  for (int k=0; k<freqShape(0); ++k) {
    if (freq(k) < freqMin) mask(k) = false;
    else if (freq(k) > freqMax) mask(k) = false;
    else mask(k) = true;
  }

  return mask;
}

Vector<bool> DataFrequency::FrequencyMask (const Vector<double>& band)
{
  IPosition shape = band.shape();

  if (shape(0) == 2) {
    return DataFrequency::FrequencyMask (band(0),band(1));
  } else {
    return DataFrequency::maskFromBands ();
  }
}

Vector<bool> DataFrequency::FrequencyMask ()
{
  return DataFrequency::maskFromBands ();
}

Vector<bool> DataFrequency::maskFromBands ()
{
  IPosition shapeBands = FrequencyBands_p.shape();
  Vector<bool> frequencyMask(FFTlen_p);
  
  frequencyMask = false;

  if (shapeBands(1) == 2) {
    for (int band=0; band<shapeBands(0); band++) {
      Vector<double> range = FrequencyBands_p.row(band);
      Vector<bool> mask = DataFrequency::FrequencyMask(range);
      for (int freq=0; freq<FFTlen_p; freq++) frequencyMask(freq) += mask(freq);
    }
  } else {
    cerr << "[DataFrequency::maskFromBands] Illegal array shape "
	 << shapeBands << endl;
  } 

  return frequencyMask;
}

int DataFrequency::channelsInMask (const Vector<bool>& mask,
				   const bool selected)
{
  IPosition shape = mask.shape();
  int nofElements = 0;

  for (int i=0; i<shape(0); i++) if (mask(i)) ++nofElements;

  if (selected) return nofElements;
  else return shape(0)-nofElements;
  
}

// =============================================================================
//
//  Fourier transform settings
//
// =============================================================================

void DataFrequency::setNyquistZone (int NyquistZone) {
  NyquistZone_p = NyquistZone;
}

int DataFrequency::NyquistZone () { return NyquistZone_p; }

void DataFrequency::setFFTSize (int FFTSize) {
  FFTSize_p = FFTSize;
}

int DataFrequency::FFTSize () { return FFTSize_p; }

void DataFrequency::setFFTlen (int FFTlen) {
  FFTlen_p = FFTlen;
}

int DataFrequency::FFTlen () { return FFTlen_p; }


// =============================================================================
//
//  Frequency values
//
// =============================================================================

Vector<double> DataFrequency::Frequencies ()
{
  Vector<double> freq = DataFrequency::calcFrequencies ();
  return freq;
}

Vector<double> DataFrequency::Frequencies (bool selected=true)
{
  // Step 1: get all available frequency values
  Vector<double> freq = DataFrequency::calcFrequencies ();
  // Step 2: get the masking array
  Vector<bool> mask = DataFrequency::maskFromBands ();
  // Step 3: return the frequency values based on the created mask
  return DataFrequency::Frequencies (mask);
}

Vector<double> DataFrequency::Frequencies (double freqMin, double freqMax)
{
  // Step 1: check the values for the frequency band
  if (freqMin < FrequencyUnit_p || freqMax < FrequencyUnit_p) {
    freqMin *= FrequencyUnit_p;
    freqMax *= FrequencyUnit_p;
  }
  // Step 2: create the frequency selection mask
  Vector<bool> mask = DataFrequency::FrequencyMask (freqMin,freqMax);
  // Step 3: use the generated mask to perform the actual frequency selection
  return DataFrequency::Frequencies (mask);
}

Vector<double> DataFrequency::Frequencies (Vector<double>& range)
{
  IPosition shape = range.shape();

  /* Proceed only if the vector providing the frequency range selection is
     of valid shape, i.e. it has two entries. */
  if (IPosition(0) == 2) {
    return DataFrequency::Frequencies (range(0),range(1));
  } else {
    cerr << "[DataFrequency::Frequencies]"
      " Invalid specification of frequency range" << endl;
    Vector<double> frequencies(1);
    frequencies = -1;
    return frequencies;
  }
}

Vector<double> DataFrequency::Frequencies (Vector<bool>& mask)
{
  Vector<double> freqALL = DataFrequency::calcFrequencies ();
  Vector<double> freqOUT;
  IPosition freqShape = freqALL.shape();
  IPosition maskShape = mask.shape();
  

  /* Check if the shape of the masking array matches that of the frequency
     values. */
  if (freqShape == maskShape) {
    int chan=0;
    int nofChannels = DataFrequency::channelsInMask (mask,true);
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

double DataFrequency::FrequencyBin () {
  return Samplerate_p*SamplerateUnit_p/FFTSize_p;
}

Vector<double> DataFrequency::calcFrequencies ()
{
  double freqBin = DataFrequency::FrequencyBin ();
  Vector<double> frequencies(FFTlen_p);

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

Vector<double> DataFrequency::Power2NoiseT (const Vector<double>& calpow)
{ 
  IPosition shape = calpow.shape();
  Vector<double> noiseTemp(shape);
  const double k = casa::QC::k.getValue();
  double freqBin = DataFrequency::FrequencyBin ();
  Vector<double> normalization(shape);

  normalization = 1/freqBin/k;

  noiseTemp = normalization*calpow;
  
  return noiseTemp;
}

Vector<double> DataFrequency::FFT2Power (Vector<DComplex> const &fftData) 
{
  IPosition shape = fftData.shape();
  Vector<double> fftPower(shape);
  double fftsize2 = (2*FFTSize_p)*(2*FFTSize_p);

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
  
  void DataFrequency::show (std::ostream& s = std::cout)
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
    for (uint i=0; i<FrequencyBands_p.nrow(); i++) {
      s << FrequencyBands_p.row(i) << " ";
    }
    s << endl;
    s << " - Shape of the frequency mask ........ : "
      << FrequencyMask_p.shape() << endl;
  }
  
}  // Namespace LOPES -- end
