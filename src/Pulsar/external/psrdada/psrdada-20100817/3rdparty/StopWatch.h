/*
 *	StopWatch.h - stopwatch timing definitions
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

#ifndef StopWatch_h
#define StopWatch_h

/**@name StopWatch
   
   Utility that uses RealTime to implement a StopWatch.  The StopWatch
   is calibrated during initialisation such that only the time between
   start and stop is measured. */

#include "RealTime.h"

#ifndef INLINE
#define INLINE extern inline
#endif

typedef struct _StopWatch StopWatch;

struct _StopWatch {
    RealTime t1;
    RealTime t2;
};

extern double StopWatch_delay_substract;
extern double StopWatch_substract;

extern double StopWatch_TimeDiff(StopWatch* s);
/* Measures the time in us between starting and stopping the StopWatch
 * with clock freq precision.  Starting and stopping themselves are
 * not included! */

extern int StopWatch_Initialise(int quiet);
/* Calibrates the StopWatch. */

extern void StopWatch_Start(StopWatch* s);
/* Starts the StopWatch */

extern void StopWatch_Stop(StopWatch* s);
/* Stops the StopWatch */

extern void StopWatch_Delay(StopWatch* s,double delay);
/* Function call waits for a time "delay" us since StopWatch_Start time.  
The function call itself is included in this time! */

extern double StopWatch_TimeDiff(StopWatch* s);
/* Returns the time difference in us between start and stop. */

extern double StopWatch_uSec(RealTime cpu_time);
/* Returns the time converted to us  */


INLINE void StopWatch_Start(StopWatch* s)
{
    s->t1 = RealTime_Time();
}

INLINE void StopWatch_Stop(StopWatch* s)
{
    s->t2 = RealTime_Time();
}

INLINE void StopWatch_Delay(StopWatch* s, double delay)
{
    s->t2 = RealTime_PlusUsD(s->t1,delay-StopWatch_delay_substract);
    while(!RealTime_LaterThan(s->t2));
}

INLINE double StopWatch_TimeDiff(StopWatch* s)
{
    return RealTime_MinusUsD(s->t2,s->t1) - StopWatch_substract;
}

INLINE double StopWatch_uSec(RealTime cpu_time)
{
    return RealTime_PlusUsD( cpu_time, 0);
}

#endif
