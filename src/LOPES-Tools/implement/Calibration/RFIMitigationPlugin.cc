/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

/* $Id: RFIMitigationPlugin.cc,v 1.3 2006/09/22 14:01:06 bahren Exp $*/

#include <lopes/Calibration/RFIMitigationPlugin.h>

/*!
  \class RFIMitigationPlugin
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

RFIMitigationPlugin::RFIMitigationPlugin ()
  : PluginBase<Complex> ()
{;}

RFIMitigationPlugin::RFIMitigationPlugin (RFIMitigationPlugin const &other)
  : PluginBase<Complex> ()
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

RFIMitigationPlugin::~RFIMitigationPlugin ()
{
  destroy();
}

void RFIMitigationPlugin::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

RFIMitigationPlugin& RFIMitigationPlugin::operator= (RFIMitigationPlugin const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void RFIMitigationPlugin::copy (RFIMitigationPlugin const &other)
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

// ---------------------------------------------------------------- setParameters

Bool RFIMitigationPlugin::setParameters (Record const &param)
{
  Bool status (True);
  int field (0);

  /*
    Parse the parameter record; process parameters in inverse order of their
    dependency, to ensure the complete updating chain is gone through .
   */

  field = param.fieldNumber ("nofSegments");
  if (field > -1) {
    uint nofSegments (1);
    param.get (field,nofSegments);
    setNofSegments (nofSegments);
  }

  field = param.fieldNumber ("blocksize");
  if (field > -1) {
    uint blocksize (1);
    param.get (field,blocksize);
    setBlocksize (blocksize);
  }

  return status;
}

// ----------------------------------------------------------------- setBlocksize

void RFIMitigationPlugin::setBlocksize (uint const &blocksize)
{
  // store the provided value
  blocksize_p = blocksize;
  // ... and trigger updating of the derived parameters
  setNofSegments ();
}

// --------------------------------------------------------------- setNofSegments

void RFIMitigationPlugin::setNofSegments ()
{
  uint nofSegments;

  /*
    recompute the number of segments based on the current value for the
    blocksize
  */

  // store the computed value 
  setNofSegments (nofSegments);
}

void RFIMitigationPlugin::setNofSegments (uint const &nofSegments)
{
  // store the provided value ...
  nofSegments_p = nofSegments;
}

void RFIMitigationPlugin::setSegmentationIndices ()
{
  Vector<uint> indices;

  // compute the indices ...

  // ... and store the values
  setSegmentationIndices (indices);
}

void RFIMitigationPlugin::setSegmentationIndices (Vector<uint> const &indices)
{}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

