/***************************************************************************
 *
 *   Copyright (C) 1999-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Phase.h"
#include "Error.h"
#include "tostring.h"

#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>

using namespace std;

double Phase::rounding_threshold = 1e-9;

const Phase Phase::zero;

void Phase::settle ()
{
  if (turns>0 && fturns < -rounding_threshold)
  {
    fturns += 1.0;
    turns--;
  }
  if (turns<0 && fturns > rounding_threshold)
  {
    fturns -= 1.0;
    turns++;
  }
}

Phase::Phase (double tns)
{
  turns = (int64_t) tns;
  fturns = tns - double (turns);
  settle ();
}

Phase::Phase (int64_t tns, double ftns) 
{
  set (tns, ftns);
}

void Phase::set (int64_t tns, double ftns)
{
  int64_t iturns = (int64_t) ftns;
  turns = tns + iturns;
  fturns = ftns - double (iturns);
  settle ();
}

double Phase::in_turns() const
{
  return double(turns)+fturns;
}

int64_t Phase::intturns() const
{
  return turns;
}

double Phase::fracturns() const
{
  return fturns;
}

string Phase::strprint (int precision) const
{
  if (precision>DBL_DIG)
  {
    cerr << "Phase::strprint warning: precision of " << precision 
	 << " exceeds that of a double" << endl;
    cerr << "- truncating to a precision of " << DBL_DIG << endl;
  }

  string s = tostring (turns);
  string f = tostring (fturns, precision, std::ios::fixed);

  if (!finite(fturns))
    s += ".NaN";
  else
  {
    int adjust = 1;

    if (fturns < 0)
    {
      // erase the leading negative sign
      f.erase (0,1);
      adjust = -1;
    }

    // if the rounded fturns are greater than one, adjust the integer turns
    if (f[0] == '1')
      s = tostring (turns + adjust);

    // remove the leading 0 or 1
    f.erase (0,1);

    s += f;
  }

  return s;
}

const Phase& Phase::operator = (const Phase &copy)
{
  turns = copy.turns;
  fturns = copy.fturns;

  return *this;
}

Phase operator + (const Phase &p1, const Phase &p2)
{
  return Phase(p1.turns + p2.turns,
	       p1.fturns + p2.fturns); 
}

Phase operator - (const Phase &p1, const Phase &p2)
{
  return Phase(p1.turns - p2.turns,
	       p1.fturns - p2.fturns); 
}

Phase operator - (const Phase &p)
{
  return Phase( -p.turns, -p.fturns );
}


// period is assumed to be given in seconds
MJD operator * (const Phase &p1, double period) 
{
  const int64_t secs_per_day = 86400;

  double bigseconds = double (p1.turns) * period;
  double smallseconds = p1.fturns * period;

  int64_t b_seconds = int64_t (bigseconds);
  int64_t s_seconds = int64_t (smallseconds);

  double b_fracsec = bigseconds - double (b_seconds);
  double s_fracsec = smallseconds - double (s_seconds);

  int b_days = int (b_seconds / secs_per_day);
  int s_days = int (s_seconds / secs_per_day);

  b_seconds -= b_days * secs_per_day;
  s_seconds -= s_days * secs_per_day;

  return MJD (b_days+s_days, int(b_seconds+s_seconds), b_fracsec+s_fracsec);
}

MJD operator / (const Phase &p1, double frequency)
{
  return p1 * (1.0/frequency);
}

const Phase& Phase::operator ++ ()
{
  turns ++;
  settle();
  return *this;
}

const Phase& Phase::operator -- ()
{
  turns --;
  settle();
  return *this;
}

const Phase& Phase::operator += (int iturns)
{
  turns += iturns;
  settle();
  return *this;
}

const Phase& Phase::operator -= (int iturns)
{
  turns -= iturns;
  settle();
  return *this;
}


// turns is converted to a Phase first, in order that large 'turns' does not
// destroy the precision of 'p1.fturns' -- WvS
Phase operator + (const Phase &p1, double turns)
{
  return p1 + Phase(turns);
}

Phase operator - (const Phase &p1, double turns)
{
  return p1 - Phase(turns);
}

const Phase& Phase::operator += (double tns)
{
  int64_t add_turns = (int64_t) tns;
  double add_fturns = tns - double (add_turns);

  set (turns + add_turns, fturns + add_fturns);
  return *this;
}

const Phase& Phase::operator -= (double tns)
{
  int64_t sub_turns = (int64_t) tns;
  double sub_fturns = tns - double (sub_turns);

  set (turns - sub_turns, fturns - sub_fturns);
  return *this;
}

const Phase& Phase::operator += (const Phase &add)
{
  set (turns + add.turns, fturns + add.fturns);
  return *this;
}

const Phase& Phase::operator -= (const Phase &sub)
{
  set (turns - sub.turns, fturns - sub.fturns);
  return *this;
}

static const double precision_limit = 2.0 * pow (10.0,-DBL_DIG);

bool operator > (const Phase &p1, const Phase &p2)
{
  if (p1.turns != p2.turns)
    return p1.turns > p2.turns;

  // double precision_limit = 2*pow(10,-DBL_DIG);
  if (fabs (p1.fturns-p2.fturns) < precision_limit)
    return false;
  else 
    return p1.fturns > p2.fturns;
}

bool operator == (const Phase &p1, const Phase &p2)
{
  if (p1.turns == p2.turns &&
      fabs (p1.fturns-p2.fturns) < precision_limit) 
    return true;
  else
    return false;  
}

bool operator >= (const Phase &p1, const Phase &p2)
{
  return p1>p2 || p1==p2;
}

bool operator < (const Phase &p1, const Phase &p2)
{
  return !(p1 >= p2);
}

bool operator <= (const Phase &p1, const Phase &p2)
{
  return p1<p2 || p1==p2;
}

bool operator != (const Phase &p1, const Phase &p2)
{
  return !(p1 == p2);
}

Phase Phase::Ceil ()
{
  return Phase (turns + (int64_t) ceil(fturns), 0.0);
}

Phase Phase::Floor ()
{
  return Phase (turns + (int64_t) floor(fturns), 0.0);
}

Phase Phase::Rint ()
{
  return Phase (turns + (int64_t) rint(fturns), 0.0);
}
