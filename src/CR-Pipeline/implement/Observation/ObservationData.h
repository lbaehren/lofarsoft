/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: ObservationData.h,v 1.11 2006/11/27 10:54:04 bahren Exp $ */

#ifndef OBSERVATIONDATA_H
#define OBSERVATIONDATA_H

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/MeasTable.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/MVTime.h>

using casa::Matrix;
using casa::MDirection;
using casa::MEpoch;
using casa::MeasFrame;
using casa::MPosition;
using casa::ObsInfo;
using casa::Quantity;
using casa::String;

/*!
  \class ObservationData

  \ingroup Observation

  \brief Store information describing an observation.
  
  \author Lars B&auml;hren

  \date 2005/04/15

  \test tObservationData.cc

  \attention This class is soon -- in combination with SkymapGrid -- 
  to replace ObservationFrame, CoordinatesWCS and SkymapGrids.

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/measures/implement/Measures/MDirection.html">MDirection</a> -- A Measure: astronomical direction.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/measures/implement/Measures/MEpoch.html">MEpoch</a> -- A Measure: instant in time.
    <li>[AIPS++] MPosition -- A Measure: position on Earth.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/measures/implement/Measures/MeasRef.html">MeasRef</a> -- Reference frame for physical measures.
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/coordinates/implement/Coordinates/ObsInfo.html">ObsInfo</a> -- Store miscellaneous information related to an
        observation.
  </ul>
 */

class ObservationData {

  // A short description of the observation/experiment.
  String description_p;

  // Miscellaneous information related to an observation.
  ObsInfo obsInfo_p;

  // The location of the observation site
  MPosition obsPosition_p;

  //! 3D antenna positions w.r.t. to the observatory position, [nCoord,nAntenna]
  Matrix<double> antennaPositions_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor.
  */
  ObservationData ();

  /*!
    \brief Argumented constructor

    \param observatory -- The name of the observatory.
  */
  ObservationData (const String observatory);

  /*!
    \brief Argumented constructor.

    \param epoch       -- The epoch/time of the observation.
    \param observatory -- The name of the observatory.
  */
  ObservationData (const Quantity epoch,
		   const String observatory);

  /*!
    \brief Argumented constructor.

    \param epoch       -- The epoch/time of the observation.
    \param observatory -- The name of the observatory.
  */
  ObservationData (const MEpoch epoch,
		   const String observatory);
  
  /*!
    \brief Argumented constructor.
    
    \param obsDate          -- The epoch/time of the observation.
    \param obsName          -- The name of the observatory.
    \param antennaPositions -- 3D antenna positions w.r.t. to the observatory
                               position, [nCoord,nAntenna]
    \param observer         -- The name (or initials) of the observer.
  */
  ObservationData (const MEpoch obsDate,
		   String const &obsName,
		   Matrix<double> const &antennaPositions,
		   String const &observer);
  
  /*!
    \brief Argumented constructor.
    
    \param obsDate          -- The epoch/time of the observation.
    \param obsName          -- The name of the observatory.
    \param obsPosition      -- The position of the observatory in global geocentric
                               coordinates.
    \param antennaPositions -- 3D antenna positions w.r.t. to the observatory
                               position, [nCoord,nAntenna]
    \param observer         -- The name (or initials) of the observer.
  */
  ObservationData (const MEpoch obsDate,
		   String const &obsName,
		   const MPosition obsPosition,
		   Matrix<double> const &antennaPositions,
		   String const &observer);
  
  /*!
    \brief Argumented constructor.

    \param obsInfo -- Observation information (epoch, telescope, observer)
                      encapsulated in an ObsInfo object.
  */
  ObservationData (const ObsInfo obsInfo);
  
