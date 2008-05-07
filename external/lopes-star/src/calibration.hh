#ifndef __defined_calibration_hh
#define __defined_calibration_hh


//!star tool headers
#include "trigger.hh"
#include "reconstruction.hh"
#include "util.hh"


//!RecRadio headers
#include "RecRadioCalib.hh"
#include "RecRadioTrigger.hh"
#include "RecRadioUtil.hh"
#include "RecRadioBasic.hh"


//!std c++
#include <iostream>
#include <time.h>
#include <math.h>


//!root 
#include <TMath.h>

using namespace std;
using namespace TMath;


/*! \file calibration.hh 
 \brief Collection of functions and methods to perform the calibration

 \date    Mar 13 2007
 \author  Thomas Asch
 
 to do
 - there is always something to do.. isn't it :-)
*/

//! Scales the ADC value to a mean field strength
/*
the pointer to the ADC values are scaled by a fixed factor to get a mean field strenght to the corresponding ADC values.
\param window_size is the length of the trace pointer
\param trace pointer to the ADC data
\param zenith angle of the observation
\param daq_id ID of daq which the data are taken, if the calibration is unknown for this daq, the trace is multiplied by 1. No error is given.
\param RefAnt switch to control the calibration method
*/
void ADC2FieldStrength(unsigned int window_size, float *trace, int daq_id, float zenith=30.0, bool RefAnt=false);


//! calibration and up-sampling in one function
/*
\param window_size is the length of the trace pointer
\param SubTrace trace of the sub-sampled data
\param NoZeros up-sampling factor -1
\param UpTrace array for the up-sampled trace
\param daq_id ID of daq which the data are taken, if the calibration is unknown for this daq, the trace is multiplied by 1. No error is given.
\param zenith angle of the observation
\param RefAnt switch to control the calibration method
*/
void ADC2FieldStrengthUpSampled(unsigned int window_size, float *SubTrace, int NoZeros, float *UpTrace, int daq_id, float zenith, bool RefAnt);


//! core function of the ADC calibration
/*
\param window_size is the length of the trace pointer
\param Amp amplitude distribution after FFT
\param Phase distribtution after FFT
\param daq_id ID of daq which the data are taken, if the calibration is unknown for this daq, the trace is multiplied by 1. No error is given.
\param zenith angle of the direction reconstruction
\param RefAnt switch to control the calibration method
*/
void ADC2FieldStrengthCore(int window_size, float *Amp, float *Phase, int daq_id, float zenith=30., bool RefAnt=false);

//! calculate the calibrated amp and phase distribution
/*
\param window_size is the length of the trace pointer
\param trace pointer to the ADC data
\param Amp array field with the calibrated amp distirbution in uV/m/MHz
\param Phase phase distribution
\param daq_id ID of the current antenna cluster
\param zenith angle
\param RefAnt switch between calibration with reference antenne or lab. measurement
\para data_window switch to enable or disable the data windowing for the FFT function
*/
void CalibratedFrequencyDomain(int window_size, float *trace, float *Amp, float *Phase, int daq_id, float zenith=30.0, bool RefAnt=true, bool data_window=true);


//! calculate the power trace
/*
\param window_size is the length of the trace pointer
\param trace pointer to the ADC data
*/
void PowerTrace(unsigned int window_size, float *trace);


//! Amplitude values of given reference source in units of field strength
/*
The given amp. values are measured in 10MHz steps and are interpolated in 1MHz steps (from manufacturer).
All values in between are linear interpolated in this function.
\param frequency of the amplitude value in field strength
*/
float AmpRefSource_FieldStrength(float frequency);


//! antenna gain
/*
The gain of the antenna calculated out of the frequency and the gain directivity
\param frequency in MHz
\param zenith angle in degree
\param azimuth angle in degree
*/
float AntennaGain(float frequency, float zenith);


//! Gain directivity of the LPDA
/*
The gain directivity is an absolut number relative to field strength
\param frequency in MHz
\param zenith angle in degree
\param azimuth angle in degree
*/
float GainDirectivity(float frequency, float zenith);


//! field strength correction
/*
Correction parameter for the field strength, corresponding units are mu V / m
This calibration is only valid for D17, D19 and D30
\param frequency in MHz
*/
float FieldStrengthCorrection_RefAnt(float frequency);


//! attenuation of a RG213 cable
/*
The attenuation factor is in dB and positive
\param length of the used cable
\param frequency in MHz
*/
float CableAttenuationRG213(float length, float frequency);


//! attenuation of a RG058 cable
/*
The attenuation factor is in dB and positive
\param length of the used cable
\param frequency in MHz
*/
float CableAttenuationRG058(float length, float frequency);


//! electric field strength gain of the electronics
/*
The calculated values are the gain of the amplitude caused by the electronics.
\param frequency in MHz
*/
float FieldStrengthElectronicsGain(float frequency);


#endif
