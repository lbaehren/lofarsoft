/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/* $Id: template-class.cc,v 1.11 2007/03/06 14:53:26 bahren Exp $*/

#include <Math/RotationMatrix.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  RotationMatrix::RotationMatrix ()
  {
    if (!setRank (1)) {
      std::cerr << "[RotationMatrix::RotationMatrix] Failed assigning valid rank!"
		<< std::endl;
    }

    angles_p.resize(1,0.0);
  }
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  vector<double> const &angles)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles);
    }
  }
  
  RotationMatrix::RotationMatrix (RotationMatrix const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  RotationMatrix::~RotationMatrix ()
  {
    destroy();
  }
  
  void RotationMatrix::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  RotationMatrix& RotationMatrix::operator= (RotationMatrix const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void RotationMatrix::copy (RotationMatrix const &other)
  {
    angles_p.resize(other.angles_p.size());

    rank_p      = other.rank_p;
    nofAngles_p = other.nofAngles_p;
    angles_p    = other.angles_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  bool RotationMatrix::setRank (unsigned int const &rank)
  {
    bool status (true);

    rank_p = rank;

    if (rank == 0) {
      std::cerr << "[RotationMatrix::setRank] Invalid rank 0!" << std::endl;
      status = false;
    }
    else if (rank == 1) {
      nofAngles_p = 0;
    } 
    else if (rank == 2) {
      nofAngles_p = 1;
    }
    else if (rank == 3) {
      nofAngles_p = 3;
    }
    else {
      std::cerr << "[RotationMatrix::setRank] Unsupported rank!" << std::endl;
      status = false;
    }

    return status;
  }

  bool RotationMatrix::setAngles (vector<double> const &angles)
  {
    bool status (true);
    unsigned int nelem = angles.size();
    unsigned int n (0);

    if (nelem == 1) {
      angles_p.resize(nofAngles_p);
      for (n=0; n<nofAngles_p; n++) {
	angles_p[n] = angles[0];
      }
    } else if (nelem == nofAngles_p) {
      angles_p.resize(nofAngles_p);
      angles_p = angles;
    } else {
      std::cerr << "[RotationMatrix::setAngles] Invalid number of rotation angles!"
		<< std::endl;
      status = false;
    }

    return status;
  }
  
  void RotationMatrix::summary (std::ostream &os)
  {
    os << "-- Rank        = " << rank_p      << std::endl;
    os << "-- nof. angles = " << nofAngles_p << std::endl;
    
    os << "-- Angles      = " << "[ ";
    for (unsigned int n(0); angles_p.size(); n++) {
      os << angles_p[n] << " ";
    }
    os << "]" << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  

} // Namespace CR -- end
