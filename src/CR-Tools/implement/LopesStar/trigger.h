#ifndef __defined_trigger_h
#define __defined_trigger_h


/*! \file trigger.h
 \brief Collection of all function related to the level one trigger 

 \date    Aug 09 2006
 \author  Thomas Asch
 
 to do
 - there is allways something to do.. isn't it :-)
*/

//! low-pass filter
/*!
\param NoZero number of inserted zeros for interpolation
\param order of the filter, have to be odd
\param frequency is the limit frequency in Hz of the filter
\param coeff are the given coefficients of the algorithm
*/
void LPFilter(int NoZero, int order, int frequency, double *coeff);


//! high-pass filter
/*!
seems to have some problems with the filter coefficients
Don't use this function now!
\param NoZero number of inserted zeros for interpolation
\param order of the filter, have to be odd
\param frequency is the limit frequency in Hz of the filter
\param coeff are the given coefficients of the algorithm
*/
void HPFilter(int NoZero, int order, int frequency, double *coeff);


//! band-pass filter
/*!
\param NoZero number of inserted zeros for interpolation
\param order of the filter, have to be odd
\param start_freq is the limit frequency in Hz of the filter
\param stop_freq is the limit frequency in Hz of the filter
\param coeff are the given coefficients of the algorithm
*/
void BandFilterFunc(int NoZero, int order, int start_freq, int stop_freq, double *coeff);


//! rectifier of HF signal to calculate the envelope signal
/*!
The given data are squared, high and low pass filtered
\param NoZero number of inserted zeros for interpolation
\param window_size the length of the trace
\param trace the array to the HF signal
*/
void Rectifier(int NoZero, int window_size, float *trace);


//! rectifier of HF signal to calculate the envelope signal
/*!
The given data are squared and filtered in the frequency domain.
\param NoZeros number of inserted zeros for interpolation
\param window_size the length of the trace array
\param trace the array to the HF signal
\param HighFilterFreq all frequencies below are rejected, input n MHz
\param LowFilterFreq all frequencies above are rejected, input in MHz
*/
void RectifierFFT(int NoZeros, int window_size, float *trace, float HighFilterFreq=.0, float LowFilterFreq=40.);


//! video filter
/*!
\param NoZero number of inserted zeros for interpolation
\param window_size the length of the trace
\param trace the array to the HF signal
*/
void VideoFilter(int NoZero, int window_size, float *trace);


//! comparison of the trace with a given threshold
/*!
\param NoChannels number of used channels
\param threshold for the given trace
\param window_size the length of the trace
\param trace the array to the HF signal
\param position is filled with the bin number when the signal is bigger than the threshold
\param delta two times delta defines the range for possible pulses in case of multi pulses
*/
void PeakDetection(int NoChannels, float *threshold, int window_size, float **trace, int *position, int delta);


//! finds the coincidence of the given channels 
/*!
\param NoCh number of antenna channels to decide for a level 1 trigger
\param position is the number of bin, where the threshold is reached
\param coincidence_time of the given channels in nano seconds, the value is overwritten with the first coincidence time found of the event
\param NoZero number of inserted zeros for interpolation
\param ant_pos array of all used antennes, same order as position
\param daq_id ID number of the DAQ system
*/
bool TriggerCoincidence(int NoCh, int *position, float *coincidence_time, int NoZero, char **ant_pos, int daq_id);


//! adds samples of two given channels
/*!
the channels samples are added each multiplied by a factor of 0.5
\param window_size the length of the trace
\param Ch1 number of channel one
\param Ch2 number of channel two
\param TriggerTrace pointer to the rectified channels' traces
*/
void ChannelAdd(int window_size, int Ch1, int Ch2, float **TriggerTrace);


