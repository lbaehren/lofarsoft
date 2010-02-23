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
  \ingroup pycr
  \brief Python bindings for the classes in the CR-Tools module Coordinates
*/

#include "pycr.h"

//_______________________________________________________________________________
//                                                                 CoordinateType

//! Bindings for the CR::CoordinateType class
void export_CoordinateType ()
{
  /* Enumeration: Coordinate type */
  bpl::enum_<CoordinateType::Types>("Types")
    .value("AzElHeight",CoordinateType::AzElHeight)
    .value("AzElRadius",CoordinateType::AzElRadius)
    .value("Cartesian",CoordinateType::Cartesian)
    .value("Cylindrical",CoordinateType::Cylindrical)
    .value("Direction",CoordinateType::Direction)
    .value("DirectionRadius",CoordinateType::DirectionRadius)
    .value("Frequency",CoordinateType::Frequency)
    .value("LongLatRadius",CoordinateType::LongLatRadius)
    .value("NorthEastHeight",CoordinateType::NorthEastHeight)
    .value("Spherical",CoordinateType::Spherical)
    .value("Time",CoordinateType::Time)
    ;

  /* Overloaded methods */
//    bool (CoordinateType::*hasProjection_public)(void) 
//      = &CoordinateType::hasProjection;
//    bool (CoordinateType::*hasProjection_static)(CoordinateType::Types const &)
//      = &CoordinateType::hasProjection;
//    bool (CoordinateType::*isCartesian_public)(void)
//      = &CoordinateType::isCartesian;
//    bool (CoordinateType::*isCartesian_static)(CoordinateType::Types const &)
//      = &CoordinateType::isCartesian;
//    bool (CoordinateType::*isSpherical_public)(void)
//      = &CoordinateType::isSpherical;
//    bool (CoordinateType::*isSpherical_static)(CoordinateType::Types const &)
//      = &CoordinateType::isSpherical;
//    bool (CoordinateType::*isSpatial_public)(void)
//      = &CoordinateType::isSpatial;
//    bool (CoordinateType::*isSpatial_static)(CoordinateType::Types const &)
//      = &CoordinateType::isSpatial;

  bpl::class_<CoordinateType>("CoordinateType")
    /* Construction */
    .def(bpl::init<>())
    .def(bpl::init<CoordinateType::Types>()) 
    /* Methods */
    .def("className", &CoordinateType::className,
	 "Get the name of the class.")
    .def("type", &CoordinateType::type,
	 "Get the coordinate type.")
    .def("setType", &CoordinateType::setType,
	 "Set the coordinate type.")
    .def("name", &CoordinateType::name,
	 "Get the name of the coordinate type.")
    .def("getName", &CoordinateType::getName,
	 "Get the name of the coordinate type.")
//     .def("hasProjection", hasProjection_public,
// 	 "Does this coordinate type contain a spherical map projection?")
//     .def("isCartesian", isCartesian_public,
// 	 "Is the coordinate of type Cartesian?")
//     .def("isSpherical", isSpherical_public,
// 	 "Is the coordinate of type Spherical?")
    /* Static methods */
//     .def("hasProjection", hasProjection_static,
// 	 "Does this coordinate type contain a spherical map projection?")
//     .def("isCartesian", isCartesian_static,
// 	 "Is the coordinate of type Cartesian?")
//     .def("isSpherical", isSpherical_static,
// 	 "Is the coordinate of type Spherical?")
    ;  
}

//_______________________________________________________________________________
//                                                                 SkymapQuantity

//! Bindings for the CR::SkymapQuantity class
void export_SkymapQuantity ()
{
  /* Enumeration: Coordinate type */
  bpl::enum_<SkymapQuantity::Type>("Type")
    .value("FREQ_FIELD",SkymapQuantity::FREQ_FIELD)
    .value("FREQ_POWER",SkymapQuantity::FREQ_POWER)
    .value("TIME_FIELD",SkymapQuantity::TIME_FIELD)
    .value("TIME_POWER",SkymapQuantity::TIME_POWER)
    .value("TIME_CC",SkymapQuantity::TIME_CC)
    ;

  void (SkymapQuantity::*summary1)() 
    = &SkymapQuantity::summary;

  bpl::class_<SkymapQuantity>("SkymapQuantity")
    /* Construction */
    .def(bpl::init<>())
    .def(bpl::init<SkymapQuantity::Type const &>())
    .def(bpl::init<std::string const &, std::string const &>())
    /* Public methods */
    .def("className", &SkymapQuantity::className)
    .def("summary", summary1)
    .def("type", &SkymapQuantity::type,
	 "Get the SkymapQuantity type.")
    .def("name", &SkymapQuantity::name,
	 "Get the SkymapQuantity name.")
    .def("setSkymapQuantity", &SkymapQuantity::setSkymapQuantity,
	 "Set the SkymapQuantity type.")
    .def("coordinateType", &SkymapQuantity::coordinateType,
	 "Get the coordinate type of the skymap quantity.")
    .def("domainName", &SkymapQuantity::domainName,
	 "Get the domain in which the quantity is defined.")
    .def("domainType", &SkymapQuantity::domainType,
	 "Get the type of the domain in which the quantity is defined.")
    .def("quantity", &SkymapQuantity::quantity,
	 "Get the type of electrical quantity.")
    ;
}

//_______________________________________________________________________________
//                                                                       TimeFreq

//! Bindings for the CR::TimeFreq class
void export_TimeFreq ()
{
  void (TimeFreq::*setSampleFrequency_stl)(double const &,
					   std::string const &) 
    = &TimeFreq::setSampleFrequency;
  void (TimeFreq::*setReferenceTime_stl)(double const &) 
    = &TimeFreq::setReferenceTime;
  bool (TimeFreq::*timeValues_stl)(std::vector<double> &,
				   std::vector<uint> const &) 
    = &TimeFreq::timeValues;
  void (TimeFreq::*summary1)() 
    = &TimeFreq::summary;
  
  bpl::class_<TimeFreq>("TimeFreq")
    /* Construction */
    .def(bpl::init<>())
    .def(bpl::init<uint>()) 
    .def(bpl::init<uint,double,uint>())
    .def(bpl::init<uint,double,uint,double>())
    /* Public methods */
    .def("className", &TimeFreq::className)
    .def("summary", summary1)
    .def("blocksize", &TimeFreq::blocksize,
	 "Get the number of samples per block of data.")
    .def("setBlocksize", &TimeFreq::setBlocksize,
	 "Set the number of samples per block of data.")
    .def("setSampleFrequency", setSampleFrequency_stl,
	 "Set the sample frequency.")
    .def("nyquistZone", &TimeFreq::nyquistZone,
	 "Get the Nyquist zone in which the data where sampled")
    .def("setNyquistZone", &TimeFreq::setNyquistZone,
	 "Set the Nyquist zone in which the data where sampled")
    .def("referenceTime", &TimeFreq::referenceTime,
	 "Get the reference time for the time axis")
    .def("setReferenceTime", setReferenceTime_stl,
	 "Set the reference time for the time axis")
    .def("nextBlock", &TimeFreq::nextBlock,
	 "Increment the sample offset by one block.")
    .def("fftLength", &TimeFreq::fftLength,
	 "Get the output length of the FFT.")
    .def("timeValues", timeValues_stl,
	 "Get the values along the time axis.")
    ;
}
