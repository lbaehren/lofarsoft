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

/* $Id: AxisIterator.cc,v 1.1 2006/07/05 16:01:08 bahren Exp $*/

#include <Coordinates/AxisIterator.h>

/*!
  \class AxisIterator
*/

namespace LOPES {  // Namespace LOPES -- begin

// ==============================================================================
//
//  Construction
//
// ==============================================================================

AxisIterator::AxisIterator ()
{
  setBlock(1);
}

AxisIterator::AxisIterator (const int& block)
{
  setBlock(block);
}

AxisIterator::AxisIterator (const int& blocksize,
			    const int& offset,
			    const int& presync,
			    const double& sampleFrequency,
			    const Vector<double>& frequencyRange)
  : AxisCoordinates (blocksize,
		     offset,
		     presync,
		     sampleFrequency,
		     frequencyRange)
{
  setBlock(1);
}

AxisIterator::AxisIterator (const int& block,
			    AxisCoordinates const& axis)
  : AxisCoordinates (axis)
{
  setBlock(block);
}

AxisIterator::AxisIterator (AxisIterator const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

AxisIterator::~AxisIterator ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

AxisIterator& AxisIterator::operator= (AxisIterator const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void AxisIterator::copy (AxisIterator const& other)
{
  block_p = other.block_p;
}

void AxisIterator::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

void AxisIterator::setBlock (const int& block)
{
  // guard against negative offset in file stream
  if (block<1) {
    block_p = 1;
  } else {
    block_p = block;
  }
  // adjust parameters of the base class
  AxisCoordinates::setOffset ((block_p-1)*AxisCoordinates::blocksize());
}


// ==============================================================================
//
//  Methods
//
// ==============================================================================

}  // Namespace LOPES -- end