//! decision of the level 1 trigger
/*!
\param NoCh number of antenna channels to decide for a level 1 trigger
\param window_size is the length of the raw undersampled trace array
\param trace is a pointer with two arrays. The first holds the number of antennas and the second the length of the trace (window_size). The first index starts with zero and contains the north-south polarisation information. The first index with number one contains the east-west pol info. So both arrays contain the hf signal of the antenna with the lowest antenna number. The first index with number two contains again the north-south pol info, but now from the second antenna. And so on .. 
\param ant_pos
\param daq_id
\param coincidence_time is the coincidence time between all detected signals, the time have to be in units of ns
\param DoRFISupp enable or disable RFI suppression, default: enabled
*/
bool L1Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time, bool DoRFISupp=true);

//! find dynamically the threshold
/*!
defines the threshold per channel by analysing the raw upsampled trace, do not use the RFISupp before
\param window_size length of the trace array
\param trace array for the adc data of length window_size
\param threshold dynamically found threshold
\param NoZeros number of zeros which are used to up-sample the raw data
\param DoRectifier switch to disable the rectifier method inside
*/
void DynamicThreshold(int window_size, float *trace, float *threshold, int NoZeros=7, bool DoRectifier=true);

//! coincidence check: like hardware
/*!
\param NoCh number of antenna channels to decide for a level 1 trigger
\param position is the number of bin, where the threshold is reached
\param coincidence_time of the given channels in nano seconds
\param NoZero number of inserted zeros for interpolation
\param ant_pos array of all used antennes, same order as position
\param daq_id ID number of the DAQ system
*/
bool TriggerCoincidenceHardware(int NoCh, int *position, int coincidence_time, int NoZero, char **ant_pos, int daq_id);


//! decision of the level 0 trigger - hardware trigger
/*!
Trace has to put without RFISup, otherwise the calculated threshold is wrong
\param NoCh number of antenna channels to decide for a level 1 trigger
\param window_size is the length of the raw undersampled trace array
\param trace is a pointer with two arrays. The first holds the number of antennas and the second the length of the trace (window_size). The first index starts with zero and contains the north-south polarisation information. The first index with number one contains the east-west pol info. So both arrays contain the hf signal of the antenna with the lowest antenna number. The first index with number two contains again the north-south pol info, but now from the second antenna. And so on .. 
\param ant_pos
\param daq_id
\param coincidence_time is the coincidence time between all detected signals, the time have to be in units of ns
\param NoZeros up-sampling factor minus 1
*/
bool L0Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time, int NoZeros=7);


//! Integral over the calculated envelope signal
/*!
\param window_size
\param trace
*/
float IntegralEnvelope(int window_size, float *trace);


//! Integral over the absolut value of the raw data
/*!
\param window_size
\param trace
*/
float IntegralRawData(int window_size, float *trace);


//!  Integral over the squared raw data
/*!
\param window_size
\param trace
*/
float IntegralRawSquareData(int window_size, float *trace);


//! Full Width Half Max of the found peak
/*!
\param window_size
\param trace
\param NoZeros factor +1 for up-sampling internal, set to null if trace is already up-sampled
\param RaisingEdge position of the FWHM peak
*/
float FWHMPeak(int window_size, float *trace, int *RaisingEdge, int NoZeros=7);


//! Give you the width of a peak defined by the fraction * height of the peak
/*!
\param window_size
\param trace
\param fraction
\param NoZeros factor +1 for up-sampling internal, set to null if trace is already up-sampled
\param RaisingEdge position of the chosen peak
\param ExpectedPosition of the pulse
\param DoRectifier switch to disable the rectifier
*/
float VarPeakWidth(int window_size, float *trace, float fraction, float *RaisingEdge, int *ExpectedPosition, int NoZeros=7, bool DoRectifier=true);


//! Give you the width of a peak defined by the fraction * height of the peak and the peak by a gausian fit
/*!
\param window_size
\param trace
\param fraction
\param NoZeros factor +1 for up-sampling internal, set to null if trace is already up-sampled
\param RaisingEdge position of the chosen peak
\param ExpectedPosition of the pulse
\param DoRectifier switch to disable the rectifier
*/
float VarPeakWidthGausFit(int window_size, float *trace, float fraction, float *RaisingEdge, int *ExpectedPosition, float *Peak, float *Peak_err, int NoZeros=7, bool DoRectifier=true);


