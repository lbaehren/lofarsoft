//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ArchiveMatch.h,v $
   $Revision: 1.4 $
   $Date: 2008/11/07 01:46:52 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveMatch_h
#define __Pulsar_ArchiveMatch_h

#include "Pulsar/Archive.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Policy used to determine if two archives match
  class Archive::Match : public Reference::Able
  {

  public:

    //! Default constructor
    Match ();

    //! Empty destructor
    virtual ~Match () {}

    //! Clone operator
    virtual Match* clone () const;

    //! Check that the selected attributes match
    virtual bool match (const Archive* a, const Archive* b) const;

    //! Get the mismatch messages from the last call to the match method
    std::string get_reason () const { return reason; }

    //! Set to check that two archives may be combined
    void set_check_mixable (bool flag = true);
    bool get_check_mixable () const;

    //! Set to check that a standard and observation may be compared
    void set_check_standard (bool flag = true);
    bool get_check_standard () const;

    //! Set to check that a calibrator may be applied to an observation
    void set_check_calibrator (bool flag = true);
    bool get_check_calibrator () const;

    //! Set to check that two archives have been processed identically
    void set_check_processing (bool flag = true);
    bool get_check_processing () const;

    //! Set to check the state attribute
    void set_check_state (bool flag = true);
    bool get_check_state () const;

    //! Set to check the type attribute
    void set_check_type (bool flag = true);
    bool get_check_type () const;

    //! Set to check the source attribute
    void set_check_source (bool flag = true);
    bool get_check_source () const;

    //! Set to check the scale attribute
    void set_check_scale (bool flag = true);
    bool get_check_scale () const;

    //! Set to check the faraday_corrected attribute
    void set_check_faraday_corrected (bool flag = true);
    bool get_check_faraday_corrected () const;

    //! Set to check the dedispersed attribute
    void set_check_dedispersed (bool flag = true);
    bool get_check_dedispersed () const;

    //! Set to check the nbin attribute
    void set_check_nbin (bool flag = true);
    bool get_check_nbin () const;

    //! Set to check the nchan attribute
    void set_check_nchan (bool flag = true);
    bool get_check_nchan () const;

    //! Set to check the npol attribute
    void set_check_npol (bool flag = true);
    bool get_check_npol () const;

    //! Set to check the receiver attribute
    void set_check_receiver (bool flag = true);
    bool get_check_receiver () const;

    //! Set to check the centre_frequency attribute
    void set_check_centre_frequency (bool flag = true);
    bool get_check_centre_frequency () const;

    //! Set to check the bandwidth attribute
    void set_check_bandwidth (bool flag = true);
    bool get_check_bandwidth () const;

    //! Return true if the bandwidths match
    virtual bool get_bandwidth_match (const Archive* a, const Archive* b) const;

    //! The maximum amount by which the centre frequency may differ, in MHz
    static Option<double> max_frequency_difference;

    //! Allow opposite sidebands to match
    static Option<bool> opposite_sideband;

    //! String used to separate mismatch messages
    static std::string separator;

    //! Return a matching strategy based on the specified method
    template <typename T>
    static Match* factory (T method)
    {
      Match* match = new Match;
      (match->*method) (true);
      return match;
    }

  protected:

    //! The mismatch messages from the last call to the match method
    mutable std::string reason;

    bool check_state;
    bool check_type;
    bool check_source;
    bool check_scale;
    bool check_faraday_corrected;
    bool check_dedispersed;
    bool check_nbin;
    bool check_nchan;
    bool check_npol;
    bool check_receiver;
    bool check_centre_frequency;
    bool check_bandwidth;
  };

}

#endif
