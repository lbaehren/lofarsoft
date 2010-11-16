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
  \brief Calculate TAI - UTC in seconds

  For dates before July 1 1972 a fixed value of 10s will be returned.

  \todo Update this routine whenever a leap second is inserted.

  \param utc Universal Time Coordinated as Julian day

  \return TAI - UTC in seconds
 */
int tmf::tai_utc(const double& utc)
{
  const int n = 24;

  // Leap second table from the IERS Earth Orientation Centre bulletin C
  const double ls[n] = {
      2441499.5,  // 1972  Jul.   1              - 1s
      2441683.5,  // 1973  Jan.   1              - 1s
      2442048.5,  // 1974  Jan.   1              - 1s
      2442413.5,  // 1975  Jan.   1              - 1s
      2442778.5,  // 1976  Jan.   1              - 1s
      2443144.5,  // 1977  Jan.   1              - 1s
      2443509.5,  // 1978  Jan.   1              - 1s
      2443874.5,  // 1979  Jan.   1              - 1s
      2444239.5,  // 1980  Jan.   1              - 1s
      2444786.5,  // 1981  Jul.   1              - 1s
      2445151.5,  // 1982  Jul.   1              - 1s
      2445516.5,  // 1983  Jul.   1              - 1s
      2446247.5,  // 1985  Jul.   1              - 1s
      2447161.5,  // 1988  Jan.   1              - 1s
      2447892.5,  // 1990  Jan.   1              - 1s
      2448257.5,  // 1991  Jan.   1              - 1s
      2448804.5,  // 1992  Jul.   1              - 1s
      2449169.5,  // 1993  Jul.   1              - 1s
      2449534.5,  // 1994  Jul.   1              - 1s
      2450083.5,  // 1996  Jan.   1              - 1s
   		2450630.5,  // 1997  Jul.   1              - 1s
      2451179.5,  // 1999  Jan.   1              - 1s
      2453736.5,  // 2006  Jan.   1              - 1s
      2454832.5,  // 2009  Jan.   1              - 1s
   };

  // Loop through the leap seconds table
  int i = 0;
  while (i<n && utc>ls[i])
  {
    i++;
  }

  // before 2441499.5 TAI - UTC = 10s
  return 10 + i;
}

/*!
  \brief Calculate TT - UTC in seconds

  For dates before July 1 1972 a fixed value of 10s + TT - TAI is returned.

  \param utc Universal Time Coordinated as Julian day

  \return TT - UTC in seconds
 */
double tmf::tt_utc(const double& utc)
{
  return 32.184 + static_cast<double>(tai_utc(utc));
}

