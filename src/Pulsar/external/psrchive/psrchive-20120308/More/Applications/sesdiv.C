/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveSort.h"

#include "dirutil.h"
#include "strutil.h"
#include "Error.h"
#include "MJD.h"

#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <string.h>

using namespace std;

void usage ()
{
  cout <<
    "sesdiv: divides archives into observing sessions \n"
    "        using output from vap, which is read from stdin \n"
    "e.g. \n"
    "        vap -Rqc \"NAME FREQ MJD\" filename[s] | sesdiv \n"
    "\n"
    "Usage: sesdiv [options] \n"
    "Where the options are as follows \n"
    " -h       this help page \n"
    " -T hours time between sessions \n"
    " -S secs  seconds between sessions \n"
    " -v       verbose output \n"
    " -V       very verbose output \n"
       << endl;
}

int main (int argc, char** argv) 
{
  float between_sessions = 10.0;
  bool include_minutes = false;

  bool verbose = false;
  bool vverbose = false;

  const char* args = "hS:T:vV";
  int c = 0;
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'h':
      usage ();
      return 0;

    case 'V':
      vverbose = true;
    case 'v':
      verbose = true;
      break;

    case 'S':
      between_sessions = atof(optarg)/3600.0;
      include_minutes = true;
      break;

    case 'T':
      between_sessions = atof(optarg);
      break;

    default:
      cerr << "invalid param '" << char(c) << "'" << endl;
    }


  list<Pulsar::ArchiveSort> entries;

  if (optind < argc) {
    ifstream input (argv[optind]);
    if (!input) {
      cerr << "sesdiv: could not open " << argv[optind] << endl;
      return -1;
    } 
    Pulsar::ArchiveSort::load (input, entries);
  }
  else
    Pulsar::ArchiveSort::load (cin, entries);

  cerr << "sesdiv: " << entries.size() << " entries loaded" << endl;
  entries.sort();

  FILE*    session = 0;
  unsigned session_count = 1;
  MJD      session_mjd;
  double   session_cfreq = 0;

  char buffer[256];

  // Pulsar::ArchiveSort::verbose = true;

  list<Pulsar::ArchiveSort>::iterator entry;
  list<Pulsar::ArchiveSort>::iterator last = entries.end();

  for (entry = entries.begin(); entry != entries.end(); entry++) try {

    if (last != entries.end() && ! ( (*last) < (*entry) ))
      cerr << "Entries not sorted!" << endl;

    string filename = entry->filename;

    MJD mjd = entry->epoch;

    if (!session) {

      // returns a string formatted as in 'strftime()'
      if (include_minutes)
        mjd.datestr (buffer, 256, "%Y-%m-%d-%H%M");
      else
        mjd.datestr (buffer, 256, "%Y-%m-%d-%H00");

      unsigned length = strlen(buffer);

      sprintf (buffer + length, "_%.0f.session", entry->centre_frequency);

      cerr << "Starting session " << session_count 
	   << " in " << buffer << endl;

      session = fopen (buffer, "w");
      if (!session) {
        perror ("sesdiv: Could not open file");
        return -1;
      }

      session_mjd = mjd;
      session_cfreq = entry->centre_frequency;

    }

    bool close_session = false;

    if ( (mjd - session_mjd).in_days()*24.0 > between_sessions ) {
      cerr << "Gap in time > " << between_sessions << " hours" << endl;
      close_session = true;
    }

    if ( entry->centre_frequency != session_cfreq ) {
      cerr << "Different centre frequency" << endl;
      close_session = true;
    }

    if (close_session) {
      cerr << "Closing session " << session_count << endl;
      fclose (session);
      entry --;
      session_count ++;
      session = 0;
    }
    else {
      if (verbose)
	cerr << "Adding " << entry->filename
	     << " to session " << session_count << endl;

      fprintf (session, "%s\n", entry->filename.c_str());
    }

    last = entry;

  }
  catch (Error& error) {
    cerr << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  if ( session ) {
    cerr << "Closing session " << session_count << endl;
    fclose (session);
  }
 
  return 0;
}


