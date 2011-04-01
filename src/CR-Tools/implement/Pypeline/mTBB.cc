/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Pim Schellart <p.schellart@astro.ru.nl>                               *
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
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mTBB.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mTBB.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

//__________________________________________________________________________
//                                                                   TBBData

/*!
  \param filename -- Name of the file from which to read in the data
*/
TBBData::TBBData (std::string const &filename)
  : DAL::TBB_Timeseries (filename)
{
  init ();
}

TBBData::~TBBData ()
{
  destroy();
}

bool TBBData::init ()
{
  return true;
}

void TBBData::destroy ()
{
  ;
}

//__________________________________________________________________________
//                                                   Python access functions
boost::python::list TBBData::python_dipoleNames()
{
  boost::python::list lst;

  std::vector<std::string> names = dipoleNames();

  for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
  {
    lst.append(*it);
  }

  return lst;
}

bool TBBData::python_selectDipoles(boost::python::list names)
{
  std::set<std::string> selection;

  const int N = boost::python::extract<int>(names.attr("__len__")());

  for (int i=0; i<N; i++)
  {
    selection.insert(boost::python::extract<std::string>(names[i]));
  }
  
  return selectDipoles(selection);
}

double TBBData::python_clockFrequency()
{
  return commonAttributes().clockFrequency();
}

std::ostream& operator<<(std::ostream& output, const TBBData& d)
{
    output << "TBBData";

    return output;
}

// ========================================================================
//$SECTION: IO functions
// ========================================================================

//$DOCSTRING: Read data
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReadTimeseriesData
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(vec)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(start)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nofSamples)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (TBBData)(data)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter, class IIter>
void HFPP_FUNC_NAME(const Iter vec_begin, const Iter vec_end, const IIter start_begin, const IIter start_end, const HInteger nofSamples, TBBData &data)
{
//  std::cout<<data.dipoleNames()<<std::endl;
//
//  std::set< std::string > selection;
//
//  selection.insert("007000000");
//  selection.insert("007000001");
//
//  data.selectDipoles(selection);
//
//  std::cout<<data.selectedDipoles()<<std::endl;

  const int N_vec = std::distance(vec_begin, vec_end);

  if (N_vec != nofSamples*data.nofSelectedDatasets())
  {
    throw PyCR::ValueError("Data array has wrong size.");
  }

  const int N_start = std::distance(start_begin, start_end);

  if (N_start != data.nofSelectedDatasets())
  {
    throw PyCR::ValueError("Array with start samples has wrong size.");
  }

  IIter start_it = start_begin;

  casa::Vector<int> start(N_start);
  for (int i=0; i<N_start; ++i)
  {
    start(i) = static_cast<int>(*start_it);
    ++start_it;
  }

  casa::IPosition vec_shape(2);
  vec_shape(0) = nofSamples;
  vec_shape(1) = data.nofSelectedDatasets();

  casa::Matrix<double> temp(vec_shape, &(*vec_begin), casa::SHARE);

  data.readData(temp, start, nofSamples);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

