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

/* $Id: TIM40Header.cc,v 1.1 2006/08/01 15:00:31 bahren Exp $*/

#include <Data/TIM40Header.h>

namespace lopestools {

// ==============================================================================
//
//  Construction
//
// ==============================================================================

TIM40Header::TIM40Header ()
{
  init ();
}

TIM40Header::TIM40Header (const String& project,
			  const String& dataType,
			  const String& filename,
			  const uInt& id,
			  const uInt& measurement,
			  const String& frequencyUnit,
			  const Double& samplerateValue,
			  const String& samplerateUnit,
			  const uInt& nyquistZone,
			  const String& location,
			  const Vector<Float>& antPos,
			  const String& weather,
			  const Int& timezone,
			  const String& time,
			  const String& date,
			  const Int& ADCMaxChann,
			  const Int& ADCMinChann,
			  const Float& maxVolt,
			  const String& localCalFile,
			  const String& antennaCalFile,
			  const String& calFileFreqUnit,
			  const String& files)
{
  TIM40Header::setProject (project);
  TIM40Header::setDataType (dataType);
  TIM40Header::setFilename (filename);
  TIM40Header::setId (id);
  TIM40Header::setMeasurement (measurement);
  TIM40Header::setFrequencyUnit (frequencyUnit);
  TIM40Header::setLocation (location);
  TIM40Header::setAntPos (antPos);
  TIM40Header::setWeather (weather);
  TIM40Header::setTimezone (timezone);
  TIM40Header::setTime (time);
  TIM40Header::setDate (date);
  TIM40Header::setADCMaxChann (ADCMaxChann);
  TIM40Header::setADCMinChann (ADCMinChann);
  TIM40Header::setMaxVolt (maxVolt);
  TIM40Header::setLocalCalFile (localCalFile);
  TIM40Header::setAntennaCalFile (antennaCalFile);
  TIM40Header::setCalFileFreqUnit (calFileFreqUnit);
  TIM40Header::setFiles (files);
  //
  TIM40Header::setSampleInfo (samplerateValue,
			      samplerateUnit,
			      nyquistZone);
}

TIM40Header::TIM40Header (TIM40Header const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

TIM40Header::~TIM40Header ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

TIM40Header &TIM40Header::operator= (TIM40Header const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void TIM40Header::copy (TIM40Header const& other)
{
  Project_p         = other.Project_p;
  DataType_p        = other.DataType_p;
  Filename_p        = other.Filename_p;
  ID_p              = other.ID_p;
  Measurement_p     = other.Measurement_p;
  FrequencyUnit_p   = other.FrequencyUnit_p;
  Samplerate_p      = other.Samplerate_p;
  NyquistZone_p     = other.NyquistZone_p;
  Location_p        = other.Location_p;
  AntPos_p          = other.AntPos_p;
  Weather_p         = other.Weather_p;
  Timezone_p        = other.Timezone_p;
  Time_p            = other.Time_p;
  Date_p            = other.Date_p;
  ADCMaxChann_p     = other.ADCMaxChann_p;
  ADCMinChann_p     = other.ADCMinChann_p;
  MaxVolt_p         = other.MaxVolt_p;
  LocalCalFile_p    = other.LocalCalFile_p;
  AntennaCalFile_p  = other.AntennaCalFile_p;
  CalFileFreqUnit_p = other.CalFileFreqUnit_p;
  Files_p           = other.Files_p;
  Bandwidth_p       = other.Bandwidth_p;
}

void TIM40Header::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

Quantity TIM40Header::samplerate (const String& unit)
{
  return Samplerate_p.get(Unit(unit));
}

void TIM40Header::setSamplerate (const Double& value,
				 const String& unit)
{
  TIM40Header::setSamplerate (Quantity(value,unit));
}

void TIM40Header::setSamplerate (const Quantity& samplerate)
{
  TIM40Header::setSamplerate (MVFrequency(samplerate));
}

void TIM40Header::setSamplerate (const MVFrequency& samplerate)
{
  Samplerate_p = samplerate;
}

void TIM40Header::setSampleInfo (const Double& samplerateValue,
				 const String& samplerateUnit,
				 const uInt& nyquistZone)
{
  TIM40Header::setSamplerate (samplerateValue,samplerateUnit);
  TIM40Header::setNyquistZone (nyquistZone);
  //
  TIM40Header::setBandwidth ();
}

Vector<Int> TIM40Header::ADCRange ()
{
  Vector<Int> range (2);
  range(0) = ADCMaxChann_p;
  range(1) = ADCMinChann_p;
  return range;
}

void TIM40Header::setADCRange (const Vector<Int>& range)
{
  ADCMaxChann_p = min(range);
  ADCMinChann_p = max(range);
}

Float TIM40Header::ADC2Voltage ()
{
  return MaxVolt_p/ADCMaxChann_p;
}

void TIM40Header::setBandwidth ()
{
  Bandwidth_p = 0.5*Samplerate_p;
}

Quantity TIM40Header::bandwidth (const String& unit)
{
  return Bandwidth_p.get(Unit(unit));
}


// ==============================================================================
//
//  Methods
//
// ==============================================================================

void TIM40Header::init () 
{
  // Default values
  Project_p         = "UNDEFINED";
  DataType_p        = "TIM40";
  Filename_p        = "UNDEFINED";
  ID_p              = 1;
  Measurement_p     = 1;
  FrequencyUnit_p   = "MHz";
  Samplerate_p      = MVFrequency(Quantity(80.0,"MHz"));
  NyquistZone_p     = 2;
  Location_p        = "UNDEFINED";
  AntPos_p          = Vector<Float> (3,0.0);
  Weather_p         = "UNDEFINED";
  Timezone_p        = 0;
  Time_p            = "UNDEFINED";
  Date_p            = "UNDEFINED";
  ADCMaxChann_p     = 2048;
  ADCMinChann_p     = -2048;
  MaxVolt_p         = 1;
  LocalCalFile_p    = "UNDEFINED";
  AntennaCalFile_p  = "UNDEFINED";
  CalFileFreqUnit_p = "MHz";
  Files_p           = "UNDEFINED";
  // derived values
  TIM40Header::setBandwidth ();
}

void TIM40Header::show ()
{
  TIM40Header::show (std::cout);
}

void TIM40Header::show (std::ostream& s = std::cout)
{
  s << "[TIM40Header]" << endl;
  s << " - Project         = " << project() << endl;
  s << " - DataType        = " << dataType() << endl;
  s << " - Filename        = " << filename() << endl;
  s << " - ID              = " << id() << endl;
  s << " - Measurement     = " << measurement() << endl;
  s << " - FrequencyUnit   = " << frequencyUnit() << endl;
  s << " - Samplerate      = " << samplerate(FrequencyUnit_p) << endl;
  s << " - Bandwidth       = " << bandwidth(FrequencyUnit_p) << endl;
  s << " - NyquistZone     = " << nyquistZone() << endl;
  s << " - Location        = " << location() << endl;
  s << " - AntPos          = " << antPos() << endl;
  s << " - Weather         = " << weather() << endl;
  s << " - Timezone        = " << timezone() << endl;
  s << " - Time            = " << time() << endl;
  s << " - Date            = " << Date_p << endl;
  s << " - ADCMaxChann     = " << ADCMaxChann_p << endl;
  s << " - ADCMinChann     = " << ADCMinChann_p << endl;
  s << " - MaxVolt         = " << maxVolt() << endl;
  s << " - LocalCalFile    = " << localCalFile() << endl;
  s << " - AntennaCalFile  = " << antennaCalFile() << endl;
  s << " - CalFileFreqUnit = " << calFileFreqUnit() << endl;
  s << " - Files           = " << files() << endl;
}

}
