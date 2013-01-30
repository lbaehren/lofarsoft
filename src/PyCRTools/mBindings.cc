/**************************************************************************
 *  CR Pipeline Python bindings.                                          *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
 *  Heino Falcke <h.falcke@astro.ru.nl>                                   *
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

#include <boost/python/numeric.hpp>

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"
#include "numpy_ptr.h"

#include "core.h"
#include "mArray.h"
#include "mVector.h"
#include "mMath.h"
#include "mFFT.h"
#include "mFFTW.h"
#include "mFilter.h"
#include "mFitting.h"
#include "mIO.h"
#include "mRF.h"
#include "mCalibration.h"
#include "mEndPointRadiation.h"
#include "mTBB.h"
#include "legacy.h"
#include "mImaging.h"

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
  hInitFitting();
}

// ========================================================================
//
//  Exception translations
//
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  // Exception translators
  void translateException(Exception const& e)
  {
    PyErr_SetString(PyExc_Exception, e.message.c_str());
  }

  void translateValueError(ValueError const& e)
  {
    PyErr_SetString(PyExc_ValueError, e.message.c_str());
  }

  void translateTypeError(TypeError const& e)
  {
    PyErr_SetString(PyExc_TypeError, e.message.c_str());
  }

  void translateKeyError(KeyError const& e)
  {
    PyErr_SetString(PyExc_KeyError, e.message.c_str());
  }

  void translateMemoryError(MemoryError const& e)
  {
    PyErr_SetString(PyExc_MemoryError, e.message.c_str());
  }

  void translateArithmeticError(ArithmeticError const& e)
  {
    PyErr_SetString(PyExc_ArithmeticError, e.message.c_str());
  }

  void translateEOFError(EOFError const& e)
  {
    PyErr_SetString(PyExc_EOFError, e.message.c_str());
  }

  void translateFloatingPointError(FloatingPointError const& e)
  {
    PyErr_SetString(PyExc_FloatingPointError, e.message.c_str());
  }

  void translateOverflowError(OverflowError const& e)
  {
    PyErr_SetString(PyExc_OverflowError, e.message.c_str());
  }

  void translateZeroDivisionError(ZeroDivisionError const& e)
  {
    PyErr_SetString(PyExc_ZeroDivisionError, e.message.c_str());
  }

  void translateNameError(NameError const& e)
  {
    PyErr_SetString(PyExc_NameError, e.message.c_str());
  }

  void translateNotImplementedError(NotImplementedError const& e)
  {
    PyErr_SetString(PyExc_NotImplementedError, e.message.c_str());
  }

  void translateIOError(IOError const& e)
  {
    PyErr_SetString(PyExc_IOError, e.message.c_str());
  }

#ifdef HAS_EXCEPTION_HANDLING
  void translateDAL1IOError(DAL1::IOError const& e)
  {
    PyErr_SetString(PyExc_IOError, e.message.c_str());
  }
#endif

  bool multicore()
  {
#ifdef _OPENMP
    return true;
#else
    return false;
#endif // _OPENMP
  }
} // Namespace PyCR -- end

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

BOOST_PYTHON_MODULE(_hftools)
{
   import_array();
   boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

   using namespace boost::python;

   docstring_options doc_options;
   doc_options.enable_user_defined();
   doc_options.disable_py_signatures();
   doc_options.disable_cpp_signatures();

// ________________________________________________________________________
//                                                    Exception translation

  register_exception_translator<PyCR::Exception>(PyCR::translateException);
  register_exception_translator<PyCR::ValueError>(PyCR::translateValueError);
  register_exception_translator<PyCR::TypeError>(PyCR::translateTypeError);
  register_exception_translator<PyCR::KeyError>(PyCR::translateKeyError);
  register_exception_translator<PyCR::MemoryError>(PyCR::translateMemoryError);
  register_exception_translator<PyCR::ArithmeticError>(PyCR::translateArithmeticError);
  register_exception_translator<PyCR::EOFError>(PyCR::translateEOFError);
  register_exception_translator<PyCR::FloatingPointError>(PyCR::translateFloatingPointError);
  register_exception_translator<PyCR::OverflowError>(PyCR::translateOverflowError);
  register_exception_translator<PyCR::ZeroDivisionError>(PyCR::translateZeroDivisionError);
  register_exception_translator<PyCR::NameError>(PyCR::translateNameError);
  register_exception_translator<PyCR::NotImplementedError>(PyCR::translateNotImplementedError);
  register_exception_translator<PyCR::IOError>(PyCR::translateIOError);
#ifdef HAS_EXCEPTION_HANDLING
  register_exception_translator<DAL1::IOError>(PyCR::translateDAL1IOError);
#endif

// ________________________________________________________________________
//                                                         State inspectors
  def("multicore", &PyCR::multicore);

// ________________________________________________________________________
//                                                       Core functionality

  class_<std::vector<HInteger> >("IntVec")
    .enable_pickling()
    .def(vector_indexing_suite<std::vector<HInteger> >())
    ;

  class_<std::vector<HNumber> >("FloatVec")
    .enable_pickling()
    .def(vector_indexing_suite<std::vector<HNumber> >())
    ;

  class_<std::vector<bool> >("BoolVec")
    .def(vector_indexing_suite<std::vector<bool> >())
    ;

  class_<std::vector<HComplex> >("ComplexVec")
    .enable_pickling()
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

  //#include "mArray.def.h"

// ________________________________________________________________________
//                                                                   Vector

#include "mVector.def.h"

  HString (*hWriteRawVectorHInteger)(std::vector<HInteger>&) = hWriteRawVector;
  HString (*hWriteRawVectorHNumber)(std::vector<HNumber>&) = hWriteRawVector;
  HString (*hWriteRawVectorHComplex)(std::vector<HComplex>&) = hWriteRawVector;

  def("hWriteRawVector", hWriteRawVectorHInteger);
  def("hWriteRawVector", hWriteRawVectorHNumber);
  def("hWriteRawVector", hWriteRawVectorHComplex);

  void (*hReadRawVectorHInteger)(std::vector<HInteger>&, HString) = hReadRawVector;
  void (*hReadRawVectorHNumber)(std::vector<HNumber>&, HString) = hReadRawVector;
  void (*hReadRawVectorHComplex)(std::vector<HComplex>&, HString) = hReadRawVector;

  def("hReadRawVector", hReadRawVectorHInteger);
  def("hReadRawVector", hReadRawVectorHNumber);
  def("hReadRawVector", hReadRawVectorHComplex);

// ________________________________________________________________________
//                                                                     Math

  enum_<hWEIGHTS>("hWEIGHTS")
    .value("FLAT",WEIGHTS_FLAT)
    .value("LINEAR",WEIGHTS_LINEAR)
    .value("GAUSSIAN", WEIGHTS_GAUSSIAN);


#include "mMath.def.h"

// ________________________________________________________________________
//                                                              Calibration

#include "mCalibration.def.h"

// ________________________________________________________________________
//                                                                      FFT

#include "mFFT.def.h"

// ________________________________________________________________________
//                                                                     FFTW

#include "mFFTW.def.h"

  class_<FFTWPlanManyDft>("FFTWPlanManyDft", init<int, int, int, int, int, int, enum fftw_sign, enum fftw_flags>())
    ;

  class_<FFTWPlanManyDftR2c>("FFTWPlanManyDftR2c", init<int, int, int, int, int, int, enum fftw_flags>())
    ;

  class_<FFTWPlanManyDftC2r>("FFTWPlanManyDftC2r", init<int, int, int, int, int, int, enum fftw_flags>())
    ;

  enum_<fftw_flags>("fftw_flags")
    .value("ESTIMATE", ESTIMATE)
    .value("MEASURE", MEASURE)
    .value("PATIENT", PATIENT)
    .value("EXHAUSTIVE", EXHAUSTIVE)
    ;

  enum_<fftw_sign>("fftw_sign")
    .value("FORWARD", FORWARD)
    .value("BACKWARD", BACKWARD)
    ;

// ________________________________________________________________________
//                                                                   Filter

#include "mFilter.def.h"

// ________________________________________________________________________
//                                                                  Fitting

#include "mFitting.def.h"

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

#include "mImaging.def.h"

// ________________________________________________________________________
//                                                                       IO

#include "mIO.def.h"

#ifdef PYCRTOOLS_WITH_AERA
  class_<AERA::Datareader>("AERADatareader")
    //    .def("open",&hAERAFileOpen)
    .def("close",&hAERAFileClose)
    .def("firstEvent", &hAERAFirstEvent)
    .def("prevEvent", &hAERAPrevEvent)
    .def("nextEvent", &hAERANextEvent)
    .def("lastEvent", &hAERALastEvent)
    .def("firstLocalStation", &hAERAFirstLocalStation)
    .def("prevLocalStation", &hAERAPrevLocalStation)
    .def("nextLocalStation", &hAERANextLocalStation)
    .def("lastLocalStation", &hAERALastLocalStation)
    .def("getAttribute", &hAERAGetAttribute)
    .def("fileSummary",&hAERAFileSummary)
    .def("eventSummary",&hAERAEventSummary)
    .def("localStationSummary",&hAERALocalStationSummary)
    ;
#endif /* PYCRTOOLS_WITH_AERA */

