/***************************************************************************
 *
 *   Copyright (C) 2006 by Albert Teoh
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cpgplot.h>
#include <time.h>
#include <sstream>
#include <map>
#include <unistd.h>

#include "Pulsar/Plotter.h"
#include "Reference.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/StandardSNR.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "strutil.h"
#include "genutil.h"
#include "slalib.h"
#include "getopt.h"

using namespace std;
using namespace Pulsar;

///////////////////////////////////////////////////////////////////
// psrchive_template.C
//
// Written by: Albert Teoh
// 
// This is a template application, initially programmed for plotting
// profiles. However, this can be changed to do anything you like
// as long as you can make the calls to various library routines.
//
// Some annotations are provided to assist you in making these changes
// but these can only help so much. Please let me know if you need
// any assistance with this.
//
// ateoh@swin.edu.au or albert.teoh@csiro.au

//////////////////////////
// Method definitions

// Initialise any parameters or states here
void init();

// Prints usage information
void usage();

// You shouldn't need to change this. This reads all the archive files that you
// provide on the command line.
vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
                 bool breakup_archive);

// Plots the profile on PGPLOT
void plotProfile(const Profile * profile, Pulsar::Plotter plotter);

// Sets the viewport to where the profile should be displayed
// on the PGPLOT window
void goToProfileViewPort();

///////////////////////////
/// Global variables

bool verbose = false;
bool force = false;
int beginFilenamesIndex;

/////////////////////////////
// Constants
const float PLOT_CHAR_HEIGHT = 0.8;

// Approx. number of ticks to be drawn on the profile
const int NUM_VERT_PROFILE_TICKS = 4;


void usage ()
{
  cout << "Usage instructions: " << endl;

  // Add your usage instructions here:
  cout << "This is a template application for developing your own psrchive app" << endl;
}


void init() {
  force = false;
  verbose = false;
}

void cpg_next ()
{
  cpgsch (1);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  //cpgsvp (0.1, 0.9, 0.15, 0.9);
  cpgsvp (0, 1, 0, 1);
  cpgpage ();
}

int main (int argc, char** argv) 
{
  RealTimer clock;
  double elapsed;
  
	// Default plot device
  string plot_device = "?";
  
  char* metafile = NULL;
  
  vector<string> filenames;

  // bools of whether archive should be broken up into its subints
  vector<int> breakup_archives; 
    
  Pulsar::Plotter plotter;


  // Get the command line parameters

	// If you wish to add a new parameter, insert the flag in the string below.
	// If this new flag requires an argument, make sure you put a : next to it.
	const char* args = "g:hv";
	
	const int HELP = 200;
	const int VERBOSE = 201;
	
  static struct option long_options[] = {
    { "help", no_argument, 0, HELP },		
    { "verbose", no_argument, 0, VERBOSE },
    { 0, 0, 0, 0 }
  };
	
	int c = 0;
	
  while (1) {
    int options_index = 0;
    
    c = getopt_long(argc, argv, args,
		    long_options, &options_index);
    
    if (c == -1) 
      break;
    
    switch (c) {
    	case 'g':
      	plot_device = optarg;
      	break;
			
			case 'h':
				usage();
				return 0;
			
			case 'v':
				verbose = true;
				break;
				
			case HELP:
				usage();
				return 0;
			
			case VERBOSE:
				verbose = true;
				break;
				
			default:
				cerr << "psrchive_template: Unrecognized command\n";
				return -1;
		}
	}
		
	// Set this to 3 if you wish to see debugging statements
  Pulsar::Archive::verbose = 0;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);
  
  if (filenames.size() == 0) {          
    cerr << "psrchive_template: please specify filename[s]" << endl;
    return -1;
  }
  
	// Open the plot device
  if (cpgopen(plot_device.c_str()) < 0) {
    cout << "Error: Could not open plot device" << endl;
    return -1;
  }

  clock.start();
  
  cpgask(!force);
    
  // Foreach input archive file
  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {
    
    // Read in the archive files
    vector<Reference::To<Pulsar::Archive> > archives = 
      get_archives(filenames[ifile],breakup_archives[ifile]);

    for( unsigned iarch = 0; iarch < archives.size(); iarch++){
    
      // reset global variables    
      init();
    
      Reference::To<Pulsar::Archive> archive = archives[iarch];
            
      cout << endl << 
        "Working on archive " << archive->get_source() << 
        ": " << archive->get_filename() << 
        endl << endl;

      cpg_next();

      // Don't override my viewport settings!
      plotter.set_svp(false);
      
      // Make a new copy so the original archive isn't changed accidentally
      Reference::To<Pulsar::Archive> copy = archive->clone();
      
      goToProfileViewPort();
      
      // Do the plotting. There are two ways to do this. The simplest and easiest
      // is this (you can scrunch it on any dimension first if you like):
      plotter.singleProfile(copy);
      
      // However, if you want more flexibility, you can call the plotProfile method
      // that is defined below. So for example:
      // plotProfile(copy->get_Profile(0,0,0), plotter);
      // You can alter plotProfile to suit your needs whereas singleProfile will
      // stick to a specific output format on pgplot.
      
			// Writing to disk
			// ---------------
			//
			// To write a new archive file to disk after modifying, use the following:
	  	// 	archive->unload(the_new);
			// Where "the_new" is the new filename for the archive
    }
  }
  catch (Error& error) {
    cerr << "Caught Error on file '" << filenames[ifile] 
     << "': " << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  cpgend();
  
  clock.stop();
  
  elapsed = clock.get_elapsed();

  printf ("\nThis application took %.2lf seconds to compute results.\n", elapsed  );  
  
  return 0;
}

// Reads in the archive files into something psrchive can understand
vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
                 bool breakup_archive) {
  vector<Reference::To<Pulsar::Archive> > archives;
  Reference::To<Pulsar::Archive> arch( Pulsar::Archive::load(filename) );
    
  if( !breakup_archive ){
    archives.push_back( arch );
    return archives;
  }
  
  for( unsigned isub=0; isub<arch->get_nsubint(); isub++){
    vector<unsigned> subints(1,isub);
    archives.push_back( arch->extract(subints) );
    archives.back()->set_filename( arch->get_filename() + 
           " subint " + make_string(isub) );
  }
  
  return archives;
}

// Method to plot a profile which gives some flexibility on labelling the plot
void plotProfile(const Profile * profile, Plotter plotter) {

  cpgsch(0.7);
  cpgslw(1);
  goToProfileViewPort();
  plotter.set_char_height(PLOT_CHAR_HEIGHT);
  plotter.plot (profile);
  double min, max;
  
  min = profile->min();
  max = profile->max();
  
  int div = 2;
  
  double tickSpace = cpgrnd((max-min)/(float)(NUM_VERT_PROFILE_TICKS-1), &div);
    
  cpgbox ("BINTS", 0.0, 0, "BNTSI", tickSpace, 0);
}

// Allows you to determine the location and dimensions of your plot on the pgplot window
void goToProfileViewPort() {
  // Fill most of the window from x1 = 0.1 to x2 = 0.9 and y1 = 0.1 to y2 = 0.9
  // where these values range from 0 to 1
  cpgsvp(0.1, 0.9, 0.1, 0.9);
}
