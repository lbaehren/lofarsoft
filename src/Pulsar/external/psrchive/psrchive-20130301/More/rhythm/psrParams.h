//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* ///////////////////////////////////////////////////////////////////////
   psrParams --- light-weight object that can store TEMPO parameter set

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __PSRPARAMS_H
#define __PSRPARAMS_H
   
#include "psrephem.h"

#include <vector>

class psrParameter;  // implementation detail defined in psrParameter.h

class psrParams
{
  friend class qt_psrephem;

 public:
  psrParams () {};
  psrParams (const psrParams& p) { *this = p; }
  psrParams& operator = (const psrParams& p);

  void set_psrephem (const Legacy::psrephem& eph);
  void get_psrephem (Legacy::psrephem& eph);

  // static members and methods
  static bool verbose;

 protected:
  std::vector <psrParameter*> params;    // pulsar parameters
  void destroy ();
};

#endif
