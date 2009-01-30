/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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
#include <Coordinates/PositionVector.h>
#include <Math/Constants.h>
#include <Math/VectorNorms.h>

using casa::Matrix;
using casa::MVPosition;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeomDelay
    
    \ingroup CR_Imaging
    
    \brief Geometrical delays for the beamforming
    
    \author Lars B&auml;hren

    \date 2008/12/17

    \test tGeomDelay.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>casa::<a href="http://www.astron.nl/casacore/doc/html/classcasa_1_1MVPosition.html">MVPosition</a> class
    </ul>
    
    <h3>Synopsis</h3>

    The reference implementation for the computation of the geometrical delay
    is located in
    \code
    static double delay (std::vector<double> const &antPosition,
			 std::vector<double> const &skyPosition,
			 bool const &farField=false);
    \endcode
    Depending on the value of <tt>farField</tt> one of the following two
    expressions is evaluated:
    <ul>
      <li>Near-field:
      \f[ \tau_{j} = \frac{|\vec \rho|}{c} \left( \sqrt{1 + \frac{|\vec x_j|^2 -
        2 \langle \vec \rho , \vec x_j \rangle}{|\vec \rho|^2}} - 1 \right) \f]
      <li>Far-field:
      \f[ \tau_{j} = - \frac{1}{c} \frac{\langle \vec \rho , \vec x_j \rangle}{|\vec \rho|} \f]
    </ul>
    
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
    GeomDelay () {
      init();
    }

    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    GeomDelay (Matrix<double> const &antPositions,
	       Matrix<double> const &skyPositions,
	       bool const &anglesInDegrees=true,
	       bool const &farField=false,
	       bool const &bufferDelays=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param antCoord     -- 
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param skyCoord     --
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    GeomDelay (Matrix<double> const &antPositions,
	       CoordinateType::Types const &antCoord,
	       Matrix<double> const &skyPositions,
	       CoordinateType::Types const &skyCoord,
	       bool const &anglesInDegrees=true,
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
	       Vector<MVPosition> const &skyPositions,
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
    virtual ~GeomDelay ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomDelay object from which to make a copy.
    */
    GeomDelay& operator= (GeomDelay const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are the values of the geometrical delays getting buffered?
      
      \return bufferDelays -- Returns <tt>true</tt> in case the values of the 
              geometrical delays are buffered.
    */
    inline bool bufferDelays () const {
      return bufferDelays_p;
    }

    /*!
      \brief Enable/Disable buffering of the values of the geometrical weights
      
      \param bufferDelays -- Choice wether or not to buffer the values of the
             geometrical delays.
    */
    void bufferDelays (bool const &bufferDelays);

    /*!
      \brief Are the geometrical delays computed for the far-field geometry?

      \return farField -- Returns <tt>true</tt> in case the delays are computed
              for the far-field geometry.
    */
    inline bool farField () const {
      return farField_p;
    }

    /*!
      \brief Select computation of the delays for far-field geometry

      \param farField -- If set to <tt>true</tt> delays will be computed for
             the far-field geometry.
    */
    void farField (bool const &farField);

    /*!
      \brief Are the geometrical delays computed for the near-field geometry?

      \return farField -- Returns <tt>true</tt> in case the delays are computed
              for the near-field geometry.
    */
    inline bool nearField () const {
      return !farField_p;
    }

    /*!
      \brief Select computation of the delays for near-field geometry

      \param farField -- If set to <tt>true</tt> delays will be computed for
             the near-field geometry.
    */
    inline void nearField (bool const &nearField) {
      farField (!nearField);
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
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
    */
    bool setAntPositions (Matrix<double> const &antPositions,
			  CoordinateType::Types const &type=CoordinateType::Cartesian,
			  bool const &anglesInDegrees=true);

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
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
    */
    bool setSkyPositions (Matrix<double> const &skyPositions,
			  CoordinateType::Types const &type=CoordinateType::Cartesian,
			  bool const &anglesInDegrees=true);

    bool setSkyPositions (Vector<double> const &skyPositions,
			  CoordinateType::Types const &type=CoordinateType::Cartesian,
			  bool const &anglesInDegrees=true);
    
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
      \brief Get the number of antenna positions for which to compute the delays
      
      \return nofAntPositions -- The number of antenna positions stored for
              the computation of the geometrical delays
    */
    inline uint nofAntPositions () {
      return antPositions_p.nrow();
    }

    /*!
      \brief Get the number of sky positions for which to compute the delays
      
      \return nofSkyPositions -- The number of sky positions stored for the
              computation of the geometrical delays
    */
    inline uint nofSkyPositions () {
      return skyPositions_p.nrow();
    }
    
    /*!
      \brief Get the shape of the array storing the geometrical weights
      
      \return shape -- [nofAntennas,nofSkyPositions] The shape of the array
              storing the values of the geometrical weights
    */
    inline casa::IPosition shape () {
      return casa::IPosition(2,nofAntPositions(),nofSkyPositions());
    }

    //! Get the number of antenna baselines
    inline uint nofBaselines () {
      return antPositions_p.nrow()*(antPositions_p.nrow()-1)/2;
    }
    
    /*!
      \brief Get the geometrical delays
      
      \return delays -- [ant,sky] The geometrical delays for the given set of
              antenna and sky positions.
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

  protected:
    
    //! Set/Update the values of the geometrical delays if required
    virtual void setDelays ();
    
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
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    void init (Matrix<double> const &antPositions,
	       Matrix<double> const &skyPositions,
	       bool const &anglesInDegrees=true,
	       bool const &farField=false,
	       bool const &bufferDelays=false);

    /*!
      \brief Initialize internal parameters

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param anglesInDegrees -- If the coordinates of the antenna positions
             contain an angular component, is this component given in degrees?
	     If set <tt>false</tt> the angular components are considered to be
	     given in radians.
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
    */
    void init (Matrix<double> const &antPositions,
	       CoordinateType::Types const &antCoord,
	       Matrix<double> const &skyPositions,
	       CoordinateType::Types const &skyCoord,
	       bool const &anglesInDegrees=true,
	       bool const &farField=false,
	       bool const &bufferDelays=false);

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
    void init (Vector<MVPosition> const &antPositions,
	       Vector<MVPosition> const &skyPositions,
	       bool const &farField=false,
	       bool const &bufferDelays=false);
    
  }; // Class GeomDelay -- end
  
} // Namespace CR -- end

#endif /* GEOMDELAY_H */

