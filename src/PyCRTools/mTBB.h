/**************************************************************************
 *  Header file of the TBB module for the CR Pipeline Python bindings.    *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  P.Schellart@astro.ru.nl <p.schellart@astro.ru.nl>                     *
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

#ifndef CR_PIPELINE_TBB_H
#define CR_PIPELINE_TBB_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

/* DAL1 header files */
#include <data_hl/TBB_Timeseries.h>

class TBBData : public DAL1::TBB_Timeseries {

private:
  int sample_number_correction();

public:

  // === Construction ======================================================

  //! Argumented constructor
  TBBData (std::string const &filename);

  // === Destruction =======================================================

  //! Destructor
  ~TBBData ();

  // === Parameters access =================================================

  // === Methods ===========================================================
  int version ();

  // === Python specific methods ===========================================
  boost::python::list python_dipoleNames();

  boost::python::list python_selectedDipoles();

  bool python_selectDipoles(boost::python::list);

  boost::python::list python_time();

  boost::python::list python_sample_number();

  boost::python::list python_data_length();

#if TBB_TIMESERIES_VERSION > 0
  boost::python::list python_dipole_calibration_delay();

  boost::python::list python_dipole_calibration_delay_unit();
#endif

  boost::python::list python_cable_delay();

  boost::python::list python_cable_delay_unit();

  boost::python::list python_sample_frequency_value();

  boost::python::list python_sample_frequency_unit();

  boost::python::list python_sample_offset(int refAntenna);

  boost::python::list python_alignment_offset(int refAntenna);

  boost::python::list python_antenna_position();

  boost::python::list python_antenna_position_itrf();

  uint python_maximum_read_length(int refAntenna);

  int python_alignment_reference_antenna();

  boost::python::list python_channelID();

  boost::python::list python_nyquist_zone();

  std::string python_antenna_set();

  std::string python_filetype();

  std::string python_filedate();

  std::string python_telescope();

  std::string python_observer();

  double python_clockFrequency();

  std::string python_clockFrequencyUnit();

  std::string python_filterSelection();

  std::string python_target();

  std::string python_systemVersion();

  std::string python_pipelineName();

  std::string python_pipelineVersion();

  std::string python_notes();

  std::string python_projectID();

  std::string python_projectTitle();

  std::string python_projectPI();

  std::string python_projectCoI();

  std::string python_projectContact();

  std::string python_observationID();

  std::string python_startMJD();

  std::string python_startTAI();

  std::string python_startUTC();

  std::string python_endMJD();

  std::string python_endTAI();

  std::string python_endUTC();

  int python_nofStations();

  boost::python::list python_stationList();

  double python_frequencyMin();

  double python_frequencyMax();

  double python_frequencyCenter();

  std::string python_frequencyUnit();

  std::string python_summary();

  // === Operators =========================================================
  friend std::ostream& operator<<(std::ostream& output, const TBBData& d);

};

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mTBB.def.h"

#endif /* CR_PIPELINE_TBB_H */

