/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <Coordinates/PositionVector.h>

namespace CR { // Namespace  -- begin
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  PositionVector::~PositionVector ()
  {
    destroy();
  }
  
  void PositionVector::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  PositionVector& PositionVector::operator= (PositionVector const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void PositionVector::copy (PositionVector const &other)
  {
    type_p = other.type_p;

    position_p.resize(other.position_p.shape());
    position_p = other.position_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void PositionVector::summary (std::ostream &os)
  {
    unsigned int nelem = position_p.nelements();

    os << "[PositionVector] Summary of internal parameters." << std::endl;
    os << "-- Coordinate type  = " << type_p.type() << std::endl;
    os << "-- Coordinate name  = " << type_p.name() << std::endl;
    os << "-- Coordinate value = [";
    for (unsigned int n(0); n<nelem; n++) {
      os << " " << position_p(n);
    }
    os << " ]" << std::endl;
  }
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init

  void PositionVector::init (CoordinateType const &coordType)
  {
    casa::Vector<double> pos (3,0.0);

    init (coordType,
	  pos);
  }

  //_____________________________________________________________________________
  //                                                                         init
  
  void PositionVector::init (CoordinateType const &coordType,
			     casa::Vector<double> const &position)
  {
    type_p = coordType;

    position_p.resize(position.shape());
    position_p = position;
  }
  
  //_____________________________________________________________________________
  //                                                                      convert
  
  bool PositionVector::convert (casa::Vector<double> &out,
				CR::CoordinateType::Types const &typeOut,
				casa::Vector<double> const &in,
				CR::CoordinateType::Types const &typeIn,
				bool const &anglesInDegrees)
  {
    bool status (true);

    CR::PositionVector vec;

    switch (typeIn) {
      //______________________________________________________________
      //                                           AzElHeight -> other
    case CoordinateType::AzElHeight:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	out = in;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->Cartesian"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cylindrical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->Cylindrical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->LongLatRadius"
		  << std::endl;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->NorthEastHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Spherical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElHeight->Spherical"
		  << std::endl << std::flush;
	status = false;
	break;
      default:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: "
		  << CoordinateType::getName(typeIn)
		  << " -> "
		  << CoordinateType::getName(typeOut)
		  << std::endl;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                           AzElRadius -> other
    case CoordinateType::AzElRadius:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElRadius->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	out = in;
	break;
      case CoordinateType::Cartesian:
	vec.AzElRadius2Cartesian (out(0), out(1), out(2),
				  in(0) ,  in(1),  in(2),
				  anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElRadius->Cylindrical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElRadius->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElRadius->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: AzElRadius->NorthEastHeight"
		  << std::endl;
	status = false;
	break;
      case CoordinateType::Spherical:
	vec.AzElRadius2Spherical (out(0), out(1), out(2),
				  in(0) ,  in(1),  in(2),
				  anglesInDegrees);
	break;
      default:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: "
		  << CoordinateType::getName(typeIn)
		  << " -> "
		  << CoordinateType::getName(typeOut)
		  << std::endl;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                            Cartesian -> other
    case CoordinateType::Cartesian:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cartesian->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cartesian->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	out = in;
	break;
      case CoordinateType::Cylindrical:
	vec.Cartesian2Cylindrical (out(0), out(1), out(2),
				   in(0) ,  in(1),  in(2),
				   anglesInDegrees);
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cartesian->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cartesian->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	vec.Cartesian2NorthEastHeight (out(0), out(1), out(2),
				       in(0) ,  in(1),  in(2),
				       anglesInDegrees);
	break;
      case CoordinateType::Spherical:
	vec.Cartesian2Spherical (out(0), out(1), out(2),
				 in(0) ,  in(1),  in(2),
				 anglesInDegrees);
	break;
      default:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: "
		  << CoordinateType::getName(typeIn)
		  << " -> "
		  << CoordinateType::getName(typeOut)
		  << std::endl;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                          Cylindrical -> other
    case CoordinateType::Cylindrical:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cylindrical->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cylindrical->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	vec.Cylindrical2Cartesian (out(0), out(1), out(2),
				   in(0) ,  in(1),  in(2),
				   anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	out = in;
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cylindrical->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cylindrical->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Cylindrical->NorthEastHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Spherical:
	vec.Cylindrical2Spherical (out(0), out(1), out(2),
				   in(0) ,  in(1),  in(2),
				   anglesInDegrees);
	break;
      case CoordinateType::Direction:
      case CoordinateType::Frequency:
      case CoordinateType::Time:
	std::cerr << "[PositionVector::convert]"
		  << " No such conversion: Cylindrical->(Direction,Frequency,Time)"
		  << std::endl << std::flush;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                      DirectionRadius -> other
    case CoordinateType::DirectionRadius:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->Cartesian"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cylindrical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->Cylindrical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::DirectionRadius:
	out = in;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->NorthEastHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Spherical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: DirectionRadius->Spherical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Direction:
      case CoordinateType::Frequency:
      case CoordinateType::Time:
	std::cerr << "[PositionVector::convert]"
		  << " No such conversion: DirectionRadius->(Direction,Frequency,Time)"
		  << std::endl << std::flush;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                        LongLatRadius -> other
    case CoordinateType::LongLatRadius:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: LongLatRadius->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: LongLatRadius->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	vec.LongLatRadius2Cartesian (out(0), out(1), out(2),
				     in(0) ,  in(1),  in(2),
				     anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: LongLatRadius->Cylindrical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: LongLatRadius->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	out = in;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: LongLatRadius->NorthEastHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Spherical:
	vec.LongLatRadius2Spherical (out(0), out(1), out(2),
				     in(0) ,  in(1),  in(2),
				     anglesInDegrees);
	break;
      case CoordinateType::Direction:
      case CoordinateType::Frequency:
      case CoordinateType::Time:
	std::cerr << "[PositionVector::convert]"
		  << " No such conversion: LongLatRadius->(Direction,Frequency,Time)"
		  << std::endl << std::flush;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                      NorthEastHeight -> other
    case CoordinateType::NorthEastHeight:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	vec.NorthEastHeight2Cartesian (out(0),out(1),out(2),
				       in(0),in(1),in(2),
				       anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->Cylindrical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	out = in;
	break;
      case CoordinateType::Spherical:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: NorthEastHeight->Spherical"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Direction:
      case CoordinateType::Frequency:
      case CoordinateType::Time:
	std::cerr << "[PositionVector::convert]"
		  << " No such conversion: NorthEastHeight->(Direction,Frequency,Time)"
		  << std::endl << std::flush;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                                            Spherical -> other
    case CoordinateType::Spherical:
      switch (typeOut) {
      case CoordinateType::AzElHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Spherical->AzElHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::AzElRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Spherical->AzElRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Cartesian:
	vec.Spherical2Cartesian (out(0), out(1), out(2),
				 in(0) ,  in(1),  in(2),
				 anglesInDegrees);
	break;
      case CoordinateType::Cylindrical:
	vec.Spherical2Cylindrical (out(0), out(1), out(2),
				   in(0) ,  in(1),  in(2),
				   anglesInDegrees);
	break;
      case CoordinateType::DirectionRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Spherical->DirectionRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::LongLatRadius:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Spherical->LongLatRadius"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::NorthEastHeight:
	std::cerr << "[PositionVector::convert]"
		  << " Conversion not implemented: Spherical->NorthEastHeight"
		  << std::endl << std::flush;
	status = false;
	break;
      case CoordinateType::Spherical:
	out = in;
	break;
      case CoordinateType::Direction:
      case CoordinateType::Frequency:
      case CoordinateType::Time:
	std::cerr << "[PositionVector::convert]"
		  << " No such conversion: Spherical->(Direction,Frequency,Time)"
		  << std::endl << std::flush;
	status = false;
	break;
      };
      break;
      //______________________________________________________________
      //                     Catch further conversions not implemented
    default:
      std::cerr << "[PositionVector::convert]"
		<< " Conversion not implemented: "
		<< CoordinateType::getName(typeIn)
		<< " -> "
		<< CoordinateType::getName(typeOut)
		<< std::endl;
      status = false;
      break;
    };
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                      Conversion -> Cartesian

  void PositionVector::Spherical2Cartesian (double &out_x,
					    double &out_y,
					    double &out_z,
					    double const &in_r,
					    double const &in_phi,
					    double const &in_theta,
					    bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      Spherical2Cartesian (out_x,out_y,out_z,
			   in_r,deg2rad(in_phi),deg2rad(in_theta),
			   false);
    } else {
      out_x = in_r*sin(in_theta)*cos(in_phi);
      out_y = in_r*sin(in_theta)*sin(in_phi);
      out_z = in_r*cos(in_theta); 
    }
  }
  
  void PositionVector::Cylindrical2Cartesian (double &out_x,
					      double &out_y,
					      double &out_z,
					      double const &in_rho,
					      double const &in_phi,
					      double const &in_z,
					      bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      Cylindrical2Cartesian (out_x,out_y,out_z,
			     in_rho,deg2rad(in_phi),in_z,
			     false);
    } else {
      out_x = in_rho*cos(in_phi);
      out_y = in_rho*sin(in_phi);
      out_z = in_z;
    }
  }
  
  void PositionVector::AzElRadius2Cartesian (double &out_x,
					     double &out_y,
					     double &out_z,
					     double const &in_az,
					     double const &in_el,
					     double const &in_r,
					     bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      AzElRadius2Cartesian (out_x,out_y,out_z,
			    deg2rad(in_az),deg2rad(in_el),in_r,
			    false);
    } else {
      out_x = in_r*cos(in_el)*sin(in_az);
      out_y = in_r*cos(in_el)*cos(in_az);
      out_z = in_r*sin(in_el);
    }
  }
  
  void PositionVector::LongLatRadius2Cartesian (double &out_x,
						double &out_y,
						double &out_z,
						double const &in_long,
						double const &in_lat,
						double const &in_r,
						bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      LongLatRadius2Cartesian (out_x,out_y,out_z,
			       deg2rad(in_long),deg2rad(in_lat),in_r,
			       false);
    } else {
      out_x = in_r*cos(in_lat)*cos(in_long);
      out_y = in_r*cos(in_lat)*sin(in_long);
      out_z = in_r*sin(in_lat);
    }
  }
  
  //_____________________________________________________________________________
  //                                                     Conversions -> Spherical
  
  void PositionVector::Cartesian2Spherical (double &r,
					    double &phi,
					    double &theta,
					    double const &x,
					    double const &y,
					    double const &z,
					    bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      r     = sqrt(x*x+y*y+z*z);
      phi   = rad2deg(atan2(y,x));
      theta = rad2deg(acos(z/r));
    } else {
      r     = sqrt(x*x + y*y + z*z);
      phi   = atan2(y,x);
      theta = acos(z/r);
    }

    if (r == 0) {
      theta = 0;
    }

  }
  
  void PositionVector::Cylindrical2Spherical (double &out_r,
					      double &out_phi,
					      double &out_theta,
					      double const &in_rho,
					      double const &in_phi,
					      double const &in_z,
					      bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_r     = sqrt(in_rho*in_rho+in_z*in_z);
      out_phi   = in_phi;
      out_theta = rad2deg(atan2(in_rho,in_z));
    } else {
      out_r     = sqrt(in_rho*in_rho+in_z*in_z);
      out_phi   = in_phi;
      out_theta = atan2(in_rho,in_z);
    }
  }
  
  void PositionVector::AzElRadius2Spherical (double &out_r,
					     double &out_phi,
					     double &out_theta,
					     double const &in_az,
					     double const &in_el,
					     double const &in_r,
					     bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_r     = in_r;
      out_phi   = 90-in_az;
      out_theta = 90-in_el;
    } else {
      out_r     = in_r;
      out_phi   = deg2rad(90)-in_az;
      out_theta = deg2rad(90)-in_el;
    }
  }

