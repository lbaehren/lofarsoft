//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ChannelSubsetMatch_h
#define __Pulsar_ChannelSubsetMatch_h

#include "Pulsar/Config.h"
#include "Pulsar/Database.h"

namespace Pulsar {

  class Archive;

  //! Determine if one set of channels is a subset of another
  class ChannelSubsetMatch {

  public:

    //! Default constructor
    ChannelSubsetMatch ();

    //! Default constructor
    ~ChannelSubsetMatch ();

    //! Check that Archive sub's channels all exist in super 
    bool match (const Archive* super, const Archive* sub);

    //! Same test for Database::Entry objects
    bool match (const Database::Entry& super, const Database::Entry& sub);

    //! Return channel in super matching the given channel in sub
    //! -1 == no match
    int super_channel (const Archive* super, const Archive* sub, int subchan);

    //! Return channel in sub matching the given channel in super
    //! -1 == no match
    int sub_channel (const Archive* super, const Archive* sub, int superchan);

    //! Get reason for match failure
    std::string get_reason() const { return reason; }

    template<class Container>
    unsigned match_channel (const Container* container, double frequency);

  protected:

    //! Match failure reason
    std::string reason;

    //! Fractional freq matching tolerance
    double freq_tol;

    //! Fractional BW matching tolerance
    double bw_tol;
  };

}

template<class Container>
unsigned Pulsar::ChannelSubsetMatch::match_channel (const Container* container,
						    double frequency)
{
  for (unsigned j=0; j<container->get_nchan(); j++)
  {
    double cfreq = container->get_centre_frequency(j);
    if (fabs((frequency-cfreq)/frequency) < freq_tol)
      return j;
  }

  throw Error (InvalidParam, "Pulsar::ChannelSubsetMatch::match_channel",
	       "no match for frequency=%lf", frequency);
}

#endif
