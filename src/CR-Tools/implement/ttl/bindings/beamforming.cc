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
#include <ttl/beamforming.h>

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

    double phase(const double &frequency, const double &time)
    {
      return ttl::beamforming::phase(frequency, time);
    }

    bool complexWeights(boost::python::numeric::array weights, boost::python::numeric::array delays, boost::python::numeric::array frequencies)
    {
      return ttl::beamforming::complexWeights(ttl::numpyBeginPtr< std::complex<double> >(weights), ttl::numpyEndPtr< std::complex<double> >(weights), ttl::numpyBeginPtr<double>(delays), ttl::numpyEndPtr<double>(delays), ttl::numpyBeginPtr<double>(frequencies), ttl::numpyEndPtr<double>(frequencies));
    }

    double geometricDelayFarField(boost::python::numeric::array antPosition, boost::python::numeric::array skyDirection, const double length)
    {
      return ttl::beamforming::geometricDelayFarField(ttl::numpyBeginPtr<TYPE0>(antPosition), ttl::numpyBeginPtr<TYPE1>(skyDirection), length);
    }

    double geometricDelayNearField(boost::python::numeric::array antPosition, boost::python::numeric::array skyPosition, const double distance)
    {
      return ttl::beamforming::geometricDelayNearField(ttl::numpyBeginPtr<TYPE0>(antPosition), ttl::numpyBeginPtr<TYPE1>(skyPosition), distance);
    }

    void geometricDelays(boost::python::numeric::array delays, boost::python::numeric::array antPositions, boost::python::numeric::array skyPositions, const bool farfield)
    {
      ttl::beamforming::geometricDelays(ttl::numpyBeginPtr<TYPE0>(delays), ttl::numpyEndPtr<TYPE0>(delays), ttl::numpyBeginPtr<TYPE1>(antPositions), ttl::numpyEndPtr<TYPE1>(antPositions), ttl::numpyBeginPtr<TYPE2>(skyPositions), ttl::numpyEndPtr<TYPE2>(skyPositions), farfield);
    }

    void geometricPhases(boost::python::numeric::array phases, boost::python::numeric::array frequencies, boost::python::numeric::array antPositions, boost::python::numeric::array skyPositions, const bool farfield)
    {
      ttl::beamforming::geometricPhases(ttl::numpyBeginPtr<TYPE0>(phases), ttl::numpyEndPtr<TYPE0>(phases), ttl::numpyBeginPtr<TYPE1>(frequencies), ttl::numpyEndPtr<TYPE1>(frequencies), ttl::numpyBeginPtr<TYPE2>(antPositions), ttl::numpyEndPtr<TYPE2>(antPositions), ttl::numpyBeginPtr<TYPE3>(skyPositions), ttl::numpyEndPtr<TYPE3>(skyPositions), farfield);
    }

    void geometricWeights(boost::python::numeric::array weights, boost::python::numeric::array frequencies, boost::python::numeric::array antPositions, boost::python::numeric::array skyPositions, const bool farfield)
    {
      ttl::beamforming::geometricWeights(ttl::numpyBeginPtr<TYPE0>(weights), ttl::numpyEndPtr<TYPE0>(weights), ttl::numpyBeginPtr<TYPE1>(frequencies), ttl::numpyEndPtr<TYPE1>(frequencies), ttl::numpyBeginPtr<TYPE2>(antPositions), ttl::numpyEndPtr<TYPE2>(antPositions), ttl::numpyBeginPtr<TYPE3>(skyPositions), ttl::numpyEndPtr<TYPE3>(skyPositions), farfield);
    }

    } // End bindings
} // End ttl

BOOST_PYTHON_MODULE(beamforming)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  def("phase", ttl::bindings::phase, "Returns the interferometer phase in radians for a given\n      frequency and time.\n\n*&frequency* \n*&time* \n");
  def("complexWeights", ttl::bindings::complexWeights, "Calculates the complex weights for given delays and frequencies\n\n*weights* array of complex with length (delays * frequencies)\n*weights_end* \n*delays* delays in seconds\n*delays_end* \n*frequencies* frequencies in Hz\n*frequencies_end* \n");
  def("geometricDelayFarField", ttl::bindings::geometricDelayFarField, "Calculates the time delay in seconds for a signal received at\n      an antenna position relative to a phase center from a source located\n      in a certain direction in farfield.\n\n*antPosition* \n*skyDirection* \n*length* \n");
  def("geometricDelayNearField", ttl::bindings::geometricDelayNearField, "Calculates the time delay in seconds for a signal received at\n      an antenna position relative to a phase center from a source located\n      at a certain 3D space coordinate in nearfield.\n\n*antPosition* \n*skyPosition* \n*distance* \n");
  def("geometricDelays", ttl::bindings::geometricDelays, "Calculates the time delay in seconds for signals received at\n      various antenna positions relative to a phase center from sources\n      located at certain 3D space coordinates in near or far field.\n\n      antennas and positions [antenna index runs fastest:\n      (ant1,pos1),(ant2,pos1),...] - length of vector has to be number of\n      antennas times positions.\n      a reference location (phase center) - vector of length number of\n      antennas times three.\n      towards a sky location, relative to phase center - vector of length\n      number of skypositions times three.\n      do near field calculation.\n\n*delays* Output vector containing the delays in seconds for all\n*delays_end* \n*antPositions* Cartesian antenna positions (Meters) relative to\n*antPositions_end* \n*skyPositions* Vector in Cartesian coordinates (Meters) pointing\n*skyPositions_end* \n*farfield* Calculate in farfield approximation if true, otherwise\n");
  def("geometricPhases", ttl::bindings::geometricPhases, "Calculates the phase gradients for signals received at various\n      antenna positions relative to a phase center from sources located at\n      certain 3D space coordinates in near or far field and for differentfrequencies.\n\n*phases* \n*phases_end* \n*frequencies* \n*frequencies_end* \n*antPositions* \n*antPositions_end* \n*skyPositions* \n*skyPositions_end* \n*farfield* \n");
  def("geometricWeights", ttl::bindings::geometricWeights, "Calculates the phase gradients as complex weights for signals\n      received at various antenna positions relative to a phase center from\n      sources located at certain 3D space coordinates in near or far field\n      and for different frequencies.\n\n*weights* \n*weights_end* \n*frequencies* \n*frequencies_end* \n*antPositions* \n*antPositions_end* \n*skyPositions* \n*skyPositions_end* \n*farfield* \n");
}

