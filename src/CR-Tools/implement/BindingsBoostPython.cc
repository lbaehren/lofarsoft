/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <string>
#include <vector>

#include <crtools.h>

// Basic Python header
#include <Python.h>

// Boost.Python header files
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

#include <Coordinates/TimeFreq.h>
#include <IO/DataIterator.h>
#include <IO/DataReader.h>
#include <Math/HanningFilter.h>

namespace bpl = boost::python;

using CR::DataIterator;
using CR::DataReader;
using CR::HanningFilter;
using CR::TimeFreq;

BOOST_PYTHON_MODULE (pycr)
{
  
  // ============================================================================
  //
  //  implement/Analysis
  //
  // ============================================================================
  
  // ============================================================================
  //
  //  implement/Coordinates
  //
  // ============================================================================
  
  bpl::class_<TimeFreq>("TimeFreq")
    .def(bpl::init<>())
    .def(bpl::init<uint>()) 
    .def(bpl::init<uint,double,uint>())
    .def(bpl::init<uint,double,uint,double>())
    .def("className", &TimeFreq::className)
    .def("blocksize", &TimeFreq::blocksize,
	 "Get the number of samples per block of data")
    .def("setBlocksize", &TimeFreq::setBlocksize,
	 "Set the number of samples per block of data")
//     .def("sampleFrequency",
//  	 &TimeFreq::sampleFrequency<>(),
//  	 "Get the sample frequency in analog-to-digital conversion")
    .def("nyquistZone", &TimeFreq::nyquistZone,
	 "Get the Nyquist zone in which the data where sampled")
    .def("setNyquistZone", &TimeFreq::setNyquistZone,
	 "Set the Nyquist zone in which the data where sampled")
    .def("referenceTime", &TimeFreq::referenceTime,
	 "Get the reference time for the time axis")
    .def("setReferenceTime", &TimeFreq::setReferenceTime,
	 "Set the reference time for the time axis")
   ;
  
  // ============================================================================
  //
  //  implement/IO
  //
  // ============================================================================

  /* DataIterator */
  
  bpl::class_<DataIterator>("DataIterator")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def(bpl::init<unsigned int,unsigned int>())
    .def(bpl::init<unsigned int,long int,unsigned int>())
    .def(bpl::init<unsigned int,unsigned int,long int,unsigned int>())
    .def("stepWidth", &DataIterator::stepWidth)
//     .def("setStepWidth", &DataIterator::setStepWidth)
    .def("setStepWidthToSample", &DataIterator::setStepWidthToSample)
    .def("position", &DataIterator::position,
	 "Get the current position of the pointer")
    .def("dataStart", &DataIterator::dataStart,
	 "Get the start of the data segment")
    .def("setDataStart", &DataIterator::setDataStart,
	 "Set the start of the data segment")
    .def("blocksize", &DataIterator::blocksize,
	 "Get the number of samples per block of data")
    .def("setBlocksize", &DataIterator::setBlocksize,
	 "Set the number of samples per block of data")
    .def("fftLength", &DataIterator::fftLength)
    .def("stride", &DataIterator::stride,
	 "Get the stride between two subsequent blocks of data")
    .def("setStride", &DataIterator::setStride,
	 "Set the stride between two subsequent blocks of data")
    .def("shift", &DataIterator::shift)
    .def("setShift", &DataIterator::setShift)
    .def("setShiftInBlocks", &DataIterator::setShiftInBlocks)
    .def("setShiftInSamples", &DataIterator::setShiftInSamples)
    .def("block", &DataIterator::block)
    .def("setBlock", &DataIterator::setBlock)
    .def("nextBlock", &DataIterator::nextBlock)
    .def("maxNofBlocks", &DataIterator::maxNofBlocks)
    .def("summary", &DataIterator::summary)
    ;

  /* DataReader */

#ifndef __APPLE__

  void (DataReader::*setBlocksize1)(uint const &) = &DataReader::setBlocksize;
  void (DataReader::*setBlocksize2)(uint const &,
				    casa::Matrix<double> const &) = &DataReader::setBlocksize;
  void (DataReader::*setBlocksize3)(uint const &,
				    casa::Matrix<double> const &,
				    casa::Matrix<casa::DComplex> const &) = &DataReader::setBlocksize;
  
  bpl::class_<DataReader>("DataReader")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def(bpl::init<unsigned int,unsigned int,double>())
    .def(bpl::init<unsigned int,casa::Vector<double>,casa::Matrix<casa::DComplex> >())
    .def(bpl::init<unsigned int,casa::Matrix<double>,casa::Matrix<casa::DComplex> >())
    .def("setBlocksize", setBlocksize1)
    .def("setBlocksize", setBlocksize2)
    .def("setBlocksize", setBlocksize3)
    .def("nofStreams", &DataReader::nofStreams)
    .def("fftLength", &DataReader::fftLength)
    .def("nofAntennas", &DataReader::nofAntennas)
    .def("nofSelectedAntennas", &DataReader::nofSelectedAntennas)
    .def("block", &DataReader::block)
    .def("setBlock", &DataReader::setBlock)
    .def("setStartBlock", &DataReader::setStartBlock)
    .def("stride", &DataReader::stride)
    .def("setStride", &DataReader::setStride)
    .def("shift", &DataReader::shift)
    .def("setShift", &DataReader::setShift)
    .def("nextBlock", &DataReader::nextBlock)
    .def("toStartBlock", &DataReader::toStartBlock)
    ;

#endif
  
  // ============================================================================
  //
  //  implement/Data
  //
  // ============================================================================

//   bpl::class_<LOFAR_DipoleDataset>("LOFAR_DipoleDataset")
//     .def(bpl::init<>())
//     .def(bpl::init<std::string,std::string>())
//     ;

  // ============================================================================
  //
  //  implement/Math
  //
  // ============================================================================

  // ============================================================================
  //
  //  implement/Utilities
  //
  // ============================================================================
  
  // ============================================================================
  
}