// ________________________________________________________________________
//                                                                       RF

#include "mRF.def.h"

// ________________________________________________________________________
//                                                        EndPointRadiation

#include "mEndPointRadiation.def.h"


// ________________________________________________________________________
//                                                                      TBB

#include "mTBB.def.h"

  class_<TBBData>("TBBData", init<std::string>())
    .def("version", &TBBData::version)
    .def("summary", &TBBData::python_summary)
    .def("filename", &TBBData::filename)
    .def("nofStationGroups", &TBBData::nofStationGroups)
    .def("nofDipoleDatasets", &TBBData::nofDipoleDatasets)
    .def("nofSelectedDatasets", &TBBData::nofSelectedDatasets)
    .def("selectAllDipoles", &TBBData::selectAllDipoles)
    .def("dipoleNames", &TBBData::python_dipoleNames)
    .def("selectedDipoles", &TBBData::python_selectedDipoles)
    .def("selectDipoles", &TBBData::python_selectDipoles)
    .def("selectAllDipoles", &TBBData::selectAllDipoles)
    .def("time", &TBBData::python_time)
    .def("sample_number", &TBBData::python_sample_number)
    .def("data_length", &TBBData::python_data_length)
#if TBB_TIMESERIES_VERSION > 0
    .def("dipole_calibration_delay", &TBBData::python_dipole_calibration_delay)
    .def("dipole_calibration_delay_unit", &TBBData::python_dipole_calibration_delay_unit)
