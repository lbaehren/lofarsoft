/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: GeomDelay.h,v 1.1 2006/10/31 21:07:59 bahren Exp $*/

#ifndef GEOMDELAY_H
#define GEOMDELAY_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Exceptions/Error.h>
#include <casa/Quanta.h>
#include <scimath/Mathematics.h>

using casa::AipsError;
using casa::Vector;

namespace LOPES {
  
  /*!
    \class GeomDelay
    
    \ingroup Beamforming
    
    \brief Geometrical delay for a single baseline interferometer
    
    \author Lars B&auml;hren
    
    \date 2006/09/13
    
    \test tGeomDelay.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    For a single-baseline interferometer with antenna positions \f$ \vec x_1 \f$ and
    \f$ \vec x_2 \f$, baseline vector \f$ \vec B_{ij} = \vec x_j - \vec x_i \f$ and
    source position \f$ \vec \rho \f$ the geometrical delay is given by
    \f[
    \tau_{ij}
    = \frac{1}{c} \left( \sqrt{|\vec \rho - \vec x_i|^2 + |\vec B_{ij}|^2
    - 2 \langle \vec \rho - \vec x_i , \vec B_{ij} \rangle} - |\vec \rho - \vec x_i|
    \right)
    = \frac{1}{c} \left( \sqrt{|\vec \xi_i|^2 + |\vec B_{ij}|^2 - 2 \langle
    \vec \xi_i , \vec B_{ij} \rangle} - |\vec \xi_i| \right)
    \f]
    
    Note: though the above relationship itself does not depend on a particular
    choice for a coordinate system, within this class use <i>rectangular cartesian</i>
    coordinates.
    
    <h3>Example(s)</h3>
    
  */
  class GeomDelay {
    
    //! Position of the source w.r.t. the coordinate origin, [m]
    Vector<double> source_p;
    //! Position of the first antenna, [m]
    Vector<double> antenna1_p;
    //! Baseline between the two interferometer elements, [m]
    Vector<double> antenna2_p;
    
  protected:
    
    static double pi;
    static double lightspeed;
    
    //! Geometrical delay, [s]
    double delay_p;
    
  public:
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    GeomDelay ();
    
    /*!
      \brief Argumented constructor
      
      \param source  -- Position \f$ \vec \rho = (\rho_x, \rho_y, \rho_z) \f$ of
      the source w.r.t. the coordinate origin, [m]
      \param antenna -- Position \f$ \vec x_i = (x,y,z)_i \f$ of the antenna, [m]. For this
      type of setup the center of the coordinate system has been
      shifted to the position \f$ \vec x_i \f$ of the first
      antenna.
    */
    GeomDelay (const Vector<double> &source,
	       const Vector<double> &antenna);
    
    /*!
      \brief Argumented constructor
      
      \param source   -- Position of the source w.r.t. the coordinate origin, [m]
      \param antenna1 -- Position of the first antenna, [m]
      \param antenna2 -- Position of the second antenna, [m]
    */
    GeomDelay (const Vector<double> &source,
	       const Vector<double> &antenna1,
	       const Vector<double> &antenna2);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomDelay object from which to create this new
      one.
    */
    GeomDelay (GeomDelay const &other);
    
    // ---------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~GeomDelay ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomDelay object from which to make a copy.
    */
    GeomDelay& operator= (GeomDelay const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the position of the source
      
      \return source -- The position of the source, [m].
    */
    Vector<double> source () const {
      return source_p;
    }
    
    /*!
      \brief Set the position of the source
      
      \param source -- The position of the source, [m].
    */
    void setSource (const Vector<double> &source);
    
    /*!
      \brief Get the position of the first receiving elememt
      
      \return antenna1 -- Position of the first receiving element w.r.t. the source
      position reference point, [m].
    */
    Vector<double> antenna1 () const {
      return antenna1_p;
    }
    
    /*!
      \brief Set the position of the first receiving elememt
      
      \param antenna1 -- Position of the first receiving element w.r.t. the source
      position reference point, [m].
    */
    void setAntenna1 (const Vector<double> &antenna1);
    
    /*!
      \brief Get the position of the second receiving elememt
      
      \return antenna2 -- Position of the first receiving element w.r.t. the source
      position reference point, computed via
      \f$ \vec x_j = \vec B_{ij} + \vec x_i \f$
    */
    Vector<double> antenna2 () const {
      return antenna2_p;
    }
    
    /*!
      \brief Set the position of the second receiving elememt
      
      \param antenna2 -- Position of the first receiving element w.r.t. the source
      position reference point, [m].
    */
    void setAntenna2 (const Vector<double> &antenna2);
    
    /*!
      \brief Get the baseline
      
      \return baseline -- The baseline, i.e. the vector difference of the two
      antenna positions, [m].
    */
    Vector<double> baseline () const {
      return antenna2_p-antenna1_p;
    }
    
    /*!
      \brief Get the value of the geometrical delay
      
      \return delay -- The numerical value of the geometrical delay, [s]
    */
    double delay () const {
      return delay_p;
    }
    
    /*!
      \brief Get the value of the geometrical delay
      
      \param source   -- Position of the source w.r.t. the coordinate origin, [m]
      \param antenna1 -- Position of the first antenna, [m]
      \param antenna2 -- Position of the second antenna, [m]
      
      \return delay -- The numerical value of the geometrical delay, [s]
    */
    double delay (const Vector<double> &source,
		  const Vector<double> &antenna1,
		  const Vector<double> &antenna2);
    
    // -------------------------------------------------------------------- Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (GeomDelay const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Initialize object with default parameters
    */
    void init ();
    
    /*!
      \brief Initialize object with given parameters
      
      \param source  -- Position \f$ \vec \rho \f$ of the source w.r.t. the
      coordinate origin, [m]
      \param antenna -- Position \f$ \vec x_j \f$ of the antenna, [m]. For this
      type of setup the center of the coordinate system has been
      shifted to the position \f$ \vec x_i \f$ of the first
      antenna.
    */
    void init (const Vector<double> &source,
	       const Vector<double> &antenna);
    
    /*!
      \brief Initialize object with given parameters
    */
    void init (const Vector<double> &source,
	       const Vector<double> &antenna1,
	       const Vector<double> &antenna2);
    
    /*!
      \brief Compute the geometrical delay
      
      \return status -- Returns <i>False</i> if an error was encountered
    */
    bool setDelay();
    
  };
  
}

#endif /* GEOMDELAY_H */
