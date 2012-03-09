//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Database.h,v $
   $Revision: 1.24 $
   $Date: 2010/09/23 05:40:46 $
   $Author: straten $ */

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

    //! Cache the last calibrator?
    static bool cache_last_cal;

    //! Print verbose matching information
    static bool match_verbose;

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
    class Criterion : public Reference::Able {
      
    public:
      
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
      
      /** @name match results
       *  
       * These attributes are set by the match method
       */
      //@{

      //! log of the attributes compared and the result of the comparison
      mutable std::string match_report;

      //! count of the number of match conditions that tested positive
      mutable unsigned match_count;

      //! time between calibrator and requested epoch (in minutes)
      mutable double diff_minutes;

      //! distance between calibrator and requested position (in degrees)
      mutable double diff_degrees;

      //@}

      //! Return the best of two entries
      Entry best (const Entry& a, const Entry& b) const;

      //! Return the criterion that came closest to matching
      static Criterion closest (const Criterion& a, const Criterion& b);

    protected:

      template<typename T, typename Predicate>
      void compare (const std::string& name,
		    const T& want, const T& have,
		    Predicate equal ) const
      {
	match_report += "\t" + name
	  + " want=" + tostring(want) + " have=" + tostring(have) + " ... ";
	
	if ( equal (want, have) )
	{
	  match_report += "match \n";
	  match_count ++;
	}
	else
	{
	  match_report += "no match \n";
	  throw false;
	}
      }

      template<typename T>
      void compare (const std::string& name,
		    const T& want, const T& have) const
      {
	compare (name, want, have, std::equal_to<T>());
      }

      bool compare_times (const MJD& want, 
			  const MJD& have) const;

      bool compare_coordinates (const sky_coord& want,
				const sky_coord& have) const;
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

    //! Get the closest match report
    std::string get_closest_match_report () const;

  protected:
    
    std::vector<Entry> entries;   // list of entries in the database
    std::string path;
    Entry lastEntry;
    Reference::To<PolnCalibrator> lastPolnCal;
    Reference::To<HybridCalibrator> lastHybridCal;
    
    //! Return a pointer to a new FluxCalibrator for the given archive
    FluxCalibrator* rawFluxCalibrator (Archive* a);

    //! If set, this model of the feed is incorporated into all solutions
    Reference::To<MEAL::Complex2> feed;

    //! The criterion that last came closest to matching
    mutable Criterion closest_match;
  };

  bool operator < (const Database::Entry& a, const Database::Entry& b);

  bool operator == (const Database::Entry& a, const Database::Entry& b);

}

#endif