  /*!
    \brief Copy constructor

    \param other -- Another ObservationData object from which to create this new
                    one.
  */
  ObservationData (ObservationData const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor.
  */
  ~ObservationData ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another ObservationData object from which to make a copy.
  */
  ObservationData &operator= (ObservationData const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief A short description of the observation/experiment.
    
    \return description -- A short description of the observation/experiment.
  */
  String description () const {
    return description_p;
  }

  /*!
    \brief A short description of the observation/experiment.
    
    \param description -- A short description of the observation/experiment.
  */
  void setDescription (const String description) {
    description_p = description;
  }

  /*!
    \brief Get the epoch/time of the observation.

    \return epoch -- The epoch/time of the observation.
  */
  MEpoch epoch () const {
    return obsInfo_p.obsDate();
  }
  
  /*!
    \brief Set the epoch/time of the observation.

    \param epoch -- The epoch/time of the observation, given as Quantity.
  */
  void setEpoch (const Quantity epoch);

  /*!
    \brief Set the epoch/time of the observation.

    \param epoch -- The epoch/time of the observation, given as MEpoch.
  */
  void setEpoch (const MEpoch epoch);

  // --- Observatory -------------------------------------------------

  /*!
    \brief Get the name of the observatory/telescope.

    \return obsName -- The name of the observatory.
   */
  String observatory () const {
      return obsInfo_p.telescope();
  }

  /*!
    \brief Set the name of the Observatory.

    Set the name of the Observatory. Given the name, the related position is 
    attempted to be read from the data repository; if the telescope cannot be
    found there, the user is asked for further advice..

    \param obsName -- The name of the observatory.
   */
  void setObservatory (const String obsName);

  /*!
    \brief Get the position of the observatory/telescope.

    \return obsPosition -- The position of the observatory/telescope.
  */
  MPosition observatoryPosition () const {
    return obsPosition_p;
  }

  /*!
    \brief Get the position of the observatory/telescope.

    \param telescope -- The name of the observatory.

    \return obsPosition -- The position of the observatory/telescope.
   */
  static MPosition observatoryPosition (String const &telescope);

  /*!
    \brief Set name and position of the observatory

    You should use this, if your observatory is not available in the data
    repository.

    \param obsName     -- The name of the observatory.
    \param obsPosition -- The position of the observatory in global geocentric
                          coordinates.
   */
  void setObservatory (const String obsName,
		       const MPosition obsPosition);

  /*!
    \brief Get the number of antennas of the observatory 

    \return nofAntennas -- The number of antennas of the observatory 
  */
  uint nofAntennas ();

  /*!
    \brief Get the antenna positions

    \param coordAxisFirst    -- Return the coordinates per antenna as the first
                                axis of the array, i.e. [nCoord,nAntenna]. If
				<tt>coordAxisFirst=false</tt> the antenna
				positions will be return as [nAntenna,nCoord].

    \return antennaPositions -- 3D antenna positions w.r.t. to the observatory
                                position, [nCoord,nAntenna]

    As they may be different conventions about how to organized the array storing
    the 3-dimensional antenna positions, the <tt>coordAxisFirst</tt> parameter
    can be used to switch beteen different representations:
    \verbatim
    coordinate axis first : Axis Lengths: [3, 10]  (NB: Matrix in Row/Column order)
    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9
     0, 2, 4, 6, 8, 10, 12, 14, 16, 18
     0, 1.5, 3, 4.5, 6, 7.5, 9, 10.5, 12, 13.5]
    
    antenna number first  : Axis Lengths: [10, 3]  (NB: Matrix in Row/Column order)
    [0, 0, 0
     1, 2, 1.5
     2, 4, 3
     3, 6, 4.5
     4, 8, 6
     5, 10, 7.5
     6, 12, 9
     7, 14, 10.5
     8, 16, 12
     9, 18, 13.5]
    \endverbatim
  */
  Matrix<double> antennaPositions (bool const &coordAxisFirst=true);

  /*!
    \brief Set the antenna positions

    \param antennaPositions -- 3D antenna positions w.r.t. to the observatory
                               position, [nCoord,nAntenna], in units of [m]
    \param coordAxisFirst   -- If set <i>true</i> the array is expected to be
                               organized as [nCoord,nAntenna]; if
			       <tt>coordAxisFirst=false</tt> the array is
			       considered to be organized as [nAntenna,nCoord],
			       such that conversion takes place before storage.

    \todo Do we need to check on the shape, to ensure that we really get 3-dim
    position information?
  */
  void setAntennaPositions (Matrix<double> const &antennaPositions,
			    bool const &coordAxisFirst=true);
  
  // --- Observer ----------------------------------------------------

  /*!
    \brief The name (or initials) of the observer.

    \return observer -- The name (or initials) of the observer.
   */
  String observer () const {
    return obsInfo_p.observer();
  }

  /*!
    \brief The name (or initials) of the observer.

    \param observer -- The name (or initials) of the observer.
  */
  void setObserver (const String observer) {
    obsInfo_p.setObserver(observer);
  }
  
  // --- Data derived from basic information -------------------------

  /*!
    \brief Retrieve observation information (epoch, telescope, observer).
    
    \return obsInfo -- Observation information (epoch, telescope, observer)
                       encapsulated in an ObsInfo object.
  */
  ObsInfo obsInfo () const {
    return obsInfo_p;
  }

  /*!
    \brief Store observation information (epoch, telescope, observer).
    
    \param obsInfo        -- Observation information (epoch, telescope, observer)
                             encapsulated in an ObsInfo object.
    \param getObsPosition -- Try to set the observatory position from the
                             observatory table?
  */
  void setObsInfo (ObsInfo const &obsInfo,
		   bool const &getObsPosition=true);

  // ------------------------------------- Coordinate transformations/conversions
  
  /*!
    \brief Reference frame constructed from observation data.
    
    \return measFrame -- Reference frame for physical measures, created from
            observation data.
  */
  MeasFrame getObservationFrame ();

  /*!
    \brief Create an engine for converting coordinates between reference frames.

    \param refcode -- Reference code of the celestial coordinate system.
    \param toLocal -- Convert from celestial coordinate frame to local 
           coordinate frame (AZEL).

    \return conv -- A Measures based engine to convert directions given in
            one reference frame to another.
   */
  MDirection::Convert conversionEngine (const String refcode,
					const bool toLocal);

  /*!
    \brief Create an engine for converting coordinates between reference frames.

    \param refcodeTO   -- Reference code of the target (celestial) coordinate
           system.
    \param refcodeFROM -- Reference code of the source (celestial) coordinate
           system.

    \return conv -- A Measures based engine to convert directions given in
            one reference frame to another.
   */
  MDirection::Convert conversionEngine (const String refcodeTO,
					const String refcodeFROM);

  // ------------------------------------------------------------------- feedback
  
  /*!
    \brief Provide a summary of the internal parameters to standard output
  */
  void summary ();
  
  /*!
    \brief Provide a summary of the internal parameters
    
    \param os -- Output stream, to which the summary is written
  */
  void summary (std::ostream &os);
  
 private:

  /*!
    \brief Unconditional copying
  */
  void copy (ObservationData const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Initialize the internal data.

    \param obsInfo -- Observation information (epoch, telescope, observer)
           encapsulated in an ObsInfo object.
   */
  void init (const ObsInfo obsInfo);
  
  /*!
    \brief Initialize the internal data.
    
    \param obsDate          -- The epoch/time of the observation.
    \param obsName          -- The name of the observatory.
    \param antennaPositions -- 3D antenna positions w.r.t. to the observatory
                               position, [nCoord,nAntenna]
    \param observer         -- The name (or initials) of the observer.
   */
  void init (const MEpoch obsDate,
	     String const &obsName,
	     Matrix<double> const &antennaPositions,
	     String const &observer);
  
  /*!
    \brief Initialize the internal data.
    
    \param obsDate          -- The epoch/time of the observation.
    \param obsName          -- The name of the observatory.
    \param obsPosition      -- The position of the observatory in global geocentric
                               coordinates.
    \param antennaPositions -- 3D antenna positions w.r.t. to the observatory
                               position, [nCoord,nAntenna]
    \param observer         -- The name (or initials) of the observer.
   */
  void init (const MEpoch obsDate,
	     String const &obsName,
	     const MPosition obsPosition,
	     Matrix<double> const &antennaPositions,
	     String const &observer);

  /*!
    \brief Set the observatory position from the observatory name.

    Set the position of the observatory in global geocentric coordinates, based
    on the information stored in the Observatory table of the data repository.
   */
  void setObservatoryPosition ();

};

#endif
