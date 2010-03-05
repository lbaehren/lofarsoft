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
  \file pycr_Filters.cc
  \ingroup CR
  \ingroup pycr
  \brief Python bindings for the classes in the CR-Tools module Filters
*/

#include "pycr.h"

//_______________________________________________________________________________
//                                                                    BasicFilter

//! Bindings for the CR::BasicFilter class
void export_BasicFilter ()
{
  typedef BasicFilter<int> BasicFilter_int;
  typedef BasicFilter<uint> BasicFilter_uint;
  typedef BasicFilter<short> BasicFilter_short;
  typedef BasicFilter<long> BasicFilter_long;
  typedef BasicFilter<float> BasicFilter_float;
  typedef BasicFilter<double> BasicFilter_double;

  bpl::class_<BasicFilter_int >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_int::blocksize)
    .def("weights", &BasicFilter_int::weights)
    .def("setWeights", &BasicFilter_int::setWeights)
     ;  

  bpl::class_<BasicFilter_uint >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_uint::blocksize)
    .def("weights", &BasicFilter_uint::weights)
    .def("setWeights", &BasicFilter_uint::setWeights)
     ;  

  bpl::class_<BasicFilter_short >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_short::blocksize)
    .def("weights", &BasicFilter_short::weights)
    .def("setWeights", &BasicFilter_short::setWeights)
     ;  

  bpl::class_<BasicFilter_long >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_long::blocksize)
    .def("weights", &BasicFilter_long::weights)
    .def("setWeights", &BasicFilter_long::setWeights)
     ;  

  bpl::class_<BasicFilter_float >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_float::blocksize)
    .def("weights", &BasicFilter_float::weights)
    .def("setWeights", &BasicFilter_float::setWeights)
     ;  

  bpl::class_<BasicFilter_double >("BasicFilter")
    .def(bpl::init<>())
    .def(bpl::init<unsigned int>())
    .def("blocksize", &BasicFilter_double::blocksize)
    .def("weights", &BasicFilter_double::weights)
    .def("setWeights", &BasicFilter_double::setWeights)
     ;  
}

//_______________________________________________________________________________
//                                                                  HanningFilter

//! Bindings for the CR::HanningFilter class
void export_HanningFilter ()
{
  typedef HanningFilter<int> HanningFilter_int;
  typedef HanningFilter<uint> HanningFilter_uint;
  typedef HanningFilter<short> HanningFilter_short;
  typedef HanningFilter<long> HanningFilter_long;
  typedef HanningFilter<float> HanningFilter_float;
  typedef HanningFilter<double> HanningFilter_double;

//   bpl::class_<HanningFilter<class T> >("HanningFilter")
//     .def(bpl::init<>())
//     .def(bpl::init<unsigned int>())
//     ;  
}
