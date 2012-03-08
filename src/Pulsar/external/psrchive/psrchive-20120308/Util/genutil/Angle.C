/***************************************************************************
 *
 *   Copyright (C) 1999 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Angle.h"
#include "Cartesian.h"
#include "coord_parse.h"
#include "Error.h"

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>

using namespace std;

bool Angle::verbose = false;
Angle::Type Angle::default_type = Angle::Radians;

Angle::Angle (const double & rad) { init(); setRadians(rad); }

void Angle::wrap()
{
  //while(radians>wrap_point) radians-=2.0*wrap_point;
  //while(radians<-wrap_point) radians+=2.0*wrap_point;

  // first cut the radian down to size

  while (fabs(radians) > 4.0*M_PI)
  {
    double irad = floor(0.5*radians/M_PI);
    irad *= 2.0*M_PI;
    radians -= irad;
  }

  while (radians>wrap_point) radians-=2.0*M_PI;
  while (radians<wrap_point-2.0*M_PI) radians+=2.0*M_PI;
}

char * Angle::getHMS(char *str, int places) const
{
  ra2str (str, ANGLE_STRLEN, radians, places);
  return str;
}

static char angle_str [ANGLE_STRLEN];

std::string Angle::getHMS (int places) const
{
  // cerr << "std::string Angle::getHMS" << endl;
  ra2str (angle_str, ANGLE_STRLEN, radians, places);
  return angle_str;
}

int Angle::setHMS(const char *str)
{
  int ret = str2ra (&radians, str);
  setWrapPoint (2*M_PI);
  return ret;
}

int Angle::setDMS(const char *str)
{
  return str2dec2(&radians, str);
}

void Angle::setHMS (int hours, int minutes, double seconds)
{
  radians = M_PI * ( hours/12.0 + minutes/720.0 + seconds/43200.0 );
  setWrapPoint (2*M_PI);
}

void Angle::getHMS (int& hours, int& minutes, double& seconds) const
{
  double posradians = radians;
  if (posradians < 0.0)
     posradians += 2.0*M_PI;

  hours = (int)floor(posradians * 12.0/M_PI); 
  minutes = (int)floor(posradians * 720.0/M_PI) % 60;
  seconds = posradians * 43200.0/M_PI - ((hours*60.0)+minutes)*60.0;
}

void Angle::setDMS (int degrees, int minutes, double seconds)
{
  radians = (M_PI/180.0) * ( degrees + minutes/60.0 + seconds/3600.0 );
}

double from_ttmmss (double ttmmss)
{
  // cerr << "ttmmss=" << ttmmss << endl;

  int tt = int (ttmmss * 1e-4);
  ttmmss -= tt * 10000;
  // cerr << "tt=" << tt << endl;

  int mm = int (ttmmss * 1e-2);
  ttmmss -= mm * 100;
  // cerr << "mm=" << mm << endl;

  double turns = tt + double(mm)/60.0 + ttmmss/3600.0;
  // cerr << "turns=" << turns << endl;

  return turns;
}

double to_ttmmss (double turns)
{
  // cerr << "turns=" << turns << endl;

  int tt = int (turns);
  turns -= tt;
  // cerr << "tt=" << tt << endl;

  turns *= 60.0;
  int mm = int (turns);
  turns -= mm;
  // cerr << "mm=" << mm << endl;

  double ttmmss = double(tt)*1e4 + double(mm)*1e2 + turns*60;
  // cerr << "ttmmss=" << ttmmss << endl;

  return ttmmss;
}

void Angle::setDegMS (double deg_mmss)
{
  setRadians( from_ttmmss(deg_mmss) * M_PI/180.0 );
}

double Angle::getDegMS () const
{
  return to_ttmmss(radians * 180.0/M_PI);
}

void Angle::setHourMS (double hour_mmss)
{
  setRadians( from_ttmmss(hour_mmss) * M_PI/12.0 );
}

double Angle::getHourMS () const
{
  return to_ttmmss(radians * 12.0/M_PI);
}

void Angle::setTurnMS (double turn_mmss)
{
  setRadians( from_ttmmss(turn_mmss) * (2.0*M_PI) );
}

double Angle::getTurnMS () const
{
  return to_ttmmss(radians / (2.0*M_PI));
}

void Angle::getDMS (int& degrees, int& minutes, double& seconds) const
{
  degrees = (int)floor(fabs(radians) * 180.0/M_PI); 
  minutes = (int)floor(fabs(radians) * 10800.0/M_PI) % 60;
  seconds = fabs(radians) * 648000.0/M_PI - ((degrees*60.0)+minutes)*60.0;
}

char * Angle::getDMS (char *str,int places) const
{
  dec2str2 (str, ANGLE_STRLEN, radians, places);
  return str;
}

std::string Angle::getDMS (int places) const
{
  dec2str2 (angle_str, ANGLE_STRLEN, radians, places);
  return angle_str;
}


Angle& Angle::operator= (const Angle & a)
{
  radians = a.radians;
  wrap_point = a.wrap_point;
  return *this;
}

Angle& Angle::operator= (const double &rad)
{
  setRadians(rad);
  return *this;
}

Angle& Angle::operator+= (const Angle & a1)
{
  *this = *this + a1;
  return(*this);
}

Angle& Angle::operator-= (const Angle & a1)
{
  *this = *this - a1;
  return(*this);
}

Angle& Angle::operator+= (const double & d)
{
  *this = *this + d;
  return(*this);
}

Angle& Angle::operator-= (const double & d)
{
  *this = *this - d;
  return(*this);
}

Angle& Angle::operator*= (const double & d)
{
  *this = *this * d;
  return(*this);
}

Angle& Angle::operator/= (const double & d)
{
  *this = *this / d;
  return(*this);
}

const Angle operator + (const Angle &a1, const Angle &a2)
{
  Angle a;
  a.setWrapPoint((a1.wrap_point > a2.wrap_point ? 
                 a1.wrap_point: a2.wrap_point));
  a.setRadians(a1.radians+a2.radians);
  return a;
}

// returns the negative
const Angle operator - (const Angle& a)
{
  Angle ret(a);
  ret.setRadians(-a.radians);
  return ret;
}

const Angle operator - (const Angle &a1, const Angle &a2)
{
  Angle a3 = a2*-1.0;
  return (a1+a3);
}

const Angle operator + (const Angle &a1, double d)
{
  Angle a(a1);
  a.setRadians(a.radians+d);
  return a;
}

const Angle operator - (const Angle &a1, double d)
{
  Angle a(a1);
  a.setRadians(a.radians-d);
  return a;
}

const Angle operator * (const Angle &a1, double d)
{
  Angle a(a1);
  a.setRadians(a.radians*d);
  return a;
}

// NOTE : this returns double, not angle, since angle^2 is not
// limited to <pi radians as angle is
// it returns the product of the angles.radians
double operator * (const Angle &a1, const Angle &a2)
{
  return a1.getRadians()*a2.getRadians();
}

const Angle operator / (const Angle &a1, double d)
{
  Angle a(a1);
  a.setRadians(a.radians/d);
  return a;
}

int operator > (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getRadians()-a2.getRadians())<precision_limit) return(0);
  else return (a1.getRadians()>a2.getRadians());
}

int operator >= (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getRadians()-a2.getRadians())<precision_limit) return(1);
  else return (a1.getRadians()>a2.getRadians());
}

int operator < (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getRadians()-a2.getRadians())<precision_limit) return(0);
  else return (a1.getRadians()<a2.getRadians());
}

int operator <= (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if(fabs(a1.getRadians()-a2.getRadians())<precision_limit) return(1);
  else return (a1.getRadians()<a2.getRadians());
}

int operator == (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if ((fabs(a1.getRadians()-a2.getRadians())<precision_limit)) 
      return (1);
  else
      return (0);  
}

int operator != (const Angle &a1, const Angle &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  if ((fabs(a1.getRadians()-a2.getRadians())>precision_limit)) 
      return (1);
  else
      return (0);  
}



std::ostream& operator << (std::ostream& os, const Angle& angle)
{
  switch (Angle::default_type)
  {
  case Angle::Radians:
    // the previous default operation
    return os << angle.getRadians();
  case Angle::Degrees:
    return os << angle.getDegrees() << "deg";
  case Angle::Turns:
    return os << angle.getTurns() << "trn";
  }
  return os;
}

std::istream& operator >> (std::istream& is, Angle& angle)
{
  double value;
  is >> value;

  if (is.fail())
    return is;

  bool use_default = true;

  if (!is.eof())
  {
    use_default = false;

    std::streampos pos = is.tellg();

    std::string unit;
    is >> unit;
    if (unit == "deg")
      angle.setDegrees (value);
    else if (unit == "rad")
      angle.setRadians (value);
    else if (unit == "trn")
      angle.setTurns (value);
    else 
    {
      // replace the text and revert to the default
      is.seekg (pos);
      use_default = true;
    }
  }

  if (use_default)
  {
    switch (Angle::default_type)
    {
    case Angle::Radians:
      angle.setRadians (value); break;
    case Angle::Degrees:
      angle.setDegrees(value); break;
    case Angle::Turns:
      angle.setTurns(value); break;
    }
  }

  return is;
}


AnglePair::AnglePair (const AnglePair& a)
{
  angle1 = a.angle1;
  angle2 = a.angle2;
}


// AnglePair stuff for convenience

AnglePair::AnglePair(const Angle & a1, const Angle & a2)
{
  angle1 = a1;
  angle2 = a2;
}

AnglePair::AnglePair(const double d1, const double d2)
{
  angle1 = d1;
  angle2 = d2;
}

AnglePair::AnglePair (const std::string& astr)
{ 
  if (str2coord (&angle1.radians, &angle2.radians, astr.c_str()) < 0)
    throw Error (InvalidParam, "AnglePair::AnglePair",
                 "str2coord(" + astr + ") failure");
  angle1.setWrapPoint (2*M_PI);
}

int AnglePair::setHMSDMS(const char *astr)
{ 
  int ret = str2coord (&angle1.radians, &angle2.radians, astr);
  angle1.setWrapPoint (2*M_PI);
  return ret;
}

int AnglePair::setHMSDMS(const string& s1, const string& s2)
{
  return setHMSDMS (s1.c_str(), s2.c_str());
}

int AnglePair::setHMSDMS(const char *s1, const char *s2)
{
  if (angle1.setHMS(s1) < 0)
    return -1;
  return angle2.setDMS(s2);
}

void AnglePair::getHMSDMS(char *s1, char *s2, int places1, int places2) const
{
  s1 = angle1.getHMS(s1,places1);
  s2 = angle2.getDMS(s2,places2);
}

std::string
AnglePair::getHMSDMS(int places1, int places2, bool space) const
{ 
  std::string cspace;
  if (space) cspace = " ";

  return angle1.getHMS(places1) + cspace + angle2.getDMS(places2);
}

void 
AnglePair::setDegrees(double d1, double d2)
{
  angle1.setDegrees(d1);
  angle2.setDegrees(d2);
}

void 
AnglePair::getDegrees(double *d1, double *d2) const
{
  *d1 = angle1.getDegrees();
  *d2 = angle2.getDegrees();
}

std::string AnglePair::getDegrees() const
{
  sprintf (angle_str, "(%8.4f,%8.4f)",
           angle1.getDegrees(), angle2.getDegrees());
  return std::string (angle_str);
}

void 
AnglePair:: setRadians(double r1, double r2)
{
  angle1.setRadians(r1);
  angle2.setRadians(r2);
}

void 
AnglePair:: getRadians(double *r1, double *r2) const
{
  *r1 = angle1.getRadians();
  *r2 = angle2.getRadians();
}

std::string AnglePair::getRadians() const
{
  sprintf(angle_str,
          "(%8.4f,%8.4f)", angle1.getRadians(),angle2.getRadians());
  return std::string (angle_str);
}

void
AnglePair:: setRadMS(long int az, long int ze)
{
  angle1.setRadMS(az);
  angle2.setRadMS(ze);
}

void
AnglePair:: getRadMS(long int *az, long int *ze)
{
  *az = angle1.getRadMS();
  *ze = angle2.getRadMS();
}


// redwards 10Aug99 function to compute angular separation of a pair
// of spherical coordinates
// straten 05Oct99 got rid of references to slalib
Angle AnglePair::angularSeparation(const AnglePair& other) const
{
  // convert the angles in spherical coordinates to unit Cartesian vectors
  Cartesian u1 (*this);
  Cartesian u2 (other);

  return Cartesian::angularSeparation (u1, u2);
}

AnglePair & AnglePair::operator*= (const double mult)
{
  angle1 *= mult;
  angle2 *= mult;
  return *this;
}

AnglePair operator* (const AnglePair & pair, const double mult)
{
  return AnglePair(pair.angle1*mult, pair.angle2*mult);
}

AnglePair &
AnglePair::operator= (const AnglePair & a)
{
  angle1 = a.angle1;
  angle2 = a.angle2;

  return *this;
}

std::ostream& operator<< (std::ostream & os, const AnglePair & pair)
{
  return os << "(" << pair.angle1 
            << "," << pair.angle2 << ")";
}

int operator == (const AnglePair &a1, const AnglePair &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  double ra1, ra2, dec1, dec2;
  a1.getRadians(&ra1, &dec1);
  a2.getRadians(&ra2, &dec2);
  if (fabs(ra1-ra2)<precision_limit && fabs(dec1-dec2)<precision_limit) 
      return (1);
  else
      return (0);  
}

int operator != (const AnglePair &a1, const AnglePair &a2)
{
  double precision_limit = 2*pow(10.0,-DBL_DIG);
  double ra1, ra2, dec1, dec2;
  a1.getRadians(&ra1, &dec1);
  a2.getRadians(&ra2, &dec2);
  if (fabs(ra1-ra2)>precision_limit || fabs(dec1-dec2)>precision_limit)
      return (1);
  else
      return (0);  
}

// redwards : trig functions. Not members since we are accustomed to
// calling normal functions for these
double sin(const Angle& a) { return sin(a.getRadians());}
double cos(const Angle& a) { return cos(a.getRadians());}
double tan(const Angle& a) { return tan(a.getRadians());}
Angle arctan(double g) { return Angle(atan(g));}
Angle arctan(double y, double x) { return Angle(atan2(y,x));}
