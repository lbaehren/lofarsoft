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

#include <sstream>

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
}

TBBData::~TBBData ()
{
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

boost::python::list TBBData::python_time()
{
  boost::python::list lst;

  std::vector<uint> vec = time();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_sample_number()
{
  boost::python::list lst;

  std::vector<uint> vec = sample_number();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_data_length()
{
  boost::python::list lst;

  std::vector<uint> vec = data_length();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_sample_frequency_value()
{
  boost::python::list lst;

  std::vector<double> vec = sample_frequency_value();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_sample_frequency_unit()
{
  boost::python::list lst;

  std::vector< std::string > vec = sample_frequency_unit();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_sample_offset(int refAntenna)
{
  boost::python::list lst;

  std::vector<int> vec = sample_offset(static_cast<uint>(refAntenna));

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_alignment_offset(int refAntenna)
{
  boost::python::list lst;

  std::vector<int> vec = sample_offset(static_cast<uint>(refAntenna));

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(-1 * vec[i]);
  }

  return lst;
}

int TBBData::python_find_reference_antenna()
{
  // Reference antenna for alignment
  int refAntenna = 0;
  double current = 0.;

  // Get TIME for each antenna
  std::vector<uint> t = time();

  // Get SAMPLE_NUMBER for each antenna
  std::vector<uint> sn = sample_number();

  // Get FREQUENCY_VALUE for each antenna
  std::vector<double> f = sample_frequency_value();

  // Find antenna that starts getting data last and use it as reference
  double max = static_cast<double>(t[0])+(static_cast<double>(sn[0])/(f[0]*1.e6));

  for (uint i=1; i<t.size(); ++i)
  {
    current = static_cast<double>(t[i])+(static_cast<double>(sn[i])/(f[i]*1.e6));

    if (current > max)
    {
      refAntenna = i;
      max = current;
    }
  }

  return refAntenna;
}

boost::python::list TBBData::python_channelID()
{
  boost::python::list lst;

  std::vector<int> vec = channelID();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_nyquist_zone()
{
  boost::python::list lst;

  std::vector<uint> vec = nyquist_zone();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

std::string TBBData::python_antenna_set()
{
  DAL::CommonAttributes c = commonAttributes();
  return c.antennaSet(); 
}

std::string TBBData::python_summary()
{
  std::ostringstream os;

  summary(os);

  return os.str();
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
#define HFPP_PARDEF_0 (HNumber)(vec)()("Output vector, expected to be of length Na * Ns where Na is the number of selected antennas and Ns is the number of samples per antenna.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(start)()("Vector with start positions in samples, expected to have length Na where Na is the number of selected antennas.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nofSamples)()("Number of samples, Ns, to read.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (TBBData)(data)()("TBBData object to read from.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter, class IIter>
void HFPP_FUNC_NAME(const Iter vec_begin, const Iter vec_end, const IIter start_begin, const IIter start_end, const HInteger nofSamples, TBBData &data)
{
  // Sanity checks
  const uint N_vec = std::distance(vec_begin, vec_end);

  if (N_vec != nofSamples*data.nofSelectedDatasets())
  {
    throw PyCR::ValueError("Data array has wrong size.");
  }

  const uint N_start = std::distance(start_begin, start_end);

  if (N_start != data.nofSelectedDatasets())
  {
    throw PyCR::ValueError("Array with start samples has wrong size.");
  }

  // Get start numbers into casa vector
  IIter start_it = start_begin;

  casa::Vector<int> start(N_start);
  for (uint i=0; i<N_start; ++i)
  {
    start(i) = static_cast<int>(*start_it);
    ++start_it;
  }

  // Set shape of temp array
  casa::IPosition vec_shape(2);
  vec_shape(0) = nofSamples;
  vec_shape(1) = data.nofSelectedDatasets();

  // Create temporary casa matrix sharing memory with the output vector
  casa::Matrix<double> temp(vec_shape, &(*vec_begin), casa::SHARE);

  // Read data into matrix and hence to the output vector
  data.readData(temp, start, nofSamples);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

