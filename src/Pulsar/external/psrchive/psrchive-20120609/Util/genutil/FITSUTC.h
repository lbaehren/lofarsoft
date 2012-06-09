/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#ifndef __FITS_UTC_h
#define __FITS_UTC_h



#include "MJD.h"
#include <string>
#include <iostream>




class FITSUTC : public MJD
{
public:
  FITSUTC() : MJD() {}
  FITSUTC( const std::string &src );
  FITSUTC( const MJD &src ) : MJD( src ) {}

  int GrabInt( std::istream &instr, char delim );
  std::string ToString( void ) const;

  void FromString( const std::string &src );
};

std::ostream& operator<< (std::ostream& ostr, const FITSUTC& fitsutc );



#endif

