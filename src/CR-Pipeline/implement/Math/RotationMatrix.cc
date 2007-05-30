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

  RotationMatrix::RotationMatrix (unsigned int const &rank)
  {
    if (!setRank (rank)) {
      std::cerr << "[RotationMatrix::RotationMatrix] Failed assigning valid rank!"
		<< std::endl;
    }

    angles_p.resize(nofAngles_p,0.0);
  }

  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  vector<double> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles,anglesInDegree);
    }
  }
  
#ifdef HAVE_BLITZ
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  blitz::Array<double,1> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles);
    }
  }
  
#endif

#ifdef HAVE_CASA
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  casa::Vector<double> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles);
    }
  }

#endif
    
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

  // -------------------------------------------------------------------- setRank
  
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

  // ------------------------------------------------------------------ setAngles

  bool RotationMatrix::setAngles (vector<double> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    unsigned int nelem (angles.size());
    unsigned int n (0);
    vector<double> anglesInRadian (nelem);

    if (anglesInDegree) {
      anglesInRadian = deg2rad (angles);
    } else {
      anglesInRadian = angles;
    }

    if (nelem == 1) {
      angles_p.resize(nofAngles_p);
      for (n=0; n<nofAngles_p; n++) {
	angles_p[n] = anglesInRadian[0];
      }
    } else if (nelem == nofAngles_p) {
      angles_p.resize(nofAngles_p);
      angles_p = anglesInRadian;
    } else {
      std::cerr << "[RotationMatrix::setAngles] Invalid number of rotation angles!"
		<< std::endl;
      status = false;
    }

    return status;
  }
  
  // ------------------------------------------------------------------ setAngles

#ifdef HAVE_BLITZ
  
  bool RotationMatrix::setAngles (blitz::Array<double,1> const &angles,
				  bool const &anglesInDegree)
  {
    unsigned int nelem (angles.numElements());
    vector<double> rotationAngles (nelem);
    
    for (unsigned int n(0); n<nelem; n++) {
      rotationAngles[n] = angles(n);
    }
    
    return setAngles (rotationAngles,anglesInDegree);
  }
  
#endif

  // ------------------------------------------------------------------ setAngles
  
#ifdef HAVE_CASA
  
  bool RotationMatrix::setAngles (casa::Vector<double> const &angles,
				  bool const &anglesInDegree)
  {
    unsigned int nelem (angles.nelements());
    vector<double> rotationAngles (nelem);

    for (unsigned int n(0); n<nelem; n++) {
      rotationAngles[n] = angles(n);
    }

    return setAngles (rotationAngles,anglesInDegree);
  }

#endif

  // -------------------------------------------------------------------- summary

  void RotationMatrix::summary (std::ostream &os)
  {
    vector<double> rotationAngles (angles(true));

    os << "-- Rank         = " << rank_p      << std::endl;
    os << "-- nof. angles  = " << nofAngles_p << std::endl;
    
    os << "-- Angles (deg) = " << "[ ";
    for (unsigned int n(0); n<angles_p.size(); n++) {
      os << rotationAngles[n] << " ";
    }
    os << "]" << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------------- rotate
  
  bool RotationMatrix::rotate (vector<double> &out,
			       vector<double> const &in)
  {
    bool status (true);

    // check nof. elements in the input vector
    unsigned int nelem (in.size());

    if (nelem == rank_p) {
      out.resize(nelem);
      /*
	Rotation in 2-dimensional space
      */
      if (nelem == 2) {
	out[0] =  cos(angles_p[0])*in[0] + sin(angles_p[0])*in[1];
	out[1] = -sin(angles_p[0])*in[0] + cos(angles_p[0])*in[1];
      }
      /*
	Rotation in 3-dimensional space
       */
      else if (nelem == 3) {
      } else {
	out = in;
	std::cerr << "[RotationMatrix::rotate] Unsupported vector rank!"
		  << std::endl;
      }
    } else {
      status = false;
    }
    
    return status;
  }
  
#ifdef HAVE_BLITZ
    
  bool RotationMatrix::rotate (blitz::Array<double,1> &out,
			       blitz::Array<double,1> const &in)
  {
    bool status (true);

    // check nof. elements in the input vector
    unsigned int nelem (in.numElements());

    if (nelem == rank_p) {
      out.resize(nelem);
      /*
	Rotation in 2-dimensional space
      */
      if (nelem == 2) {
	out(0) =  cos(angles_p[0])*in(0) + sin(angles_p[0])*in(1);
	out(1) = -sin(angles_p[0])*in(0) + cos(angles_p[0])*in(1);
      }
      /*
	Rotation in 3-dimensional space
      */
      else if (nelem == 3) {
      } else {
	out = in;
	std::cerr << "[RotationMatrix::rotate] Unsupported vector rank!"
		  << std::endl;
      }
    } else {
      status = false;
    }

    return status;
  }
  
  bool RotationMatrix::rotate (blitz::Array<double,2> &out,
			       blitz::Array<double,2> const &in)
  {
    bool status (true);

    return status;
  }
    
#endif

} // Namespace CR -- end
