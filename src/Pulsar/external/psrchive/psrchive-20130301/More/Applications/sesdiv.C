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
    " -h                     this help page \n"
    "\n"
    " -b name:freq1-freq2    add a band designation \n"
    "                        sessions for a given frequency range will \n"
    "                        be placed in name/*.session \n"
    "\n"
    " -T hours               time between sessions \n"
    " -S secs                seconds between sessions \n"
    " -v                     verbose output \n"
    " -V                     very verbose output \n"
       << endl;
}

class band
{
public:
        band (const char* txt);
	string name;
	double min_frequency;
	double max_frequency;
};

ostream& operator<< (ostream& os, const band& b)
{
  return os << b.name << ":" << b.min_frequency << "-" << b.max_frequency;
}

band::band (const char* txt)
{
  string text = txt;
  size_t index = text.find(':');
  if (index == string::npos)
    throw ios_base::failure ("malformed band designation"
                             " - missing colon after name in '" + text + "'");

  name = text.substr (0, index);

  text = text.substr (index+1);

  char separator;
  sscanf (text.c_str(), "%lf%c%lf", &min_frequency, &separator, &max_frequency);

  if (index == string::npos)
    throw ios_base::failure ("malformed band designation"
                             " - missing hyphen between frequencies");

  if (max_frequency < min_frequency)
    throw ios_base::failure ("malformed band designation"
                             " - max less than min frequency");
}

int main (int argc, char** argv) 
{
  vector<band> bands;

  float between_sessions = 10.0;
  bool include_minutes = false;

  bool verbose = false;
  bool vverbose = false;

  const char* args = "hb:S:T:vV";
  int c = 0;
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'b':
    {
      band b (optarg);
      cerr << "sesdiv: adding band designation " << b << endl;
      bands.push_back(b);
      break;
    }

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

  if (bands.size())
  {
    for (unsigned iband=0; iband < bands.size(); iband++)
      if (makedir (bands[iband].name.c_str()) < 0)
      {
        cerr << "Could not create " << bands[iband].name << " sub-directory"
             << endl;
        return -1;
      }
  }

  for (entry = entries.begin(); entry != entries.end(); entry++) try
  {
    if (last != entries.end() && ! ( (*last) < (*entry) ))
      cerr << "Entries not sorted!" << endl;

    MJD mjd = entry->epoch;

    if (!session)
    {
      // returns a string formatted as in 'strftime()'
      if (include_minutes)
        mjd.datestr (buffer, 256, "%Y-%m-%d-%H%M");
      else
        mjd.datestr (buffer, 256, "%Y-%m-%d-%H00");

      unsigned length = strlen(buffer);

      sprintf (buffer + length, "_%.0f.session", entry->centre_frequency);

      string filename = buffer;

      if (bands.size())
        for (unsigned iband=0; iband < bands.size(); iband++)
          if (bands[iband].min_frequency < entry->centre_frequency &&
              bands[iband].max_frequency > entry->centre_frequency)
          {
            filename = bands[iband].name + "/" + filename;
            break;
          }

      cerr << "Starting session " << session_count 
	   << " in " << filename << endl;

      session = fopen (filename.c_str(), "w");
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


