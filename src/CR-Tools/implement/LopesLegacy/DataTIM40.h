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

#ifndef DATATIM40_H
#define DATATIM40_H

/* $Id: DataTIM40.h,v 1.2 2006/08/01 15:03:25 bahren Exp $ */

#include <casa/aips.h>
#include <casa/iostream.h>

namespace CR {  // Namespace CR -- begin

/*!
  \class DataTIM40

  \ingroup LopesLegacy

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
  
  inline int ADCMinChannel () {
    return ADCMinChannel_p;
  }
  
  inline int ADCMaxChannel () {
    return ADCMaxChannel_p;
  }
  
  inline double maxVoltage () {
    return maxVoltage_p;
  }
  
  inline double presync () {
    return presync_p;
  }
  
  // --- Characteristics of the signal sampling ----------------------
  
  inline double frequencyUnit () {
    return frequencyUnit_p;
  }
  
  inline double samplerateUnit () {
    return samplerateUnit_p;
  }
  
  inline double samplerate () {
    return samplerate_p;
  }
  
  inline int nyquistZone () {
    return nyquistZone_p;
  }
  
  // --- Show the current settings for TIM40 -------------------------

  void show (std::ostream& os);

 private:

  void defaultSettings ();

};

}  // Namespace CR -- end

#endif
