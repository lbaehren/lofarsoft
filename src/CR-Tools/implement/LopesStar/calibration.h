#ifndef __defined_calibration_h
#define __defined_calibration_h


/*! \file calibration.h 
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
*/
void ADC2FieldStrength(unsigned int window_size, float *trace, float zenith, int daq_id);


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


//! Gain directivity of the LPDA
/*
The gain directivity is an absolut number relative to field strength
\param frequency in MHz
\param zenith angle in degree
*/
float GainDirectivity(float frequency, float zenith);


//! field strength correction
/*
Correction parameter for the field strength, corresponding units are mu V / m
This calibration is only valid for D17, D19 and D30
\param frequency in MHz
*/
float FieldStrengthCorrection_RefAnt(float frequency);
#endif
