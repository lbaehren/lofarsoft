/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1964 2008-09-06 17:52:38Z baehren                    $ |
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

#ifndef GEOMWEIGHT_H
#define GEOMWEIGHT_H

// Standard library header files
#include <iostream>
#include <string>

// casacore header files
#include <casa/BasicSL/Complex.h>

// CR-Tools header files
#include <Imaging/GeomDelay.h>
#include <Imaging/GeomPhase.h>

using casa::Cube;
using casa::DComplex;
using casa::IPosition;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeomWeight
    
    \ingroup CR_Imaging
    
    \brief Brief description for class GeomWeight
    
    \author Lars B&auml;hren

    \date 2009/01/15

    \test tGeomWeight.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::GeomDelay
      <li>CR::GeomPhase
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class GeomWeight : public GeomPhase {

  protected:
    
    //! Buffer the values of the geometrical weights?
    bool bufferWeights_p;
    //! Array with the values of the geometrical weights
    Cube<DComplex> weights_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor

      \param bufferWeights -- Buffer the values of the weights?
    */
    GeomWeight (bool const &bufferWeights=false);
    
    /*!
      \brief Default constructor

      \param geomPhase     -- 
      \param bufferWeights -- Buffer the values of the weights?
    */
    GeomWeight (GeomPhase const &geomPhase,
		bool const &bufferWeights=false);
    
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
      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the weights?
    */
    GeomWeight (Matrix<double> const &antPositions,
		CoordinateType::Types const &antCoord,
		Matrix<double> const &skyPositions,
		CoordinateType::Types const &skyCoord,
		bool const &anglesInDegrees,
		bool const &farField,
		bool const &bufferDelays,
		Vector<double> const &frequencies,
		bool const &bufferPhases,
		bool const &bufferWeights=false);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- Positions of the antennas
      \param skyPositions -- Positions towards which the beams are formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
      \param bufferWeights -- Buffer the values of the weights?
    */
    GeomWeight (Vector<MVPosition> const &antPositions,
		Vector<MVPosition> const &skyPositions,
		bool const &farField,
		bool const &bufferDelays,
		Vector<MVFrequency> const &frequencies,
		bool const &bufferPhases,
		bool const &bufferWeights=false);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomWeight object from which to create this new
             one.
    */
    GeomWeight (GeomWeight const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~GeomWeight ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomWeight object from which to make a copy.
    */
    GeomWeight& operator= (GeomWeight const &other); 
    
    // --------------------------------------------------------------- Parameters

    inline IPosition shape () {
      return GeomPhase::shape();
    }

    /*!
      \brief Are the values of the weighs getting buffered?

      \return bufferWeights -- Returns \e true in case the values of the
              geometrical weights are getting buffered and need not be recomputed
	      at each request
    */
    inline bool bufferWeights () const {
      return bufferWeights_p;
    }

    /*!
      \brief Are the values of the weights to be buffered?

      \param bufferWeights -- If set <tt>true</tt> the values of the geometrical
             weights are getting buffered and need not be recomputed for each 
	     request.
    */
    void bufferWeights (bool const &bufferWeights);

    Cube<DComplex> weights ();
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeomWeight.
    */
    inline std::string className () const {
      return "GeomWeight";
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
      \brief Compute the geometrical weights from delays and frequencies

      \param delays -- [ant,sky] The values of the geometrical delays, for a
             given set of antenna and sky positions
      \param frequencies -- Frequency values, [Hz].

      return weights -- Geometrical weights
    */
    static Cube<DComplex> calcWeights (Matrix<double> const &delays,
				       Vector<double> const &frequencies);
    
    /*!
      \brief Compute the geometrical weights

      \param phases -- [ant,sky] The values of the geometrical phases, for a
             given set of antenna and sky positions
      \param frequencies -- Frequency values, [Hz].

      return weights -- Geometrical weights
    */
    static Cube<DComplex> calcWeights (Cube<double> const &phases);
    
  protected:
    
    //! Compute and set the values of the geometrical delays
    void setDelays ();
    
    //! Compute and set the values of the geometrical phases
    void setPhases ();
    
    //! Compute and set the values of the geometrical weights
    void setWeights ();
    
  private:
    
    //! Unconditional copying
    void copy (GeomWeight const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class GeomWeight -- end
  
} // Namespace CR -- end

#endif /* GEOMWEIGHT_H */
  
