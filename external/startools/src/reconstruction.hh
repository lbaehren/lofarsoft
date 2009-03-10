#ifndef __defined_reconstruction_hh
#define __defined_reconstruction_hh


//!star tool headers
#include "trigger.hh"
#include "util.hh"
#include "calibration.hh"


//!RecRadio headers
#include "RecRadioCalib.hh"
#include "RecRadioTrigger.hh"
#include "RecRadioUtil.hh"
#include "RecRadioBasic.hh"


//!std c++
#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>


//!root 
#include <TMath.h>


using namespace std;
using namespace TMath;


/*!
  \file reconstruction.hh 
  \ingroup startools
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
\param NoZeros corresponds to the number of up-sampled values between each raw data point. Recommended: NoZeros+1 = 2^n
\param ZPTrace is the pointer to an array with the up-sampled data. The memory has to be allocated before calling the function. The length of this array must be (NoZeros+1)*window_size
\param bSubtractPedestal switch to disable pedestal subtraction, if enabled, the pedestal is subtracted form the trace array
*/
void ZeroPaddingFFT(int window_size, short int *trace, int NoZeros, float* ZPTrace, bool bSubtractPedestal=true);


//! Zero Padding of the trace
/*!
same like the other ZeroPaddingFFT but just with float type of trace.
*/
void ZeroPaddingFFT(int window_size, float *trace, int NoZeros, float* ZPTrace, bool bSubtractPedestal=true);


//! Zero Padding of the trace
/*!
same like the other ZeroPaddingFFT but just with double type of trace.
*/
void ZeroPaddingFFT(int window_size, double *Dtrace, int NoZeros, double* DZPTrace, bool bSubtractPedestal=true);


//! Core function of the Zero Padding
/*!
core function is called by the wrapper functions
*/
void ZeroPaddingFFTCore(int window_size, float *FFTdata, int NoZeros, float* ZPTrace);


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
\param trace is the pointer to the raw data array, will be manipulated
\param trace_suppressed is filled by the function after suppressing the RFI, the length must be window_size
\param NoZeros no of interpolated values, important for the inverse FFT function
*/
void RFISuppressionCutOff(int window_size, float *trace, float *trace_suppressed, int NoZeros = 0);


//! Noise Suppression in the frequency domain
/*
internal link to RFISuppressionCutOff()
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array, will be manipulated
\param trace_suppressed is filled by the function after suppressing the RFI, the length must be window_size
\param NoZeros no of interpolated values, important for the inverse FFT function
*/
void RFISuppression(int window_size, float *trace, float *trace_suppressed, int NoZeros = 0);


//! Noise Suppression in the frequency domain
/*
Same as RFISupupressionCutOff(), but the spectrum is replaced by the median function
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array, will be manipulated
\param trace_suppressed is filled by the function after suppressing the RFI, the length must be window_size
\param NoZeros no of interpolated values, important for the inverse FFT function
*/
void RFISuppressionMedian(int window_size, float *trace, float *trace_suppressed, int NoZeros = 0);

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
\param IntegralPre Integral over a defined time range before the pulse begins
\param IntegralPost Integral over a definded time range after the pulse
\param IntegralPulse integral value of the pulse region around the defined pulse width
\param threshold the used threshold for search for pulses, if set to zero the maximum value in the trace is used as pulse maximum
\param NoZeros NoZeros +1 corresponds to the up-sampling factor
*/
void EstimatePulseParameter(int window_size, const float *trace, float *PulseLength, float *PulsePosition, float *Maximum, float *IntegralPre, float *IntegralPost, float *IntegralPulse, float threshold=0, int NoZeros=7);


//! Antenna Coordinates in the KASCADE grid (rotated clockwise by 15degree)
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
\param PulseTime array of dim NoChannels with the calculated time postitions of the pulse in nano seconds
\param Zenith 2dim array. [0] = value, [1] = error
\param Azimuth 2dim array. [0] = value, [1] = error
*/
void PlaneFit(const int NoChannels, char **AntPos, const int daq_id, const float *PulseAmp, float *PulseTime, float *Zenith, float *Azimuth);


