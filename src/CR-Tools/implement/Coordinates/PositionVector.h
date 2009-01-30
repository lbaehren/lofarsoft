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

#ifndef POSITIONVECTOR_H
#define POSITIONVECTOR_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

#include <Math/Constants.h>
#include <Coordinates/CoordinateType.h>

namespace CR { // Namespace  -- begin
  
  /*!
    \class PositionVector
    
    \ingroup CR_Coordinates
    
    \brief A vector describing a position in space
    
    \author Lars B&auml;hren

    \date 2008/12/18

    \test tPositionVector.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::CoordinateType - Definition of the various coordinate types we
      are dealing with
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class PositionVector {

    //! Coordinate type of the position vector
    CR::CoordinateType type_p;
    //! Numerical values of the position vector
    casa::Vector<double> position_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    PositionVector () {
      init (CR::CoordinateType::Cartesian);
    }
    
    /*!
      \brief Argumented constructor

      \param coordType -- Type of the coordinate
    */
    PositionVector (CoordinateType const &coordType) {
      init (coordType);
    }
    
    /*!
      \brief Argumented constructor

      \param coordType -- Type of the coordinate.
      \param position  -- Position values.
    */
    PositionVector (CoordinateType const &coordType,
		    casa::Vector<double> const &position) {
      init (coordType,
	    position);
    }
    
