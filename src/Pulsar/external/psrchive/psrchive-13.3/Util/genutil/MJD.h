//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1998 by Matthew Britton
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/MJD.h,v $
   $Revision: 1.32 $
   $Date: 2009/06/17 08:12:20 $
   $Author: straten $ */

#ifndef __GENUTIL_MJD_H
#define __GENUTIL_MJD_H

#include <inttypes.h>
#include "utc.h"

#include <iostream>
#include <limits>
#include <string>
#include <stdio.h>

#ifdef HAVE_MPI
#include <mpi.h>
#endif

class MJD {

 private:
  int    days;
  int    secs;
  double fracsec;

  void add_day (double dd);
  void settle();

 public:
  static const MJD zero;

  static double   precision;
  static unsigned ostream_precision;

  static int verbose;

  // null constructor
  MJD () { days=0;secs=0;fracsec=0.0; };

  // construct from standard C types
  MJD (double dd);                        // mjd given as day
  MJD (int intday, double fracday);       // mjd given with separate fracday
  MJD (double dd, double ss, double fs);  // mjd with seconds and fracsec
  MJD (int dd, int ss, double fs);        // again

  // construct from a C string of the form "50312.4569"
  MJD (const char* mjdstring);

  // some standard C time formats
  MJD (time_t time);                // returned by time()
  MJD (const struct tm& greg);      // returned by gmtime()
  MJD (const struct timeval& tp);   // returned by gettimeofday()

  // simple little struct invented in S2 days
  MJD (const utc_t& utc);

  // construct from a C++ string
  MJD (const std::string& mjd);

  // parses a string of the form 51298.45034 ish
  int Construct (const char* mjdstr);

  // constructs an MJD from the unix time_t
  int Construct (time_t time);

  // constructs an MJD from the unix struct tm
  int Construct (const struct tm& greg);

  // another UNIX time standard
  int Construct (const struct timeval& tp);

  // constructs an MJD from the home-grown utc_t
  int Construct (const utc_t& utc);

  // constructs from a BAT (binary atomic time), ie MJD in microseconds
  // stored in two 32 bit unsigned integers --- as returned by the AT clock
  int Construct (unsigned long long bat);

  double in_seconds() const;
  double in_days()    const;
  double in_minutes() const;

  // cast into other forms
  int UTC       (utc_t* utc, double* fsec=NULL) const;
  int gregorian (struct tm* gregdate, double* fsec=NULL) const;

  MJD & operator = (const MJD &);
  MJD & operator += (const MJD &);
  MJD & operator -= (const MJD &);
  MJD & operator += (const double &);
  MJD & operator -= (const double &);
  MJD & operator *= (const double &);
  MJD & operator /= (const double &);
  const friend MJD operator + (const MJD &, const MJD &);
  const friend MJD operator - (const MJD &, const MJD &);
  const friend MJD operator + (const MJD &, double);  // Add seconds to an MJD
  const friend MJD operator - (const MJD &, double);  // Take seconds from MJD
  const friend MJD operator * (const MJD &, double);  
  const friend MJD operator * (double a, const MJD& m)
    { return m * a; }
  const friend MJD operator / (const MJD &, double);  

  // return the -ve of m
  const friend MJD operator - (MJD m);

  friend int operator > (const MJD &, const MJD &);
  friend int operator < (const MJD &, const MJD &);
  friend int operator >= (const MJD &, const MJD &);
  friend int operator <= (const MJD &, const MJD &);
  friend int operator == (const MJD &, const MJD &);
  friend int operator != (const MJD &, const MJD &);

  // function to return plotable value to xyplot template class
  float plotval() const { return float (in_days()); };
  
  // These bits are useful for tempo
  int    intday()  const;                // To access the integer day
  double fracday() const;                // To access fractional day
  int    get_secs() const {return(secs);};
  double get_fracsec() const {return(fracsec);};

  // return LST in hours (longitude given in degrees East of Greenwich)
  double LST (double longitude) const;

  std::string printdays (unsigned precision) const;

  int print (FILE *stream);
  int println (FILE *stream);
  std::string printall()    const;
  std::string printdays()   const;
  std::string printhhmmss() const;
  std::string printfs()     const;
  std::string strtempo() const; 

  // returns a string formatted by strftime
  // e.g. format = "%Y-%m-%d %H:%M:%S"
  char* datestr (char* dstr, int len, const char* format) const;

#ifdef HAVE_MPI
  friend int mpiPack_size (const MJD&, MPI_Comm comm, int* size);
  friend int mpiPack   (const MJD&, void* outbuf, int outcount, 
			int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			MJD*, MPI_Comm comm);
#endif

 protected:
  friend bool equal (const MJD &m1, const MJD &m2);

};

inline double cast_double(const MJD&m) {return m.in_days();}

std::ostream& operator<< (std::ostream& ostr, const MJD& mjd);
std::istream& operator>> (std::istream& istr, MJD& mjd);

// Enable use of the MJD class with std C++ numeric_limits traits
namespace std {
  template<>
  class numeric_limits<MJD> {
    public:
    static const int digits10 = 15;
  };
}

#endif  /* not __MJD_H defined */

