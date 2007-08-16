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

/* $Id: tDataFrequency.cc,v 1.1 2005/07/15 07:16:47 bahren Exp $ */

#include <casa/aips.h>

#include <ApplicationSupport/Glish.h>
#include <LopesLegacy/DataFrequency.h>

using casa::GlishRecord;
using CR::DataFrequency;

/*!
  \file tDataFrequency.cc

  \brief A collection of test routines for DataFrequency.

  \author Lars B&auml;hren

  \data 2005/03/14
*/

// =============================================================================

GlishRecord setupGlishRecord () {

  GlishRecord rec;
  
  double FrequencyUnit  = 1e06;
  double SamplerateUnit = 1e06;
  double Samplerate     = 80;
  int NyquistZone       = 2;
  double FrequencyLow   = 0e06;
  double FrequencyHigh  = 40e06;
  int FFTSize           = 8191;
  int FFTlen            = 4096;

  rec.add("FrequencyUnit",FrequencyUnit);
  rec.add("SamplerateUnit",SamplerateUnit);
  rec.add("Samplerate",Samplerate);
  rec.add("NyquistZone",NyquistZone);
  rec.add("FrequencyLow",FrequencyLow);
  rec.add("FrequencyHigh",FrequencyHigh);
  rec.add("FFTSize",FFTSize);
  rec.add("FFTlen",FFTlen);

  return rec;
}

// =============================================================================

void test_construction (GlishRecord& rec) {

  cout << "\n[tDataFrequency::test_construction]\n" << endl;

  // Empty Constructor
  {
    DataFrequency df = DataFrequency ();
    df.show(std::cout);
  }

  // Argumented constructor
  {
    DataFrequency df (rec);
    df.show(std::cout);
  }

}

// =============================================================================

void test_subBands (GlishRecord& rec)
{
  cout << "\n[tDataFrequency::test_subBands]\n" << endl;

  int nofBands = 10;
  String division;
  Vector<double> freqRange;
  double bandwidth;
  double frac;

  DataFrequency df (rec);

  freqRange = df.frequencyRange ();

  bandwidth = freqRange(1)-freqRange(0);
  frac = 0.1*bandwidth;

  for (int i=0; i<5; i++) {
    freqRange(0) = freqRange(1)-bandwidth+i*frac;
    //
    division = "lin";
    df.setFrequencyBands (freqRange,nofBands,division);
    //
    division = "log";
    df.setFrequencyBands (freqRange,nofBands,division);
  }

}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main () {

  GlishRecord record = setupGlishRecord ();

  test_construction (record);

  test_subBands (record);

  return 0;

}
