/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef TIM40_H
#define TIM40_H

/* $Id: TIM40.h,v 1.2 2006/08/01 15:03:25 bahren Exp $ */

#include <casa/aips.h>
#include <casa/iostream.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class TIM40
    
    \ingroup CR_Data
    
    \brief Data and functions for the 40 MHz bandwidth TIM-Board
    
    \author Lars B&auml;hren
    
    \date 2005/05
    
    \test tTIM40.cc
    
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
  
  class TIM40 {
    
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
    TIM40 ();
    
    /*!
      \brief Argumented constructor
      
      \param ADCMinChannel -- Minimum channel in the ADC conversion
      \param ADCMaxChannel -- Maximum channel in the ADC conversion
      \param maxVoltage    -- Voltage corresponding to the maximum ADC channel
      \param presync       -- 
    */
    TIM40 (int const &ADCMinChannel, 
	   int const &ADCMaxChannel,
	   double const &maxVoltage,
	   double const &presync);
    
    // --- Destruction -------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~TIM40 ();
    
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
  
}  // Namespace CR -- end

#endif
