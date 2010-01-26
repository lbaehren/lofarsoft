//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/BasicArchive.h,v $
   $Revision: 1.40 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __BasicArchive_h
#define __BasicArchive_h

#include "Pulsar/Archive.h"
  
namespace Pulsar {

  //! Defines the pure virtual methods declared in Pulsar::Archive
  /*!  This class is designed to make it easier to develop new objects
    of type Pulsar::Archive.  It is a minimal implimentation of the
    Pulsar::Archive class that defines a set of variables which can be
    used to store the important information required by the
    Pulsar::Archive class.  It also defines most of the pure virtual
    methods required.  If a user wishes to work with data from a new
    instrument, they can start with the BasicArchive class and simply
    define the load and unload functions to perform file I/O. This
    should simply be a matter of reading from the file and plugging
    information into the variables defined in this class. It is
    possible for the user to add as many more features to their new
    class as required.  */

  class BasicArchive : public Archive {

  public:

    //! null constructor
    BasicArchive ();

    //! copy constructor
    BasicArchive (const BasicArchive& archive);

    //! assignment operator
    const BasicArchive& operator = (const BasicArchive& archive);

    //! destructor
    ~BasicArchive ();

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Get the name of the telescope
    virtual std::string get_telescope () const;
    //! Set the name of the telescope
    virtual void set_telescope (const std::string&);

    //! Get the state of the profiles
    virtual Signal::State get_state () const;
    //! Set the state of the profiles
    virtual void set_state (Signal::State state);

    //! Get the scale in which flux density is measured
    virtual Signal::Scale get_scale () const;
    //! Set the scale in which flux density is measured
    virtual void set_scale (Signal::Scale scale);

    //! Get the observation type (psr, cal)
    virtual Signal::Source get_type () const;
    //! Set the observation type (psr, cal)
    virtual void set_type (Signal::Source type);

    //! Get the source name
    virtual std::string get_source () const;
    //! Set the source name
    virtual void set_source (const std::string& source);

    //! Get the coordinates of the source
    virtual sky_coord get_coordinates () const;
    //! Set the coordinates of the source
    virtual void set_coordinates (const sky_coord& coordinates);

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nbin () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nchan () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_npol () const;

    //! Get the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nsubint () const;

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const;
    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw);

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const;
    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf);

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual void set_dispersion_measure (double dm);

    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const;
    //! Set the status of the dispersion delay flag
    virtual void set_dedispersed (bool done = true);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual double get_rotation_measure () const;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual void set_rotation_measure (double rm);

    //! data have been corrected for ISM faraday rotation
    virtual bool get_faraday_corrected () const;
    //! Set the status of the ISM RM flag
    virtual void set_faraday_corrected (bool done = true);

    //! Data has been poln calibrated
    virtual bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    virtual void set_poln_calibrated (bool done = true);

    //! Ensure that BasicArchive code is linked
    /*! This method is called in Archive::load only to ensure that the
      BasicArchive object code is linked for use by plugin classes. */
    static void ensure_linkage ();

  protected:

    //! Set the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nbin (unsigned nbin);

    //! Set the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nchan (unsigned nchan);

    //! Set the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual void set_npol (unsigned npol);

    //! Set the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual void set_nsubint (unsigned nsubint);

    //! The name of the telescope
    std::string telescope;

    //! The feed configuration of the receiver
    Signal::Basis basis;

    //! The state of the profiles
    Signal::State state;

    //! The scale in which flux density is measured
    Signal::Scale scale;

    //! The observation type (psr, cal)
    Signal::Source type;

    //! The source name
    std::string source;

    //! The coordinates of the source
    sky_coord coordinates;

    //! The number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    unsigned nbin;

    //! The number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned nchan;

    //! The number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    unsigned npol;

    //! The number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    unsigned nsubint;

    //! The overall bandwidth of the observation
    double bandwidth;

    //! The centre frequency of the observation
    double centre_frequency;

    //! The dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double dispersion_measure;

    //! The rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double rotation_measure;

    //! data have been dedispersed
    bool dedispersed;

    //! data have been corrected for ISM faraday rotation
    bool faraday_corrected;

    //! Data has been poln calibrated
    bool poln_calibrated;

     //! Return a pointer to a new BasicIntegration
    Integration* new_Integration (const Integration* copy_this = 0);
    
  };

}

#endif

