/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "FITSUTC.h"
#include <sstream>
#include <tostring.h>



using namespace std;



FITSUTC::FITSUTC( const std::string &src )
{
  FromString( src );
}

int FITSUTC::GrabInt( istream &instr, char delim )
{
  char nextval[51];
  instr.getline( nextval, 50, delim );

  return fromstring<int>( nextval );
}




void FITSUTC::FromString( const std::string &src )
{
  istringstream instr( src );

  tm tmd;

  tmd.tm_year = GrabInt( instr, '-' ) - 1900;
  tmd.tm_mon = GrabInt( instr, '-'  ) - 1;
  tmd.tm_mday = GrabInt( instr, 'T'  );
  tmd.tm_hour = GrabInt( instr, ':'  );
  tmd.tm_min = GrabInt( instr, ':'  );
  tmd.tm_sec = GrabInt( instr, '\n' );

  Construct( tmd );
}



std::string FITSUTC::ToString( void ) const
{
  char dstr[31];
  datestr( dstr, 30, "%Y-%m-%eT%H:%M:%S" );
  return dstr;
}


std::ostream& operator<< (std::ostream& ostr, const FITSUTC& fitsutc )
{
  return ostr << fitsutc.ToString();
}



