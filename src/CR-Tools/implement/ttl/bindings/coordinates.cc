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

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
#include <ttl/coordinates.h>

// LOCAL INCLUDES
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"
#include "numpy_ptr.h"

// FORWARD REFERENCES
//

namespace ttl
{
  namespace bindings
  {

    bool toWorld(boost::python::numeric::array world, boost::python::numeric::array pixel, const std::string refcode, const std::string projection, const double refLong, const double refLat, const double incLong, const double incLat, const double refX, const double refY)
    {
      return ttl::coordinates::toWorld(ttl::numpyBeginPtr<double>(world), ttl::numpyEndPtr<double>(world), ttl::numpyBeginPtr<double>(pixel), ttl::numpyEndPtr<double>(pixel), refcode, projection, refLong, refLat, incLong, incLat, refX, refY);
    }

    bool toPixel(boost::python::numeric::array pixel, boost::python::numeric::array world, const std::string refcode, const std::string projection, const double refLong, const double refLat, const double incLong, const double incLat, const double refX, const double refY)
    {
      return ttl::coordinates::toPixel(ttl::numpyBeginPtr<double>(pixel), ttl::numpyEndPtr<double>(pixel), ttl::numpyBeginPtr<double>(world), ttl::numpyEndPtr<double>(world), refcode, projection, refLong, refLat, incLong, incLat, refX, refY);
    }

    void azElRadius2Cartesian(boost::python::numeric::array out, boost::python::numeric::array in, bool anglesInDegrees)
    {
      ttl::coordinates::azElRadius2Cartesian(ttl::numpyBeginPtr<double>(out), ttl::numpyEndPtr<double>(out), ttl::numpyBeginPtr<double>(in), ttl::numpyEndPtr<double>(in), anglesInDegrees);
    }

  } // End bindings
} // End ttl

BOOST_PYTHON_MODULE(coordinates)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  def("toWorld", ttl::bindings::toWorld, "Get world coordinates for given vector of pixel coordinates\n\n*world* begin itterator for world coordinate array\n*world_end* end itterator for world coordinate array\n*pixel* begin itterator for pixel coordinate array\n*pixel_end* end itterator for pixel coordinate array\n*refcode* reference code for coordinate system e.g. AZEL,J2000,...\n*projection* the projection used e.g. SIN,STG,...\n*refLong* reference value for longtitude (CRVAL)\n*refLat* reference value for latitude (CRVAL)\n*incLong* \n*incLat* increment value for latitude (CDELT)\n*refX* reference x pixel (CRPIX)\n*refY* reference y pixel (CRPIX)\n");
  def("toPixel", ttl::bindings::toPixel, "Get pixel coordinates for given vector of world coordinates\n\n*pixel* begin itterator for pixel coordinate array\n*pixel_end* end itterator for pixel coordinate array\n*world* begin itterator for world coordinate array\n*world_end* end itterator for world coordinate array\n*refcode* reference code for coordinate system e.g. AZEL,J2000,...\n*projection* the projection used e.g. SIN,STG,...\n*refLong* reference value for longtitude (CRVAL)\n*refLat* reference value for latitude (CRVAL)\n*incLong* \n*incLat* increment value for latitude (CDELT)\n*refX* reference x pixel (CRPIX)\n*refY* reference y pixel (CRPIX)\n");
  def("azElRadius2Cartesian", ttl::bindings::azElRadius2Cartesian, "Given an array of (az, el, r, az, el, r, ...) coordinates\n      return an array of (x, y, z, x, y, z, ...) coordinates.\n\n*out* \n*out_end* \n*in* \n*in_end* \n*anglesInDegrees* \n");
}

