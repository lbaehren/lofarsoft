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


#ifndef OBSERVATIONFRAME_H
#define OBSERVATIONFRAME_H

/* $Id: ObservationFrame.h,v 1.3 2006/11/27 10:54:04 bahren Exp $ */

// C++ Standard library
#include <cstdio>
#include <sstream>
#include <fstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <tasking/Glish.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>

using casa::Cube;
using casa::IPosition;
using casa::Matrix;
using casa::MeasFrame;
using casa::MeasTable;
using casa::MEpoch;
using casa::MPosition;
using casa::Quantity;
using casa::String;
using casa::Time;
using casa::Vector;

/*!
  \class ObservationFrame

  \ingroup Observation

  \brief A class for the creation of a reference frame of an observation.

  \author Lars B&auml;hren

  <h3>Prerequisite</h3>

  <ul type=square">
  <li><a href="http://aips2.nrao.edu/docs/aips/implement/Quanta.html">AIPS++
  Quanta module</a> for units and quantities.
  <li><a href="http://aips2.nrao.edu/docs/aips/implement/Measures.html">AIPS++
  Measures module</a>:  Measures are physical quantities within a certain
  reference frame. Examples are the Hour-angle and Declination of a source at a
  certain time and observatory; an Ra/Dec for a certain mean epoch; an apparent
  frequency at a certain time given in eV; a local sidereal time at an observatory.
  <li><a href="http://aips2.nrao.edu/docs/aips/implement/Measures/MeasTable.html">MeasTable</a>:
  interface for all data that comes from Tables rather than the program.
  <li>The <a href="http://aips2.nrao.edu/docs/reference/System/node13.html">AIPS++
  data repository</a> contains all of the standard global data used internally by
  AIPS++.
  </ul>

  <h3>Synopsis</h3>

  This class stores and manages the main parameters associated with a measurment,
  i.e.
  <ul>
    <li>Time - as given by the epoch
    <li>Location - as given by the position of the observation (i.e. telescope)
	site
    <li>3D antenna positions w.r.t. to the phase center of the array; the latter
        can be either a central point of the array or the position of one of the
	antennas.
  </ul>
*/

class ObservationFrame {

  // Epoch of the observation
  Quantity epoch_p;
  // The name of the observation site
  String observatoryName_p;
  // The location of the observation site
  MPosition observatoryPosition_p;
  // Antenna positions w.r.t. the array phase center
  Matrix<double> antennaPositions_p;
  Vector<bool> antennaSelection_p;
  
 public:

  // === Construction / Deconstruction =========================================

  //! Empty constructor
  /*!
    This creates an <i>ObservationFrame</i> object using a set of default
    values (uses present time as default epoch); the values of the member data
    are set through the ObservationFrame::setObservationFrame function.
  */
  ObservationFrame();

  //! Argumented constructor
  /*! Given the name of the observatory the AIPS++ data respository is inspected
    to see wether this observatory is known to the system; if it is, the 
    position information is set as well.
    \param epoch - The epoch of the observation, provided as AIPS++ Quantity.
    \param obsName - The name of the observatory.
  */
  ObservationFrame(Quantity epoch,
		   String obsName);

  //! Argumented constructor
  /*!
    \param epoch - The epoch of the observation, provided as AIPS++ Quantity.
    \param obsPosition - The position of the observatory in global geocentric
    coordinates.
   */
  ObservationFrame(Quantity epoch,
		   MPosition obsPosition);

  //! Argumented constructor
  /*!
    \param epoch - The epoch of the observation, provided as AIPS++ Quantity.
    \param obsName - The name of the observatory.
    \param obsPosition - The position of the observatory in global geocentric
    coordinates.
   */
  ObservationFrame(Quantity epoch,
		   String obsName,
		   MPosition obsPosition);

  // === Initialization ========================================================

  //! Initialization of the main data members.
  /*!
    \param epoch   - The epoch of the observation, provided as AIPS++ Quantity.
    \param obsName - The name of the observatory.
  */
  void setObservationFrame (Quantity epoch,
			    String obsName);