    /*!
      \brief Copy constructor
      
      \param other -- Another PositionVector object from which to create this new
             one.
    */
    PositionVector (PositionVector const &other) {
      copy (other);
    }
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~PositionVector ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another PositionVector object from which to make a copy.
    */
    PositionVector& operator= (PositionVector const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the coordinate type of the position vector

      \return type - The coordinate type of the position vector
    */
    inline CR::CoordinateType::Types getType () const {
      return type_p.type();
    }

    /*!
      \brief Get the coordinate type of the position vector

      \return type - The coordinate type of the position vector
    */
    inline std::string getName () {
      return type_p.name();
    }

    /*!
      \brief Get the values of the position vector

      \return position -- Numerical values of the position vector.
    */
    inline casa::Vector<double> getValue () const {
      return position_p;
    }

    /*!
      \brief Get the values of the position vector in specific coordinates

      \return position -- Numerical values of the position vector.
    */
    casa::Vector<double> getValue (CR::CoordinateType::Types const &type);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, PositionVector.
    */
    inline std::string className () const {
      return "PositionVector";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Convert position vector to another
    
      \retval out            -- Output position vector.
      \param typeOut         -- Coordinate type of the output position vector.
      \param in              -- Input position vector.
      \param typeIn          -- Coordinate type of the input position vector.
      \param anglesInDegrees -- If the coordinate contains angular coordinates, 
             are their values given in radians or degrees?

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
     */
    static bool convert (casa::Vector<double> &out,
			 CR::CoordinateType::Types const &typeOut,
			 casa::Vector<double> const &in,
			 CR::CoordinateType::Types const &typeIn,
			 bool const &anglesInDegrees=false);

  private:
    
    //! Unconditional copying
    void copy (PositionVector const &other);
    
    //! Unconditional deletion 
    void destroy(void);

    /*!
      \brief Initialize the object's internal parameters

      \param coordType -- Type of the coordinate.
    */
    void init (CoordinateType const &coordType);

    /*!
      \brief Initialize the object's internal parameters

      \param coordType -- Type of the coordinate.
      \param position  -- Position values.
    */
    void init (CoordinateType const &coordType,
	       casa::Vector<double> const &position);
    
    //! Convert degress to radian
    inline double deg2rad (double const &deg) {
      return deg*CR::pi/180.0;
    }
    
    //! Convert radian to degrees
    inline double rad2deg (double const &rad) {
      return rad*180/CR::pi;
    }
    
    //________________________________________________________________
    //                                             AzElRadius -> other

    //! Convert \f$ (Az,El,R) \f$ to \f$ (x,y,z) \f$.
    void AzElRadius2Cartesian (double &out_x,
				  double &out_y,
				  double &out_z,
				  double const &in_az,
				  double const &in_el,
				  double const &in_r,
				  bool const &anglesInDegrees=false);
    
    //! Convert \f$ (Az,El,R) \f$ to \f$ (r,\phi,\theta) \f$
    void AzElRadius2Spherical (double &out_r,
			       double &out_phi,
			       double &out_theta,
			       double const &in_az,
			       double const &in_el,
			       double const &in_r,
			       bool const &anglesInDegrees=false);
    
    //________________________________________________________________
    //                                              Cartesian -> other

    //! Convert \f$ (x,y,z) \f$ to \f$ (Az,El,R) \f$
    void Cartesian2AzElRadius (double &out_rho,
			       double &out_phi,
			       double &out_z,
			       double const &in_az,
			       double const &in_el,
			       double const &in_r,
			       bool const &anglesInDegrees=false);
    
    //! Convert \f$ (x,y,z) \f$ to \f$ (\rho,\phi,z) \f$
    void Cartesian2Cylindrical (double &out_rho,
				double &out_phi,
				double &out_z,
				double const &in_x,
				double const &in_y,
				double const &in_z,
				bool const &anglesInDegrees=false);
    
    //! Convert \f$ (x,y,z) \f$ to \f$ (N,E,H) \f$
    void Cartesian2NorthEastHeight (double &out_north,
				    double &out_east,
				    double &out_height,
				    double const &in_x,
				    double const &in_y,
				    double const &in_z,
				    bool const &anglesInDegrees=false);
    
    //! Convert \f$ (x,y,z) \f$ to \f$ (r,\phi,\theta) \f$
    void Cartesian2Spherical (double &out_r,
			      double &out_phi,
			      double &out_theta,
			      double const &in_x,
			      double const &in_y,
			      double const &in_z,
			      bool const &anglesInDegrees=false);
    
    //________________________________________________________________
    //                                            Cylindrical -> other

    //! Convert \f$ (\rho,\phi,z) \f$ to \f$ (x,y,z) \f$.
    void Cylindrical2Cartesian (double &out_x,
				double &out_y,
				double &out_z,
				double const &in_rho,
				double const &in_phi,
				double const &in_z,
				bool const &anglesInDegrees=false);
    
    //! Convert \f$ (\rho,\phi,z) \f$ to \f$ (r,\phi,\theta) \f$
    void Cylindrical2Spherical (double &out_r,
				double &out_phi,
				double &out_theta,
				double const &in_rho,
				double const &in_phi,
				double const &in_z,
				bool const &anglesInDegrees=false);
    
    //________________________________________________________________
    //                                        NorthEastHeight -> other

    //! Convert \f$ (H,E,H) \f$ to \f$ (x,y,z) \f$.
    void NorthEastHeight2Cartesian (double &out_x,
				    double &out_y,
				    double &out_z,
				    double const &in_north,
				    double const &in_east,
				    double const &in_height,
				    bool const &anglesInDegrees=false);
    
    //________________________________________________________________
    //                                          LongLatRadius -> other

    //! Convert \f$ (Long,Lat,R) \f$ to \f$ (x,y,z) \f$.
    void LongLatRadius2Cartesian (double &out_x,
				  double &out_y,
				  double &out_z,
				  double const &in_long,
				  double const &in_lat,
				  double const &in_r,
				  bool const &anglesInDegrees=false);
    
    //! Convert \f$ (Long,Lat,R) \f$ to \f$ (r,\phi,\theta) \f$
    void LongLatRadius2Spherical (double &out_r,
				  double &out_phi,
				  double &out_theta,
				  double const &in_long,
				  double const &in_lat,
				  double const &in_r,
				  bool const &anglesInDegrees=false);
    
    //________________________________________________________________
    //                                              Spherical -> other

    //! Convert \f$ (r,\phi,\theta) \f$ to \f$ (x,y,z) \f$.
    void Spherical2Cartesian (double &out_x,
			      double &out_y,
			      double &out_z,
			      double const &in_r,
			      double const &in_phi,
			      double const &in_theta,
			      bool const &anglesInDegrees=false);

    //! Convert \f$ (r,\phi,\theta) \f$ to \f$ (\rho,\phi,z) \f$
    void Spherical2Cylindrical (double &out_rho,
				double &out_phi,
				double &out_z,
				double const &in_r,
				double const &in_phi,
				double const &in_theta,
				bool const &anglesInDegrees=false);
    
  };
  
} // Namespace  -- end

#endif /* POSITIONVECTOR_H */
  
