/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <Coordinates/AntennaArray.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                 AntennaArray
  
  AntennaArray::AntennaArray ()
  {
    init ("UNDEFINED",
	  casa::MEpoch(),
	  casa::MPosition());
  }
  
  //_____________________________________________________________________________
  //                                                                 AntennaArray
  
  AntennaArray::AntennaArray (std::string const &name,
			      casa::MEpoch const &epoch,
			      casa::MPosition const &location)
  {
    init (name,
	  epoch,
	  location);
  }
  
  //_____________________________________________________________________________
  //                                                                 AntennaArray
  
  AntennaArray::AntennaArray (AntennaArray const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  AntennaArray::~AntennaArray ()
  {
    destroy();
  }
  
  void AntennaArray::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  AntennaArray& AntennaArray::operator= (AntennaArray const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void AntennaArray::copy (AntennaArray const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void AntennaArray::summary (std::ostream &os)
  {
    os << "[AntennaArray] Summary of internal parameters." << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void AntennaArray::init (std::string const &name,
			   casa::MEpoch const &epoch,
			   casa::MPosition const &location)
  {
    setName (name);
    setEpoch(epoch);
    setLocation(location);
  }
  

} // Namespace CR -- end
