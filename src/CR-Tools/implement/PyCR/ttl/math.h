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

#ifndef TTL_MATH_H
#define TTL_MATH_H

// SYSTEM INCLUDES
#include <cmath>

// PROJECT INCLUDES
#include <ttl/constants.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

// DOXYGEN TAGS
/*!
  \file math.h
  \ingroup CR
  \ingroup CR_TTL
 */

namespace ttl
{
  namespace math
  {
    /*!
      \brief Take the sin of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void sin(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::sin(*it);
          ++it;
        }
      }

    /*!
      \brief Take the cos of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void cos(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::cos(*it);
          ++it;
        }
      }

    /*!
      \brief Take the tan of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void tan(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::tan(*it);
          ++it;
        }
      }

    /*!
      \brief Take the sinh of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void sinh(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::sinh(*it);
          ++it;
        }
      }

    /*!
      \brief Take the cosh of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void cosh(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::cosh(*it);
          ++it;
        }
      }

    /*!
      \brief Take the tanh of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void tanh(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::tanh(*it);
          ++it;
        }
      }

    /*!
      \brief Take the asin of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void asin(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::asin(*it);
          ++it;
        }
      }

    /*!
      \brief Take the acos of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void acos(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::acos(*it);
          ++it;
        }
      }

    /*!
      \brief Take the atan of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void atan(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::atan(*it);
          ++it;
        }
      }

    /*!
      \brief Take the log of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void log(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::log(*it);
          ++it;
        }
      }

    /*!
      \brief Take the log10 of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void log10(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::log10(*it);
          ++it;
        }
      }

    /*!
      \brief Take the log of all elements in the itterable but return LOW_NUMBER if element is zero

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void logSave(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          if (*it>0)
          {
            *it=std::log(*it);
          }
          else
          {
            *it=constants::LOW_NUMBER;
          }
          ++it;
        }
      }

    /*!
      \brief Take the abs of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void abs(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::abs(*it);
          ++it;
        }
      }

    /*!
      \brief Take the exp of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void exp(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::exp(*it);
          ++it;
        }
      }

    /*!
      \brief Take the sqrt of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void sqrt(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::sqrt(*it);
          ++it;
        }
      }

    /*!
      \brief Take the fabs of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void fabs(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::fabs(*it);
          ++it;
        }
      }

    /*!
      \brief Take the floor of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void floor(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::floor(*it);
          ++it;
        }
      }

    /*!
      \brief Take the ceil of all elements in the itterable

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      void ceil(const RIter vec, const RIter vec_end)
      {
        RIter it=vec;
        while (it!=vec_end)
        {
          *it=std::ceil(*it);
          ++it;
        }
      }

    /*!
      \brief Returns the norm (or length) of a vector

      \\sqrt(\\sum_{i}(x_{i}^2)).

      \param vec begin iterator
      \param vec_end end iterator
     */
    template <class RIter>
      double norm(const RIter vec, const RIter vec_end)
      {
        double sum=0;
        RIter it=vec;
        while (it!=vec_end)
        {
          sum += (*it) * (*it); ++it;
        };
        return std::sqrt(sum);
      }
  } // End math
} // End ttl

#endif // TTL_MATH_H

