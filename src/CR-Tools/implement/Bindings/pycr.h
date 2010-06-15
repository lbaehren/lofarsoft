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
  \file pycr.h
  \ingroup CR
  \ingroup pycr
  \brief Python bindings
*/

#include <string>
#include <vector>

// ==============================================================================
//
//  Header files
//
// ==============================================================================

//#### fix to work around a bug in the DAL ####
//#### needs to be done before DAL includes! ####
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#define NO_IMPORT_ARRAY

// CR-Tools header files
#include <crtools.h>
#include <Analysis/DynamicSpectrum.h>
#include <Coordinates/CoordinateType.h>
#include <Coordinates/SkymapQuantity.h>
#include <Coordinates/TimeFreq.h>
#include <Filters/BasicFilter.h>
#include <Filters/HanningFilter.h>
#include <Filters/RaisedCosineFilter.h>
#include <IO/LOFAR_TBB.h>
#include <IO/DataIterator.h>
#include <IO/DataReader.h>
#include <Imaging/GeomDelay.h>
#include <Imaging/GeomPhase.h>

// Basic Python header
#include <Python.h>

//________________________________________________
//                       Boost.Python header files

#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

namespace bpl = boost::python;

// ==============================================================================
//
//  Function prototypes & Namespace usage
//
// ==============================================================================

using CR::DynamicSpectrum;

// === Coordinates ===============================

using CR::CoordinateType;
using CR::SkymapQuantity;
using CR::TimeFreq;

void export_CoordinateType ();
void export_SkymapQuantity ();
void export_TimeFreq ();

// === Filters ===================================

using CR::BasicFilter;
using CR::HanningFilter;

void export_BasicFilter ();
void export_HanningFilter ();

// === IO ========================================

using CR::DataIterator;
using CR::DataReader;
using CR::LOFAR_TBB;

void export_LOFAR_TBB ();

// === Imaging ===================================

using CR::GeomDelay;
using CR::GeomPhase;

void export_GeomDelay ();
void export_GeomPhase ();
