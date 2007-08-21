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

  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix ()
  {
    init (1);
  }

  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix (unsigned int const &rank)
  {
    init (rank);
  }

  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  vector<double> const &angles,
				  bool const &anglesInDegrees)
  {
    init (rank,
	  angles,
	  anglesInDegrees);
  }
  
  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix (RotationMatrix const &other)
  {
    copy (other);
  }

  // ----------------------------------------------------------------------- init
  
  void RotationMatrix::init (uint const &rank)
  {
    vector<double> valAngles (rank,0.0);
    bool anglesInDegrees (false);
    // forward the parameters
    init (rank,
	  valAngles,
	  anglesInDegrees);
  }
  
  // ----------------------------------------------------------------------- init

  void RotationMatrix::init (uint const &rank,
			     vector<double> const &angles,
			     bool const &anglesInDegrees)
  {
    bool status (true);

    status = setRank (rank);
    status = setAngles (angles,
			anglesInDegrees);
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
    // adjust size of array holding rotation angles
    angles_p.resize(other.angles_p.size());
    // adjust size of the array holding values of the rotation matrix
    rotationMatrix_p.resize(other.rotationMatrix_p.size());

    rank_p           = other.rank_p;
    nofAngles_p      = other.nofAngles_p;
    angles_p         = other.angles_p;
    rotationMatrix_p = other.rotationMatrix_p;
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

    // Store the rank
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
    vector<double> anglesInRadian (nelem);
    
    /*
      Check the number of angles to parametrize the rotation matrix; if we have
      too few parameters, the input has to be rejected. If we receive more 
      parameters as actually required, we pick the first necessary elements,
      but issue a warning.
     */
    if (nelem < nofAngles_p) {
      std::cerr << "[RotationMatrix::setAngles] Too few parameters for rotation"
		<< std::endl;
      return false;
    } else {
      if (nelem > nofAngles_p) {
	std::cerr << "[RotationMatrix::setAngles] Over-determined parametrization"
		  << std::endl;
      }
    }
    
    /*
      If the rotation angles are provided in degrees, we first need to convert
      to radian.
    */
    if (anglesInDegree) {
      anglesInRadian = deg2rad (angles);
    } else {
      anglesInRadian = angles;
    }

    /*
      Store the provided rotation parameters.
    */
    angles_p.resize(nofAngles_p);
    for (uint n(0); n<nofAngles_p; n++) {
      angles_p[n] = anglesInRadian[n];
    }

    status = setRotationMatrix ();

    return status;
  }
  
  // ---------------------------------------------------------- setRotationMatrix

  bool RotationMatrix::setRotationMatrix ()
  {
    bool status (true);
    uint nelem (rank_p*rank_p);

    rotationMatrix_p.resize(nelem);
    
    if (rank_p == 2) {
      rotationMatrix_p[0] =  cos(angles_p[0]);    // [x,x]
      rotationMatrix_p[1] =  sin(angles_p[0]);    // [x,y]
      rotationMatrix_p[2] = -sin(angles_p[0]);    // [y,x]
      rotationMatrix_p[3] =  cos(angles_p[0]);    // [y,y]
    } else if (rank_p == 3) {
    } else {
      status = false;
    }

    return status;
  }
  
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
  
  // ============================================================================
  //
  //  Methods using Blitz++ library
  //
  // ============================================================================

#ifdef HAVE_BLITZ
  
  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  blitz::Array<double,1> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles);
    }
  }
  
  // ------------------------------------------------------------------ setAngles

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
  
  // --------------------------------------------------------------------- rotate
  
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
  
  // --------------------------------------------------------------------- rotate
  
  bool RotationMatrix::rotate (blitz::Array<double,2> &out,
			       blitz::Array<double,2> const &in)
  {
    bool status (true);

    return status;
  }
    
#endif

  // ============================================================================
  //
  //  Methods using CASA library
  //
  // ============================================================================

#ifdef HAVE_CASA

  // ------------------------------------------------------------- RotationMatrix
  
  RotationMatrix::RotationMatrix (unsigned int const &rank,
				  casa::Vector<double> const &angles,
				  bool const &anglesInDegree)
  {
    bool status (true);
    
    if (setRank(rank)) {
      status = setAngles (angles);
    }
  }

  // ------------------------------------------------------------------ setAngles
  
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

} // Namespace CR -- end
