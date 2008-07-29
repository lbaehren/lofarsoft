/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/*!
  \file tNumericalRanges.cc
  
  \ingroup CR_test

  \brief Print numerical ranges of the various data types
  
  \author Lars B&auml;hren
*/

#include <iostream>
#include <limits.h> 
#include <float.h>

using std::cout;
using std::endl;

int main(void) {
  
  /*  a byte doesn't have to contain 8 bits (but usually does) */
  printf("1 byte contains %i bits", CHAR_BIT);
  cout << endl;
  
  /*  char may be signed or unsigned, and all three will always be 1 byte  */
  
  cout << " -- unsigned char " << sizeof(unsigned char) << " byte, from "
       << 0 << " to " << UCHAR_MAX
       << endl;
  
  cout << " -- signed char " << sizeof(signed char) << " byte, from "
       << SCHAR_MIN << " to " << SCHAR_MAX
       << endl;
  
  cout << " -- char " << sizeof(char) << " byte, from "
       << CHAR_MIN << " to " << CHAR_MAX
       << endl;
  
  /*  short, int and long are signed  */
  
  cout << " -- unsigned short " << sizeof(unsigned short) << " byte, from "
       << 0 << " to " << USHRT_MAX
       << endl;
  
  cout << " -- signed short " << sizeof(signed short) << " byte, from "
       << SHRT_MIN << " to " << SHRT_MAX
       << endl;
  
  cout << " -- unsigned int " << sizeof(unsigned int) << " byte, from "
       << (unsigned int) 0 << " to " << (unsigned int) UINT_MAX
       << endl;
  
  cout << " -- signed int " << sizeof(signed int) << " byte, from "
       << INT_MIN << " to " << INT_MAX
       << endl;
  
  cout << " -- unsigned long " << sizeof(unsigned long) << " byte, from "
       << (unsigned long) 0 << " to " << ULONG_MAX
       << endl;
  
  cout << " -- signed long " << sizeof(signed long) << " byte, from "
       << LONG_MIN << " to " << LONG_MAX
       << endl;
  
  /*   floating point types can store 0, may also be able to store 
       strange numbers like +Infinity and  */

#ifdef SIZEOF_FLOAT  
  printf("float           %u bytes, positive range from %12e to %12e", 
         sizeof(float), FLT_MIN, FLT_MAX);
  printf("                         epsilon = %e", FLT_EPSILON);
  cout << endl;
#endif 

#ifdef SIZEOF_DOUBLE
  printf("double          %u bytes, positive range from %12e to %12e", 
         sizeof(double), DBL_MIN, DBL_MAX);
  printf("                         epsilon = %e", DBL_EPSILON);
  cout << endl;
#endif
  
  /*   L in format string is not universally supported  */
  printf("long double     %u bytes, positive range from %12Le to %12Le", 
         sizeof(long double), LDBL_MIN, LDBL_MAX);
  printf("                         epsilon = %Le", LDBL_EPSILON);
  cout << endl;
  
  return 0;
}
