/**************************************************************************
 *  CR Pipeline Python bindings.                                          *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

// ========================================================================
//
//  Definitions
//
// ========================================================================


//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mBindings.def.h
//-----------------------------------------------------------------------


// ========================================================================
//
//  Header file
//
// ========================================================================

#include "core.h"
#include "mArray.h"
#include "mVector.h"
#include "mMath.h"
#include "mFFT.h"
#include "mFilter.h"
#include "mFitting.h"
#include "mImaging.h"
#include "mIO.h"
#include "mRF.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

//!initialize functions of the library
void hInit(){
  PyCR::Fitting::hInit();
}



// ========================================================================
//
//  Python bindings
//
// ========================================================================


// Tell the preprocessor (for generating wrappers) that this is a c++
// header file for Python exposure
#undef HFPP_FILETYPE
#define HFPP_FILETYPE hPYTHON

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hArray_setDimensions_overloads,setDimensions<HNumber>,1,2)

BOOST_PYTHON_MODULE(hftools)
{
  using namespace boost::python;

// ________________________________________________________________________
//                                                       Core functionality

  class_<std::vector<HInteger> >("IntVec")
    .def(vector_indexing_suite<std::vector<HInteger> >())
    ;

  class_<std::vector<HNumber> >("FloatVec")
    .def(vector_indexing_suite<std::vector<HNumber> >())
    ;

  class_<std::vector<bool> >("BoolVec")
    .def(vector_indexing_suite<std::vector<bool> >())
    ;

  class_<std::vector<HComplex> >("ComplexVec")
    .def(vector_indexing_suite<std::vector<HComplex> >())
    ;

  class_<std::vector<HString> >("StringVec")
    .def(vector_indexing_suite<std::vector<HString> >())
    ;

  enum_<DATATYPE>("TYPE")
    .value("POINTER",POINTER)
    .value("INTEGER",INTEGER)
    .value("NUMBER", NUMBER)
    .value("COMPLEX",COMPLEX)
    .value("STRING",STRING)
    .value("BOOLEAN",BOOLEAN)
    .value("UNDEF",UNDEF);

  def("hInit",hInit);

// ________________________________________________________________________
//                                                                    Array

  HFPP_hARRAY_BOOST_PYTHON_WRAPPER(HNumber,FloatArray);
  HFPP_hARRAY_BOOST_PYTHON_WRAPPER(HInteger,IntArray);
  HFPP_hARRAY_BOOST_PYTHON_WRAPPER(HComplex,ComplexArray);
  HFPP_hARRAY_BOOST_PYTHON_WRAPPER(HString,StringArray);
  HFPP_hARRAY_BOOST_PYTHON_WRAPPER(HBool,BoolArray);

  def("trackHistory",&hArray_trackHistory);

#include "../../../../build/cr/implement/Pypeline/mArray.def.h"

// ________________________________________________________________________
//                                                                   Vector

#include "../../../../build/cr/implement/Pypeline/mVector.def.h"

// ________________________________________________________________________
//                                                                     Math

  enum_<hWEIGHTS>("hWEIGHTS")
    .value("FLAT",WEIGHTS_FLAT)
    .value("LINEAR",WEIGHTS_LINEAR)
    .value("GAUSSIAN", WEIGHTS_GAUSSIAN);


#include "../../../../build/cr/implement/Pypeline/mMath.def.h"

// ________________________________________________________________________
//                                                                      FFT

#include "../../../../build/cr/implement/Pypeline/mFFT.def.h"

// ________________________________________________________________________
//                                                                   Filter

#include "../../../../build/cr/implement/Pypeline/mFilter.def.h"

// ________________________________________________________________________
//                                                                  Fitting

#include "../../../../build/cr/implement/Pypeline/mFitting.def.h"

// ________________________________________________________________________
//                                                                  Imaging


  enum_<CR::CoordinateType::Types>("CoordinateTypes")
    //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
    .value("AzElHeight",CR::CoordinateType::AzElHeight)
    //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
    .value("AzElRadius",CR::CoordinateType::AzElRadius)
    //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    .value("Cartesian",CR::CoordinateType::Cartesian)
    //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
    .value("Cylindrical",CR::CoordinateType::Cylindrical)
    //! Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
    .value("Direction",CR::CoordinateType::Direction)
    //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
    .value("DirectionRadius",CR::CoordinateType::DirectionRadius)
    //! Frquency
    .value("Frequency",CR::CoordinateType::Frequency)
    //! Longitude-Latitude-Radius
    .value("LongLatRadius",CR::CoordinateType::LongLatRadius)
    //! North-East-Height
    .value("NorthEastHeight",CR::CoordinateType::NorthEastHeight)
    //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
    .value("Spherical",CR::CoordinateType::Spherical)
    //! Time
    .value("Time",CR::CoordinateType::Time);

#include "../../../../build/cr/implement/Pypeline/mImaging.def.h"

// ________________________________________________________________________
//                                                                       IO

  def("hgetFiletype",hgetFiletype);
  def("hgetFileExtension",hgetFileExtension);

#include "../../../../build/cr/implement/Pypeline/mIO.def.h"

  class_<CR::DataReader>("DataReader")
    .def("get",&hFileGetParameter)
    .def("summary",&hFileSummary)
    ;


// ________________________________________________________________________
//                                                                       RF

#include "../../../../build/cr/implement/Pypeline/mRF.def.h"

}

#undef HFPP_FILETYPE
