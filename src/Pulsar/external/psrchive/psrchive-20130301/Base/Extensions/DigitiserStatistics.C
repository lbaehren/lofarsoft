//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <string.h>

#include "Pulsar/DigitiserStatistics.h"
#include "TextInterface.h"

// //////////////////////////////////////////////////
// DigitiserStatistics methods
//

Pulsar::DigitiserStatistics::DigitiserStatistics () 
  : Extension ("DigitiserStatistics")
{
  init(); 
}
 
Pulsar::DigitiserStatistics::DigitiserStatistics (const DigitiserStatistics& extension)
  : Extension ("DigitiserStatistics")
{
  ndigr = extension.ndigr;
  npar = extension.npar;
  ncycsub = extension.ncycsub;
  diglev = extension.diglev;
  rows = extension.rows;
}

const Pulsar::DigitiserStatistics& 
Pulsar::DigitiserStatistics::operator= (const DigitiserStatistics& extension)
{
  ndigr = extension.ndigr;
  npar = extension.npar;
  ncycsub = extension.ncycsub;
  diglev = extension.diglev;
  rows = extension.rows;
  return *this;
}

Pulsar::DigitiserStatistics::~DigitiserStatistics ()
{
}

void Pulsar::DigitiserStatistics::init ()
{
  rows.resize(0);
}

Pulsar::DigitiserStatistics::row& Pulsar::DigitiserStatistics::get_row (unsigned i)
{
  return rows[i];
}

Pulsar::DigitiserStatistics::row& Pulsar::DigitiserStatistics::get_last ()
{
  return rows.back();
}

void Pulsar::DigitiserStatistics::push_blank_row ()
{
  rows.push_back(row());
  rows.back().index = rows.size();
}

// Row methods

Pulsar::DigitiserStatistics::row::~row ()
{
}

void Pulsar::DigitiserStatistics::row::init ()
{
//   dig_mode = "unknown";
//   diglev   = "unknown";
// 
//   ndigr   = 0;
//   nlev    = 0;
//   ncycsub = 0;

  data.resize(0);
}

void Pulsar::DigitiserStatistics::resize (unsigned nsubint, unsigned npol,
			      unsigned nchan, unsigned nbin)
{
  rows.resize(nsubint);
}

class RowInterface
  : public TextInterface::To<Pulsar::DigitiserStatistics::row>
{
public:
  RowInterface( Pulsar::DigitiserStatistics::row *s_instance = NULL )
  {
    if( s_instance )
      set_instance( s_instance );

    VGenerator<float> generator;
    add_value( generator( "atten", "Attenuator setting",
			  &Pulsar::DigitiserStatistics::row::get_atten,
			  &Pulsar::DigitiserStatistics::row::set_atten,
			  &Pulsar::DigitiserStatistics::row::get_natten ) );
  }
};

class Pulsar::DigitiserStatistics::Interface
  : public TextInterface::To<DigitiserStatistics>
{
public:
  Interface( DigitiserStatistics *s_instance = NULL )
  {
    if( s_instance )
      set_instance( s_instance );

    add( &DigitiserStatistics::get_ndigr,
	 "ndigr", "Number of digitised channels (I)" );

    add( &DigitiserStatistics::get_npar,
	 "npar", "Number of digitiser parameters" );
    
    add( &DigitiserStatistics::get_ncycsub,
	 "ncycsub", "Number of correlator cycles per subint" );

    add( &DigitiserStatistics::get_diglev,
	 "diglev", "Digitiser level-setting mode (AUTO, FIX)" );

    import( "int", RowInterface(),
	    &DigitiserStatistics::get_row_ptr,
	    &DigitiserStatistics::get_nrow );
  }
};

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::DigitiserStatistics::get_interface()
{
  return new Interface( this );
}
