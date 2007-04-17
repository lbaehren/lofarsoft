/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: DataIterator.cc,v 1.13 2007/03/13 22:48:33 bahren Exp $*/

#include <iostream>
#include <complex>
#include <IO/DataIterator.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------- DataIterator::DataIterator
  
  DataIterator::DataIterator ()
  {
    stepWidth_p = 1;
    //
    dataStart_p = 0;
    stride_p    = 0;
    shift_p     = 0;
    blocksize_p = 1;
    fftLength_p = 1;
    block_p     = 1;
    // 
    setPosition();
  }
  
  // ------------------------------------------------- DataIterator::DataIterator
  
  DataIterator::DataIterator (unsigned int const &blocksize)
  {
    stepWidth_p = 1;
    //
    dataStart_p = 0;
    stride_p    = 0;
    shift_p     = 0;
    blocksize_p = blocksize;
    block_p     = 1;
    // 
    setPosition();
  }
  
  // ------------------------------------------------- DataIterator::DataIterator
  
  DataIterator::DataIterator (unsigned int const &blocksize,
			      unsigned int const &dataStart)
  {
    stepWidth_p = 1;
    //
    dataStart_p = dataStart;
    stride_p    = 0;
    shift_p     = 0;
    blocksize_p = blocksize;
    block_p     = 1;
    // 
    setPosition();
  }
  
  // --------------------------------------------------- DataIterator::DataIterator
  
  DataIterator::DataIterator (unsigned int const &blocksize,
			      unsigned int const &dataStart,
			      long int const &stride,
			      unsigned int const &shift)
  {
    stepWidth_p = 1;
    //
    dataStart_p = dataStart;
    stride_p    = stride;
    shift_p     = shift;
    blocksize_p = blocksize;
    block_p     = 1;
    // 
    setPosition();
  }
  
  DataIterator::DataIterator (DataIterator const &other)
  {
    copy (other);
  }
  
  
  // ==============================================================================
  //
  //  Destruction
  //
  // ==============================================================================
  
  // -------------------------------------------------- DataIterator::~DataIterator
  
  DataIterator::~DataIterator ()
  {
    destroy ();
  }
  
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  // ----------------------------------------------------------- DataIterator::copy
  
  void DataIterator::copy (DataIterator const& other)
  {
    dataStart_p = other.dataStart_p;
    //
    blocksize_p = other.blocksize_p;
    fftLength_p = other.fftLength_p;
    stride_p    = other.stride_p;
    shift_p     = other.shift_p;
    block_p     = other.block_p;
    //
    stepWidth_p = other.stepWidth_p;
    //
    position_p = other.position_p;
  }
  
  // -------------------------------------------------------- DataIterator::destroy
  
  void DataIterator::destroy(void)
  {;}
  
  // ------------------------------------------------------ DataIterator::operator=
  
  DataIterator& DataIterator::operator= (DataIterator const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ==============================================================================
  //
  //  Member data
  //
  // ==============================================================================
  
  // --------------------------------------------------- DataIterator::setStepWidth
  
  template <class T> void DataIterator::setStepWidth (T const &var)
  {
    stepWidth_p = sizeof (var);
    setPosition ();
  }
  
  // ------------------------------------------- DataIterator::setStepWidthToSample
  
  void DataIterator::setStepWidthToSample ()
  {
    stepWidth_p = 1;
    setPosition ();
  }
  
  // ---------------------------------------------------- DataIterator::setPosition
  
  void DataIterator::setPosition ()
  {
    position_p = dataStart_p + shift_p;
    position_p += (block_p - 1)*(blocksize_p+stride_p)*stepWidth_p;
  }
  
  // -------------------------------------------------------- DataIterator::setBlock
  
  void DataIterator::setBlock (unsigned int const &block)
  {
    if (block>0) {
      block_p = block;
    } else {
      block_p = 1;
      std::cerr << "[DataIterator::setBlock] Invalid input for block number: "
		<< block << ". Fall-back to block 1." << std::endl;
    }
    //
    setPosition ();
  }
  
  // ------------------------------------------------------ DataIterator::nextBlock
  
  void DataIterator::nextBlock ()
  {
    block_p += 1;
    setPosition ();
  }
  
  // ------------------------------------------------------ DataIterator::setStride
  
  void DataIterator::setStride (long int const &stride)
  {
    stride_p = stride;
    setPosition ();
  }
  
  // ------------------------------------------------------- DataIterator::setShift
  
  void DataIterator::setShift (unsigned int const &shift)
  {
    shift_p = shift;
    setPosition ();
  }
  
  void DataIterator::setShiftInSamples (unsigned int const &shift)
  {
    setShift (shift*stepWidth_p);
  }
  
  void DataIterator::setShiftInBlocks (unsigned int const &shift)
  {
    setShiftInSamples (shift*blocksize_p);
  }
  
  // --------------------------------------------------- DataIterator::setDataStart
  
  void DataIterator::setDataStart (unsigned int const &dataStart)
  {
    dataStart_p = dataStart;
    setPosition ();
  }
  
  // --------------------------------------------------- DataIterator::setBlocksize
  
  void DataIterator::setBlocksize (unsigned int const &blocksize)
  {
    blocksize_p = blocksize;
    //
    fftLength_p = (unsigned int)(blocksize/2.0)+1;
    //
    setPosition ();
  }
  
  void DataIterator::summary ()
  {
    std::cout << "-- nof. samples per block      = " << blocksize_p << std::endl;
    std::cout << "-- FFT output lenght           = " << fftLength_p << std::endl;
    std::cout << "-- Start of the data segment   = " << dataStart_p << std::endl;
    std::cout << "-- Optional short w.r.t. start = " << shift_p     << std::endl;
    std::cout << "-- Stride between blocks       = " << stride_p << std::endl;
    std::cout << "-- Current data block number   = " << block_p << std::endl;
    std::cout << "-- Stepwidth                   = " << stepWidth_p << std::endl;
    std::cout << "-- Current position            = " << position_p << std::endl;
  }
  
  // ============================================================================
  //
  //  Quantities derived from basic internal parameters
  //
  // ============================================================================
  
  // ------------------------------------------------- DataIterator::maxNofBlocks
  
  unsigned int DataIterator::maxNofBlocks (unsigned int const &filesize)
  {
    double maxBlocks (1.0);
    
    maxBlocks *= (filesize - dataStart_p - shift_p);
    maxBlocks /= ((blocksize_p+stride_p)*stepWidth_p);
    maxBlocks += 1.0;
    
    return (unsigned int)(maxBlocks);
  }
  
  
  // ==============================================================================
  //
  //  Template instantiation: uncomment lines below if compiling code outside
  //                          the AIPS++/CASA build environment
  //
  // ==============================================================================
  
  template void DataIterator::setStepWidth (bool const &var);
  template void DataIterator::setStepWidth (short const &var);
  template void DataIterator::setStepWidth (int const &var);
  template void DataIterator::setStepWidth (uint const &var);
  template void DataIterator::setStepWidth (long const &var);
  template void DataIterator::setStepWidth (float const &var);
  template void DataIterator::setStepWidth (double const &var);
  
}  // Namespace CR -- end
