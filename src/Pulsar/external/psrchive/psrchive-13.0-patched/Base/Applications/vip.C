/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Pointing.h"

#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>
#include <string.h>

using namespace std;

void usage ()
{
  cout <<
    "A program for displaying Pulsar::Integration Extension info\n"
    "Options:\n"
    "\n"
    " -a       Display all available parameters\n"
    " -c PARMS Specify a list of parameters\n"
    "          e.g. vip -c \"LST PARA\" f9509*ar\n"
    "\n"
    "-------------------------------------------------\n"
    "                    PARAMETERS                   \n"
    "-------------------------------------------------\n"
    "\n"
    "Pointing Extension attributes:\n"
    "LST     - Local sidereal time\n"
    "RA      - Right ascension\n"
    "DEC     - Declination\n"
    "GLAT    - Galactic latitude\n"
    "GLONG   - Galactic longitude\n"
    "FDA     - Feed angle\n"
    "POSA    - Position angle\n"
    "PARA    - Parallactic angle\n"
    "AZ      - Telescope azimuth\n"
    "ZEN     - Telescope zenith\n"
       << endl;
}


int main (int argc, char *argv[]) try {  
  
  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);

  if (argc < 2) {
    printf("Use -h to get usage information\n");
    return -1;
  }
  
  bool verbose = false;
  bool all_parameters = false;

  vector <string> keys;

  int gotc;
  while ((gotc = getopt (argc, argv, "ac:hvV")) != -1)
    switch (gotc) {

    case 'a':
      all_parameters = true;
      break;

    case 'c': {
      char whitespace[5] = " \n\t";
      char* key = strtok (optarg, whitespace);
      while (key) {
        keys.push_back(key);
        key = strtok (NULL, whitespace);
      }
      break;
    }

    case 'h':
      usage ();
      return -1;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    default:
      cerr << "Unknown command line option" << endl;
    }
  

  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  ostream* ost = &cout;

  if (all_parameters) {
    keys.push_back("LST");
    keys.push_back("RA");
    keys.push_back("DEC");
    keys.push_back("GLAT");
    keys.push_back("GLONG");
    keys.push_back("FDA");
    keys.push_back("POSA");
    keys.push_back("PARA");
    keys.push_back("AZ");
    keys.push_back("ZEN");
  }

  string heading = "FILE\t\t\tSUBINT\t";

  for (unsigned ikey = 0; ikey < keys.size(); ikey++)
    heading += keys[ikey] + "\t";

  heading += '\n';

  *ost << heading;

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load(filenames[ifile]);
      
    for (unsigned isub=0; isub<archive->get_nsubint(); isub++)  {

      Reference::To<Pulsar::Integration> subint;
      subint = archive->get_Integration (isub);

      string s = archive->get_filename();

      if (s.size() < 8)
	s += "\t\t\t";
      else if (s.size() < 16)
	s += "\t\t";
      else
	s += "\t";

      *ost << s << isub << "\t";

      s = "";	

      for (unsigned j = 0; j < keys.size(); j++) {
	
	const Pulsar::Pointing* pointing = subint->get<Pulsar::Pointing>();

        if (verbose && !pointing)
          cerr << "vip: Integration has no Pointing Extension" << endl;

	if (!strcasecmp (keys[j].c_str(), "lst")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f hours\t",
			      pointing->get_local_sidereal_time()/3600);
	}
	
	else if (!strcasecmp (keys[j].c_str(), "ra")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_right_ascension().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "dec")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_declination().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "glat")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_galactic_latitude().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "glong")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_galactic_longitude().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "fda")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_feed_angle().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "posa")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_position_angle().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "para")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_parallactic_angle().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "az")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_telescope_azimuth().getDegrees());
	}
	
	else if (!strcasecmp (keys[j].c_str(), "zen")) {
	  if (!pointing)
	    s += "N/A\t\t";
	  else
	    s += stringprintf("%.4f deg\t",
			      pointing->get_telescope_zenith().getDegrees());
	}
	
	else
	    s += "N/A\t\t";

      } // for each key

      *ost << s << endl;

    } // for each sub-integration

  } // for each archive

  catch (Error& error) {
    if (verbose)
      cerr << error << endl;
    *ost << "INVALID_ARCHIVE" << endl;
  }

  return 0;

}
catch (Error& error) {
  cerr << "vip: " << error << endl;
  return -1;
}