#endif
    .def("cable_delay", &TBBData::python_cable_delay)
    .def("cable_delay_unit", &TBBData::python_cable_delay_unit)
    .def("sample_frequency_value", &TBBData::python_sample_frequency_value)
    .def("sample_frequency_unit", &TBBData::python_sample_frequency_unit)
    .def("sample_offset", &TBBData::python_sample_offset)
    .def("alignment_offset", &TBBData::python_alignment_offset)
    .def("antenna_position", &TBBData::python_antenna_position)
    .def("antenna_position_itrf", &TBBData::python_antenna_position_itrf)
    .def("alignment_reference_antenna", &TBBData::python_alignment_reference_antenna)
    .def("maximum_read_length", &TBBData::python_maximum_read_length)
    .def("channelID", &TBBData::python_channelID)
    .def("nyquist_zone", &TBBData::python_nyquist_zone)
    .def("antenna_set", &TBBData::python_antenna_set)
    .def("filetype", &TBBData::python_filetype)
    .def("filedate", &TBBData::python_filedate)
    .def("telescope", &TBBData::python_telescope)
    .def("observer", &TBBData::python_observer)
    .def("clockFrequency", &TBBData::python_clockFrequency)
    .def("clockFrequencyUnit", &TBBData::python_clockFrequencyUnit)
    .def("filterSelection", &TBBData::python_filterSelection)
    .def("target", &TBBData::python_target)
    .def("systemVersion", &TBBData::python_systemVersion)
    .def("systemVersion", &TBBData::python_systemVersion)
    .def("pipelineName", &TBBData::python_pipelineName)
    .def("pipelineVersion", &TBBData::python_pipelineVersion)
    .def("notes", &TBBData::python_notes)
    .def("projectID", &TBBData::python_projectID)
    .def("projectTitle", &TBBData::python_projectTitle)
    .def("projectPI", &TBBData::python_projectPI)
    .def("projectCoI", &TBBData::python_projectCoI)
    .def("projectContact", &TBBData::python_projectContact)
    .def("observationID", &TBBData::python_observationID)
    .def("startMJD", &TBBData::python_startMJD)
    .def("startTAI", &TBBData::python_startTAI)
    .def("startUTC", &TBBData::python_startUTC)
    .def("endMJD", &TBBData::python_endMJD)
    .def("endTAI", &TBBData::python_endTAI)
    .def("endUTC", &TBBData::python_endUTC)
    .def("nofStations", &TBBData::python_nofStations)
    .def("stationList", &TBBData::python_stationList)
    .def("frequencyMin", &TBBData::python_frequencyMin)
    .def("frequencyMax", &TBBData::python_frequencyMax)
    .def("frequencyCenter", &TBBData::python_frequencyCenter)
    .def("frequencyUnit", &TBBData::python_frequencyUnit)
    ;

}

#undef HFPP_FILETYPE
