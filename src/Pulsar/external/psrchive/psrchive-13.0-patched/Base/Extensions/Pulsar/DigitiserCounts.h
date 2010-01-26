//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/DigitiserCounts.h,v $
   $Revision: 1.10 $
   $Date: 2009/09/14 05:32:21 $
   $Author: jonathan_khoo $ */



#ifndef __Pulsar_DigitiserCounts_h
#define __Pulsar_DigitiserCounts_h

#include "Pulsar/ArchiveExtension.h"

using std::string;
using std::vector;



namespace Pulsar
{
  class DigitiserCounts : public Archive::Extension
  {
  public:
    //Default constructor
    DigitiserCounts ();

    // Copy constructor
    DigitiserCounts (const Pulsar::DigitiserCounts& extension);

    // Operator =
    const DigitiserCounts& operator= (const DigitiserCounts& extension);

    // Destructor
    ~DigitiserCounts ();

    //! Append the counts from another DigitiserCounts to this one
    void Append( const DigitiserCounts &src );

    //! Accumulate the counts from another DigitiserCounts onto this one
    void Accumulate( const DigitiserCounts &src );

    //! Clone method
    DigitiserCounts* clone () const { return new DigitiserCounts( *this ); }

    //! Combine, by adding, multiple subints into one
    void CombineSubints(const unsigned subint, const unsigned start,
            const unsigned stop);

    //! Get the text interface
    TextInterface::Parser* get_interface();
    class Interface;

    //! Class representing an array of digitiser counts for a subint
    class SubintCounts
    {
      public:
	SubintCounts() {};
	~SubintCounts() {};
	
	vector<long> data;
    };
    vector<SubintCounts> subints;

    void set_dig_mode( string s_dig_mode ) { dig_mode = s_dig_mode; }
    void set_nlev( int s_nlev ) { nlev = s_nlev; }
    void set_npthist( int s_npthist ) { npthist = s_npthist; }
    void set_diglev( string s_diglev ) { diglev = s_diglev; }
    void set_ndigr(int s_ndigr ) { ndigr = s_ndigr; }
    void set_dyn_levt( float s_dyn_levt ) { dyn_levt = s_dyn_levt; }

    string get_dig_mode( void ) const { return dig_mode; }
    int get_nlev( void ) const { return nlev; }
    int get_npthist( void ) const { return npthist; }
    string get_diglev( void ) const { return diglev; }
    int get_ndigr( void ) const { return ndigr; }
    float get_dyn_levt( void ) const { return dyn_levt; }
  private:
    string dig_mode;
    int nlev;
    int npthist;
    int ndigr;
    string diglev;
    float dyn_levt;
  };
}

#endif


