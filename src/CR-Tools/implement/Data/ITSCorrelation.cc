/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id$*/

#include <Data/ITSCorrelation.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

ITSCorrelation::ITSCorrelation ()
{;}

ITSCorrelation::ITSCorrelation (String const &metafile)
{
  setMetafile (metafile);
}

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

Cube<DComplex> ITSCorrelation::ccSpectra (Bool const &fromCalFFT)
{
  uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
  
  Cube<DComplex> out (nofSelectedAntennas,
		      nofSelectedAntennas,
		      fftLength_p);
  
  out = 0.0;
  
  return out;
}
