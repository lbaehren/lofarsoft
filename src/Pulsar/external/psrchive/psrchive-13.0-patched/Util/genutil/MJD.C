/***************************************************************************
 *
 *   Copyright (C) 1998 by Matthew Britton
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MJD.h"
#include "Error.h"

#include "ieee.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#if defined(sun) && !defined(__GNUC__)
#include <float.h>
#include <sunmath.h>
#endif
#include <math.h>
#include <limits.h>

using namespace std;

#define RINGBUFFER_SIZE 10

int MJD::verbose = 0; 

double   MJD::precision = 2.0e-12;
unsigned MJD::ostream_precision = 15;
const MJD MJD::zero;

const int seconds_in_day = 86400;

int ss2hhmmss (int* hours, int* min, int* sec, int seconds)
{
  *hours   = seconds/3600;
  seconds -= 3600 * (*hours);
  *min     = seconds/60;
  seconds -= 60 * (*min);
  *sec     = seconds;

  return 0;
}

// #define _DEBUG 1

string MJD::printdays (unsigned prec) const
{
#ifdef _DEBUG
  cerr << "MJD::printdays prec=" << prec << " days=" << days 
       << " secs=" << secs << " fracsec=" << fracsec << endl;
#endif

  const unsigned size = prec + 80;

  char temp [size];
  snprintf (temp, size, "%d", days);
  string output = temp;

  if (prec > 0)
  {
    snprintf (temp, size, "%*.*lf", prec+3, prec, fracday());
    char* period = strchr (temp, '.');
    if (!period)  {
      output += ".";
      output += temp;
    }
    else
      output += period;
  }
  return output;
}

string MJD::printdays() const
{
  char dummy[10];
  snprintf (dummy, 10, "%d", days);
  return dummy;
}

string MJD::printhhmmss() const
{
  char dummy[10];
  int hh, mm, ss;

  ss2hhmmss (&hh, &mm, &ss, secs);
  snprintf (dummy, 10, "%2.2d%2.2d%2.2d", hh,mm,ss);
  return dummy;
}

string MJD::printfs() const
{
  char dummy[20];
  snprintf (dummy, 20, "%.15lf", fracsec);
  return dummy + 1;
}

string MJD::printall() const
{
  char dummy[40];
  snprintf (dummy, 40, "%s:%s%s",
	    printdays().c_str(),
	    printhhmmss().c_str(),
	    printfs().c_str());
  return dummy;
}

char* MJD::datestr (char* dstr, int len, const char* format) const
{
  struct tm greg;

  if (gregorian (&greg, NULL) < 0)
    return NULL;

  if (strftime (dstr, len, format, &greg) == 0)
    return NULL;

  return dstr;
}

string MJD::strtempo() const
{
  char temp[20];
  snprintf (temp, 20, "%14.11lf", fracday());

  char* period = strchr (temp, '.');

  char dummy[40];
  snprintf (dummy, 40, "%5d.%s", days,period+1);

  return dummy;
}

double MJD::in_seconds() const 
{
  return((double)days*86400.0+(double)secs+fracsec);
}

double MJD::in_days() const
{
  return((double)days + ((double)secs+fracsec)/86400.0);
}

double MJD::in_minutes() const
{
  return((double) days*1440.0 + ((double)secs+fracsec)/60.0);
}

int MJD::intday() const {
  return(days);
}

double MJD::fracday() const {
  return((double)secs/86400.0 + fracsec/86400.0);
}

MJD& MJD::operator = (const MJD &in_mjd)
{
  if (this != &in_mjd) {
    days = in_mjd.days;
    secs = in_mjd.secs;
    fracsec = in_mjd.fracsec;
  }
  return *this;
}

MJD& MJD::operator += (const MJD &in_mjd)
{
  *this = *this + in_mjd;
  return *this;
}

MJD& MJD::operator -= (const MJD &in_mjd)
{
  *this = *this - in_mjd;
  return *this;
}

MJD& MJD::operator += (const double & d)
{
  *this = *this + d;
  return *this;
}

MJD& MJD::operator -= (const double & d)
{
  *this = *this - d;
  return *this;
}

MJD& MJD::operator *= (const double & d)
{
  *this = *this*d;
  return *this;
}

MJD& MJD::operator /= (const double & d)
{
  *this = *this/d;
  return *this;
}

const MJD operator + (const MJD &m1, const MJD &m2) {
  // Let constructor do the dirty work.
  return MJD (m1.intday() + m2.intday(),
	      m1.get_secs() + m2.get_secs(),
	      m1.get_fracsec() + m2.get_fracsec());
}

const MJD operator - (const MJD &m1, const MJD &m2) {
  // Let constructor do the dirty work.
  return MJD (m1.intday() - m2.intday(),
	      m1.get_secs() - m2.get_secs(),
	      m1.get_fracsec() - m2.get_fracsec());
}

const MJD operator + (const MJD &m1, double sss)
{
  int isec = int (sss);
  sss -= double (isec);

  return MJD(m1.days, m1.secs+isec, m1.fracsec+sss);
}

const MJD operator - (const MJD &m1, double sss) {
  return m1 + -sss;
}

const MJD operator - (MJD m1) {
  m1.days = -m1.days;
  m1.secs = -m1.secs;
  m1.fracsec = -m1.fracsec;
  return m1;
}

const MJD operator * (const MJD &m1, double d) {
  double ddays = ((double) m1.intday()) * d;
  double dsecs = ((double) m1.get_secs()) * d; 
  double dfracsec = m1.get_fracsec() * d;
  return MJD(ddays,dsecs,dfracsec);
}

const MJD operator / (const MJD &m1, double divisor) {
  return m1 * (1.0/divisor);
}

MJD abs (const MJD & in_mjd) {
  if (in_mjd.intday() > 0)
    return in_mjd;
  else 
    return -in_mjd;
}

int operator > (const MJD &m1, const MJD &m2)
{
  if (m1.days != m2.days)
    return m1.days > m2.days;
  if (m1.secs != m2.secs)
    return m1.secs > m2.secs;
  if (fabs(m1.fracsec-m2.fracsec) < MJD::precision)
    return 0;
  else 
    return m1.fracsec > m2.fracsec;
}

int operator < (const MJD &m1, const MJD &m2)
{
  if (m1.days != m2.days)
    return m1.days < m2.days;
  if (m1.secs != m2.secs)
    return m1.secs < m2.secs;
  if (fabs(m1.fracsec-m2.fracsec) < MJD::precision)
    return 0;
  else 
    return m1.fracsec < m2.fracsec;
}

int operator == (const MJD &m1, const MJD &m2) {
  return equal(m1, m2) ||
    equal(m1+1.5*MJD::precision, m2) ||
    equal(m1,m2+1.5*MJD::precision);
}

int operator >= (const MJD &m1, const MJD &m2) {
  return m1 == m2 || m1 > m2;
}

int operator <= (const MJD &m1, const MJD &m2) {
  return m1 == m2 || m1 < m2;
}

int operator != (const MJD &m1, const MJD &m2) {
  return !(m1 == m2);
}

int MJD::print (FILE *stream)
{
  fprintf(stream, printall().c_str());
  return 0;
}

int MJD::println (FILE *stream)
{
  fprintf(stream, "%s\n", printall().c_str());
  return 0;
}

MJD::MJD (double dd, double ss, double fs)
{
  cerr.precision(15);

  if (verbose)
    cerr << "MJD (double dd=" << dd << ", double ss=" 
         << ss << ", double fs=" << fs << ")" << endl;

  // pull the integer/fractional seconds out of fs
  secs = int (fs);
  fracsec = fs - double(secs);

  // pull the integer/fractional seconds out of ss
  int isecs = int (ss);
  secs += isecs;
  fracsec += ss - double(isecs);   // fracsec may be > 1.0 at this point

  days = 0;
  add_day (dd);

  settle ();

  if (verbose)
    cerr << "MJD this=" << *this << endl;
}

MJD::MJD (int d, int s, double f) {

  if (verbose)        
    cerr << "MJD (int d=" << d << ", int s=" << s
         << ", double f=" << f << ")" << endl;

  days = d;
  secs = s;
  fracsec = f;
  settle ();

  if (verbose)
    cerr << "MJD this=" << *this << endl;
}

MJD::MJD (double mjd) {

  if (verbose)        
    cerr << "MJD (double mjd=" << mjd << ")" << endl;

  days = 0;
  secs = 0;
  fracsec = 0.0;
  add_day (mjd);
  settle ();

  if (verbose)
    cerr << "MJD this=" << *this << endl;
}

void MJD::add_day (double dd)
{
  // pull the integer day out of dd
  int idays = int (dd);
  days += idays;

  double fdays = dd - double(idays);

  // and the integer/fractional seconds out of fractional day
  double seconds = fdays * double(seconds_in_day);
  int isecs = int (seconds);
  secs += isecs;
  fracsec += seconds - double(isecs);
}

void MJD::settle()
{
  // pull integer secs out of fracsec, if any
  int isecs = int (fracsec);
  secs += isecs;
  fracsec -= double (isecs);

  // pull integer days out of secs, if any
  int idays = (int) floor(double(secs)/double(seconds_in_day));
  days += idays;
  secs -= idays * seconds_in_day;

  // Everything should have the same sign.
  int sign = -1;
  if (double(days) + (secs+fracsec)/double(seconds_in_day) >= 0.0) 
    sign = 1;

  // check the sign on the fractional seconds
  if (sign*fracsec < 0.0) {
    secs -= sign;
    fracsec += double(sign);
  }

  // check the sign on the integer seconds
  if (sign*secs < 0) {
    days -= sign;
    secs += sign * seconds_in_day;
  }

#ifdef DESPARATE_MEASURES

  // Make sure fractional seconds are truly fractional
  // round to the nearest fempto second
  if (fabs(fracsec + sign*precision) > 1.0) {
    secs += sign;
    fracsec -= double (sign);
  }

  // Make sure that there aren't too many seconds'
  while (sign*secs >= seconds_in_day) {
    days ++;
    secs -= seconds_in_day;
  }

#endif

  // sanity check
  if (fabs(fracsec) > 1.0)
    throw Error (InvalidState, "MJD::settle", "|fracsec=%lf| > 1.0", fracsec);

  if (abs(secs) >= 86400)
    throw Error (InvalidState, "MJD::settle", "|secs=%d| > 86400", secs);

}

MJD::MJD (const char* mjdstring) {

  if (verbose)
    cerr << "MJD (const char*=" << mjdstring << ")"  << endl;

  if (Construct (mjdstring) < 0)
    throw string ("MJD::MJD(char*) construct error");
}

MJD::MJD (const string& mjd) {
  if (Construct (mjd.c_str()) < 0)
    throw string ("MJD::MJD(string&) construct error");
}

MJD::MJD (int intday, double fracday)
{
  if (verbose)
    cerr << "MJD (int intday=" << intday << ", double fracday=" << fracday
         << ")" << endl;

  secs = 0;
  fracsec = 0.0;
  days = intday;

  add_day (fracday);
  settle();

  if (verbose)
    cerr << "MJD this=" << *this << endl;
}

int MJD::UTC (utc_t* utc, double* fsec) const
{
  struct tm  greg;

  gregorian (&greg, fsec);
  return tm2utc (utc, greg);
}

int MJD::gregorian (struct tm* gregdate, double* fsec) const
{
  int julian_day = days + 2400001;

  int n_four = 4  * (julian_day+((6*((4*julian_day-17918)/146097))/4+1)/2-37);
  int n_dten = 10 * (((n_four-237)%1461)/4) + 5;

  gregdate->tm_year = n_four/1461 - 4712 - 1900; // extra -1900 for C struct tm
  gregdate->tm_mon  = (n_dten/306+2)%12;         // struct tm mon 0->11
  gregdate->tm_mday = (n_dten%306)/10 + 1;

  ss2hhmmss (&gregdate->tm_hour, &gregdate->tm_min, &gregdate->tm_sec, secs);

  if (fsec)
    *fsec = fracsec;

  gregdate->tm_isdst = -1;
  time_t date = mktime (gregdate);
  if (date == (time_t)-1)
    return -1;

  return 0;
}

// construct an MJD from a UTC
MJD::MJD (const utc_t& utc)
{
  if (Construct (utc) < 0)
    throw ("MJD::MJD(utc_t) construct error");
}


int MJD::Construct (const utc_t& utc)
{
  struct tm greg;
  if (utc2tm (&greg, utc) < 0)  {
    fprintf (stderr, "MJD::Construct(utc_t) error converting to gregorian\n");
    return -1;
  }

#if 0
  char buffer [100];
  strftime (buffer, 50, "%Y-%j-%H:%M:%S", &greg);
  fprintf (stderr, "MJD::Construct utc:%s = greg:%s\n",
	   utc2str (buffer+50, utc, "yyyy-ddd-hh:mm:ss"),
	   buffer);
#endif

  return Construct (greg);
}

MJD::MJD (time_t time)
{
  if (Construct (time) < 0)
    throw ("MJD::MJD(time_t) construct error");
}

int MJD::Construct (time_t time)
{
  struct tm date = *gmtime(&time);
  return Construct (date);
}

MJD::MJD (const struct timeval& tp)
{
  if (Construct (tp) < 0)
    throw ("MJD::MJD(struct timeval) construct error");
}

int MJD::Construct (const struct timeval& tp)
{
  time_t utcsecs = tp.tv_sec;
  if (Construct (utcsecs) < 0)
    return -1;
  fracsec = double(tp.tv_usec) * 1e-6;
  return 0;
}

// long long to get a 64-bit unsigned
int MJD::Construct(unsigned long long bat)
{
  // Not sure how to do 64-bit constants so let's construct it
  unsigned long long microsecPerDay;

  microsecPerDay = 86400; microsecPerDay *= 1000000;

  days = bat/microsecPerDay;
  secs = (bat%microsecPerDay)/1000000;
  fracsec = 1.0e-6 * (bat%100000);

  return 0;
}
  
// construct an MJD from a gregorian
MJD::MJD (const struct tm& greg)
{
  if (Construct (greg) < 0)
    throw ("MJD::MJD(struct tm) construct error");
}

int MJD::Construct (const struct tm& greg)
{
  int year = greg.tm_year + 1900;
  int month = greg.tm_mon + 1;

  days = (1461*(year-(12-month)/10+4712))/4
    +(306*((month+9)%12)+5)/10
    -(3*((year-(12-month)/10+4900)/100))/4
    +greg.tm_mday-2399904;

  // Work out seconds, fracsecs always zero.
  secs = 3600 * greg.tm_hour + 60 * greg.tm_min + greg.tm_sec;
  fracsec = 0.0;
  return 0;
}

// parses a string of the form 51298.45034 ish
int MJD::Construct (const char* mjdstr)
{
  if (sscanf (mjdstr, "%d", &days) < 1) {
    fprintf (stderr, "MJD::Construct Could not parse '%s'\n", mjdstr);
    return -1;
  }
  fracsec = 0.0;
  secs = 0;

  const char* fracstr = strchr (mjdstr, '.');
  if (fracstr) {
    double fracday;
    if (sscanf (fracstr, "%lf", &fracday) < 1)
      return -1;
    add_day (fracday);
  }
  return 0;
}

ostream& operator << (ostream& ostr, const MJD& mjd)
{
  int precision = ostr.precision();
  if (!precision)
    precision = MJD::ostream_precision;

  double ddays = mjd.in_days();

  if (precision < std::numeric_limits<double>::digits10)
    return ostr << ddays;
  else
  {
    int digits = 1;
    if (ddays != 0)
      digits = int(log(fabs(ddays))/log(10.0)) + 1;
    return ostr << mjd.printdays(precision-digits);
  }
}

istream& operator >> (istream& istr, MJD& mjd)
{
  string parse;
  istr >> parse;
  if (mjd.Construct (parse.c_str()) < 0)
    istr.setstate (ios::failbit);
  return istr;
}

bool equal (const MJD &m1, const MJD &m2)
{
  return m1.days == m2.days &&
    m1.secs == m2.secs &&
    (fabs (m1.fracsec-m2.fracsec))/2.0 < MJD::precision;
}

