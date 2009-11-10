/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <Data/ITSCorrelation.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                 ITSCorrelation

ITSCorrelation::ITSCorrelation ()
{;}

//_______________________________________________________________________________
//                                                                 ITSCorrelation

ITSCorrelation::ITSCorrelation (String const &metafile)
{
  setMetafile (metafile);
}

//_______________________________________________________________________________
//                                                                 ITSCorrelation

ITSCorrelation::ITSCorrelation (ITSCorrelation const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

ITSCorrelation::~ITSCorrelation ()
{
  destroy();
}

void ITSCorrelation::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

ITSCorrelation& ITSCorrelation::operator= (ITSCorrelation const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void ITSCorrelation::copy (ITSCorrelation const &other)
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                    setMetafile

void ITSCorrelation::setMetafile (String const &metafile)
{
  bool status (true);
  
  // make a connection to the metafile and get its contents
  try {
    metadata_p.setMetafile (metafile);
  } catch (AipsError x) {
    cerr << "[ITSCapture::setMetafile] " << x.getMesg() << endl;
    status = false;
  }
  
  // Set up the file streams by which to connect to the data stored on disk
  if (status) {
    status = setStreams ();
  }
}

Bool ITSCorrelation::setStreams ()
{
  Bool status (True);

  return status;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                      ccSpectra

Cube<DComplex> ITSCorrelation::ccSpectra (Bool const &fromCalFFT)
{
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  
  Cube<DComplex> data (nofSelectedAntennas,
		      nofSelectedAntennas,
		      fftLength_p);

  ITSCorrelation::ccSpectra (data,fromCalFFT);
  
  return data;
}

//_______________________________________________________________________________
//                                                                      ccSpectra

void ITSCorrelation::ccSpectra (Cube<DComplex> &data,
				Bool const &fromCalFFT)
{
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  casa::IPosition shape (3,
			 nofSelectedAntennas,
			 nofSelectedAntennas,
			 fftLength_p);

  /* Check the shape of the array returning the data */
  if (data.shape() != shape) {
    data.resize(shape);
  }

  data = 0.0;
}
