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
  \file pycr_Imaging.cc
  \ingroup CR
  \ingroup pycr
  \brief Python bindings for the classes in the CR-Tools module Imaging
*/

#include "pycr.h"

//_______________________________________________________________________________
//                                                                      GeomDelay

//! Bindings for the CR::GeomDelay class
void export_GeomDelay ()
{
  void (GeomDelay::*summary1)()                  = &GeomDelay::summary;
  void (GeomDelay::*summary2)(ostream &)         = &GeomDelay::summary;
  bool (GeomDelay::*bufferDelays1)() const       = &GeomDelay::bufferDelays;
  void (GeomDelay::*bufferDelays2)(bool const &) = &GeomDelay::bufferDelays;
  bool (GeomDelay::*farField1)() const           = &GeomDelay::farField;
  void (GeomDelay::*farField2)(bool const &)     = &GeomDelay::farField;
  
  bpl::class_<GeomDelay>("GeomDelay")
    .def(bpl::init<>())
    .def("summary", summary1,
	 "Provide a summary of the internal status.")
    .def("summary", summary2,
	 "Provide a summary of the internal status.")
    .def("bufferDelays", bufferDelays1,
	 "Are the values of the geometrical delays getting buffered?")
    .def("bufferDelays", bufferDelays2,
	 "Enable/Disable buffering of the values of the geometrical weights.")
    .def("farField", farField1,
	 "Are the geometrical delays computed for the far-field geometry?")
    .def("farField", farField2,
	 "Select computation of the delays for far-field geometry.")
    .def("nofAntPositions", &GeomDelay::nofAntPositions,
	 "Get the number of antenna positions for which to compute the delays.")
    .def("nofSkyPositions", &GeomDelay::nofSkyPositions,
	 "Get the number of sky positions for which to compute the delays.")
    ;  
}

//_______________________________________________________________________________
//                                                                      GeomPhase

//! Bindings for the CR::GeomPhase class
void export_GeomPhase ()
{
  void (GeomPhase::*summary1)()                  = &GeomPhase::summary;
  void (GeomPhase::*summary2)(ostream &)         = &GeomPhase::summary;
//   bool (GeomPhase::*bufferDelays1)()             = &GeomPhase::bufferDelays;
//   void (GeomPhase::*bufferDelays2)(bool const &) = &GeomPhase::bufferDelays;
//   bool (GeomPhase::*farField1)()                 = &GeomPhase::farField;
//   void (GeomPhase::*farField2)(bool const &)     = &GeomPhase::farField;
  
  bpl::class_<GeomPhase>("GeomPhase")
    .def(bpl::init<>())
    .def("summary", summary1,
	 "Provide a summary of the internal status.")
    .def("summary", summary2,
	 "Provide a summary of the internal status.")
//     .def("bufferDelays",bufferDelays1,
// 	 "Are the values of the geometrical delays getting buffered?")
//     .def("bufferDelays",bufferDelays2,
// 	 "Enable/Disable buffering of the values of the geometrical weights.")
//     .def("farField",farField1,
// 	 "Are the geometrical delays computed for the far-field geometry?")
//     .def("farField",farField2,
// 	 "Select computation of the delays for far-field geometry.")
    ;  
}
