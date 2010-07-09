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
#include "pycr_RFI.h"

BOOST_PYTHON_MODULE (_pycr)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  //__________________________________________________________________
  //                                                    implement/RFI
  CR::PYCR::export_RFIMitigation ();
  
  //__________________________________________________________________
  //                                                      implement/IO
  
  CR::PYCR::export_DataReader ();

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