  void PositionVector::LongLatRadius2Spherical (double &out_r,
						double &out_phi,
						double &out_theta,
						double const &in_long,
						double const &in_lat,
						double const &in_r,
						bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_r     = in_r;
      out_phi   = in_long;
      out_theta = 90-in_lat;
    } else {
      out_r     = in_r;
      out_phi   = in_long;
      out_theta = deg2rad(90)-in_lat;
    }
  }
  
  //_____________________________________________________________________________
  //                                                   Conversions -> Cylindrical
  
  void PositionVector::Cartesian2Cylindrical (double &out_rho,
					      double &out_phi,
					      double &out_z,
					      double const &in_x,
					      double const &in_y,
					      double const &in_z,
					      bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_rho = sqrt(in_x*in_x+in_y*in_y);
      out_phi = rad2deg(atan2(in_y,in_x));
      out_z   = in_z;
    } else {
      out_rho = sqrt(in_x*in_x+in_y*in_y);
      out_phi = atan2(in_y,in_x);
      out_z   = in_z;
    }
  }
  
  void PositionVector::Spherical2Cylindrical (double &out_rho,
					      double &out_phi,
					      double &out_z,
					      double const &in_r,
					      double const &in_phi,
					      double const &in_theta,
					      bool const &anglesInDegrees)
  {
    if (anglesInDegrees) {
      out_rho = in_r*sin(deg2rad(in_theta));
      out_phi = in_phi;
      out_z   = in_r*cos(deg2rad(in_theta));
    } else {
      out_rho = in_r*sin(in_theta);
      out_phi = in_phi;
      out_z   = in_r*cos(in_theta);
    }
  }

  //_____________________________________________________________________________
  //_____________________________________________________________________________


  //_____________________________________________________________________________
  //                                                           Cartesian -> other

  void PositionVector::Cartesian2NorthEastHeight (double &out_north,
						  double &out_east,
						  double &out_height,
						  double const &in_x,
						  double const &in_y,
						  double const &in_z,
						  bool const &anglesInDegrees)
  {
    out_north  = in_y;
    out_east   = in_x;
    out_height = in_z;
  }
  
  //_____________________________________________________________________________
  //                                                     NorthEastHeight -> other
  
  void PositionVector::NorthEastHeight2Cartesian (double &out_x,
						  double &out_y,
						  double &out_z,
						  double const &in_north,
						  double const &in_east,
						  double const &in_height,
						  bool const &anglesInDegrees)
  {
    out_x = in_east;
    out_y = in_north;
    out_z = in_height;
  }
  
  
} // Namespace  -- end
