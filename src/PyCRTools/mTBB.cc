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

#include <measures/Measures.h>
#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MCPosition.h>

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
  : DAL1::TBB_Timeseries (filename, DAL1::IO_Mode::ReadOnly)
{
}

TBBData::~TBBData ()
{
}

int TBBData::version ()
{
  int v = 0;

#ifdef TBB_TIMESERIES_VERSION
  v = TBB_TIMESERIES_VERSION;
#endif
  return v;
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

boost::python::list TBBData::python_selectedDipoles()
{
  boost::python::list lst;

  std::set<std::string> dipoles = selectedDipoles();

  for(std::set<std::string>::iterator it = dipoles.begin(); it != dipoles.end(); ++it)
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

int TBBData::sample_number_correction()
{
  int correction = 0;

  // Get clock frequency from common attributes
  DAL1::CommonAttributes c = commonAttributes();

  if (static_cast<int>(c.clockFrequency()) == 200)
  {
    switch (time()[0] % 2)
    {
      case 0:
        correction = -512;
        break;
      case 1:
        correction = 512;
        break;
    }
  }

  return correction;
}

boost::python::list TBBData::python_sample_number()
{
  const int correction = sample_number_correction();

  boost::python::list lst;

  std::vector<uint> vec = sample_number();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i] + correction);
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

#if TBB_TIMESERIES_VERSION > 0
boost::python::list TBBData::python_dipole_calibration_delay()
{
  boost::python::list lst;

  std::vector<double> vec = dipole_calibration_delay();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_dipole_calibration_delay_unit()
{
  boost::python::list lst;

  std::vector<std::string> vec = dipole_calibration_delay_unit();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}
#endif

boost::python::list TBBData::python_cable_delay()
{
  boost::python::list lst;

  std::vector<double> vec = cable_delay();

  for(uint i=0; i<vec.size(); ++i)
  {
    lst.append(vec[i]);
  }

  return lst;
}

boost::python::list TBBData::python_cable_delay_unit()
{
  boost::python::list lst;

  std::vector<std::string> vec = cable_delay_unit();

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

// WARNING Needed for incompatibility between sample_offset (negative) and
// readData which expects positive offset. This needs to be corrected.
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

boost::python::list TBBData::python_antenna_position()
{
  boost::python::list lst;

  // Get antenna positions
  casa::Vector<casa::MPosition> vec = antenna_position();
  casa::IPosition shape = vec.shape();

  for(int i=0; i<shape(0); ++i)
  {
    // Append x,y,z positions to list
    lst.append(static_cast<double>(vec(i).getValue()(0)));
    lst.append(static_cast<double>(vec(i).getValue()(1)));
    lst.append(static_cast<double>(vec(i).getValue()(2)));
  }

  return lst;
}

/**
 * \brief Returns Python list with antenna positions in ITRF automatically
 * converted from whatever frame is used in the file.
 */
boost::python::list TBBData::python_antenna_position_itrf()
{
  boost::python::list lst;

  // Get antenna positions
  casa::Vector<casa::MPosition> vec = antenna_position();
  casa::IPosition shape = vec.shape();

  for(int i=0; i<shape(0); ++i)
  {
    // Convert to ITRF
    casa::MPosition pos = casa::MPosition::Convert(vec(i), casa::MPosition::Ref(casa::MPosition::ITRF))();

    // Append x,y,z positions to list
    lst.append(static_cast<double>(pos.getValue()(0)));
    lst.append(static_cast<double>(pos.getValue()(1)));
    lst.append(static_cast<double>(pos.getValue()(2)));
  }

  return lst;
}

// WARNING Non trivial method needs to be migrated to parent class
uint TBBData::python_maximum_read_length(int refAntenna)
{
  std::vector<uint> length = data_length();
  std::vector<int> offset = sample_offset(refAntenna);

  uint maxLength = UINT_MAX;
  uint antenna_maxSample = 0;

  for (uint i = 0; i < length.size(); ++i) {
    antenna_maxSample = (uint)(length[i] + offset[i]);
    if (antenna_maxSample < maxLength) {
      maxLength = antenna_maxSample;
    }
  }

  return maxLength;
}

// WARNING Non trivial method needs to be migrated to parrent class
int TBBData::python_alignment_reference_antenna()
{
  // Store current antenna selection
  std::set<std::string> selection = selectedDipoles();

  // Select all dipoles to find overall best antenna for alignment
  selectAllDipoles();

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
    // WARNING this assumes frequency is in MHz this needs to be migrated
    // to parrent class taking units into account.
    current = static_cast<double>(t[i])+(static_cast<double>(sn[i])/(f[i]*1.e6));

    if (current > max)
    {
      refAntenna = i;
      max = current;
    }
  }

  // Restore antenna selection
  selectDipoles(selection);

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
  DAL1::CommonAttributes c = commonAttributes();
  return c.antennaSet();
}

std::string TBBData::python_filetype()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.filetype();
}

std::string TBBData::python_filedate()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.filedate();
}

std::string TBBData::python_telescope()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.telescope();
}

std::string TBBData::python_observer()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.observer();
}

double TBBData::python_clockFrequency()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.clockFrequency();
}

std::string TBBData::python_clockFrequencyUnit()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.clockFrequencyUnit();
}

std::string TBBData::python_filterSelection()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.filterSelection();
}

std::string TBBData::python_target()
{

  DAL1::CommonAttributes c = commonAttributes();
  return c.target();
}

std::string TBBData::python_systemVersion()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.systemVersion();
}

std::string TBBData::python_pipelineName()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.pipelineName();
}

std::string TBBData::python_pipelineVersion()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.pipelineVersion();
}

std::string TBBData::python_notes()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.notes();
}

std::string TBBData::python_projectID()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.projectID();
}

std::string TBBData::python_projectTitle()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.projectTitle();
}

std::string TBBData::python_projectPI()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.projectPI();
}

std::string TBBData::python_projectCoI()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.projectCoI();
}

std::string TBBData::python_projectContact()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.projectContact();
}

std::string TBBData::python_observationID()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.observationID();
}

std::string TBBData::python_startMJD()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.startMJD();
}

std::string TBBData::python_startTAI()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.startTAI();
}

std::string TBBData::python_startUTC()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.startUTC();
}

std::string TBBData::python_endMJD()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.endMJD();
}

std::string TBBData::python_endTAI()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.endTAI();
}

std::string TBBData::python_endUTC()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.endUTC();
}

int TBBData::python_nofStations()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.nofStations();
}

boost::python::list TBBData::python_stationList()
{
  boost::python::list lst;

  DAL1::CommonAttributes c = commonAttributes();
  std::vector<std::string> stations = c.stationsList();

  for(std::vector<std::string>::iterator it = stations.begin(); it != stations.end(); ++it)
  {
    lst.append(*it);
  }

  return lst;
}

double TBBData::python_frequencyMin()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.frequencyMin();
}

double TBBData::python_frequencyMax()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.frequencyMax();
}

double TBBData::python_frequencyCenter()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.frequencyCenter();
}

std::string TBBData::python_frequencyUnit()
{
  DAL1::CommonAttributes c = commonAttributes();
  return c.frequencyUnit();
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
#define HFPP_PARDEF_0 (HNumber)(vec)()("Output vector, expected to be of length :math:`N_a * N_s` where :math:`N_a` is the number of selected antennas and :math:`N_s` is the number of samples per antenna.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(start)()("Vector with start positions in samples, expected to have length :math:`N_a` where :math:`N_a` is the number of selected antennas.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nofSamples)()("Number of samples, :math:`N_s`, to read.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
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

