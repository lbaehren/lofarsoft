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

#ifndef TTL_BEAMFORMING_H
#define TTL_BEAMFORMING_H

// SYSTEM INCLUDES
#include <cmath>
#include <complex>

// PROJECT INCLUDES
#include <ttl/math.h>
#include <ttl/constants.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

namespace ttl
{
  namespace beamforming
  {
    /*!
      \brief Returns the interferometer phase in radians for a given
      frequency and time.

      \param frequency Frequency in Hz
      \param time Time in seconds
     */
    template <class T0, class T1>
      double phase(const T0 &frequency, const T1 &time)
      {
        return 2*M_PI*frequency*time;
      }

    /*!
      \brief Calculates the complex weights for given delays and frequencies

      \param weights array of complex with length (delays * frequencies)
      \param delays delays in seconds
      \param frequencies frequencies in Hz
     */
    template <class CIter, class DIter>
      bool complexWeights(CIter weights, CIter weights_end,
                          DIter delays, DIter delays_end,
                          DIter frequencies, DIter frequencies_end)
      {
        CIter w=weights;
        DIter d=delays;
        DIter f=frequencies;

        while (w!=weights_end && d!=delays_end)
        {
          // Repeat frequencies until weights array is full
          f=frequencies;
          while (f!=frequencies_end && w!=weights_end)
          {
            *w=std::exp(std::complex<double>(0.0, static_cast<double>(phase(*f, *d))));

            ++f;
            ++w;
          }
          ++d;
        }

        // Check if all arrays have been processed correctly
        if (w==weights_end && d==delays_end && f==frequencies_end)
        {
          return true;
        }
        else
        {
          return false;
        }
      }

    /*!
      \brief Calculates the time delay in seconds for a signal received at
      an antenna position relative to a phase center from a source located
      in a certain direction in farfield.
     */
    template <class DIter, class T>
      double geometricDelayFarField(const DIter antPosition,
                                    const DIter skyDirection,
                                    const T length)
      {
        return - (*skyDirection * *antPosition
            + *(skyDirection+1) * *(antPosition+1)
            + *(skyDirection+2) * *(antPosition+2))/length/constants::LIGHT_SPEED;
      }

    /*!
      \brief Calculates the time delay in seconds for a signal received at
      an antenna position relative to a phase center from a source located
      at a certain 3D space coordinate in nearfield.
     */
    template <class DIter, class T>
      double geometricDelayNearField(const DIter antPosition,
                                     const DIter skyPosition,
                                     const T distance)
      {
        return (std::sqrt(
                    pow(*skyPosition - *antPosition,2)
                    +pow(*(skyPosition+1) - *(antPosition+1),2)
                    +pow(*(skyPosition+2) - *(antPosition+2),2)
                    ) - distance
               )/constants::LIGHT_SPEED;
      }

    /*!
      \brief Calculates the time delay in seconds for signals received at
      various antenna positions relative to a phase center from sources
      located at certain 3D space coordinates in near or far field.

      \param delays Output vector containing the delays in seconds for all
      antennas and positions [antenna index runs fastest:
      (ant1,pos1),(ant2,pos1),...] - length of vector has to be number of
      antennas times positions.
      \param antPositions Cartesian antenna positions (Meters) relative to
      a reference location (phase center) - vector of length number of
      antennas times three.
      \param skyPositions Vector in Cartesian coordinates (Meters) pointing
      towards a sky location, relative to phase center - vector of length
      number of skypositions times three.
      \param farfield Calculate in farfield approximation if true, otherwise
      do near field calculation.
    */
    template <class DIter>
      void geometricDelays(const DIter delays,
                           const DIter delays_end,
                           const DIter antPositions,
                           const DIter antPositions_end,
                           const DIter skyPositions,
                           const DIter skyPositions_end,
                           const bool farfield)
      {
        double distance;
        DIter
          ant=antPositions,
          sky=skyPositions,
          del=delays,
          ant_end=antPositions_end-2,
          sky_end=skyPositions_end-2;

        if (farfield)
        {
          while (del < delays_end)
          {
            distance = math::norm(sky,sky+3);
            *del=geometricDelayFarField(ant,sky,distance);
            ++del;
            ant+=3;

            if (ant>=ant_end)
            {
              ant=antPositions;
              sky+=3;
              if (sky>=sky_end) sky=skyPositions;
            };
          };
        }
        else
        {
          while (del < delays_end)
          {
            distance = math::norm(sky,sky+3);
            *del=geometricDelayNearField(ant,sky,distance);
            ++del;
            ant+=3;

            if (ant>=ant_end)
            {
              ant=antPositions;
              sky+=3;
              if (sky>=sky_end) sky=skyPositions;
            };
          };
        };
      }

