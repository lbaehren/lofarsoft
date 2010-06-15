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

/*!
  \file pycr.cc
  \ingroup CR
  \ingroup pycr
  \brief Python bindings
*/


//#### because of abug in the DAL this needs to be ####
//#########   done before #include "pycr.h    #########
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include <num_util.h>

#include "pycr.h"


#include "pycr_IO.h"

BOOST_PYTHON_MODULE (pycr)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  
  //__________________________________________________________________
  //                                                implement/Analysis


  //__________________________________________________________________
  //                                             implement/Coordinates

  export_CoordinateType ();
  export_SkymapQuantity ();
  export_TimeFreq ();
  
  //__________________________________________________________________
  //                                                 implement/Imaging
  
  export_GeomDelay ();
  export_GeomPhase ();
  
  //__________________________________________________________________
  //                                                    implement/Data

  CR::PYCR::export_DataReader ();
  
  //__________________________________________________________________
  //                                                      implement/IO
  
  //__________________________________________________________________
  //                                                      DataIterator
  
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

  //__________________________________________________________________
  //                                                        DataReader
  
//   void (DataReader::*setBlocksize1)(uint const &) = &DataReader::setBlocksize;
//   void (DataReader::*setBlocksize2)(uint const &,
// 				    casa::Matrix<double> const &) = &DataReader::setBlocksize;
//   void (DataReader::*setBlocksize3)(uint const &,
// 				    casa::Matrix<double> const &,
// 				    casa::Matrix<casa::DComplex> const &) = &DataReader::setBlocksize;
//   int (DataReader::*shift1)(unsigned int const &) = &DataReader::shift;
//   void (DataReader::*setShift1)(int const &) = &DataReader::setShift;
//   void (DataReader::*setShift2)(int const &,
// 				unsigned int const &) = &DataReader::setShift;
  
//   bpl::class_<DataReader>("DataReader")
//     .def(bpl::init<>())
//     .def(bpl::init<unsigned int>())
//     .def(bpl::init<unsigned int,unsigned int,double>())
//     .def(bpl::init<unsigned int,casa::Vector<double>,casa::Matrix<casa::DComplex> >())
//     .def(bpl::init<unsigned int,casa::Matrix<double>,casa::Matrix<casa::DComplex> >())
//     .def("setBlocksize", setBlocksize1)
//     .def("setBlocksize", setBlocksize2)
//     .def("setBlocksize", setBlocksize3)
//     .def("nofStreams", &DataReader::nofStreams)
//     .def("fftLength", &DataReader::fftLength)
//     .def("nofAntennas", &DataReader::nofAntennas)
//     .def("nofSelectedAntennas", &DataReader::nofSelectedAntennas)
//     .def("block", &DataReader::block)
//     .def("setBlock", &DataReader::setBlock)
//     .def("setStartBlock", &DataReader::setStartBlock)
//     .def("stride", &DataReader::stride)
//     .def("setStride", &DataReader::setStride)
//     .def("shift", shift1)
//     .def("setShift", setShift1)
//     .def("setShift", setShift2)
//     .def("nextBlock", &DataReader::nextBlock)
//     .def("toStartBlock", &DataReader::toStartBlock)
//     ;

  //__________________________________________________________________
  //                                               implement/Utilities

  bool (DynamicSpectrum::*setFilename1)(std::string const &)
    = &DynamicSpectrum::setFilename;

  bpl::class_<DynamicSpectrum>("DynamicSpectrum")
    .def(bpl::init<CR::LOFAR_TBB,int,int>())
    .def("setFilename", setFilename1)
    .def("toFITS",&DynamicSpectrum::toFITS)
    ;

}
