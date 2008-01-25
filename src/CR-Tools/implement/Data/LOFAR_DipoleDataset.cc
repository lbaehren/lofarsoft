/*-------------------------------------------------------------------------*
 | $Id:: NewClass.cc 1159 2007-12-21 15:40:14Z baehren                   $ |
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

#include <Data/LOFAR_DipoleDataset.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  LOFAR_DipoleDataset::LOFAR_DipoleDataset ()
  {
    H5fileID_p    = 0;
    H5datasetID_p = 0;
  }
  
  LOFAR_DipoleDataset::LOFAR_DipoleDataset (LOFAR_DipoleDataset const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LOFAR_DipoleDataset::~LOFAR_DipoleDataset ()
  {
    destroy();
  }
  
  void LOFAR_DipoleDataset::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  LOFAR_DipoleDataset& LOFAR_DipoleDataset::operator= (LOFAR_DipoleDataset const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void LOFAR_DipoleDataset::copy (LOFAR_DipoleDataset const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void LOFAR_DipoleDataset::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------- attribute_id
  
  hid_t LOFAR_DipoleDataset::attribute_id (std::string const &name)
  {
    hid_t id (0);
    
    /*
     * First check if the attribute actually exists
     */
    id = H5Aopen_name(H5datasetID_p,
		      name.c_str());

    
    return id;
  }

  // ----------------------------------------------------------------- station_id

  uint LOFAR_DipoleDataset::station_id ()
  {
    uint id (0);

    if (h5get_attribute(id,"STATION_ID",H5datasetID_p)) {
      return id;
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rsp_id

  uint LOFAR_DipoleDataset::rsp_id ()
  {
    uint id (0);

    if (h5get_attribute(id,"RSP_ID",H5datasetID_p)) {
      return id;
    } else {
      return 0;
    }
  }
  
  // --------------------------------------------------------------------- rcu_id

  uint LOFAR_DipoleDataset::rcu_id ()
  {
    uint val (0);

    if (h5get_attribute(val,"RCU_ID",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------------------- time

  uint LOFAR_DipoleDataset::time ()
  {
    uint val (0);

    if (h5get_attribute(val,"TIME",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }

  // ----------------------------------------------------------- sample_frequency
  
  double LOFAR_DipoleDataset::sample_frequency ()
  {
    double val (0);

    if (h5get_attribute(val,"SAMPLE_FREQ",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }

  // ----------------------------------------------------------------------- time

  uint LOFAR_DipoleDataset::nyquist_zone ()
  {
    uint val (0);

    if (h5get_attribute(val,"NYQUIST_ZONE",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // -------------------------------------------------------------- sample_number

  uint LOFAR_DipoleDataset::sample_number ()
  {
    uint val (0);

    if (h5get_attribute(val,"SAMPLE_NR",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------- samples_per_frame

  uint LOFAR_DipoleDataset::samples_per_frame ()
  {
    uint val (0);

    if (h5get_attribute(val,"SAMPLES_PER_FRAME",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ---------------------------------------------------------------- data_length

  uint LOFAR_DipoleDataset::data_length ()
  {
    uint val (0);

    if (h5get_attribute(val,"DATA_LENGTH",H5datasetID_p)) {
      return val;
    } else {
      return 0;
    }
  }
  
  // ----------------------------------------------------------------------- feed

  std::string LOFAR_DipoleDataset::feed ()
  {
    std::string val;

    if (h5get_attribute(val,"FEED",H5datasetID_p)) {
      return val;
    } else {
      return std::string ("");
    }
  }

} // Namespace CR -- end
