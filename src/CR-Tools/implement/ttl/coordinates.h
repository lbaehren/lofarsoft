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
#include <iostream> // NOTE remove this

// PROJECT INCLUDES
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
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
                 const std::string telescope,
                 const double obsDate,
                 const double refLong, const double refLat,
                 const double incLong, const double incLat,
                 const double refX, const double refY
                 )
    {
      // Set up the internal coordinate system
      casa::CoordinateSystem cs;

      // Set observation information
      casa::ObsInfo obsinfo;

      // Observation date and time
      casa::MEpoch epoch(casa::MVEpoch(casa::Quantity(static_cast<casa::Double>(obsDate), "d")), casa::MEpoch::Ref(casa::MEpoch::UTC));

      obsinfo.setObsDate(epoch);

      // Observation telescope
      obsinfo.setTelescope(static_cast<casa::String>(telescope));

      // Add observation information to coordinate system
      cs.setObsInfo(obsinfo);

      // Add spatial direction information to coordinate system
      casa::Matrix<casa::Double> xform(2,2);
      xform = 0.0; xform.diagonal() = 1.0;

      casa::DirectionCoordinate dir(casa::MDirection::AZEL,
                                    casa::Projection(casa::Projection::STG),
                                    static_cast<casa::Double>(refLong),
                                    static_cast<casa::Double>(refLat),
                                    static_cast<casa::Double>(incLong),
                                    static_cast<casa::Double>(incLat),
                                    xform,
                                    static_cast<casa::Double>(refX),
                                    static_cast<casa::Double>(refY));
      
      cs.addCoordinate(dir);

      std::cout<<"-- World axis names        = "<<cs.worldAxisNames()<<std::endl;
      std::cout<<"-- World axis units        = " <<cs.worldAxisUnits()<<std::endl;
      std::cout<<"-- Reference pixel (CRPIX) = " <<cs.referencePixel()<<std::endl;
      std::cout<<"-- Increment       (CDELT) = " <<cs.increment()<<std::endl;
      std::cout<<"-- Reference value (CRVAL) = " <<cs.referenceValue()<<std::endl;
      std::cout<<"-- Telescope name          = " <<obsinfo.telescope()<<std::endl;
      std::cout<<"-- Observing date          = "<<obsinfo.obsDate()<<std::endl;

      // Generate grid
      Iter ix_it=ix_begin;
      Iter iy_it=iy_begin;
      Iter ox_it=ox_begin;
      Iter oy_it=oy_begin;

      // Placeholders for conversion
      casa::Vector<casa::Double> world(2), pixel(2); 

      // Loop over all pixels
      while (ix_it!=ix_end && iy_it!=iy_end && ox_it!=ox_end && oy_it!=oy_end)
      {
        // Get pixel coordinates into casa vector for conversion
        pixel[0]=static_cast<casa::Double>(*ix_it);
        pixel[1]=static_cast<casa::Double>(*iy_it);

        // Convert pixel to world coordinates
        cs.toWorld(world, pixel);

        // Retrieve world coordinates
        *ox_it=static_cast<double>(world[0]);
        *oy_it=static_cast<double>(world[1]);

        // Next pixel
        ++ix_it;
        ++iy_it;
        ++ox_it;
        ++oy_it;
      }
    }
} // End ttl

#endif // TTL_COORDINATES_H