  //! Initialization of the main data members.
  /*!
    \param epoch   - The epoch of the observation, provided as AIPS++ Quantity.
    \param obsName - The name of the observatory.
    \param antpos  - 3D positions of the antennae (in case of an array).
  */
  void setObservationFrame (Quantity epoch,
			    String obsName,
			    Matrix<double> antpos);

  //! Initialization of the main data members.
  /*!
    \param epoch       - The epoch of the observation, provided as AIPS++
                         Quantity.
    \param obsName     - The name of the observatory.
    \param obsPosition - The position of the observatory in global
                         geocentric coordinates.
  */
  void setObservationFrame (Quantity epoch,
			    String obsName,
			    MPosition obsPosition);
  
  /*!
    \brief Set up member data from Glish record  

    \param record - Glish record containing the following fields:
    Epoch, ObservatoryName
  */
  void setObservationFrame (casa::GlishRecord&);

  //! Get the epoch and the location of the observation
  /*!
    \retval epoch       - Epoch ob the observation.
    \retval obsPosition - Position of the observatory.
   */
  void getObservationFrame (Quantity& epoch,
			    MPosition& obsPosition);

  //! Get the epoch and the location as measures frame
  /*!
    \retval frame - A reference frame constructed of the observation epoch and 
    location.
   */
  MeasFrame getObservationFrame ();

  // === Epoch of the observation ==============================================

  /*!
    \brief Set the epoch of the observation.

    (Re)Set the epoch of the observation. Input is a AIPS++ quantity, i.e. a
    value with a physical unit.
  */
  void setEpoch (const Quantity epoch);

  /*!
    \brief Set the epoch of the observation.

    \param value = Value of the quantity, e.g. time in days.
    \param unit  = Unit of the quantity, e.g. days.
   */
  void setEpoch (const double value,
		 const String unit);

  //! Get the time of observation (epoch) as AIPS++ Quantity.
  void epoch (Quantity& epoch);

  //! Get the time of observation (epoch) as AIPS++ Measure.
  void epoch (MEpoch& epoch);

  // === Name and location of the Observatory ==================================

  /*!
    \brief Set the name of the Observatory.

    Set the name of the Observatory. Given the name, the related position is 
    attempted to be read from the data repository; if the telescope cannot be
    found there, the user is asked for further advice..
   */
  void setObservatory (const String observatoryName);

  /*!
    \brief Set the position of the observatory.

    Provided a fully specified geodetic position of the observatory; this in most
    cases will be a 3-dim within a global geodetic reference frame (e.g. WGS84)
    
    \todo Check if the input really has te correct format, i.e. if the position
    has valid values or the supplied reference code is known to the system. Further
    enable reverse lookup, i.e. scan the data repository, wether the Observatory
    name to the provided values can be found in the database.
  */
  void setObservatory (MPosition);

  /*!
    \brief Set name and position of the observatory

    \param obsName - The name of the observatory.
    \param obsPosition - The position of the observatory in global geocentric
           coordinates.
   */
  void setObservatory (String obsName,
		       MPosition obsPosition);

  /*!
    \brief Get the name of the observatory.

    \return observatoryName - The name of the observatory.
  */
  String getObservatoryName ();

  //! Get the position of the observatory as AIPS++ Measure.
  void getObservatory (MPosition&);

  //! Get the name (as string) and the position of the observatory (as measure).
  void getObservatory (String&,MPosition&);

  // === Array antennae ========================================================

  //! Set the 3D position of the antennae.
  void setAntennaPositions (const Matrix<double> antennaPositions);

  //! Get the 3D positions of the antennae.
  Matrix<double> antennaPositions ();

  //! Get the geometrical baselines between the antennae of the array.
  /*!
    Get the geometrical baselines between the antennae of the array: for two
    antennae at positions \f$ x_i \f$ and \f$ x_j \f$ we have \f$ u_{ij} = 
    x_j - x_i \f$.
   */
  Cube<double> baselines ();

  //! Of how many antennae does the array consist?
  int nofAntennae ();

  //! Number of selected/deselected antennae.
  int nofAntennae (bool);

  // === Debugging output ======================================================

  //! Print the values of the member data
  void show (std::ostream&);
  
 private: 
  /*!
    Set the observatory position by looking up the observatory name in the data
    repository.
  */
  void setObservatoryPosition ();

};

#endif
