//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/EPN/Pulsar/EPNArchive.h,v $
   $Revision: 1.17 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __EPN_Archive_h
#define __EPN_Archive_h

#include "Pulsar/Archive.h"
#include "Pulsar/Agent.h"
#include "epnio.h"

namespace Pulsar {

  //! Reads the European Pulsar Network (EPN) file format
  class EPNArchive : public Archive {

  public:

    //! Default constructor
    EPNArchive ();

    //! Copy constructor
    EPNArchive (const EPNArchive& archive);

    //! Destructor
    ~EPNArchive ();

    //! Assignment operator
    const EPNArchive& operator = (const EPNArchive& archive);
    
    //! Base copy constructor
    EPNArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Return a pointer to a new copy constructed instance equal to this
    EPNArchive* clone () const;

    //! Get the telescope name
    std::string get_telescope () const;
    //! Set the telescope name
    void set_telescope (const std::string&);

    //! Get the state of the profiles
    Signal::State get_state () const;
    //! Set the state of the profiles
    void set_state (Signal::State state);

    //! Get the scale of the profiles
    Signal::Scale get_scale () const;
    //! Set the scale of the profiles
    void set_scale (Signal::Scale scale);

    //! Get the observation type (psr, cal)
    Signal::Source get_type () const;
    //! Set the observation type (psr, cal)
    void set_type (Signal::Source type);

    //! Get the source name
    std::string get_source () const;
    //! Set the source name
    void set_source (const std::string& source);

    //! Get the coordinates of the source
    sky_coord get_coordinates () const;
    //! Set the coordinates of the source
    void set_coordinates (const sky_coord& coordinates);

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    unsigned get_nbin () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned get_nchan () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned get_npol () const;

    //! Get the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    unsigned get_nsubint () const;

    //! Get the overall bandwidth of the observation
    double get_bandwidth () const;
    //! Set the overall bandwidth of the observation
    void set_bandwidth (double bw);

    //! Get the centre frequency of the observation
    double get_centre_frequency () const;
    //! Set the centre frequency of the observation
    void set_centre_frequency (double cf);

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    void set_dispersion_measure (double dm);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double get_rotation_measure () const;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    void set_rotation_measure (double rm);

    //! Inter-channel dispersion delay has been removed
    bool get_dedispersed () const;
    //! Set true when the inter-channel dispersion delay has been removed
    void set_dedispersed (bool done = true);

    //! Data has been corrected for ISM faraday rotation
    bool get_faraday_corrected () const;
    //! Set the status of the ISM RM flag
    void set_faraday_corrected (bool done = true);

    //! Data has been poln calibrated
    bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    void set_poln_calibrated (bool done = true);

  protected:

    friend class Archive::Advocate<EPNArchive>;

    // Advocates the use of the EPNArchive plugin
    class Agent : public Archive::Advocate<EPNArchive> {
    
      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of EPNArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the EPNArchive plugin
        std::string get_name () { return "EPN"; }

        //! Return description of this plugin
        std::string get_description ();

    }; 

    //! Load the header information from filename
    void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    Integration* load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is not implemented
    bool can_unload () const { return false; }

    //! Unload the Archive (header and Integration data) to filename
    void unload_file (const char* filename) const;

    /** @name EPN data
     *  These structures contain data returned by crwepn */
    //@{

    epn_header_line1 line1;

    epn_header_line2 line2;
    
    epn_header_line3 line3;
    
    epn_header_line4 line4;
    
    epn_header_line5 line5;
    
    epn_block_subheader_line1 sub_line1;

    epn_block_subheader_line2 sub_line2;

    epn_data_block data;

    unsigned current_record;

    //@}

    //! The centre frequency
    double centre_frequency;

    //! The bandwidth
    double bandwidth;

    //! The state
    Signal::State state;

    //! The number of sub-integrations
    unsigned nsubint;

    //! Set the number of pulsar phase bins
    virtual void set_nbin (unsigned numbins);

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned numchan);

    //! Set the number of polarization measurements
    virtual void set_npol (unsigned numpol);

    //! Set the number of sub-integrations
    virtual void set_nsubint (unsigned nsubint);

    //! The subints vector will point to BasicIntegrations
    virtual Integration* new_Integration (const Integration* copy_this = 0);

    //! Initialize data structures
    void init();

    //! Read the requested record from filename
    void read_record (const char* filename, unsigned record);

  };



}

#endif
