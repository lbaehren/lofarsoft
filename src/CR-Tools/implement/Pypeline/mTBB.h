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

/* DAL header files */
#include <data_hl/TBB_Timeseries.h>

class TBBData : public DAL::TBB_Timeseries {

public:

  // === Construction ======================================================

  //! Argumented constructor
  TBBData (std::string const &filename);

  // === Destruction =======================================================

  //! Destructor
  ~TBBData ();

  // === Parameters access =================================================
  inline uint nofSelectedDatasets() { return selectedDatasets_p.size(); };

  // === Methods ===========================================================

  // === Python specific methods ===========================================
  boost::python::list python_dipoleNames();

  bool python_selectDipoles(boost::python::list);

  boost::python::list python_time();

  boost::python::list python_sample_number();

  boost::python::list python_data_length();

  boost::python::list python_sample_frequency_value();

  boost::python::list python_sample_frequency_unit();

  boost::python::list python_sample_offset(int refAntenna);

  boost::python::list python_alignment_offset();

  boost::python::list python_channelID();

  boost::python::list python_nyquist_zone();

  std::string python_antenna_set();

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
#include "../../../../build/cr/implement/Pypeline/mTBB.def.h"

#endif /* CR_PIPELINE_TBB_H */

