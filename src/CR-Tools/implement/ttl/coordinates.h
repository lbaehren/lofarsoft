/**************************************************************************
 *  This file is part of the Transient Template Library.                  *
 *  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        * 
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef TTL_COORDINATES_H
#define TTL_COORDINATES_H

// SYSTEM INCLUDES
#include <string>

// PROJECT INCLUDES
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <measures/Measures/MEpoch.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

namespace ttl
{
  /*!
    \brief Generate coordinate grid
   */
  template <class Iter>
    void getGrid(const Iter ox_begin, const Iter ox_end,
                 const Iter oy_begin, const Iter oy_end,
                 const Iter ix_begin, const Iter ix_end,
                 const Iter iy_begin, const Iter iy_end,
                 const std::string telescope)
    {
      // Set up the internal coordinate system
      casa::CoordinateSystem cs;

      // Set observation information
      casa::ObsInfo obsinfo;

      // Observation time
      casa::MEpoch epoch;
      obsinfo.setObsDate(epoch);

      // Observation telescope
      obsinfo.setTelescope(static_cast<casa::String>(telescope));

      // Add observation information to coordinate system
      cs.setObsInfo(obsinfo);
//      cs.addCoordinate (dir);

      // Generate grid
      Iter ix_it=ix_begin;
      Iter iy_it=iy_begin;
      Iter ox_it=ox_begin;
      Iter oy_it=oy_begin;

      while (ix_it!=ix_end)
      {
        *ox_it=*ix_it;
        *oy_it=*iy_it;

        ++ix_it;
        ++iy_it;
        ++ox_it;
        ++oy_it;
      }
    }
} // End ttl

#endif // TTL_COORDINATES_H