//! Plane Fit of given channels with coordinates
/*
\param NoChannels no of used channels for the plane fit
\param AntCooridnate pointer with all antenna position data
\param PulseAmp array of dim NoChannels with the given signal height
\param PulseTime array of dim NoChannels with the calculated time postitions of the pulse in nano seconds
\param Zenith 2dim array. [0] = value, [1] = error
\param Azimuth 2dim array. [0] = value, [1] = error
*/
void PlaneFitCore(const int NoChannels, float **AntCoordinate, const float *PulseAmp, float *PulseTime, float *Zenith, float *Azimuth);


//! reconstruction of direction for one atenna array
/*
\param NoChannels no of used channels for the plane fit
\param trace pointer with all trace data
\param AntPos string array of dim NoChannels with the given antenna names
\param daq_id daq id of the used antennas
\param Zenith 2dim array. [0] = value, [1] = error
\param Azimuth 2dim array. [0] = value, [1] = error
\param RFISupp switch to controll the RFI suppression method in the function
*/
void RecDirectionSingleArray(const int NoChannels, int window_size, float **trace, char **AntPos, const int daq_id, float *Zenith, float *Azimuth, bool RFISupp=true);


//! Geomagnetic Angle
/*
\param Azimuth in degree of reconstructed shower direction
\param Zenith in degree of reconstructed shower direction
\param daq_id id of the used setup
\return geomagnetic angle in degree
*/
float GeoMagAngle(float Azimuth, float Zenith, int daq_id);


//! Signal to Noise Ratio
/*
The given raw data are up-sampled by a factor of 8 and the envelope with hilbert trafo are calculated.
The is calculated excluded the region around the maximum. SNR is in sense of power calculated.
\param window_size length of the trace pointer
\param trace array with sub-sampled data
\param NoZeros factor +1 for up-sampling, if chosen null, no up-sampling is internally done
\param DoRectifier switch to disable rectifier
\return snr
*/
float SNR(int window_size, float *trace, int NoZeros=7, bool DoRectifier=true);


//! Get Distance to shower axis
/*
\param struct h3 pointer to Grande struct h3
\param AntPos char to indentify the antenna position
\param daq_id daq id of the used antenna cluster
\param Zenith reconstructed shower direction
\param Azimuth reconstructed shower direction
\param distance distance to shower axis
\param error error of reconstructed direction taking the position error of Grande into account
*/
void GetDistanceToShowerAxis(struct h3 *h3, char *AntPos, int daq_id, float Zenith, float Azimuth, float *distance, float *error);


//! Get Distance to shower axis
/*
\param CorePosX x position of core in ground coordinates
\param CorePosY y position
\param AntPosCoordinates pointer with x, y and z cooridnate of the used antennas
\param Zenith reconstructed shower direction
\param Azimuth reconstructed shower direction
\param distance distance to shower axis
\param error error of reconstructed direction taking the position error of Grande into account
*/
void GetDistanceToShowerAxisRadio(float CorePosX, float CorePosY, float *AntPosCoordinates, float Zenith, float Azimuth, float *distance, float *error);


//! Get the peak value of the channel
/*
\param window_size length of the trace array
\param trace pointer to the data
\param ExpectedPosition of the peak
\return peak value in units of the given trace
*/
float GetChannelPeak(int window_size, float *trace, int ExpectedPosition);


//! calculate antenna field strength
/*
\param NoChannels number of used channels
\param ChannelFieldStrength pointer of length NoChannels with the field strength per channel
\param AntPosition position pointer of the used channels
\param AntennaFieldStrength calculated field strength per antenna of length NoChannels
*/
void CalculateAntennaFieldStrength(int NoChannels, const float *ChannelFieldStrength, char **AntPosition, float *AntennaFieldStrength);


//! calculate the transformed field strength into the LOPES30 coordinate system
/*
experimentell status
\param NoChannels
\param ChannelFieldStrength
\param AntPosition
\param AntPolarisation
\param EPhi
\param ETheta
\param Azimuth
*/
void CalculateTransformedFieldStrength( int NoChannels, const float *ChannelFieldStrength,char **AntPosition,
					char **AntPolarisation, float *EPhi, float *ETheta, float Azimuth);

#endif
