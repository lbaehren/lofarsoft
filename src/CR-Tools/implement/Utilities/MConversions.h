/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#ifndef MCONVERSIONS_H
#define MCONVERSIONS_H

// Standard library header files
#include <iostream>
#include <string>

#include <casa/aips.h>
#include <casa/string.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <coordinates/Coordinates/Projection.h>
#include <measures/Measures.h>
#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MCDirection.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/MeasTable.h>

using casa::String;
using casa::Quantity;

namespace CR { // Namespace CR -- begin
  
  /*!
    \file MConversions.h

    \ingroup CR_Utilities
    
    \brief A set of functions for conversions of CASA Measures
    
    \author Lars B&auml;hren

    \date 2007/09/07

    \test tMConversions.cc
    
    <h3>Prerequisite</h3>

    <ul>
      <li>casacore <a href="http://www.astron.nl/casacore/doc/html/group__Coordinates__module.html">Coordinates</a> module
      <li>casacore <a href="http://www.astron.nl/casacore/doc/html/group__Measures__module.html">Measures</a> module
    </ul>
    
    <h3>Synopsis</h3>

    <ul>
      <li>The Modified Julian Day (MJD) is the number of days (with decimal
      fraction of the day) that have elapsed since midnight at the beginning of
      Wednesday November 17, 1858. In terms of the Julian day:
      \f[ \hbox{MJD} = \hbox{JD} - 2,400,000.5 \f]
      The day is found by rounding downward, currently giving 54543. This number
      changes at midnight UT or TT. It is 2,400,001 less than the Julian day
      number of the afternoon half of the same day (which is the same as the JD
      at noon). It is a multiple of 7 on Wednesdays.<br>
      The MJD was introduced by the Smithsonian Astrophysical Observatory in 1957
      to record the orbit of Sputnik via an IBM 704 (36-bit machine) and using
      only 18 bits until 2576-08-07. MJD is the epoch of OpenVMS, using 63-bit
      date/time postponing the next Y2K campaign to 31-JUL-31086 02:48:05.47.
      <li>Unix Time uses January 1, 1970 as the epoch, but counts by the second,
      not by the day.
      \f[ \hbox{Unix Time} = (JD - 2440587.5) \times 86400 \f]
    </ul>

    Given the central role of the Measures model the documentation provided by
    both CASA and casacore is everything by clear when it comes to providing the
    help of how to implement the solution of a given problem. A simple example
    for this is how to set up Measures describing the position of the station and
    the individual antennas: whereas a position given in WGS84 coordinates
    requires the position values to be passed as Quantities, this is not the case
    a position in ITRF.
    <ul>
    <li>Position in WGS84 coordinates:
    \code
    MPosition pos_wgs84 (MVPosition(Quantity(25, "m"),     // Height
				    Quantity(20, "deg"),   // East longitude
				    Quantity(53, "deg")),  // Lattitude
			 MPosition::WGS84);                // Reference type
    \endcode
    <li>Position in ITRF coordinates
    \code
    MPosition pos_itrf (MVPosition(884084.2636,     // X
				   -4924578.7481,   // Y
				   3943734.3354),   // Z
			MPosition::ITRF);           // Reference type
    \endcode
    </ul>
    
    <h3>Example(s)</h3>

    <ol>
      <li>Get the MDirection type from the reference code of a coordinate
      system:
      \code
      casa::MDirection::Types mtype;
      mtype = MDirectionType ("B1950");
      \endcode
    </ol>
    
  */

  // ============================================================================
  //
  //  Directions
  //
  // ============================================================================

  /*!
    \brief Get MDirection type from the reference code of the coordinate frame
    
    \param refcode -- Reference code of the coordinate frame
    
    \return tp -- MDirection type
  */
  casa::MDirection::Types MDirectionType (String const &refcode);

  /*!
    \brief Get MDirection reference code from the type of the coordinate frame

    \param tp -- MDirection type

    \return refcode -- Reference code of the coordinate frame    
  */
  casa::String MDirectionName (casa::MDirection::Types const &tp);
  
