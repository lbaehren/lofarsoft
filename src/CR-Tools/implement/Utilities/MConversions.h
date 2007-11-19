/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
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
#include <coordinates/Coordinates/Projection.h>
#include <measures/Measures.h>
#include <measures/Measures/MCDirection.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/MeasTable.h>

using casa::String;

namespace CR { // Namespace CR -- begin
  
  /*!
    \file MConversions.h

    \ingroup CR_Utilities
    
    \brief A set of functions for conversions of CASA Measures
    
    \author Lars B&auml;hren

    \date 2007/09/07

    \test tMConversions.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
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

  /*!
    \brief Get MDirection type from reference code of the coordinate frame
    
    \param refcode -- Reference code of the coordinate frame
    
    \return tp -- MDirection type
  */
  casa::MDirection::Types MDirectionType (String const &refcode);
  
  /*!
    \brief Get Projection type from reference code of the projection

    \param refcode -- Reference code of the projection, e.g. <i>AIT</i>

    \return tp -- Projection type
  */
  casa::Projection::Type ProjectionType (String const &refcode);

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
  
} // Namespace CR -- end

#endif /* MCONVERSIONS_H */
