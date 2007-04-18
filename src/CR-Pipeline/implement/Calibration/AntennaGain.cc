/***************************************************************************
 *   Copyright (C) 2005 by Lars B"ahren                                    *
 *   bahren@astron.nl                                                      *
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

/* $Id: AntennaGain.cc,v 1.7 2006/10/31 18:23:24 bahren Exp $ */

#include <Calibration/AntennaGain.h>

// ----------------------------------------------------------------------------
//
//  Construction
//
// ----------------------------------------------------------------------------

AntennaGain::AntennaGain (const int& nofAntennas,
			  const int& nofBands)
{
  // set array with default gain values ...
  Matrix<double> gainCurves (nofBands,nofAntennas,1.0);
  // ... and set them as initial values
  AntennaGain::setGainCurves (gainCurves);
}

AntennaGain::AntennaGain (const Matrix<double>& gainCurves)
{
  // just forwards the input
  AntennaGain::setGainCurves (gainCurves);
}

AntennaGain::AntennaGain (const Matrix<double>& spectra,
			  const int& nofBands)
{
  AntennaGain::setGainCurves (spectra, nofBands);
}

// ----------------------------------------------------------------------------
//
//  Destruction
//
// ----------------------------------------------------------------------------

AntennaGain::~AntennaGain () 
{ }

// ----------------------------------------------------------------------------
//
//  Access to the gain information
//
// ----------------------------------------------------------------------------

void AntennaGain::setGainCurves (const Matrix<double>& gainCurves) 
{
  gainCurves.shape(nofBands_p,nofAntennas_p);
  //
  gainCurves_p.resize (gainCurves.shape());
  gainCurves_p = gainCurves;
}

void AntennaGain::setGainCurves (const Matrix<double>& spectra,
				 const int& nofBands)
{
  int ant(0);
  int band(0);
  IPosition shape (spectra.shape());
  IPosition blc (1,0);
  IPosition trc (1,0);
  double minBand (0);
  Vector<double> spectrum(shape(0));
  // Set up the indices to break down the frequencies into sub-bands.
  AntennaGain::setBandIndices (spectra, nofBands);
  
  for (ant=0; ant<nofAntennas_p; ant++) {
    spectrum = spectra.column(ant);
    for (band=0; band<nofBands; band++) {
      blc(0) = bandIndices_p(band);
      trc(0) = bandIndices_p(band+1);
      // 
      minBand = min(spectrum(blc,trc));
      //
      if (minBand < gainCurves_p(ant,band)) {
	gainCurves_p(ant,band) = minBand;
      }
    }
  }
  
}

// ----------------------------------------------------------------------------
//
//  Extraction of the antenna gain curves
//
// ----------------------------------------------------------------------------

void AntennaGain::setBandIndices (const Matrix<double>& spectra,
				  const int& nofBands)
{
  // get the shape of the array with the spectra, [freq,ant]
  IPosition shape (spectra.shape());

  // Store the shape information
  nofBands_p = nofBands;
  nofAntennas_p = shape(1);

  // Set up the range of array indices which has to be split
  Vector<int> indexRange(2);
  indexRange(0) = 0;
  indexRange(1) = shape(0)-1;

  // Set up the band indices
  bandIndices_p.resize(nofBands);
  bandIndices_p = CR::splitRange(indexRange,nofBands,"lin");
}

// ----------------------------------------------------------------------------
//
//  Export of the calibration data
//
// ----------------------------------------------------------------------------

Matrix<double> AntennaGain::gainCurves (const Matrix<double>& gainCurves)
{
  return gainCurves_p;
}

Bool AntennaGain::toTable (const String& tableName)
{
  Bool ok (True);
  IPosition gainShape (gainCurves_p.shape());
  Vector<String> columnLabels;
  ostringstream columnLabel;

  // -----------------------------------------------------------------
  // Set up the labels fo the antenna gain columns

  cout << " - Creating labels for the antenna gain columns ..." << endl;

  columnLabels.resize (nofAntennas_p);

  for (int nant=0; nant<nofAntennas_p; nant++) {
    columnLabel << "Antenna" << (nant+1);
    columnLabels(nant) = columnLabel.str();
    columnLabel.seekp(0, ios::beg);
  }

  // -----------------------------------------------------------------
  // Start setting up and filling the table
  
  cout << " - Build the table description ..." << endl;
  TableDesc td("gaintable", "1", TableDesc::Scratch);
  td.comment() = "A table of ITS antenna gaincurves";


  return ok;
}
