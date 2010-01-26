//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Database.h,v $
   $Revision: 1.20 $
   $Date: 2009/05/07 06:09:40 $
   $Author: sosl $ */

#ifndef __Pulsar_Database_h
#define __Pulsar_Database_h

#include "Pulsar/Calibrator.h"
#include "Pulsar/Config.h"

#include "MEAL/Complex2.h"

#include "sky_coord.h"
#include "MJD.h"
#include "Types.h"

namespace Pulsar {

  // forward declarations
  class FluxCalibrator;
  class PolnCalibrator;
  class HybridCalibrator;
  class ReferenceCalibrator;
  class Archive;

  //! Pulsar Observation Database
  class Database : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;
    //! Cache the last calibrator?
    static bool cache_last_cal;

    //! Time scale over which calibrator flux and cross-coupling remain stable
    static Option<double> long_time_scale;

    //! Time scale over which differential gain and phase remain stable
    static Option<double> short_time_scale;

    //! The maximum angular separation between calibrator and pulsar
    static Option<double> max_angular_separation;

    //! Pass this to the criterion methods to retrieve any or all matches
    static const Pulsar::Archive* any;

    //! Supported matching policies
    /*! Different level-setting strategies may dictate the policy
      for matching an observation with the right calibrator */
    enum Policy {
      //! Use the nearest calibrator (default)
      NoPolicy,
      //! Use only calibrators recorded before the observation
      CalibratorBefore,
      //! Use only calibrators recorded after the observation
      CalibratorAfter
    };

    //! Null constructor
    Database ();
    
    //! Construct a database from archives in a metafile
    Database (const std::string& path, const std::string& metafile);

    //! Construct a database from archives in a directory
    Database (const std::string& path, const std::vector<std::string>& exts);

    //! Construct a database from a pre-built ascii file
    Database (const std::string& filename);
    
    //! Destructor.
    ~Database ();
    
    //! Construct from the list of filenames
    void construct (const std::vector<std::string>& filenames);

    //! Write a text file representing the database to disk for storage.
    void unload (const std::string& dbase_filename);

    //! Read a text file summary and construct a database
    void load (const std::string& dbase_filename);
    
    //! Add the given Archive to the database
    void add (const Pulsar::Archive* archive);

    //! Return a pointer to a new PolnCalibrator for the given archive
    PolnCalibrator* generatePolnCalibrator (Archive*,
					    const Calibrator::Type* m);
 
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* generateFluxCalibrator (Archive*, bool allow_raw=false);
    
    //! Return a pointer to a new HybridCalibrator
    HybridCalibrator* generateHybridCalibrator (ReferenceCalibrator*,Archive*);

    //! Set the feed transformation to be incorporated into PolnCalibrator
    void set_feed (MEAL::Complex2* xform) { feed = xform; }

    //! Returns the full path to the database summary file
    std::string get_path () const;
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 
    //! Pulsar Database Entry
    class Entry {
      
    public:
      
      // Critical information about the entry
      
      Signal::Source   obsType;      // FluxCal, PolnCal, Pulsar, etc.

      Reference::To<const Calibrator::Type> calType;

      MJD              time;         // Mid time of observation
      sky_coord        position;     // Where the telescope was pointing
      double           bandwidth;    // Bandwidth of observation
      double           frequency;    // Centre frequency of observation
      unsigned         nchan;        // Number of channels across bandwidth
      std::string      instrument;   // name of backend
      std::string      receiver;     // name of receiver
      std::string      filename;     // relative path of file
      
      //! Null constructor
      Entry ();

      //! Construct from an ASCII string
      explicit Entry (const std::string&);

      //! Construct from a Pulsar::Archive
      Entry (const Archive& arch);

      //! Destructor
      ~Entry();
      
      // load from ascii string
      void load (const std::string& str);

      // unload ascii string
      void unload (std::string& str);
      
    protected:
      
      //! Clean slate
      void init ();
      
    };
    
    //! Describes Database matching criteria
    class Criterion  {
      
    public:
      
      static bool match_verbose;

      //! The parameters to match
      Entry entry;

      //! The matching policy
      Policy policy;

      double minutes_apart;
      double deg_apart;
      
      bool check_receiver;
      bool check_instrument;
      bool check_frequency;
      bool check_bandwidth;
      bool check_obs_type;
      bool check_time;
      bool check_coordinates;
      bool check_frequency_array;

      //! Default constructor
      Criterion ();
      
      //! Called when no observation parameters are to be matched
      void no_data ();

      //! Return true if entry matches within the criterion
      bool match (const Entry& entry) const;
      
      //! Return the best of two entries
      Entry best (const Entry& a, const Entry& b) const;
    };
    
    
    //! Get the default matching criterion for all observations
    static Criterion get_default_criterion ();

    //! Set the default matching criterion for all observations
    static void set_default_criterion (const Criterion&);

    //! Returns the best Entry that matches the given Criterion
    Entry best_match (const Criterion&) const;

    //! Fills a vector with Entry instances that match the given Criterion
    void all_matching (const Criterion&, std::vector<Entry>& matches) const;
    
    //! Return the Criterion for the specified Pulsar::Archive
    Criterion criterion (const Pulsar::Archive* arch,
			 Signal::Source obsType) const;
    
    //! Return the Criterion for the specified Pulsar::Archive
    Criterion criterion (const Pulsar::Archive* archive,
			 const Calibrator::Type* calType) const;

    //! Returns the full pathname of the Entry filename
    std::string get_filename (const Entry&) const;
    
    //! Add the given entry to the database
    void add (Pulsar::Database::Entry& entry);

  protected:
    
    std::vector<Entry> entries;   // list of entries in the database
    std::string path;
    Entry lastEntry;
    Reference::To<PolnCalibrator> lastPolnCal;
    
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* rawFluxCalibrator (Archive* a);

    //! If set, this model of the feed is incorporated into all solutions
    Reference::To<MEAL::Complex2> feed;

  };

  bool operator < (const Database::Entry& a, const Database::Entry& b);

  bool operator == (const Database::Entry& a, const Database::Entry& b);

}

#endif
