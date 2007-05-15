/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <LopesBase/DataTIM40.h>

using std::endl;

namespace CR {  // Namespace CR -- begin
  
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

}  // Namespace CR -- end
