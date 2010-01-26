/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DigitiserCounts.h"
#include "TextInterface.h"



using Pulsar::DigitiserCounts;
using std::cerr;
using std::cout;
using std::endl;






//Default constructor
DigitiserCounts::DigitiserCounts ()
    : Extension( "DigitiserCounts" ), nlev(0), npthist(0), ndigr(0), dyn_levt(0)
{}

// Copy constructor
DigitiserCounts::DigitiserCounts (const Pulsar::DigitiserCounts& extension)
    : Extension( "DigitiserCounts" )
{
  operator = (extension);
}

// Operator =
const DigitiserCounts& DigitiserCounts::operator= (const DigitiserCounts& extension)
{
  dig_mode = extension.dig_mode;
  nlev = extension.nlev;
  npthist = extension.npthist;
  ndigr = extension.ndigr;
  diglev = extension.diglev;
  dyn_levt = extension.dyn_levt;
  subints = extension.subints;

  return *this;
}

// Destructor
DigitiserCounts::~DigitiserCounts ()
{}


class DigitiserCounts::Interface : public TextInterface::To<DigitiserCounts>
{
public:
  Interface( DigitiserCounts *s_instance )
  {
    if( s_instance )
      set_instance( s_instance );

    add( &DigitiserCounts::get_dig_mode,
	 "dig_mode", "Digitiser mode" );

    add( &DigitiserCounts::get_nlev,
	 "nlev", "Number of digitiser levels" );
    
    add( &DigitiserCounts::get_npthist, 
	 "npthist", "Number of points in histogram (I)" );
    
    add( &DigitiserCounts::get_diglev, 
	 "diglev", "Digitiser level-setting mode (AUTO, FIX)" );
  }
};

TextInterface::Parser* DigitiserCounts::get_interface()
{
  return new Interface( this );
}



/**
 * Append               Append the counts for the subints from the source to our subints vector
 *
 * @param src           The DigitiserCounts extension to append to this one.
 * @throws InvalidState If the number of subints in the source does not match ours
 **/

void DigitiserCounts::Append( const DigitiserCounts &src )
{
  if( subints.size() != 0 && src.subints.size() != 0 )
    if( subints[0].data.size() != src.subints[0].data.size() )
      throw Error( InvalidState, "DigitiserCounts::Append", "Can\'t append DigitiserCounts with differing nbin" );

  for( unsigned s = 0; s < src.subints.size(); s ++ )
    subints.push_back( src.subints[s] );
}


/**
 * Accumulate           Add the count values together with those from the source given.
 *
 * @param src           The DigitiserCounts extension to add to this one.
 * @throws InvalidState If the number of subints or counts int the source differs from ours
 **/

void DigitiserCounts::Accumulate( const DigitiserCounts &src )
{
  if( subints.size() != src.subints.size() )
    throw Error( InvalidState, "DigitiserCounts::Accumulate", "source number of subints differs" );

  if( subints[0].data.size() != src.subints[0].data.size() )
    throw Error( InvalidState, "DigitiserCounts::Accumulate", "source number of counts differs" );

  for( unsigned s = 0; s < subints.size(); s ++ )
    for( unsigned d = 0; d < subints[s].data.size(); d ++ )
      subints[s].data[d] += src.subints[s].data[d];
}


/**
 * CombineSubints       Add the count values between two subints to a target subint.
 *
 * @param subint        The target subint which the values will be added to.
 * @param start         The beginning of the range.
 * @param stop          The end of the range.
 * @throws InvalidRange If the start subint is > the stop subint.
 * @throws InvalidRange If the target subint index is greater than or equal to the number of existing subints.
 **/

void DigitiserCounts::CombineSubints(const unsigned subint, const unsigned start,
        const unsigned stop)
{
    if (Archive::verbose > 2)
        cerr << "Pulsar::DigitiserCounts::CombineSubints subint=" << subint <<
            " start=" << start << " stop=" << stop << endl;

    if (start > stop)
        throw Error(InvalidRange, "DigitiserCounts::CombineSubints", 
                "first subint=%u > last subint=%u", start, stop);

    if (subint >= subints.size())
        throw Error(InvalidRange, "DigitiserCounts::CombineSubints",
                "subint=%u >= number of existing subints");

    const unsigned dataSize = subints[0].data.size();

    for (unsigned iadd = start; iadd < stop; ++iadd)
        for (unsigned idata = 0; idata < dataSize; ++idata)
            subints[subint].data[idata] = subints[iadd].data[idata];
}
