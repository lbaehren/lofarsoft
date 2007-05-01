
#include <LopesBase/DataTIM40.h>

// =============================================================================
//
//  Construction
//
// =============================================================================

DataTIM40::DataTIM40 ()
{
  DataTIM40::defaultSettings ();
}

DataTIM40::DataTIM40 (const int ADCMinChannel, 
		      const int ADCMaxChannel,
		      const double maxVoltage,
		      const double presync)
{
  DataTIM40::defaultSettings ();
  //
  ADCMinChannel_p = ADCMinChannel;
  ADCMaxChannel_p = ADCMaxChannel;
  maxVoltage_p = maxVoltage;
  presync_p = presync;
}

DataTIM40::~DataTIM40 ()
{;}

// =============================================================================
//
//  Initialization
//
// =============================================================================

void DataTIM40::defaultSettings ()
{
  // Minimum channel in the ADC conversion
  ADCMinChannel_p = -2048;
  // Maximum channel in the ADC conversion
  ADCMaxChannel_p = 2048;
  // Voltage corresponding to the maximum ADC channel
  maxVoltage_p = 1.0;
  
  // Unit for the frequency values
  frequencyUnit_p = 1e06;
  // Unit for the sample rate
  samplerateUnit_p = 1e06;
  // Samplerate given in units of SamplerateUnit
  samplerate_p = 80;
  // The Nyquist Zone
  nyquistZone_p = 2;
  // Frequency Bandwidth
  bandwidth_p = 40;
}

// =============================================================================
//
//  Access to the board parameters
//
// =============================================================================

int DataTIM40::ADCMinChannel () 
{
  return ADCMinChannel_p;
}

int DataTIM40::ADCMaxChannel () 
{
  return ADCMaxChannel_p;
}

double DataTIM40::maxVoltage ()
{
  return maxVoltage_p;
}

double DataTIM40::presync ()
{
  return presync_p;
}

// =============================================================================
//
//  Access to the sampling parameters
//
// =============================================================================

double DataTIM40::frequencyUnit ()
{
  return frequencyUnit_p;
}

double DataTIM40::samplerateUnit ()
{
  return samplerateUnit_p;
}

double DataTIM40::samplerate ()
{
  return samplerate_p;
}

int DataTIM40::nyquistZone ()
{
  return nyquistZone_p;
}

// =============================================================================
//
//  Show the current settings for the TIM40 board
//
// =============================================================================

void DataTIM40::show (std::ostream& os)
{
  os << "[DataTIM40]" << endl;

  os << " - Minimum channel in the ADC conversion .......... : "
     << ADCMinChannel_p << endl;
  os << " - Maximum channel in the ADC conversion .......... : "
     << ADCMaxChannel_p << endl;
  os << " - Voltage corresponding to the maximum ADC channel : "
     << maxVoltage_p << endl;
  
  os << " - Unit for the frequency values .................. : "
     << frequencyUnit_p << endl;
  os << " - Unit for the sample rate ....................... : "
     << samplerateUnit_p << endl;
  os << " - Samplerate given in units of SamplerateUnit .... : "
     << samplerate_p << endl;
  os << " - The Nyquist Zone ............................... : "
     << nyquistZone_p << endl;
  os << " - Frequency Bandwidth ............................ : "
     << bandwidth_p << endl;
}