  /*!
    \brief Get Projection type from reference code of the projection

    \param refcode -- Reference code of the projection, e.g. <i>AIT</i>

    \return tp -- Projection type
  */
  casa::Projection::Type ProjectionType (String const &refcode);

  // ============================================================================
  //
  //  Positions
  //
  // ============================================================================

  /*!
    \brief Retrieve the position of an observatory from the measures data

    \todo Replace all calls of ObservationData::observatoryPosition with this
          function to remove the need of creating an object first.

    \param observatory -- Name of the observatory for which to look up the
           position in the measures data tables.

    \return obsPosition -- The position of the observatory/telescope, 
            encapsulated in a casa::MPosition object, as e.g. used to create
	    a reference frame or a conversion engine.
  */
  casa::MPosition ObservatoryPosition (String const &observatory);

  // ============================================================================
  //
  //  Conversions
  //
  // ============================================================================

  /*!
    \brief Convert UNIX seconds (since Jan 1, 1970) to Julian Day

    \f[ \hbox{JD} = \frac{\hbox{UNIX}}{86400} + 2440587.5 \f]
    
    \param seconds -- UNIX seconds, as elapsed since Jan 1, 1970

    \return jd -- Julian Day
  */
  inline double unix2julianDay (uint const &seconds=0) {
    return seconds/86400+2440587.5;
  }
  
  /*!
    \brief Convert Julian Day into UNIX seconds
    
    \f[ \hbox{Unix Time} = (JD - 2440587.5) \times 86400 \f]
    
    \param jd -- Time as a Julian Day
    
    \return seconds -- Time as seconds elapsed since Jan 1, 1970.
  */
  inline uint julianDay2unix (double const &jd=0.0) {
    return uint((jd - 2440587.5)*86400);
  }
  
  /*!
    \brief Convert LOPES-type timestamp for observation epoch to measure

    \param JDR -- KASCADE-style timestamp (JDR)
    \param TL  -- KASCADE-style timestamp (TL)
    
    \return epoch -- Observation epoch as casa::MEpoch measures object
  */
  casa::MEpoch LOPES2MEpoch(casa::uInt JDR=0,
			    int TL=0);
  
  /*!
    \brief Set up a Measures conversion frame

    \param epoch -- Instant in time for which the conversion should be performed
    \param pos   -- Position in space to which the conversion is anchored.
    
    \return frame -- Measures conversion frame which can be used to set up a 
            conversion engine
  */
  inline casa::MeasFrame MeasuresFrame (casa::MEpoch const &epoch,
					casa::MPosition const &pos)
    {
      return casa::MeasFrame (epoch,
			      pos);
    }

  /*!
    \brief Set up a Measures conversion frame

    \param obsInfo -- 
    \param pos     -- Position in space to which the conversion is anchored.
    
    \return frame -- Measures conversion frame which can be used to set up a 
            conversion engine
  */
  inline casa::MeasFrame MeasuresFrame (casa::ObsInfo const &obsInfo,
					casa::MPosition const &pos)
    {
      return casa::MeasFrame (obsInfo.obsDate(),
			      pos);
    }
  
  /*!
    \brief Set up a Measures conversion frame

    \param obsInfo -- Container for information regarding the time and location
           of an observation

    \return frame -- Measures conversion frame which can be used to set up a 
            conversion engine
  */
  inline casa::MeasFrame MeasuresFrame (casa::ObsInfo const &obsInfo)
    {
      // extract the telescope position from the observation info
      casa::MPosition obsPosition = ObservatoryPosition (obsInfo.telescope());
      casa::MEpoch obsEpoch       = obsInfo.obsDate();
      // forward the function call
      return casa::MeasFrame (obsEpoch,
			      obsPosition);
    }
  
} // Namespace CR -- end

#endif /* MCONVERSIONS_H */
