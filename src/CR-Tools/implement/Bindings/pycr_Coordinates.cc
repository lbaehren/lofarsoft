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
  \file pycr_Coordinates.cc
  \ingroup CR
  \brief Python bindings for the classes in the CR-Tools module Coordinates
*/

#include "pycr.h"

//_______________________________________________________________________________
//                                                                 CoordinateType

void export_CoordinateType ()
{
  bpl::class_<CoordinateType>("CoordinateType")
    .def(bpl::init<>())
    .def(bpl::init<CoordinateType::Types>()) 
    .def("className", &CoordinateType::className)
    .def("type", &CoordinateType::type)
    .def("name", &CoordinateType::name)
    .def("getName", &CoordinateType::getName)
    ;  
}

//_______________________________________________________________________________
//                                                                       TimeFreq

void export_TimeFreq ()
{
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
    ;
}
