/*
 *	StopWatch.c - timing functions
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

#include <stdio.h>

#include "StopWatch.h"
#include "Statistics.h"


double StopWatch_delay_substract = 0.0;
double StopWatch_substract = 0.0;

int StopWatch_Initialise(int quiet)
{
/* Calibrate the StopWatch. */
    Statistics t;
    int i;
    StopWatch sw;
    StopWatch delay;

    StopWatch_substract = 0.0;
    Statistics_Init(&t);

    for(i=0;i<10000;i++) {
	StopWatch_Start(&sw);
	StopWatch_Stop(&sw);
	Statistics_Add(&t,RealTime_MinusUsD(sw.t2,sw.t1));
    }
    if(!quiet) printf("StopWatch_Stop calibrated: %6.4f us minimum\n",Statistics_Min(&t));
    StopWatch_substract = Statistics_Min(&t);
    
    StopWatch_delay_substract = 0.0;
    Statistics_Init(&t);
    for(i=1000;i<1100;i++) {
	StopWatch_Start(&sw);
	StopWatch_Start(&delay);
	StopWatch_Delay(&delay,i);
	StopWatch_Stop(&sw);
	Statistics_Add(&t,StopWatch_TimeDiff(&sw)-i);
    }
    if(!quiet) printf("StopWatch_Delay calibrated: %6.4f us minimum\n",Statistics_Min(&t));
    StopWatch_delay_substract = Statistics_Min(&t);
    return 0;
}





