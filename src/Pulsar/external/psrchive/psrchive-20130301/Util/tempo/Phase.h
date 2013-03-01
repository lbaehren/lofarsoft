/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __POLYCO_PHASE_H
#define __POLYCO_PHASE_H

#include <inttypes.h>
#include "MJD.h"

#include <string>
#include <math.h>

#ifdef HAVE_MPI
#include <mpi.h>
#endif

class Phase {

 private:
  int64_t turns;
  double fturns;

  void settle ();

 public:

  //! The largest number to be considered "close enough" to zero
  static double rounding_threshold;

  static const Phase zero;

  //! Default constructor
  Phase (double turns=0);

  Phase (int64_t tns, double ftns);

  const Phase& operator= (const Phase&);

  friend Phase operator + (const Phase &, double); 
  friend Phase operator - (const Phase &, double); 
  friend Phase operator + (const Phase &, const Phase &);
  friend Phase operator - (const Phase &, const Phase &);
  friend Phase operator - (const Phase &);

  // some may disagree with this usage of the operator
  friend MJD operator * (const Phase &, double period);
  friend MJD operator / (const Phase &, double frequency);

  const Phase& operator += (const Phase &);
  const Phase& operator -= (const Phase &);
  const Phase& operator += (double);
  const Phase& operator -= (double);
  const Phase& operator += (int);
  const Phase& operator -= (int);
  // increment/decrement by one turn
  const Phase& operator ++ ();
  const Phase& operator -- ();

  friend bool operator > (const Phase &, const Phase &) ;
  friend bool operator < (const Phase &, const Phase &) ;
  friend bool operator >= (const Phase &, const Phase &);
  friend bool operator <= (const Phase &, const Phase &);
  friend bool operator == (const Phase &, const Phase &);
  friend bool operator != (const Phase &, const Phase &);

  Phase Ceil  ();
  Phase Floor ();
  Phase Rint  ();

  void set (int64_t tns, double ftns);

  double in_turns() const;
  int64_t  intturns() const;
  double fracturns() const;
  std::string strprint(int precision) const;

#ifdef HAVE_MPI
  friend int mpiPack_size (const Phase&, MPI_Comm comm, int* size);
  friend int mpiPack   (const Phase&, void* outbuf, int outcount, 
			int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			Phase*, MPI_Comm comm);
#endif

};

inline std::ostream& operator<< (std::ostream& ostr, const Phase& sz) 
{ return ostr << sz.strprint(8); }

#endif
