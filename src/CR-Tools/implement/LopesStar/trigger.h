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


//! simple rectifier of HF signal
/*!
\param NoZero number of inserted zeros for interpolation
\param window_size the length of the trace
\param trace the array to the HF signal
*/
void Rectifier(int NoZero, int window_size, float *trace);


//! simple rectifier of HF signal like the hardware do it
/*!
\param NoZero number of inserted zeros for interpolation
\param window_size the length of the trace
\param trace the array to the HF signal
*/
void RectifierHardware(int NoZero, int window_size, float *trace);


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
*/
void DynamicThreshold(int window_size, float *trace, float *threshold);

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
*/
bool L0Decide(int NoCh, int window_size, float **trace, char **ant_pos, int daq_id, float *coincidence_time);


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
*/
float FWHMPeak(int window_size, float *trace);


//! Give you the width of a peak defined by the fraction * height of the peak
/*!
\param window_size
\param trace
\param fraction
*/
float VarPeakWidth(int window_size, float *trace, float fraction);

#endif
