/*
 *	Statistics.h - statistics interface definitions
 *	Copyright (C) 1998-1999 Marcel Boosten
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef Statistics_h
#define Statistics_h

/**@name Statistics
   
   Functions that allow the calculation of the average and standard
   deviation of a large set of numbers.  */



#include <math.h>

#ifndef INLINE
#define INLINE extern inline
#endif

typedef struct _Statistics Statistics;

struct _Statistics {
  int n;
  double sum;
  double sum_of_squares;
  double min;
  double max;
};

extern void Statistics_Init(Statistics* s);
extern void Statistics_Add(Statistics* s, double value);
extern double Statistics_E(Statistics* s);
extern double Statistics_Var(Statistics* s);
extern double Statistics_StdDev(Statistics* s);
extern double Statistics_PlusMin(Statistics* s);
extern double Statistics_Min(Statistics* s);
extern double Statistics_Max(Statistics* s);

INLINE void Statistics_Init(Statistics* s)
{
  s->n = 0;
  s->sum = 0.0;
  s->sum_of_squares = 0.0;
  s->min = 1e199;
  s->max = -1e199;
}

INLINE void Statistics_Add(Statistics* s, double value)
{
  /* include value in the statistics */
  s->n++;
  s->sum += value;
  s->sum_of_squares += value*value;
  if (s->min > value) {
    s->min = value;
  }
  if (s->max < value) {
    s->max = value;
  }
}

INLINE double Statistics_E(Statistics* s)
{
  if (s->n >0) return (s->sum / s->n); 
  else  return 0;
}

INLINE double Statistics_Var(Statistics* s)
{
  double E = Statistics_E(s);
  double var= (s->n >0)? (s->sum_of_squares/s->n) - E*E: 0.0;
  if (var<0) {
    return 0.0;
  } else {
    return var;
  }
}

INLINE double Statistics_StdDev(Statistics* s)
{
  return sqrt(Statistics_Var(s));
}

INLINE double Statistics_PlusMin(Statistics* s)
{
  return 100.0*Statistics_StdDev(s)/Statistics_E(s);
}

INLINE double Statistics_Min(Statistics* s)
{
  return s->min;
}

INLINE double Statistics_Max(Statistics* s)
{
  return s->max;
}

#endif
