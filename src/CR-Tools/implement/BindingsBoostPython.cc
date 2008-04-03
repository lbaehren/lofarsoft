/*-------------------------------------------------------------------------*
 | $Id:: LOFAR_TBB.h 1285 2008-03-06 11:03:39Z baehren                   $ |
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

#include <vector>

// Basic Python header
#include <Python.h>

// Boost.Python header files
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

#include <Coordinates/TimeFreq.h>
#include <IO/DataIterator.h>

namespace bpl = boost::python;

using CR::DataIterator;
using CR::TimeFreq;

BOOST_PYTHON_MODULE (pycr)
{
  
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
    .def("blocksize", &TimeFreq::blocksize)
    .def("setBlocksize", &TimeFreq::setBlocksize)
    .def("sampleFrequency", &TimeFreq::sampleFrequency)
    .def("nyquistZone", &TimeFreq::nyquistZone)
    .def("setNyquistZone", &TimeFreq::setNyquistZone)
   ;
  
  // ============================================================================
  //
  //  implement/IO
  //
  // ============================================================================
  
  bpl::class_<DataIterator>("DataIterator")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def(bpl::init<unsigned int,unsigned int>())
    ;
  
  // ============================================================================
  
}
