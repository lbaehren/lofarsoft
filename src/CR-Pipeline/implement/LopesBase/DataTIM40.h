#ifndef DATATIM40_H
#define DATATIM40_H

/* $Id: DataTIM40.h,v 1.2 2006/08/01 15:03:25 bahren Exp $ */

#include <casa/aips.h>
#include <casa/iostream.h>

#include <casa/namespace.h>

/*!
  \class DataTIM40

  \ingroup LopesBase

  \brief Data and functions for the 40 MHz bandwidth TIM-Board

  \author Lars B&auml;hren

  \date 2005/05

  \test tDataTIM40.cc

  <h3>Synopsis</h3>

  This class keeps and provides the basic data on the 40 MHz bandwidth
  TIM-Board used for data aquisition on LOPES and LOFAR ITS. The C++ class
  is modelled after the initial Glish implementation in <i>data-TIM40.g</i>

  \verbatim
  DataType='TIM40',
  FrequencyUnit='1e6',
  SamplerateUnit='1e6',
  Samplerate='80',
  NyquistZone='2',
  ADCMaxChann='2048',
  ADCMinChann='-2048',
  MaxVolt='1',
  LocalCalFile='empty.tab',
  AntennaCalFile='empty.tab',
  CalFileFreqUnit='1e6',
  \endverbatim
*/

class DataTIM40 {

  // Minimum channel in the ADC conversion
  int ADCMinChannel_p;
  // Maximum channel in the ADC conversion
  int ADCMaxChannel_p;
  // Voltage corresponding to the maximum ADC channel
  double maxVoltage_p;

  double presync_p;
  
  // Unit for the frequency values
  double frequencyUnit_p;
  // Unit for the sample rate
  double samplerateUnit_p;
  // Samplerate given in units of SamplerateUnit
  double samplerate_p;
  // The Nyquist Zone
  int nyquistZone_p;
  // Frequency Bandwidth
  double bandwidth_p;

  double calFileFreqUnit_p;

 public:

  // --- Construction ------------------------------------------------

  /*!
    \brief Default constructor
  */
  DataTIM40 ();

  /*!
    \brief Argumented constructor

    \param ADCMinChannel -- Minimum channel in the ADC conversion
    \param ADCMaxChannel -- Maximum channel in the ADC conversion
    \param maxVoltage    -- Voltage corresponding to the maximum ADC channel
    \param presync       -- 
  */
  DataTIM40 (const int ADCMinChannel, 
	     const int ADCMaxChannel,
	     const double maxVoltage,
	     const double presync);

  // --- Destruction -------------------------------------------------

  /*!
    \brief Destructor
  */
  ~DataTIM40 ();

  // --- Characteristics of the ADC conversion -----------------------

  int ADCMinChannel ();

  int ADCMaxChannel ();

  double maxVoltage ();

  double presync ();

  // --- Characteristics of the signal sampling ----------------------

  double frequencyUnit ();

  double samplerateUnit ();

  double samplerate ();

  int nyquistZone ();

  // --- Show the current settings for TIM40 -------------------------

  void show (std::ostream& os);

 private:

  void defaultSettings ();

};

#endif