    /*!
      \brief Calculates the phase gradients for signals received at various
      antenna positions relative to a phase center from sources located at
      certain 3D space coordinates in near or far field and for differentfrequencies.
    */
    template <class DIter>
      void geometricPhases(const DIter phases,
                           const DIter phases_end,
                           const DIter frequencies,
                           const DIter frequencies_end,
                           const DIter antPositions,
                           const DIter antPositions_end,
                           const DIter skyPositions,
                           const DIter skyPositions_end,
                           const bool farfield
                           )
      {
        double distance;
        DIter
          ant,
          freq,
          sky=skyPositions,
          phi=phases,
          ant_end=antPositions_end-2,
          sky_end=skyPositions_end-2;

        if (farfield)
        {
          while (sky < sky_end && phi < phases_end)
          {
            distance = math::norm(sky,sky+3);
            ant=antPositions;
            while (ant < ant_end && phi < phases_end)
            {
              freq=frequencies;
              while (freq < frequencies_end && phi < phases_end)
              {
                *phi=phase(*freq,geometricDelayFarField(ant,sky,distance));
                ++phi; ++freq;
              };
              ant+=3;
            };
            sky+=3;
          };
        }
        else
        {
          while (sky < sky_end && phi < phases_end)
          {
            distance = math::norm(sky,sky+3);
            ant=antPositions;
            while (ant < ant_end && phi < phases_end)
            {
              freq=frequencies;
              while (freq < frequencies_end && phi < phases_end)
              {
                *phi=phase(*freq,geometricDelayNearField(ant,sky,distance));
                ++phi; ++freq;
              };
              ant+=3;
            };
            sky+=3;
          };
        };
      }

    /*!
      \brief Calculates the phase gradients as complex weights for signals
      received at various antenna positions relative to a phase center from
      sources located at certain 3D space coordinates in near or far field
      and for different frequencies.
    */
    template <class CIter, class DIter>
      void geometricWeights(const CIter weights,
                            const CIter weights_end,
                            const DIter frequencies,
                            const DIter frequencies_end,
                            const DIter antPositions,
                            const DIter antPositions_end,
                            const DIter skyPositions,
                            const DIter skyPositions_end,
                            const bool farfield
                            )
      {
        double distance;
        DIter
          ant,
          freq,
          sky=skyPositions,
          ant_end=antPositions_end-2,
          sky_end=skyPositions_end-2;
        CIter weight=weights;

        if (farfield)
        {
          while (sky < sky_end && weight < weights_end)
          {
            distance = math::norm(sky,sky+3);
            ant=antPositions;
            while (ant < ant_end && weight < weights_end)
            {
              freq=frequencies;
              while (freq < frequencies_end && weight < weights_end)
              {
                *weight=exp(std::complex<double>(0.0,phase(*freq,geometricDelayFarField(ant,sky,distance))));
                ++weight; ++freq;
              };
              ant+=3;
            };
            sky+=3;
          };
        }
        else
        {
          while (sky < sky_end && weight < weights_end)
          {
            distance = math::norm(sky,sky+3);
            ant=antPositions;
            while (ant < ant_end && weight < weights_end)
            {
              freq=frequencies;
              while (freq < frequencies_end && weight < weights_end)
              {
                *weight=exp(std::complex<double>(0.0,phase(*freq,geometricDelayNearField(ant,sky,distance))));
                ++weight; ++freq;
              };
              ant+=3;
            };
            sky+=3;
          };
        };
      }
  } // End beamforming
} // End ttl

#endif // TTL_BEAMFORMING_H

