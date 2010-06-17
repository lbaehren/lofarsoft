/**************************************************************************
 *   This file is part of ttl.                                            *
 *   Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>           *
 *                                                                        *
 *   ttl is free software: you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 3 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        * 
 *   ttl is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with ttl.  If not, see <http://www.gnu.org/licenses/>.         *
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
  /*!
    \brief Returns the interferometer phase in radians for a given
    frequency and time.

    \param frequency Frequency in Hz
    \param time Time in seconds
   */
  template <class T>
    double phase(const T &frequency, const T &time)
    {
      return 2*M_PI*frequency*time;
    }

  /*!
    \brief Calculates the time delay in seconds for a signal received at
    an antenna position relative to a phase center from a source located
    in a certain direction in farfield.
   */
  template <class Iter, class T>
    double geometricDelayFarField(const Iter antPosition,
                                  const Iter skyDirection,
                                  const double &length)
    {
      return - (*skyDirection * *antPosition
          + *(skyDirection+1) * *(antPosition+1)
          + *(skyDirection+2) * *(antPosition+2))/length/LIGHT_SPEED;
    }

  /*!
    \brief Calculates the time delay in seconds for a signal received at
    an antenna position relative to a phase center from a source located
    at a certain 3D space coordinate in nearfield.
   */
  template <class Iter>
    double geometricDelayNearField(const Iter antPosition,
                                   const Iter skyPosition,
                                   const double &distance)
    {
      return (std::sqrt(
                  pow(*skyPosition - *antPosition,2)
                  +pow(*(skyPosition+1) - *(antPosition+1),2)
                  +pow(*(skyPosition+2) - *(antPosition+2),2)
                  ) - distance
             )/LIGHT_SPEED;
    }

  /*!
    \brief Calculates the time delay in seconds for signals received at
    various antenna positions relative to a phase center from sources
    located at certain 3D space coordinates in near or far field.
  */
  template <class Iter>
    void geometricDelays(const Iter delays,
                         const Iter delays_end,
                         const Iter antPositions,
                         const Iter antPositions_end,
                         const Iter skyPositions,
                         const Iter skyPositions_end,
                         const bool farfield)
    {
      double distance;
      Iter
        ant=antPositions,
        sky=skyPositions,
        del=delays,
        ant_end=antPositions_end-2,
        sky_end=skyPositions_end-2;

      if (farfield)
      {
        while (del < delays_end)
        {
          distance = norm(sky,sky+3);
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
          distance = norm(sky,sky+3);
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
  template <class Iter>
    void geometricPhases(const Iter phases,
                         const Iter phases_end,
                         const Iter frequencies,
                         const Iter frequencies_end,
                         const Iter antPositions,
                         const Iter antPositions_end,
                         const Iter skyPositions,
                         const Iter skyPositions_end,
                         const bool farfield
                         )
    {
      double distance;
      Iter
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
          distance = norm(sky,sky+3);
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
          distance = norm(sky,sky+3);
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
  template <class CIter, class Iter>
    void geometricWeights(const CIter weights,
                          const CIter weights_end,
                          const Iter frequencies,
                          const Iter frequencies_end,
                          const Iter antPositions,
                          const Iter antPositions_end,
                          const Iter skyPositions,
                          const Iter skyPositions_end,
                          const bool farfield
                          )
    {
      double distance;
      Iter
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
          distance = norm(sky,sky+3);
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
          distance = norm(sky,sky+3);
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
} // End ttl

#endif // TTL_BEAMFORMING_H

