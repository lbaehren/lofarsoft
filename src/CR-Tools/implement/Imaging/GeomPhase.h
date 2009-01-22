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

#ifndef GEOMPHASE_H
#define GEOMPHASE_H

// Standard library header files
#include <iostream>
#include <string>
// casacore header files
#include <casa/Arrays/Cube.h>
#include <casa/Quanta/MVFrequency.h>
// CR-Tools header files
#include <Imaging/GeomDelay.h>

using casa::Cube;
using casa::Matrix;
using casa::MVFrequency;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeomPhase
    
    \ingroup CR_Imaging
    
    \brief Brief description for class GeomPhase
    
    \author Lars B&auml;hren

    \date 2008/12/17

    \test tGeomPhase.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>CR::GeomDelay
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class GeomPhase : public GeomDelay {
    
  protected:

    //! Buffer the values of the phases?
    bool bufferPhases_p;
    //! Frequency values
    Vector<double> frequencies_p;
    //! The geometrical phases
    Cube<double> phases_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    GeomPhase ();
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (Vector<double> const &frequencies,
	       bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (Vector<MVFrequency> const &frequencies,
	       bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (GeomDelay const &geomDelay,
	       Vector<double> const &frequencies,
	       bool const &bufferPhases=false);
    
    /*!
      \brief Argumented constructor

      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (GeomDelay const &geomDelay,
	       Vector<MVFrequency> const &frequencies,
	       bool const &bufferPhases=false);
    
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
    */
    GeomPhase (Matrix<double> const &antPositions,
	       CoordinateType::Types const &antCoord,
	       Matrix<double> const &skyPositions,
	       CoordinateType::Types const &skyCoord,
	       bool const &anglesInDegrees,
	       bool const &farField,
	       bool const &bufferDelays,
	       Vector<double> const &frequencies,
	       bool const &bufferPhases);
    
    /*!
      \brief Argumented constructor

      \param antPositions -- [antenna,3], positions of the antennas
      \param skyPositions -- [position,3], positions towards which the beams are
             formed
      \param farField     -- Compute geometrical delay for far-field? By default
             no approximation is made and the full 3D geometry is taken into
	     account.
      \param bufferDelays -- Buffer the values of the geometrical delays?
      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    GeomPhase (Vector<MVPosition> const &antPositions,
	       Vector<MVPosition> const &skyPositions,
	       bool const &farField,
	       bool const &bufferDelays,
	       Vector<MVFrequency> const &frequencies,
	       bool const &bufferPhases);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another GeomPhase object from which to create this new
             one.
    */
    GeomPhase (GeomPhase const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    virtual ~GeomPhase ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another GeomPhase object from which to make a copy.
    */
    GeomPhase& operator= (GeomPhase const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Are the values of the phases getting buffered?

      \return bufferPhases -- Returns \e true in case the values of the
              geometrical phases are getting buffered and need not be recomputed
	      at each request
    */
    inline bool bufferPhases () const {
      return bufferPhases_p;
    }

    /*!
      \brief Are the values of the phases to be buffered?

      \param bufferPhases -- If set <tt>true</tt> the values of the geometrical
             phases are getting buffered and need not be recomputed for each 
	     request.
    */
    void bufferPhases (bool const &bufferPhases);

    /*!
      \brief Get the frequency values

      \return frequencies -- The frequency values for which the geometrical
              phases are computed
    */
    inline Vector<double> frequencies () const {
      return frequencies_p;
    }

    /*!
      \brief Set the frequency values

      \param frequencies -- The frequency values for which the geometrical
             phases are computed

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setFrequencies (Vector<double> const &frequencies);
    
    /*!
      \brief Set the frequency values

      \param frequencies -- The frequency values for which the geometrical
             phases are computed

      \return status -- Status of the operation; returns \e false in case an
              error was encountered.
    */
    bool setFrequencies (Vector<MVFrequency> const &frequencies);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, GeomPhase.
    */
    inline std::string className () const {
      return "GeomPhase";
    }

    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    //! Get the shape of the array holding the geometrical phases
    casa::IPosition shape();

    /*!
      \brief Compute the gemetrical phase

      \param delay -- The value of the geometrical delay, as computed for a
             combination of antenna and pointing position. [s]
      \param freq  -- The frequency. [Hz]

      \return phase -- The value of the geometrical phase
    */
    static double calcPhases (double const &delay,
			      double const &freq) {
      return CR::_2pi*freq*delay;
    }
    
    /*!
      \brief Compute the gemetrical phase

      \param delay -- The values of the geometrical delay, as computed for a
             combination of antenna and pointing positions. [s]
      \param freq  -- The frequency. [Hz]

      \return phase -- [ant,sky] The values of the geometrical phases
    */
    static Matrix<double> calcPhases (Matrix<double> const &delay,
				      double const &freq) {
      return CR::_2pi*freq*delay;
    }
    
    /*!
      \brief Compute the gemetrical phase

      \param delay -- The values of the geometrical delay, as computed for a
             combination of antenna and pointing positions. [s]
      \param freq  -- Frequency values. [Hz]

      \return phase -- [freq,ant,sky] The values of the geometrical phases
    */
    static Cube<double> calcPhases (Matrix<double> const &delay,
				    Vector<double> const &freq);
    
    /*!
      \brief Get the values of the geometrical phases

      \return phases -- [freq,ant,sky] The numerical values of the geometrical
              phases.
    */
    Cube<double> phases ();

  protected:

    virtual void setDelays();

    //! Compute and set the values of the geometrical phases
    virtual void setPhases();
    
  private:
    
    //! Unconditional copying
    void copy (GeomPhase const &other);
    
    //! Unconditional deletion 
    void destroy(void);

    /*!
      \brief Initialize internal parameters

      \param frequencies  -- The frequencies, [Hz], for which the phases are
             being computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    void init (Vector<double> const &frequencies,
	       bool const &bufferPhases=false);

    /*!
      \brief Initialize internal parameters

      \param frequencies  -- The frequencies for which the phases are being
             computed.
      \param bufferPhases -- Buffer the values of the phases?
    */
    void init (Vector<MVFrequency> const &frequencies,
	       bool const &bufferPhases=false);

    /*!
      \brief Get the values of the geometrical phases

      \param delays -- Array with the values of the geometrical delays for which
             the phases are to be computed.

      \return phases --  The numerical values of the geometrical phases.
    */
    Cube<double> phases (Matrix<double> const &delays);
    
  };
  
} // Namespace CR -- end

#endif /* GEOMPHASE_H */
  