//! Signal to Noise Ratio
/*
The raw trace is squared and the ratio of the max to mean value is calculated. Also the rms and mean is given back.
\param window_size is the length of the trace array
\param trace is the pointer to the raw data array
\param snr signal to noise ratio
\param mean mean value over the squared trace
\param rms root mean square of the trace
\param debug enables the debug info
*/
void SNR(int window_size, const float *trace, float *snr, float *mean, float *rms, bool debug=false);


//! Counts the pulses of raw ADC trace over a threshold
/*!
The raw, under-sampled ADC trace is squared and smoothed. After that the mean and its sigma is calculated. 
All pulses above a calculated threshold are detected and counted
When the algorithm detects a pulse, the next search bin will 30 bins later.
\param window_size defines the length of the ADC trace.
\param Trace is the pointer to the data array (type: float). It is overwritten by the power spectrum of the trace!
\param Mean
\param Sigma
\param Max
\param debug enable degub output, default disable
\return The number of peaks over the given threshold, if no peak is found, 0 is returned. If a problem happens -1 is given back.
*/
int PulseCount(unsigned int window_size, const float *Trace, double *Mean, double *Sigma, double *Max, bool debug=false);


//! SNR and number of pulses for the channel trigger
/*!
\param window_size length of trace
\param trace pointer to the adc data
\param SNR signal to noise ratio
\param NoPulses number of found pulses
\param debug enable degub output, default disable
*/
void SNRandNoPulses(int window_size, const float *trace, float *SNR, int *NoPulses, bool debug=false);


//! Hilbert Transformation
/*!
Calculates the hilbert transformation of the given signal -> complex part of a real signal
\param window_size length of the given array
\param data pointer with length window_size of the data
\param result pointer with length windwo_size where the result is written in
\param data_window switch to disable or enable a window function of the data
*/
void hilbert(int window_size, float *data, float *result, bool data_window=true);


//! Envelope with Hilbert Trafo
/*!
calculated is the envelope signal by using the hilbert transformation
\param NoZeros number of values to up-sample the raw data
\param window_size length of the sub-sampled trace
\param sub_trace pointer with length window_size with the raw sup-sampled data
\param up_trace pointer with the length window_size * (NoZeros+1) with the calculated envelope signal
*/
void RectifierHilbert(int NoZeros, int window_size, const float* sub_trace, float* up_trace);


//! Squared envelope with Hilbert Trafo
/*!
\param window_size length of the trace
\param trace pointer to the data
*/
void RectifierHilbertSquare(int window_size, float *trace);


//! Envelope with Hilbert Trafo
/*!
No up-sampling is done!
\param window_size length of the trace
\param trace pointer to the data
*/
void RectifierHilbert(int window_size, float* trace);


//! calculate the coincidence time for given zenith angle  and daq id
/*!
\param ID daq_id of the used antenna array
\param MaxZenith maximal zenith angle which is allowed, units in degree!
\return the given time is in units of ns
*/
float GetCoincidenceTime(short int ID, float MaxZenith=60);


//! count the crossing points of the RF signal with a dynamic thresold
/*!
The time differences between the crossing point are calculated in nano seconds.
Internal up sampling is applied.
\param window_size length of the Trace array
\param Trace pointer to the sub sampled data
\param ChannelID of the channel
\param AnaFlag pointer to the AnaFlag struct
\param NoZeros number of zeros for up sampling
\return gives the maximal found delay between two threshold crossing points in nanoseconds back
*/
float MaxThresholdCrossingDelay(int window_size, float *Trace, int ChannelID, struct AnaFlag *AnaFlag, int NoZeros=7);

#endif
