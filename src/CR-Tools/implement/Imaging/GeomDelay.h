/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1964 2008-09-06 17:52:38Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (bahren@astron)                                          *
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

#ifndef GEOMDELAY_H
#define GEOMDELAY_H

// Standard library header files
#include <iostream>
#include <string>
// casacore header files
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Quanta/MVPosition.h>
// CR-Tools header files
#include <Coordinates/CoordinateType.h>
#include <Math/Constants.h>
#include <Math/VectorNorms.h>

using casa::Matrix;
using casa::MVPosition;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeomDelay
    
    \ingroup CR_Imaging
    
    \brief Brief description for class GeomDelay
    
    \author Lars B&auml;hren

    \date 2008/12/17

    \test tGeomDelay.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    The reference implementation for the computation of the geometrical delay
    is located in
    \code
    static double delay (std::vector<double> const &antPosition,
			 std::vector<double> const &skyPosition,
			 bool const &farField=false);
    \endcode
    
    <h3>Example(s)</h3>
    
  */  
  class GeomDelay {

  protected:

    //! Antenna positions, [antenna,3]
    Matrix<double> antPositions_p;
    //! Sky positions, [position,3]
    Matrix<double> skyPositions_p;
    //! The geometrical delays
    Matrix<double> delays_p;

    //! Compute geometrical delay for far-field?
    bool farField_p;
    //! Buffer the values of the delays?
    bool bufferDelays_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    GeomDelay ();

    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    GeomDelay (Matrix<double> const &antPositions,
	       Matrix<double> const &skyPositions,
	       bool const &farField=false,
	       bool const &bufferDelays=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    GeomDelay (Matrix<double> const &antPositions,
	       CoordinateType::Types const &antCoord,
	       Matrix<double> const &skyPositions,
	       CoordinateType::Types const &skyCoord,
	       bool const &farField=false,
	       bool const &bufferDelays=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    GeomDelay (Vector<MVPosition> const &antPositions,
	       CoordinateType::Types const &antCoord,
	       Vector<MVPosition> const &skyPositions,
	       CoordinateType::Types const &skyCoord,
	       bool const &farField=false,
	       bool const &bufferDelays=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomDelay object from which to create this new
             one.
    */
    GeomDelay (GeomDelay const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~GeomDelay ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomDelay object from which to make a copy.
    */
    GeomDelay& operator= (GeomDelay const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the number of stored antenna positions
      
      \return nofAntennaPositions -- The number of antenna positions stored for
              the computation of the geometrical delays
    */
    inline unsigned int nofAntennaPositions () const {
      return antPositions_p.nrow();
    }
    
    /*!
      \brief Get the number of stored sky positions
      
      \return nofSkyPositions -- The number of sky positions stored for the
              computation of the geometrical delays
    */
    inline unsigned int nofSkyPositions () const {
      return skyPositions_p.nrow();
    }
    
    /*!
      \brief Get the antenna positions
      
      \return antPositions -- [position,3] array with the antenna positions
    */
    inline Matrix<double> antPositions () const {
      return antPositions_p;
    }
    
    /*!
      \brief Set the antenna positions

      \param antPositions -- [antenna,3] array with the antenna positions.
      \param type         -- Coordinate type as which the antenna positions are
             provided; if necessary conversion is performed internally.
    */
    bool setAntPositions (Matrix<double> const &antPositions,
			  CoordinateType::Types const &type=CoordinateType::Cartesian);

    /*!
      \brief Set the antenna positions

      \param antPositions -- Array with the antenna positions.
    */
    bool setAntPositions (Vector<MVPosition> const &antPositions);
    
    /*!
      \brief Get the sky positions

      \return skyPositions -- [position,3] array with the sky positions
    */
    inline Matrix<double> skyPositions () const {
      return skyPositions_p;
    }

    /*!
      \brief Set the sky positions

      \param skyPositions -- [antenna,3] array with the sky positions.
      \param type         -- Coordinate type as which the sky positions are
             provided; if necessary conversion is performed internally.
    */
    bool setSkyPositions (Matrix<double> const &skyPositions,
			  CoordinateType::Types const &type=CoordinateType::Cartesian);
    
    /*!
      \brief Set the sky positions

      \param skyPositions -- Array with the sky positions.
    */
    bool setSkyPositions (Vector<MVPosition> const &skyPositions);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeomDelay.
    */
    inline std::string className () const {
      return "GeomDelay";
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
      \brief Get the geometrical delays
      
      \return delays -- The geometrical delays for the given set of antenna and
              sky positions
    */
    Matrix<double> delays ();
    
    /*!
      \brief Compute the geometrical delay

      \b Note: This function contains the reference implementation for the
      actual computation of the geometrical delay (no matter if for the near-field
      or far-field case). All further methods should either a) use this method
      or b) constructed from a copy of the function body!

      \param antPosition -- 
      \param skyPosition -- 
      \param farField    -- Compute geometrical delay for far-field?

      \return delay -- The value of the geometrical delay
    */
    static double delay (std::vector<double> const &antPosition,
			 std::vector<double> const &skyPosition,
			 bool const &farField=false);
    
    /*!
      \brief Compute the geometrical delay

      \param antPosition -- 
      \param skyPosition -- 
      \param farField    -- Compute geometrical delay for far-field?

      \return delay -- The value of the geometrical delay
    */
    static double delay (casa::Vector<double> const &antPosition,
			 casa::Vector<double> const &skyPosition,
			 bool const &farField=false);
    
    /*!
      \brief Compute the geometrical delay

      \param antPositions -- Set of antenna positions
      \param skyPositions -- Set of sky positions
      \param farField     -- Compute geometrical delay for far-field?

      \return delay -- The value of the geometrical delay
    */
    static Matrix<double> delay (casa::Matrix<double> const &antPositions,
				 casa::Matrix<double> const &skyPositions,
				 bool const &farField=false);
    
  private:
    
    //! Unconditional copying
    void copy (GeomDelay const &other);
    
    //! Unconditional deletion 
    void destroy(void);

    //! Initialize internal parameters
    void init ();
    
    /*!
      \brief Initialize internal parameters

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    void init (Matrix<double> const &antPositions,
	       Matrix<double> const &skyPositions,
	       bool const &farField=false,
	       bool const &bufferDelays=false);

  };
  
} // Namespace CR -- end

#endif /* GEOMDELAY_H */
  
