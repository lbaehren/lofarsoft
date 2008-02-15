#ifndef __defined_reconstruction_h
#define __defined_reconstruction_h


/*! \file reconstruction.h 
 \brief Collection of functions and methods to reconstruct shower observables

 \date    Mar 13 2007
 \author  Thomas Asch
 
 to do
 - there is always something to do.. isn't it :-)
*/

//! Zero Padding of the trace
/*!
Interpolation of the trace by a given factor. If the factor + 1 is a multiple of 2 the algorithm works faster.
\param window_size is the length of the raw ADC trace
\param trace is a pointer to the ADC data
\param NoZeros is factor of interpolation. (NoZeros+1 = 2*n)
\param ZPTrace is the pointer to an array with the interpolated data. The memory has to be allocated before calling the function. The length of this array must be (NoZeros+1)*window_size
*/
void ZeroPaddingFFT(int window_size, short int *trace, int NoZeros, float* ZPTrace);


//! Zero Padding of the trace
/*!
same like the other ZeroPaddingFFT but just with float type of trace.
*/
void ZeroPaddingFFT(int window_size, float *trace, int NoZeros, float* ZPTrace);


//! Core function of the Zero Padding
/*!
core function is called by the wrapper functions
*/
void ZeroPaddingFFTCore(int window_size, void *trace, bool Flag, int NoZeros, float* ZPTrace);


//! RFI Suppression (simple) in the frequency domain
/*!
The raw time signal is Fourier transformed, all mono-frequent sources above the mean signal plus sigma of the mean is set to 
the mean plus sigma.

\param window_size defines the length of the ADC trace.
\param trace is the pointer to the data array 
\param TraceOut is the pointer filled with the noise suppressed time data. The length of this pointer must be window_size
*/
void RFISuppressionSimple(int window_size, short int* trace, float *TraceOut);


//! RFI Suppression (simple) in the frequency domain
/*!
same as above, just with float type of raw data
*/
void RFISuppressionSimple(int window_size, float* trace, float *TraceOut);


//! Core function of the RFI Suppression (simple)
/*!
core function is called by the wrapper functions
*/
void RFISuppressionSimpleCore(int window_size, void* trace, bool Flag, float *TraceOut);


//! Noise Suppression in the frequency domain
/*
The algorithm is based on the idea of Kalpana Singh (LOPES30).
All influence on the spectra that comes from mono frequent RFI is filtered out. All methods are done in the power spectrum
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array
\param trace_suppressed is filled by the function after suppressing the RFI, the length must be window_size
\param NoZeros no of interpolated values, important for the inverse FFT function
*/
void RFISuppression(int window_size, float *trace, float *trace_suppressed, int NoZeros = 0);


//! Estimate of Pulse Length
/*
The data of the trace pointer are not touched. After RFISuppression the envelope is generated. On that base the maximum peak is searchd and the length is calculated.
Comment: Perhaps it is a good idea to use PulseCount before PulseLengthEstimate to be sure to have just one Peak in the trace.
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array
\param PulseLength estimate of the pulse lenght of the highest peak
\param PulsePostion bin no of upsampled trace where the max is detected
\param Integral Integral over the envelope without the range of the found pulse
\param RFISupp is the switch to enable or disable (default) the RFISuppression. Useful to save cpu time.
*/
void PulseLengthEstimate(int window_size, const float *trace, float *PulseLength, float *PulsePosition, float *Integral, bool RFISupp=true, bool dynamic_thres=false);


//! Estimate of Pulse Parameter by using a given threshold
/*
The data of the trace pointer are not touched. After RFISuppression the envelope is generated. On that base the maximum peak is searchd and the length is calculated.
Comment: Perhaps it is a good idea to use PulseCount before PulseLengthEstimate to be sure to have just one Peak in the trace.
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array
\param PulseLength estimate of the pulse lenght of the highest peak
\param PulsePostion bin no of upsampled trace where the max is detected
\param Maximum 	Max value of the found peak
\param Integral Integral over the envelope without the range of the found pulse
\param threshold the used threshold for search for pulses, if set to zero the maximum value in the trace is used as pulse maximum
*/
void EstimatePulseParameter(int window_size, const float *trace, float *PulseLength, float *PulsePosition, float *Maximum, float *Integral, float threshold);


//! Signal to Noise Ratio with other trace parameters are calculated
/*
The raw trace is squared and the ratio of the max to mean value is calculated. Also the rms and mean is given back.
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array
\param snr signal to noise ratio
\param mean mean value over the squared trace
\param rms root mean square of mean
\param debug enables the debug info
*/
void SNR(int window_size, const float *trace, float *snr, float *mean, float *rms, bool debug=false);


//! Antenna Coordinates in the KASCADE grid
/*
\param AntPos string array of dim NoChannels with the given antenna names
\param daq_idd aq id of the used antennas
\param AntCoordinate array with 3 fields, 0: X-coordinate; 1: Y-coordinate; 2: Z-coordinate
*/
void GetAntPos(const char *AntPos, const int daq_id, float *AntCoordinate);


//! Calculate the bary center
/*
\param NoChannels no of channels to calculate the bary center
\param AntCoordinate double array, first index = NoChannels, second AntCoordinate size 3
\param PulseAmp pointer of dim NoChannels for the calculated amplitude
\param b pointer of dim 3 with the calculated bary center
*/
void GetBaryCenter(const int NoChannels, float **AntCoordinate, const float *PulseAmp, float *b);


//! Plane Fit of given channels
/*
\param NoChannels no of used channels for the plane fit
\param AntPos string array of dim NoChannels with the given antenna names
\param daq_id daq id of the used antennas
\param PulseAmp array of dim NoChannels with the given signal height
\param PulseTime array of dim NoChannels with the calculated time postitions of the pulse in nano seconds
\param Zenith to store the zenith value
\param Azimuth to store the azimuth value
*/
void PlaneFit(const int NoChannels, char **AntPos, const int daq_id, const float *PulseAmp, float *PulseTime, float *Zenith, float *Azimuth);

#endif
