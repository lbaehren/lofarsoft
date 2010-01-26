/***************************************************************************
 *
 *   Copyright (C) 2003 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>

#include "RealTimer.h"
using namespace std;

const char* RealTimer::timeString (double time) 
{
  static char str[80];

  if (time < 120.0)
    sprintf (str, "%g seconds", time);
  else if (time < 7200.0)
    sprintf (str, "%g minutes", time/60.0);
  else if (time < 172800.0)
    sprintf (str, "%g hours", time/3600.0);
  else
    sprintf (str, "%g days", time/86400.0);

  return str;
}

RealTimer::RealTimer() : Reference::Able()
{
  reset();
  start(); 
}

RealTimer::~RealTimer()
{ 
}

const RealTimer& RealTimer::operator += (const RealTimer& add)
{
  total_elapsed += add.total_elapsed;
}

void RealTimer::start() 
{ 
  gettimeofday (&time1, NULL);
}

void RealTimer::stop() 
{
  gettimeofday (&time2, NULL);
  elapsed = double( time2.tv_sec - time1.tv_sec ) +
    double( time2.tv_usec - time1.tv_usec ) * 1e-6;
  total_elapsed += elapsed;
  time1 = time2;
}

void RealTimer::lap() 
{
  stop(); 
}

void RealTimer::reset()
{
  elapsed = total_elapsed = 0.0; 
}

double RealTimer::get_elapsed() const
{ 
  return elapsed;
}

double RealTimer::get_total() const
{ 
  return total_elapsed;
}

const char* RealTimer::elapsedString ()
{
  return timeString (elapsed); 
}

ostream& operator<< (ostream& ostr, const RealTimer& clock)
{
  return ostr << RealTimer::timeString(clock.get_elapsed());
}
