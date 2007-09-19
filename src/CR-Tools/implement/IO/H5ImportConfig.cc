/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (<mail>)                                                 *
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

#include <IO/H5ImportConfig.h>
#include <fstream>
#include <sstream>

using std::endl;

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // ------------------------------------------------------------- H5ImportConfig
  
  H5ImportConfig::H5ImportConfig ()
  {
    int rank (2);
    int *dimensions;

    dimensions = new int [rank];
    dimensions[0] = dimensions[1] = 1;

    init (rank,
	  dimensions);
  }

  // ------------------------------------------------------------- H5ImportConfig

  H5ImportConfig::H5ImportConfig (std::vector<int> const &shape)
  {
    int rank = shape.size();
    int *dimensions;

    dimensions = new int [rank];
    
    for (int n(0); n<rank; n++) {
      dimensions[n] = shape[n];
    }
    
    init (rank,
	  dimensions);
  }
  
  // ------------------------------------------------------------- H5ImportConfig

  H5ImportConfig::H5ImportConfig (int const &rank,
				  int const *dimensions)
  {
    init (rank,
	  dimensions);
  }
  
  // ------------------------------------------------------------- H5ImportConfig

  H5ImportConfig::H5ImportConfig (std::string const &configFile,
				  int const &rank,
				  int const *dimensions)
  {
    init (configFile,
	  rank,
	  dimensions);
  }
  
  // ------------------------------------------------------------- H5ImportConfig

  H5ImportConfig::H5ImportConfig (H5ImportConfig const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  H5ImportConfig::~H5ImportConfig ()
  {
    destroy();
  }
  
  void H5ImportConfig::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  H5ImportConfig& H5ImportConfig::operator= (H5ImportConfig const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void H5ImportConfig::copy (H5ImportConfig const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  // -------------------------------------------------------------------- summary

  void H5ImportConfig::summary (std::ostream &os)
  {
    os << " -- Configuration output = " << configFile_p         << endl;
    os << " -- RANK                 = " << rank_p               << endl;
    os << " -- DIMENSION-SIZES      = " << dimensionsAsString() << endl;
    os << " -- PATH                 = " << path_p               << endl;
    os << " -- INPUT-CLASS          = " << inputClass_p         << endl;
    os << " -- INPUT-SIZE           = " << inputSize_p          << endl;
    os << " -- OUTPUT-CLASS         = " << outputClass_p        << endl;
    os << " -- OUTPUT-SIZE          = " << outputSize_p         << endl;
    os << " -- OUTPUT-ARCHITECTURE  = " << outputArchitecture_p << endl;
    os << " -- OUTPUT-BYTE-ORDER    = " << outputByteOrder_p    << endl;
  }
  
  std::string H5ImportConfig::dimensionsAsString (bool const &fastestFirst)
  {
    std::ostringstream buffer;

    if (fastestFirst) {
      for (int n(rank_p-1); n>=0; n--) {
	buffer << dimensions_p[n] << " ";
      }
    } else {
      for (int n(0); n<rank_p; n++) {
	buffer << dimensions_p[n] << " ";
      }
    }

    return buffer.str();
  }
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ----------------------------------------------------------------------- init
  
  void H5ImportConfig::init (int const &rank,
			     int const *dimensions)
  {
    init ("h5import.config",
	  rank,
	  dimensions);
  }
    
  // ----------------------------------------------------------------------- init
  
  void H5ImportConfig::init (std::string const &outfile,
			     int const &rank,
			     int const *dimensions)
  {
    configFile_p = outfile;
    
    // [1] Rank of the dataset
    rank_p = rank;
    
    // [2] Dimensions of the dataset
    dimensions_p = new int [rank];
    for (int n(0); n<rank;n++) {
      dimensions_p[n] = dimensions[n];
    }
    
    // [3] Path of the output dataset
    path_p = "dataset1";
    
    // [4] Type of input data
    inputClass_p = "FP";
    
    // [5] Size of the input data
    inputSize_p = 32;
    
    // [6] Type of output data
    outputClass_p = "FP";
    
    // [7] Size of the output data
    outputSize_p = 32;

    // [8] Type of the output architecture
    outputArchitecture_p = "NATIVE";

    // [9] Output byte order
    outputByteOrder_p = "BE";
  }

  // ------------------------------------------------------------- exportSettings

  bool H5ImportConfig::exportSettings ()
  {
    return exportSettings (configFile_p,
			   false);
  }

  // ------------------------------------------------------------- exportSettings
  
  bool H5ImportConfig::exportSettings (std::string const &outfile,
				       bool const &storeFilename)
  {
    bool status (true);

    // process optional storage of filename
    if (storeFilename) {
      status = setConfigFile (outfile);
    }

    // [1] Open the file stream to which the configuration will be written
    std::ofstream os;
    os.open (outfile.c_str(),std::ios::out);
    // [2] Write out the configuration
    try {
      os << "RANK                 " << rank_p                   << endl;
      os << "DIMENSION-SIZES      " << dimensionsAsString(true) << endl;
      os << "PATH                 " << path_p                   << endl;
      os << "INPUT-CLASS          " << inputClass_p             << endl;
      os << "INPUT-SIZE           " << inputSize_p              << endl;
      os << "OUTPUT-CLASS         " << outputClass_p            << endl;
      os << "OUTPUT-SIZE          " << outputSize_p             << endl;
      os << "OUTPUT-ARCHITECTURE  " << outputArchitecture_p     << endl;
      os << "OUTPUT-BYTE-ORDER    " << outputByteOrder_p        << endl;
    } catch (std::string message) {
      std::cerr << "" << message << endl;
      status = false;
    }
    // [3] Close the output filestream
    os.close();

    return status;
  }

} // Namespace CR -- end
