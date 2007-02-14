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

/* $Id: AxisCoordinates.cc,v 1.3 2006/07/05 16:01:08 bahren Exp $*/

#include <iostream>
#include <Coordinates/AxisCoordinates.h>

using std::cout;
using std::endl;

/*!
  \class AxisCoordinates
*/

namespace CR {  // Namespace CR -- begin

// ==============================================================================
//
//  Construction
//
// ==============================================================================

AxisCoordinates::AxisCoordinates ()
{
  Vector<double> frequencyRange(2);

  frequencyRange(0) = 40e06;
  frequencyRange(1) = 80e06;

  setBlocksize (1024);
  setOffset (0);
  setPresync (0);
  setSampleFrequency (80e06);
  setFrequencyRange (frequencyRange);
}

AxisCoordinates::AxisCoordinates (const int& blocksize,
				  const int& offset,
				  const int& presync,
				  const double& sampleFrequency,
				  const Vector<double>& frequencyRange)
{
  setBlocksize (blocksize);
  setOffset (offset);
  setPresync (presync);
  setSampleFrequency (sampleFrequency);
  setFrequencyRange (frequencyRange);
}

AxisCoordinates::AxisCoordinates (AxisCoordinates const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

AxisCoordinates::~AxisCoordinates ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

AxisCoordinates& AxisCoordinates::operator= (AxisCoordinates const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void AxisCoordinates::copy (AxisCoordinates const& other)
{
  axes_p = other.axes_p;

  offset_p          = other.offset_p;
  blocksize_p       = other.blocksize_p;
  presync_p         = other.presync_p;
  sampleFrequency_p = other.sampleFrequency_p;
  
  frequencyRange_p.resize(other.frequencyRange_p.shape());
  frequencyRange_p  = other.frequencyRange_p;
}

void AxisCoordinates::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void AxisCoordinates::setCoordinatesMap ()
{
  axes_p["Pos"]          = "Sample";
  axes_p["Time"]         = "Seconds";
  axes_p["fx"]           = "Counts";
  axes_p["Voltage"]      = "Volt";
  axes_p["IntermedFreq"] = "Hz";
  axes_p["Frequency"]    = "Hz";
  axes_p["AbsFFT"]       = "units";
  axes_p["Power"]        = "Watts/Bin";
  axes_p["MeanPower"]    = "Watts/Bin";
  axes_p["CalPower"]     = "Watts/Bin";
  axes_p["NoiseT"]       = "K";
  axes_p["SkyT"]         = "K";
  axes_p["MNT"]          = "K";
  axes_p["Phase"]        = "Degree";
  axes_p["PhaseDiff"]    = "Degree";
  axes_p["PhaseGrad"]    = "Degree";
  axes_p["AntennaGain"]  = "Volt";
  axes_p["BeamT"]        = "K";
  axes_p["BeamF"]        = "units";
  axes_p["TimeLag"]      = "Seconds";
  axes_p["PosLag"]       = "Samples";
  axes_p["CrossCorr"]    = "units";
  axes_p["PowerT"]       = "counts_2";
  axes_p["FlagF"]        = "Weight";
  axes_p["FlagT"]        = "Weight";
  axes_p["Flux"]         = "Jy";
}

void AxisCoordinates::setFrequencyRange (const Vector<double>& frequencyRange)
{
  frequencyRange_p.resize(2);
  
  frequencyRange_p(0) = min(frequencyRange);
  frequencyRange_p(1) = max(frequencyRange);
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

Vector<double> AxisCoordinates::axisValues (const AxisCoordinates::COORDINATE& which)
{
  Vector<double> values;

  switch (which) {
  case Pos:
    return getPos();
    break;
  case Time:
    return getTime ();
    break;
  case Frequency:
    return getFrequency ();
    break;
  case IntermedFreq:
    return getIntermedFreq ();
    break;
  case fx:
  case Voltage:
    cout << "This is no coordinate axis - contains actual data!" << endl;
    break;
  default:
    cout << "axis not yet handled" << endl;
  }
  
  return values;
}

// ----------------------------------------------------------------------- getPos

Vector<double> AxisCoordinates::getPos ()
{
  Vector<double> values (blocksize_p);

  values = offset_p;

  for (int pos(0); pos<blocksize_p; pos++) {
    values(pos) += pos;
  }
  
  return values;
}

// ---------------------------------------------------------------------- getTime

Vector<double> AxisCoordinates::getTime () 
{
  Vector<double> presync (blocksize_p,double(presync_p));
  Vector<double> values ((getPos()-presync)/sampleFrequency_p);
  
  return values;
}

// ----------------------------------------------------------------- frequencyBin

double AxisCoordinates::frequencyBin ()
{
  return (frequencyRange_p(1)-frequencyRange_p(0))/double(fftLength()-1.0);
}

// -------------------------------------------------------------------- fftLength

int AxisCoordinates::fftLength ()
{
  return blocksize_p/2+1;
}

// ----------------------------------------------------------------- getFrequency

Vector<double> AxisCoordinates::getFrequency () 
{
  int fftlen (fftLength());
  double freqBin (frequencyBin());
  Vector<double> values (fftlen);

  /* Compute the individual frequency values from the sampling frequency
     and the number of frequency bins; note that (k-1) -> k, since in C++
     arrays are 0-based instead of 1-based as in Glish. */
  values = frequencyRange_p(0);
  for (int k(0); k<fftlen; ++k) {
    values(k) += k*freqBin;
  }

  return values;
}

// -------------------------------------------------------------- getIntermedFreq

Vector<double> AxisCoordinates::getIntermedFreq ()
{
  int nofSteps (fftLength()-1);
  double freqBin (frequencyBin());
  Vector<double> values (nofSteps,freqBin);
  
  for (int k(0); k<nofSteps; ++k) {
    values(k) *= k;
  }
  
  return values;
}

}  // Namespace CR -- end
