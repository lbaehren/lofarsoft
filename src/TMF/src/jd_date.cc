/**************************************************************************
 *  This file is part of the Transmogrify library.                        *
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
#include <cmath>

// PROJECT INCLUDES
#include <tmf.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

using namespace std;
using namespace tmf;

/*!
  \brief Convert date in Gregorian calendar to Julian day.

  \return jd Julian day

  \param y year
  \param m month
  \param d day (with fraction)
 */
double tmf::gregoriandate2jd(const int& y, const int& m, const double& d)
{
  int Y;
  int M;

  if (m==1 || m==2)
  {
    Y = y - 1;
    M = m + 12;
  }
  else
  {
    Y = y;
    M = m;
  }

  int a = int(Y / 100);
  int b = 2-a+int(a/4);

  return int(365.25 * (Y + 4716)) + int(30.6001 * (M + 1)) + d + b - 1524.5;
}

/*!
  \brief Convert date in Julian calendar to Julian day.

  \return jd Julian day

  \param y year
  \param m month
  \param d day (with fraction)
 */
double tmf::juliandate2jd(const int& y, const int& m, const double& d)
{
  int Y;
  int M;

  if (m==1 || m==2)
  {
    Y = y - 1;
    M = m + 12;
  }
  else
  {
    Y = y;
    M = m;
  }

  int a = int(Y / 100);

  return int(365.25 * (Y + 4716)) + int(30.6001 * (M + 1)) + d - 1524.5;
}

/*!
  \brief Convert date in Gregorian or Julian calendar to Julian day.

  \return jd Julian day

  \param y year
  \param m month
  \param d day (with fraction)
 */
double tmf::date2jd(const int& y, const int& m, const double& d)
{
  // Gregorian calendar
  if (y > 1582)
  {
    return gregoriandate2jd(y, m, d);
  }
  // Julian calendar
  else if (y < 1582)
  {
    return juliandate2jd(y, m, d);
  }
  // Gregorian calendar
  else if (m > 10)
  {
    return gregoriandate2jd(y, m, d);
  }
  // Julian calendar
  else if (m < 10)
  {
    return juliandate2jd(y, m, d);
  }
  // Gregorian calendar
  else if (d >= 15)
  {
    return gregoriandate2jd(y, m, d);
  }
  // Julian calendar
  else if (d <= 4)
  {
    return juliandate2jd(y, m, d);
  }
  // Invalid date
  else
  {
    throw (InputError());
  }
}

/*!
  \brief Convert Julian day to calendar date.

  \param y year
  \param m month
  \param d day (with fraction)
  \param jd Julian day
 */
void tmf::jd2date(int& y, int& m, double& d, const double& jd)
{
  int Z, A, alpha, B, C, D, E;
  double JD, F;

  JD = jd+0.5;

  Z = int(JD); // integer part
  F = JD - Z;  // fractional part

  if (Z < 2299161)
  {
    A = Z;
  }
  else
  {
    alpha = int((Z - 1867216.25) / 36524.25);

    A = Z + 1 + alpha - int(alpha / 4);
  }

  B = A + 1524;
  C = int((B - 122.1) / 365.25);
  D = int(365.25 * C);
  E = int((B - D) / 30.6001);

  d = B - D - int(30.6001 * E) + F;
  m = (E < 14) ? E - 1 : E - 13;
  y = (m > 2) ? C - 4716 : C - 4715;
}

