/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo & Albert Teoh
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

//
// Searches over trial DM and Period ranges and obtains the optimal
// DM and Period giving the highest S/N, plot SNR vs Period vs DM,
// Phase vs Time, Phase vs Frequency
//

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cpgplot.h>
#include <time.h>
#include <sstream>
#include <map>
#include <algorithm>
#include <float.h>

#include "ColourMap.h"
#include "dirutil.h"
#include "Error.h"
#include "find_spike_edges.h"
#include "RealTimer.h"
#include "Reference.h"
#include "slalib.h"
#include "TextInterface.h"

#include "Pulsar/Archive.h"
#include "Pulsar/RemoveBaseline.h"
#include "Pulsar/TextParameters.h"

#include "Pulsar/counter_drift.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/StandardSNR.h"
#include "Pulsar/TimeSortedOrder.h"
#include "Pulsar/FortranSNR.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/Site.h"

#include <sys/stat.h>
#include <assert.h>

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#ifdef HAVE_PSRXML
#include <phcx.h>
#endif

#define RATIO 10.0

using namespace std;
using namespace Pulsar;

static PlotFactory factory;

//////////////////////////
// Method definitions

void reorder(Reference::To<Archive> arch);

void setCentreMjd(const Archive* archive);

void draw_colour_map (float *plotarray, int rows, int cols, double minx,
		double maxx, const string& xlabel, double miny, double maxy, const string& ylabel,
		const string& title, float * trf, int numVertTicks);

void draw_colour_map_only(const int rows, const int columns, const double minx,
    const double maxx, const double miny, const double maxy, const float* trf);

void minmaxval (int n, const float *thearray, float *min, float *max);

// Initialises all the global variables to default values.
void init();

vector<Reference::To<Archive> > get_archives(string filename);

// Finds the optimal DM and Period
// If any of the offset, half-range and step values are < 0,
// a natural default value will be computed and used.
// If these values are 0, then it indicates that there is only one
// bin for that respective value. ie. if periodStep_us == 0, then it
// means that there is only one subint bin.
//
//
// PRECONDITIONS:
//   * refP is Topocentric
//   * refP, periodOffset_us, periodStep_us and periodHalfRange_us are all in microseconds

void plotSNRdm (string & filename, double bestDM);

void solve_and_plot (Archive* archive,
		     double dmOffset, double dmStep, double dmHalfRange,
		     double periodOffset_us, double periodStep_us, double periodHalfRange_us,
		     ProfilePlot* total_plot, TextInterface::Parser* flui, double minwidthsecs);

// Default call to solve_and_plot above
// archive: The archive data
// refDM: The reference DM
// refP: The reference Period in microseconds

void solve_and_plot (Archive* archive,
		     ProfilePlot* total_plot, TextInterface::Parser* flui, double minwidthsecs);

// Gets the natural DM step if none was provided
// Precondition: nchan > 1
// Returns: Natural DM step
double getNaturalDMStep(const Archive * archive, double dmHalfRange);

// Gets the natural DM half range if none was provided
// Precondition: nchan > 1
// Returns: Natural DM half range
double getNaturalDMHalfRange(const Archive * archive, double dmStep);

// Gets the natural Period step in microseconds if none was provided
// Precondition: nsub > 1
// Returns: Natural period step in microseconds
double getNaturalPeriodStep(const Archive * archive, double periodHalfRange_us);

// Gets the natural Period half range in microseconds if none was provided
// Precondition: nsub > 1
// Returns: Natural period half range in microseconds
double getNaturalperiodHalfRange(const Archive * archive, double periodStep_us);

// Places focus on the header section of the window for writing pgplot text
void goToHeaderViewPort();

// Sets the viewport to where the phase vs. frequency plot
// should be displayed on the PGPLOT window
void goToPhaseFreqViewPort();

// Sets the viewport to where best values
// should be displayed on the PGPLOT window
void goToBestValuesViewPort();

// Sets the viewport to where the dDM vs dP vs SNR plot
// should be displayed on the PGPLOT window
void goToDMPViewPort();

// Sets the viewport to where the phase vs. time plot
// should be displayed on the PGPLOT window
void goToPhaseTimeViewPort();

// Sets the viewport to where the profile should be displayed
// on the PGPLOT window
void goToProfileViewPort();

// Viewport for the phase vs frequency/time plot titles
void goToPhaseFreqTitleViewPort();
void goToPhaseTimeTitleViewPort();

// Partially scrunches the archive based on the maxChannels and maxSubints
// global variables. If the number of subints and channels are larger than
// the user's maximum subints and channels, pdmp will scrunch it to that level
void scrunchPartially(Archive * scrunchedCopy);

// Get the SNR for the archive.
float getSNR(const Profile * p);  // For use with standard profiles!
float getSNR(const Profile * p, float rms, int minwidthbins);
float getSNR(const Profile * p, float rms, int minwidthbins, int maxwidthbins);

// Get the RMS of the archive
float getRMS (const Archive * archive);

// Get the coordinates from the archive
bool use_ephemeris_coordinates = false;
sky_coord getCoord (const Archive*);

void setSensibleStepSizes(const Archive * archive);

// Parse the command line parameters and set the values passed through as arguments
void parseParameters(int argc, char **argv, double &periodOffset_us,
    double &periodStep_us, double &periodHalfRange_us, double &dmOffset,
    double &dmStep, double &dmHalfRange, pgplot::ColourMap::Name &colour_map,
    double &minwidth, string &bestfilename);

// Parses a string to a number and checks if that value is a positive value
//
// Given:
//     optionName   The name of the option used over the command line. e.g. -ps
//     numberString The input string accompanied with the option. e.g. -pr 20
//                  where 20 is the numberString
//     value        The output value once the string has been parsed.
void parseAndValidatePositiveDouble(string optionName, char * numberString, double &value);
void parseAndValidatePositiveInt(string optionName, char * numberString, int &value);

// Parses a string to a number
// Returns the numerical value in "value"
void parseAndValidateDouble(string optionName, char * numberString, double &value);
void parseAndValidateInt(string optionName, char * numberString, int &value);

// Checks if the string is a valid number (any of float, double, int, etc...)
bool isNumber(char * str);

// Returns the reference period of the central subint in seconds.
double getPeriod(const Archive * archive);

// Returns the reference DM
double getDM(const Archive * archive);

// Gets the period error (milliseconds) based on the best pulse width
double computePeriodError(const Archive * archive);

// Gets the dm error based on the best pulse width
double computeDMError(const Archive * archive);

#ifdef HAVE_PSRXML
// some methods for dealing with xml candidates

// 'global' xml candidate file
phcx* xml_candidate=NULL;
bool normaliseSubints=true;
// Adds a 'section' to a xml candidate file from an archive.
void setInitialXmlCandiateSection(const Archive * archive,float minwidthsecs);
void addOptimisedXmlCandidateSection(const Archive * archive,double centrePeriod,double centreDm,double centreAccn, double centreJerk,float minwidthsecs);

#endif

// slaPvobs written by P.T.Wallace
//  Position and velocity of an observing station.
//
//  (double precision)
//
//  Given:
//     p     double     latitude (geodetic, radians)
//     h     double     height above reference spheroid (geodetic, metres)
//     stl   double     local apparent sidereal time (radians)
//
//  Returned:
//     pv    double[6]  position/velocity 6-vector (au, au/s, true
//                                         equator and equinox of date)
void slaPvobs(double p, double h, double stl, double pv[6] );

// slaGeoc written by P.T.Wallace
//  Convert geodetic position to geocentric.
//
//  (double precision)
//
//  Given:
//     p     double     latitude (geodetic, radians)
//     h     double     height above reference spheroid (geodetic, metres)
//
//  Returned:
//     *r    double     distance from Earth axis (AU)
//     *z    double     distance from plane of Earth equator (AU)
//
//  Notes:
//
//     1)  Geocentric latitude can be obtained by evaluating atan2(z,r).
//
//     2)  IAU 1976 constants are used.
void slaGeoc ( double p, double h, double *r, double *z );

// Gets the doppler factor, used by multiplying with Topocentric
// period to obtain the Barycentric period
double getDopplerFactor(const Archive * archive);

// Calculates the sidereal time
double lmst2(double mjd,double olong,double *tsid,double *tsid_der);

// Emulates the fortran mod function. % is restricted to integer arguments
double fortran_mod2(double a,double p);

//////////////////////////////////
///
/// Drawing and printing methods
///
//////////////////////////////////

// Print the header section on PGPLOT
void printHeader(const Archive * archive,
                 double periodOffset_us,       // period offset in microseconds
								 double periodStep_us,         // period step in microseconds
								 double periodHalfRange_us,    // p half range in microseconds
								 double dmOffset,           // dm offset
								 double dmStep,             // dm step
								 double dmHalfRange);       // dm half range

// Plots the profile of the first subint, channel and polarisation.
void plotProfile(const Profile * profile, ProfilePlot* plot, TextInterface::Parser* flui);

// Plots the original and corrected phase vs. time
void plotPhaseTime(const Archive * archive, Plot* plot, TextInterface::Parser* tui);

// Plots the original and corrected phase vs. freq
void plotPhaseFreq(const Archive * archive, Plot* phase_plot, TextInterface::Parser* fui);

// Finds the best gradient that fits the corrected set of
// profiles for each frequency channel
void drawBestFitPhaseFreq(const Archive * archive);

// Finds the best gradient that fits the correct set of
// profiles for each subintegration
void drawBestFitPhaseTime(const Archive * archive);

// These methods help to get the number after the decimal point
// Obtained from http://www.merrymeet.com/minow/sunclock/Astro.java
// Written by Martin Minow

// Returns the integer part of value
double TRUNC(double value);

// Returns the fractional part of value
double FRAC(double value);

void drawBestValuesCrossHair( const Archive * archive,
     double periodOffset_us,    // period offset in microseconds
		 double periodStep_us,      // period step in microseconds
		 double periodHalfRange_us, // period half range in microseconds
     double dmOffset,           // dm offset
		 double dmStep,             // dm step
		 double dmHalfRange);       // dm half range

// Prints the results onto the console and PGPLOT
void printResults(const Archive * archive);

// Writes important information into output files
void writeResultFiles(Archive * archive, int tScint, int fScint, int tfScint);

string get_scale(const Archive * archive);

//////////////////////////////////
///
/// Constant definitions
///
//////////////////////////////////

// This DM constant is in milliseconds.
const double DMCONST = 4.15 * pow((double)10, (double)6);

const double MILLISEC = 1000;

const double MICROSEC = 1000000;

const unsigned FIRST_SUBINT = 0;

const unsigned FIRST_CHAN = 0;

const unsigned FIRST_POL = 0;

const float DEFAULT_BASELINE_WIDTH = 0.4;

const int NUMERIC_CHARS_LEN = 13;

const char NUMERIC_CHARS [NUMERIC_CHARS_LEN] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'e', '.'};

const float PLOT_CHAR_HEIGHT = 0.8;

const float HEADER_X = 0.01; // x ordinate of the header.

const float HEADER_Y = 0.65; // y ordinate of the header.

// Approx. number of ticks to be drawn on the profile
const int NUM_VERT_PROFILE_TICKS = 4;

const unsigned DEFAULT_BEST_FIT_LINE_COLOUR = 2;

const unsigned BEST_FIT_LINE_WIDTH = 1;

// Parkes. Note: this is an array index = 7-1
const unsigned DEFAULT_SITE_CODE_INDEX = 6;

// Display every this many percent on the progress
const unsigned SHOW_EVERY_PERCENT_COMPLETE = 5;

//////////////////////////////////
///
/// Global variables
///
//////////////////////////////////

// Determine functionality when pdmp is run with more than 1 file
bool join = false;

// The Barycentric period in seconds
double dopplerFactor;

double tmjdctr;

// Best values
double bestSNR;

// best period value in microseconds
double bestPeriod_bc_us;
double bestDM;
double bestFreq;
unsigned bestPulseWidth;
// the 'array' of signal-to-noise with varying p/dm trial
vector<float> SNRs;

vector<float> bestSNRfreq;
vector<float> bestSNRtime;
vector<float> bestSNRtimefreq;

Reference::To<Profile> bestProfile;

// Errors
double periodError_ms;
double dmError;
double freqError;

// Coords of header text
float linex = HEADER_X;
float liney = HEADER_Y;

bool verbose = false;
bool force = false;
bool silent = false;
bool debug = false;

bool output_phcx=false;
char output_phcx_file[128];
bool input_phcx=false;
char input_phcx_file[128];

float coarseness = 1;
float range_mult = 1;

// If archive contains more than maxChannels and maxSubints, pdmp
// will partially scrunch it to improve performance
// These values are set by the -ms and -mc flags on command line
int maxChannels = -1;
int maxSubints = -1;
int maxBins = -1;

float user_rms=-1.0;
bool have_user_rms_file=false;
char user_rms_file[256];

// The standard profile filename to compare against to compute SNR
string standardProfileFilename = "";
bool useStandardProfile = false;
bool useOwnStandardProfile = false;
bool thumbnail = false;

// Set to true when the -k option is specified.
// When onlyDisplayDmPlot is true, only the DM vs period plot will be
// displayed on the pgplot device specified by plot_device (-g option)
bool onlyDisplayDmPlot = false;

// Minimal greyscale level for DM vs period plot
float onlyDisplayDmPlotMin = 0.0;

// Minimal greyscale level for DM vs period plot
float onlyDisplayDmPlotMax = 0.0;

// Number of contour levels when the contour map is drawn
unsigned numContourLevels = 0;

// Value of the first contour line
float contourFirstLevel = 0.0;

// Value of the step between each line
float contourLevelStep = 0.0;

// Set to true when the -con option is specified.
// When drawContourMap is true, a contour map will be drawn over the DM vs period
// plot
bool drawContourMap = false;

string plot_device = "/xs";

Reference::To<Profile> std_prof;
Reference::To<StandardSNR> stdSNR;

int beginFilenamesIndex = -1;

unsigned bestFitLineColour = DEFAULT_BEST_FIT_LINE_COLOUR;

vector<string> filenames;

// The mappings between the site code and the array index used to get
// the site's x,y,z coordinates and name

map<char, int> siteCode2Index;

// A convenience type for the mapping
typedef pair<char, int> entry;

void usage (bool verbose_usage)
{
  cout <<
    "                                                                               \n"
    "pdmp searches a specified range of barycentric period and DM                   \n"
    "     for those values giving the highest S/N ratio.                            \n"
    "                                                                               \n"
    "Usage: pdmp [options] file1 [file2 ...]                                        \n"
    "                                                                               \n"
    "options:                                                                       \n"
    " -do  <dm offset>               DM offset in pc/cm^3      (default=0)          \n"
    " -dr  <dm half-range>           DM half-range in pc/cm^3  (default=natural)    \n"
    " -ds  <dm step>                 DM step in pc/cm^3        (default=natural)    \n"
    " -eph-coord                     Use tempo parameter (not header) coordinates   \n"
    " -f , --force                   Force the program to compute without prompting \n"
    " -j                             Combine multiple archives                      \n"
    " -mc, --maxchannels <max chan>  Archive frequency channels will be             \n"
    "                                partially scrunched to <= this maximum         \n"
    "                                before computing                               \n"
    " -ms, --maxsubints <max subint> Archive subints will be                        \n"
    "                                partially scrunched to <= this maximum         \n"
    "                                before computing                               \n"
    " -mb, --maxbins <max bin>       Archive bins will be                           \n"
    "                                partially scrunched to <= this maximum         \n"
    " -po  <period offset>           Period offset in us       (default=0)          \n"
    " -pr  <period half-range>       Period half-range in us   (default=natural)    \n"
    " -ps  <period step>             Period step in us         (default=natural)    \n"
    " -s   <profile file>            Use <profile file> as a standard profile to    \n"
		"                                compare with                                   \n"
    " -b   <bestfilename>            file output from best of SNR vs DM             \n"
    "                                                                               \n"
    "Selection & configuration options:                                             \n"
    " -g <dev>   Manually specify a plot device                                     \n"
    "                                                                               \n"
    "Plotting options:                                                              \n"
    " -k <min level> <max level>     Only show the DM vs period plot                \n"
    "                                If <min level> and <max level> = 0,            \n"
    "                                pdmp will automatically determine these values \n"
    " -con <clevel1> <cstep> <nc>    Draw a contour map which overlays the DM vs    \n"
    "                                period plot (-k ...)                           \n"
    "       clevel1 = value of the first contour line                               \n"
    "       cstep   = step between each contour line                                \n"
    "       nc      = number of contour lines                                       \n"
    "                                                                               \n"
    "                                                                               \n"
    "                                                                               \n"
    "Other plotting options:                                                        \n"
    " -c <index>  Select a colour map for PGIMAG style plots                        \n"
    "             The available indices are:                                        \n"
    "               0 -> Greyscale                                                  \n"
    "               1 -> Inverse Greyscale                                          \n"
    "               2 -> Heat (default)                                             \n"
    "               3 -> Cold                                                       \n"
    "               4 -> Plasma                                                     \n"
    "               5 -> Forest                                                     \n"
    "               6 -> Alien Glow                                                 \n"
    "                                                                               \n"
    "Utility options:                                                               \n"
    " -h         Display this useful help page (most useful options)                \n"
		" --help     Display a complete list of help options                            \n"
		" -v         Verbose output                                                     \n"
		" -S         Silent mode. Reduce text written to standard output.               \n"
		" -G         Debug mode. Tell all SCINT SNRs.                                   \n"
    " -o         Use own (self-generated) standard profile for Scint analysis       \n"
    << endl;

  if (verbose_usage) {
    cout <<
      " -lc <index> Select the colour index for the line of best fit    \n"
      "             The available indices are:                          \n"
      "               0 -> Black                                        \n"
      "               1 -> White                                        \n"
      "               2 -> Red (default)                                \n"
      "               3 -> Lime green                                   \n"
      "               4 -> Navy blue                                    \n"
      "               5 -> Cyan                                         \n"
      "               6 -> Pink                                         \n"
      "               7 -> Yellow                                       \n"
      "               8 -> Orange                                       \n"
      "               9 -> Green                                        \n"
      "               10 -> Aqua green                                  \n"
      "               11 -> Blue                                        \n"
      "               12 -> Purple                                      \n"
      "               13 -> Pinkish red                                 \n"
      "               14 -> Dark grey                                   \n"
      "               15 -> Light grey                                  \n"
      "                                                                 \n"
      << endl;
  }
}

void init() {

	// The Barycentric period in seconds
	dopplerFactor = -1;

  tmjdctr = 0.0;

	// Best values
	bestSNR = -1;

	// best period value in microseconds
	bestPeriod_bc_us = -1;
	bestDM = -1;
	bestFreq = -1;
	bestPulseWidth = 0;

	// Errors
	periodError_ms = -1;
	dmError = -1;
	freqError = -1;

	// Coords of header text
	linex = HEADER_X;
	liney = HEADER_Y;

  // Computed SNRs
  SNRs.clear();
}


void parseParameters(int argc, char **argv, double &periodOffset_us,
    double &periodStep_us, double &periodHalfRange_us, double &dmOffset,
    double &dmStep, double &dmHalfRange, pgplot::ColourMap::Name &colour_map,
    double &minwidth, string &bestfilename)
{
	string optionArg;

	// If no command line argument was provided, then print out usage
	if (argc <= 1) {
		usage(false);
		exit(0);
	}

	// Get command line options and arguments as necessary
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-j") == 0) {
			i++;
			join = true;
		}

		// period step
		if (strcmp(argv[i], "-ps") == 0) {
			i++;
			parseAndValidatePositiveDouble("-ps", argv[i], periodStep_us);
		}

		// period range
		else if (strcmp(argv[i], "-pr") == 0) {
			i++;
			parseAndValidatePositiveDouble("-pr", argv[i], periodHalfRange_us);
		}

		// period offset
		else if (strcmp(argv[i], "-po") == 0) {
			i++;
			parseAndValidateDouble("-po", argv[i], periodOffset_us);
		}

		// dm step
		else if (strcmp(argv[i], "-ds") == 0) {
			i++;
			parseAndValidatePositiveDouble("-ds", argv[i], dmStep);
		}

		// dm range
		else if (strcmp(argv[i], "-dr") == 0) {
			i++;
			parseAndValidatePositiveDouble("-dr", argv[i], dmHalfRange);
		}

		// dm offset
		else if (strcmp(argv[i], "-do") == 0) {
			i++;
			parseAndValidateDouble("-do", argv[i], dmOffset);
		}

		// min width in secs 
		else if (strcmp(argv[i], "-mw") == 0) { 
			i++;
			parseAndValidateDouble("-mw", argv[i], minwidth);			
		}

		// graph device
		else if (strcmp(argv[i], "-g") == 0) {
			i++;
			plot_device = argv[i];

			if (verbose)
				cout << "Using plot device " << plot_device << endl;
		}

		else if (strcmp(argv[i], "-b") == 0) {
			i++;
			bestfilename = argv[i];

			if (verbose)
				cout << "Using bestfilename " << bestfilename << endl;
		}

		// colour map
		else if (strcmp(argv[i], "-c") == 0) {
			i++;
			optionArg = argv[i];
			colour_map = (pgplot::ColourMap::Name) atoi(argv[i]);

			if (verbose)
				cout << "Setting colour map to " << colour_map << endl;
		}

		// Best fit line colour
		else if  (strcmp(argv[i], "-lc") == 0) {
			i++;
			optionArg = argv[i];
			bestFitLineColour = atoi(argv[i]);

			if (verbose)
				cout << "Setting best fit line colour index to " << bestFitLineColour << endl;
		}

		// maximum channel limit
		else if (strcmp(argv[i], "-mc") == 0 || strcasecmp(argv[i], "--maxchannels") == 0) {
			i++;
			parseAndValidatePositiveInt("-mc", argv[i], maxChannels);
		}
		// maximum subint limit
		else if (strcmp(argv[i], "-ms") == 0 || strcasecmp(argv[i], "--maxsubints") == 0) {
			i++;
			parseAndValidatePositiveInt("-ms", argv[i], maxSubints);
		}
		// maximum bin limit
		else if (strcmp(argv[i], "-mb") == 0 || strcasecmp(argv[i], "--maxbins") == 0) {
			i++;
			parseAndValidatePositiveInt("-mb", argv[i], maxBins);
		}

		// use own standard for Scint work
		else if (strcmp(argv[i], "-o") == 0) {
		  useOwnStandardProfile = true;
		  cout << "Using self-generated Standard for SNR calculations "<< endl;
		}

		else if (strcmp(argv[i], "-t") == 0) {
		  thumbnail = true;
		  cout << "thumbnail on"<< endl;
		}

		// use the standard profile
		else if (strcmp(argv[i], "-s") == 0) {
			i++;

			// TODO: Check validity of file
			standardProfileFilename = argv[i];

			ifstream inFile;
			inFile.open(standardProfileFilename.c_str());
			if (inFile.fail()) {
				cout << "pdmp error: option -s: File " << standardProfileFilename << " doesn't exist." << endl;
				exit(1);
			}

			if (verbose)
				cout << "Using the standard profile in " << standardProfileFilename << endl;

			useStandardProfile = true;
		}

		// force - Don't prompt the user about anything
		else if (strcmp(argv[i], "-f") == 0 || strcasecmp(argv[i], "--force") == 0) {
			if (verbose)
				cout << "Setting force on" << endl;

			force = true;
		}

		// help!
		else if (strcmp(argv[i], "-h") == 0) {
			usage(false);
			exit(0);
	 	}
		else if (strcasecmp(argv[i], "--help") == 0) {
			usage(true);
			exit(0);
	 	}

		else if (strcasecmp(argv[i], "-G") == 0) {
		  debug = true;
	 	}

		// verbose
		else if (strcmp(argv[i], "-v") == 0) {
			if (silent) {
				cerr << "Sorry this program  verbosely silent. Please choose either verbose or silent" << endl;
				exit(1);
			}
			silent = false;
			verbose = true;
		}

		// silent
		else if (strcmp(argv[i], "-S") == 0) {
			if (verbose) {
				cerr << "Sorry, it is a paradox to be verbosely silent. Please choose either verbose or silent" << endl;
				exit(1);
			}
			verbose = false;
			silent = true;
		}
		// make a phcx file
                else if (strcmp(argv[i], "-output-phcx") == 0) {
#ifdef HAVE_PSRXML
                        output_phcx = true;
			i++;
			strcpy(output_phcx_file,argv[i]);
#else
			printf("Sorry, can't output phcx file as this pdmp was not compiled against the psrxml library.\n");
			exit(1);
#endif
                }
		// read a phcx file
		else if (strcmp(argv[i], "-input-phcx") == 0) {
#ifdef HAVE_PSRXML
			input_phcx = true;
			i++;
			strcpy(input_phcx_file,argv[i]);
#else
			cerr << "pdmp: can't read phcx file (psrchive compiled without psrxml)" << endl;
			exit(1);
#endif
		}
		else if (strcmp(argv[i], "-dont-normalise-subints") == 0)
	        {
#ifdef HAVE_PSRXML
			normaliseSubints=false;
#else
			cerr << "pdmp: -dont-normalise-subints ignored (psrchive compiled without psrxml)" << endl;
#endif
		}else if (strcmp(argv[i], "-C") == 0) {
			i++;
			coarseness = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-R") == 0) {
			i++;
			range_mult = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-k") == 0) {
			onlyDisplayDmPlot = true;

            i++;
			onlyDisplayDmPlotMin = atof(argv[i]);

            i++;
			onlyDisplayDmPlotMax = atof(argv[i]);
		}
    else if (strcmp(argv[i], "-con") == 0) {
      drawContourMap = true;
      i++;
      contourFirstLevel = atoi(argv[i]);

      i++;
      contourLevelStep = atoi(argv[i]);

      i++;
      numContourLevels = atoi(argv[i]);
    }
		else if (strcmp(argv[i], "-rms") == 0) {
			i++;
			user_rms = atof(argv[i]);
		}
		else if (strcmp(argv[i], "-rms-file") == 0) {
			i++;
			strcpy(user_rms_file,argv[i]);
			have_user_rms_file = 1;
		}
		else if (strcmp(argv[i], "-eph-coord") == 0) {
		  use_ephemeris_coordinates = true;
		}

		// Handle error if there is no such option
		else if (argv[i][0] == '-') {
			fprintf(stderr, "pdmp error: No such option \"%s\"\n", argv[i]);
			exit(1);
		}

		else {
			beginFilenamesIndex = i;
			break;
		}
	}
}


void cpg_next ()
{
  cpgsch (1);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  cpgsvp (0, 1, 0, 1);
  cpgpage ();
}

void process (Archive* archive, double minwidthsecs, string & bestfilename);

double periodOffset_us = 0;
double periodStep_us = -1;
double periodHalfRange_us = -1;
double dmOffset = 0;
double dmStep = -1;
double dmHalfRange = -1;
FortranSNR snr_obj;
StandardSNR std_obj;

int main (int argc, char** argv)
{
  Profile::rotate_in_phase_domain = true;

	RealTimer clock;

	double elapsed;
	double minwidthsecs = 0.0;


	string option;
	string bestfilename = "";

	pgplot::ColourMap::Name colour_map = pgplot::ColourMap::Heat;

	// Get the command line parameters
	parseParameters(argc, argv,  periodOffset_us, periodStep_us,
      periodHalfRange_us, dmOffset, dmStep, dmHalfRange, colour_map,
      minwidthsecs, bestfilename);

	Archive::verbose = 0;

	if (verbose)
		Archive::verbose = 3;

  if (!onlyDisplayDmPlot) {
    if (cpgopen(plot_device.c_str()) < 0) {
      cout << "Error: Could not open plot device" << endl;
      return -1;
    }

    if (thumbnail) {
      cpgpap(5.0,0.7);
    }
  }
	for (int i = beginFilenamesIndex; i < argc; i++)
		dirglob (&filenames, argv[i]);

	if (filenames.size() == 0) {
		cerr << "pdmp: please specify filename[s]" << endl;
		return -1;
	}

	// Join archives if there is more than 1 input file and the "-j"
	// option has been specified

	Reference::To<Archive> archive;
	Reference::To<Archive> joined_archive;

  if (!onlyDisplayDmPlot) {
    clock.start();
    cpgask(!force);
  }

	if (join && filenames.size() > 1)
	{
		cout << endl << "Loading archive: " << filenames[0] << endl;
                joined_archive = Archive::load (filenames[0]);

		for (unsigned ifile = 1; ifile < filenames.size(); ifile++) {
			cout << "Loading archive: " << filenames[ifile] << endl;
			archive = Archive::load (filenames[ifile]);
			joined_archive->append(archive);
		}

		cout << endl;

		reorder(joined_archive);
    setCentreMjd(joined_archive);
		process(joined_archive, minwidthsecs, bestfilename);

		// total->unload(newname); // THIS IS BROKEN!!!
	}
	else
	{
	  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try
	  {
	    archive = Archive::load(filenames[ifile]);

	    // reset global variables
	    init();

	    process(archive, minwidthsecs, bestfilename);
	  }
	  catch (Error& error)
	  {
	    cerr << "pdmp: error on file '" << filenames[ifile] << "': "
		 << error << endl;
	  }
	}

	cpgend();
	clock.stop();
	elapsed = clock.get_elapsed();

	if (!silent) printf ("\npdmp took %.2lf seconds\n", elapsed  );

	return 0;
}


void process (Archive* archive, double minwidthsecs, string & bestfilename)
{
  dopplerFactor = getDopplerFactor(archive);

  // independently remove the baseline from each subint and channel
  Archive::remove_baseline_strategy.set( new RemoveBaseline::Each,
		  &RemoveBaseline::Each::transform );

#ifdef HAVE_PSRXML
  /*
   * If we have psrxml, we can make a 
   * 'phcx' xml candidate data structure.
   *
   * This can be written out as either a
   * phcx 'pulsarhunter candidate file'
   * or a BookKeepr 'raw candidate'
   * 
   * This part fills in the inital parameters
   * and creates plots etc, if the initial section
   * did not already exist.
   */
  if(input_phcx){
	xml_candidate=read_phcx(input_phcx_file);
        /*
	 * Really we should read the last parameters
	 * from the phcx and use it as input.
	 *
	 * But we won't, as we are lazy.
	 *
	 */
  } else {
	xml_candidate=NULL;
  }
  if(output_phcx){
  	if (xml_candidate==NULL){
		xml_candidate = (phcx*)malloc(sizeof(phcx));
		xml_candidate->nsections=0;
		xml_candidate->nextrakey=0;
	}
	setInitialXmlCandiateSection(archive,minwidthsecs);
  }
#endif

  // compute profile rms if we have a user rms file from dspsr

  if (have_user_rms_file){
	FILE* rms_file = fopen(user_rms_file,"r");
	char text[128];
	float sumsq = 0;
	float weight_total = 0;
	unsigned count = 0;

	unsigned orig_nchans = 0;
	uint64_t orig_nsamp = 0;

	assert( fscanf(rms_file,"# %s %d\n", text, &orig_nchans) == 2 );
	assert( fscanf(rms_file,"# %s %lld\n", text, &orig_nsamp) == 2 );

	uint64_t nadd = orig_nsamp/(uint64_t)(archive->get_nbin() * archive->get_nsubint());
	unsigned nscr = orig_nchans / archive->get_nchan();

	for (unsigned ic = 0; ic < archive->get_nchan(); ic++)
	{
		float ch_wt=0;
		for (unsigned is = 0; is < archive->get_nsubint() ; is++)
		{
			Profile* prof = archive->get_Profile(is, FIRST_POL, ic);
			ch_wt += prof->get_weight();
		}

		for (unsigned i = 0; i < nscr; i++)
		{
		  float chan_rms = 0;
		  assert( fscanf(rms_file,"%f\n",&chan_rms) == 1 );
		  sumsq += chan_rms*chan_rms * ch_wt*ch_wt;
		}
		weight_total += ch_wt;
		count+=archive->get_nsubint();
	}

	sumsq /= weight_total*weight_total;
	sumsq *= archive->get_nchan()*archive->get_nchan();
	
//	weight_factor = archive->get_nsubint()*archive->get_nchan()/weight_total;
//	weight_factor=weight_total/archive->get_nsubint();

	nadd = orig_nsamp/(uint64_t)(archive->get_nbin() * archive->get_nsubint());
	user_rms = sqrt(sumsq/orig_nchans/nadd/nscr);

//	user_rms *= sqrt(archive->get_nsubint()*orig_nchans/weight_total);
	user_rms /= sqrt(archive->get_nsubint()*archive->get_nchan());

//	cout << sqrt(sumsq) << " " << weight_factor << " " << weight_total << endl;
	if (!silent){
		cout << "User rms = " << user_rms;
	}
  }

  Plot *phase_plot = factory.construct("freq");
  TextInterface::Parser* fui = phase_plot->get_frame_interface();

  Plot *time_plot = factory.construct("time");
  TextInterface::Parser* tui = time_plot->get_frame_interface();

  ProfilePlot *total_plot = new ProfilePlot();
  TextInterface::Parser* flui = total_plot->get_frame_interface();


  if (!silent && !join)
  {
    cout << "\nWorking on archive " << archive->get_source() <<
      ": " << archive->get_filename() << endl;
  }

  if (useStandardProfile)
  {
    if (!silent)
      cout << "Using standard profile: " << standardProfileFilename << endl;
    Reference::To<Archive> std_arch;
    std_arch = Archive::load(standardProfileFilename);
    std_arch->fscrunch();
    std_arch->pscrunch();
    std_arch->tscrunch();
    
    std_prof = std_arch->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);

    stdSNR = new StandardSNR();
    stdSNR->set_standard(std_prof);
  }

  if (!onlyDisplayDmPlot) {
    cpg_next();
  }

  ///////////////////////////////////////////////////////////
  // Get the reference values

  archive->dedisperse();
  archive->pscrunch();

  ///////////////////////////////////////////////////////////
  // Start searching for the best period and DM

  string emptyString = "";

  if (!onlyDisplayDmPlot) {
    goToDMPViewPort();
  }

  // Before entering the search loop, scrunch the archive partially
  // if necessary.
  scrunchPartially(archive);

  setSensibleStepSizes(archive);

  // Make sure to convert period to microseconds
  // Then solve for the best period and dm and plot the results
  Reference::To<Archive> partial_archive = archive->clone();
  solve_and_plot (archive,
		  dmOffset, dmStep, dmHalfRange, 
		  periodOffset_us, periodStep_us, periodHalfRange_us,
		  total_plot, flui, minwidthsecs);

  // Create two copies. One for the phase time plot and one
  // for the phase vs. frequency plot.
  Reference::To<Archive> phaseTimeCopy = archive->clone();
  Reference::To<Archive> phaseFreqCopy = archive->clone();

  if (!onlyDisplayDmPlot) {
    if (bestfilename != "") {
      plotSNRdm(bestfilename, bestDM);
    }
  }

  phaseTimeCopy->set_dispersion_measure(bestDM);
  phaseTimeCopy->dedisperse();

  // the archive will be fscrunched by the following method
  plotPhaseTime(phaseTimeCopy, time_plot, tui);
  
  counter_drift( phaseFreqCopy,
		 (bestPeriod_bc_us / (double)MICROSEC)*dopplerFactor );
  // the archive will be tscrunched by the following method

  plotPhaseFreq(phaseFreqCopy, phase_plot, fui);

  /* START MIKE_XML after period/DM optimisation */

#ifdef HAVE_PSRXML
  /*
   * Again, if we have the psrxml library, 
   * we can add the optimised parameters
   * to our 'phcx' data structure, and
   * then write out the file.
   *
   * We first make an 'optimised' clone of
   * the input archive, which has the optimal
   * period/dm (acc/jerk) applied to it.
   *
   * This is then added to the phcx as a
   * new section
   */
  if(output_phcx){
	  Reference::To<Archive> optimisedArchive = archive->clone();
	  counter_drift( optimisedArchive,
			  (bestPeriod_bc_us / (double)MICROSEC)*dopplerFactor );
	  optimisedArchive->set_dispersion_measure(bestDM);
	  optimisedArchive->dedisperse();

	  addOptimisedXmlCandidateSection(optimisedArchive,getPeriod(archive),getDM(archive),0,0,minwidthsecs);
	  printf("Outputting candidate to %s",output_phcx_file);
	  write_phcx(output_phcx_file,xml_candidate);
  }
#endif

  if (!onlyDisplayDmPlot) {
    cpgsci(6);
    cpgslw(8);

    cpgslw(1);
    cpgsci(1);

    printResults(archive);
  }

  int tScint=0;
  int fScint=0;
  int tfScint=0;

  // Determine SNR as a function of frequency, time and frequency and time into globals
  // Already have archive, phaseTimeCopy and phaseFrequencyCopy plus bestPulseWidth 
  if (useOwnStandardProfile){
    stdSNR = new StandardSNR();
    cout << " Setting standard to the best " << endl;
    std_obj.set_standard(bestProfile);
  } 
  // SNR as a function of time

  phaseTimeCopy->fscrunch();
  float rms = getRMS(phaseTimeCopy);

  for (unsigned i=0;i<phaseTimeCopy->get_nsubint();i++)
  {
    float snr;
    if (useOwnStandardProfile){
      snr = getSNR(phaseTimeCopy->get_Profile(i,0,0));
      bestSNRtime.push_back(snr);
    } else {
      snr = getSNR(phaseTimeCopy->get_Profile(i,0,0), rms, bestPulseWidth, bestPulseWidth);
      bestSNRtime.push_back(snr);
    }
    if (debug) cout << "SNRtime " << i << " " << snr << " " << bestSNR <<endl;
    //    if (snr > bestSNR) tScint++; //cout << "TIMESCINTER "<< i << " "<< snr/bestSNR << endl;
  }
  sort(bestSNRtime.begin(),bestSNRtime.end());
  float median_SNRtime = bestSNRtime[bestSNRtime.size()/2-1];
  for (unsigned i=0;i<bestSNRtime.size();i++)
  {
    if (bestSNRtime[i]>RATIO*median_SNRtime){
      cout << "TIMESCINTER RATIO TEST FAIL: RATIO " << bestSNRtime[i]/median_SNRtime << endl;
      tScint++;
    }
  }

  // SNR as a function of frequency

  rms = getRMS(phaseFreqCopy);
  for (unsigned i=0;i<phaseFreqCopy->get_nchan();i++)
  {
    float snr;
    if (useOwnStandardProfile){
      snr = getSNR(phaseFreqCopy->get_Profile(0,0,i));
      bestSNRfreq.push_back(snr);
    } else {
      snr = getSNR(phaseFreqCopy->get_Profile(0,0,i), rms, bestPulseWidth, bestPulseWidth);
      bestSNRfreq.push_back(snr);
    }
    if (debug) cout << "SNRfreq " << i << " " << snr << endl;
    //    if (snr > bestSNR) fScint++; //cout << "FREQSCINTER "<< i << " "<< snr/bestSNR << endl;
  }
  sort(bestSNRfreq.begin(),bestSNRfreq.end());
  float median_SNRfreq = bestSNRfreq[bestSNRfreq.size()/2-1];
  for (unsigned i=0;i<bestSNRfreq.size();i++)
  {
    if (bestSNRfreq[i]>RATIO*median_SNRfreq){
      cout << "FREQSCINTER RATIO TEST FAIL: RATIO " << bestSNRfreq[i]/median_SNRfreq << endl;
      fScint++;
    }
  }

  // Individual freq/time profiles

  rms = getRMS(partial_archive);
  partial_archive->pscrunch();
  for (unsigned i=0;i<partial_archive->get_nsubint();i++)
  {
    for (unsigned j=0;j<partial_archive->get_nchan();j++)
    {
      float snr;
    if (useOwnStandardProfile){
      snr = getSNR(partial_archive->get_Profile(i,0,j));
      bestSNRtimefreq.push_back(snr);
    } else{
      snr = getSNR(partial_archive->get_Profile(i,0,j), rms, bestPulseWidth, bestPulseWidth);
      bestSNRtimefreq.push_back(snr);
    }
      if (debug) cout << "SNRtimefreq " << i << " " << j << " "<< snr << endl;
    }
  }
  sort(bestSNRtimefreq.begin(),bestSNRtimefreq.end());
  float median_SNRtimefreq = bestSNRtimefreq[bestSNRtimefreq.size()/2-1];
  for (unsigned i=0;i<bestSNRtimefreq.size();i++)
  {
    if (bestSNRtimefreq[i]>RATIO*median_SNRtimefreq){ 
      cout << "TIMEFREQSCINTER RATIO TEST FAIL: RATIO " << bestSNRtimefreq[i]/median_SNRtimefreq << endl;
      tfScint++; 
    }
  }
  
  writeResultFiles(archive,tScint,fScint,tfScint);

  //&bestSNRtime[0]
  //bestSNRtime.size()
}

//  Try to find out the best DM and Period given:
// 	DM offset, DM step, DM half-range
//  Period offset (us), Period step (us), Period half-range (us)
//  NOTE: refP is Barycentric

void solve_and_plot (Archive* archive,
		     double dmOffset, double dmStep, double dmHalfRange,
		     double periodOffset_us, double periodStep_us,
		     double periodHalfRange_us,
		     ProfilePlot* total_plot, TextInterface::Parser* flui, double minwidthsecs)
{
  /* optimization: setting the phase prediction model to NULL disables
     the phase prediction model resynchronization that may otherwise take
     place when tscrunch is called - van Straten, January 2010 */

  archive->set_model(NULL);
  archive->set_ephemeris(NULL);

	// MJK: I have made the SNRs array global.. not sure if this will make
	// anything not work (i.e. too much memory use?)
	Reference::To<Profile> profile;
	float snr = 0;

	unsigned nbin = archive->get_nbin();
	unsigned nsub = archive->get_nsubint();
	unsigned nchan = archive->get_nchan();
	double refP_us = getPeriod(archive) * MICROSEC;
	double refDM = getDM(archive);

 	unsigned minwidthbins = unsigned(minwidthsecs/getPeriod(archive)*nbin);

	if (minwidthbins<1) minwidthbins=1;
	if (minwidthbins>nbin/2) minwidthbins=nbin/2;

	///////////////
	// Get the RMS
	float rms = getRMS(archive);

	//////////////////////////////////////////////////////
	// Find default step and half range if none provided

	if (dmStep < 0)
		dmStep = getNaturalDMStep(archive, dmHalfRange);

	if (dmHalfRange < 0)
		dmHalfRange = getNaturalDMHalfRange(archive, dmStep);

	if (periodStep_us < 0)
		periodStep_us = getNaturalPeriodStep(archive, periodHalfRange_us);

	if (periodHalfRange_us < 0)
		periodHalfRange_us = getNaturalperiodHalfRange(archive, periodStep_us);

	// cerr << "coarseness=" << coarseness << endl;

	dmStep *= coarseness;
	periodStep_us *= coarseness;
	periodHalfRange_us *= range_mult;
	dmHalfRange *= range_mult;

	// Make sure that the steps are sensible. Otherwise, just do one step
	if (nchan == 1 || dmStep <= 0)
		dmHalfRange = dmStep/2;

	if (nsub == 1 || periodStep_us <= 0)
		periodHalfRange_us = periodStep_us/2;


	//////////////////////////////////////////////////////

	// Now that everything's initialised, do all the searching
	int dmBins, periodBins;

	// Number of bins in the DM axis
	dmBins = (int)ceil( ( fabs(dmHalfRange)*2 ) / dmStep);

	// cerr << "periodStep_us=" << periodStep_us << endl;

	// Number of bins in the Period axis
	periodBins = (int)ceil( ( fabs(periodHalfRange_us)*2 ) / periodStep_us);

	// cerr << "periodBins=" << periodBins << endl;

  if (!onlyDisplayDmPlot) {
    // Print the header
    printHeader(archive, periodOffset_us, periodStep_us, periodHalfRange_us,
        dmOffset, dmStep, dmHalfRange);

    // Revert back to the DM vs P vs Intensity viewport
    goToDMPViewPort();
  }

	double minDM = refDM + dmOffset - fabs(dmStep*(floor((double)dmBins/2)));
	double currDM = minDM;

	double minP = refP_us +	periodOffset_us - fabs(periodStep_us*(floor((double)periodBins/2)));
	double currP = minP;

	double maxDM = minDM + dmStep * dmBins;

	double maxP = minP + periodStep_us * periodBins;

	if (!silent) cout << "Searching for optimum DM and Period...";

	// Begin the search for optimum DM and Period
	// Foreach DM

        double backup_DM = archive->get_dispersion_measure ();

	for (int dmBin = 0; dmBin < dmBins ; dmBin++)
        {
                archive->set_dispersion_measure(currDM);
		// total(false) scrunches in frequency but not time
		Reference::To<Archive> dmLoopCopy = archive->total(false);

		// Foreach Period (include one extra period bin at the end to scale with
		// the plot
		for (int periodBin = 0; periodBin <= periodBins; periodBin++) {

			// print out the search progress
			int percentComplete = (int)floor(100 * ((double)(periodBins*(dmBin) + periodBin) / 
								(double)(periodBins * dmBins)));

			int displayPercentage = (int)floor((double)percentComplete/SHOW_EVERY_PERCENT_COMPLETE);
			if (!silent) printf("%3d%%\r", displayPercentage*SHOW_EVERY_PERCENT_COMPLETE);

			// Create a new (unscrunched) copy so the values can be testedget
			Reference::To<Archive> periodLoopCopy = dmLoopCopy->clone();

			// set the trial period and dm value and update
			double newFoldingPeriod = currP/(double)MICROSEC;

			// Make sure the new folding period is topocentric
			counter_drift(periodLoopCopy, newFoldingPeriod);

			periodLoopCopy->tscrunch();

			snr = getSNR(periodLoopCopy->get_Profile(0,0,0), rms, minwidthbins);

			if (verbose)	{
				printf( "\nrefP topo = %3.10g, Set P = %3.15g dP = %3.15g\n",
				refP_us, currP, currP - refP_us);
				printf( "refDM = %3.10g, Set DM = %3.15g dDM = %3.15g, S/N = %3.10g\n\n",
				refDM, currDM, currDM - refDM, snr);
			}

			if (snr > bestSNR) {

				if (verbose) {
					fprintf(stderr, "Better S/N found: Old snr = %3.15g, New snr = %3.15g, Best DM = %3.15g and period = %3.15g \n", bestSNR, snr, currDM, newFoldingPeriod * MILLISEC);
				}

				bestSNR = snr;
				bestPeriod_bc_us = currP / dopplerFactor;
				bestDM = currDM;
				bestFreq = 1/(bestPeriod_bc_us/(double)MICROSEC);
				freqError = fabs((periodStep_us/(double)MICROSEC)/pow((bestPeriod_bc_us/(double)MICROSEC), 2));
				periodLoopCopy->remove_baseline();
				bestProfile = periodLoopCopy->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);

				// get the width of the pulse
				
				bestPulseWidth = snr_obj.get_bestwidth();

				//int rise, fall;
				//find_spike_edges(bestProfile, rise, fall);
				//
				//if (rise > fall)
				//	bestPulseWidth = fall + (nbin - rise);
				//else
				//	bestPulseWidth = fall-rise;
			}

			// put this intensity into array
			SNRs.push_back((float)snr);
			currP += periodStep_us;
		}

		// Reset the initial period value
		currP = minP;
		currDM += dmStep;
	}

        archive->set_dispersion_measure( backup_DM );

	// get the error
	periodError_ms = computePeriodError(archive);
	dmError = computeDMError(archive);

	// Now that the period step is initialised, init the freqError
	if (nsub == 1 || periodStep_us <= 0)
        {
		freqError = 0;
	}
        else
        {
		freqError = fabs((periodError_ms/(double)MICROSEC)/pow((bestPeriod_bc_us/(double)MICROSEC), 2));
		freqError *= 1000;
	}

	// multiply errors by 3
	periodError_ms *= 3;
	dmError *= 3;
	freqError *= 3;

	float trf [6] = {(float)(-periodStep_us - periodHalfRange_us) + periodOffset_us, (float)periodStep_us, 0.0, (float)(minDM - 0.5*dmStep), 0.0, (float)dmStep};

	///////////////////////////////////////////
	// Plot the deltaPeriod vs. DM vs. SNR plot
	// and the pulse profile plot

	if (dmBins > 0) {
    const double minx = periodOffset_us - periodHalfRange_us;
    const double maxx = periodOffset_us + periodHalfRange_us;

    if (onlyDisplayDmPlot) {
      draw_colour_map_only(dmBins, periodBins + 1, minx, maxx, minDM, maxDM, trf);
    } else {
      draw_colour_map(&SNRs[0], dmBins, periodBins+1, minx, maxx,
          "delta Period (us)", minDM, maxDM, "DM", "", trf, 5);
    }
  }

	plotProfile (bestProfile, total_plot, flui);

	if (verbose) {
		printf("trf = {%3.10g, %3.10g, %3.10g, %3.10g, %3.10g, %3.10g}\n",
		-0.5*periodStep_us - periodHalfRange_us, periodStep_us, 0.0, minDM - 0.5*dmStep , 0.0,  dmStep);

		printf("minx = %3.10g, maxx = %3.10g, miny = %3.10g, maxy = %3.10g, rows = %d, cols = %d\n", minP, maxP, minDM, maxDM, dmBins, periodBins);
		cout << "number of S/Ns = " << SNRs.size() << endl;
	}

  drawBestValuesCrossHair( archive,
      periodOffset_us, periodStep_us, periodHalfRange_us,
      dmOffset, dmStep, dmHalfRange);
}

// Use the the default natural values for offset, step and half-range
void solve_and_plot (Archive* archive,
		     ProfilePlot* total_plot,
		     TextInterface::Parser* flui, double minwidthsecs)
{
  solve_and_plot(archive, 0,-1,-1, 0,-1,-1, total_plot, flui, minwidthsecs);
}


void scrunchPartially(Archive * archive) {
  if (maxSubints > 1) {
    archive->tscrunch_to_nsub(maxSubints);
  }

  if (maxChannels > 1) {
    archive->fscrunch_to_nchan(maxChannels);
  }

  if (maxBins > 1) {
    archive->bscrunch_to_nbin(maxBins);
  }
}


double getNaturalDMStep(const Archive * archive, double halfRange)  {

    unsigned nchan = archive->get_nchan();

    if (halfRange < 0) {
        // give step arg value of 0 so that it gets the natural half range
        halfRange = getNaturalDMHalfRange(archive, 0);
    }

    return halfRange / (double)(2*nchan + 1);
}


double getNaturalDMHalfRange(const Archive * archive, double step)
{
  unsigned nbin = archive->get_nbin();
  double centre_period = archive->get_Integration(FIRST_SUBINT)->get_folding_period() * MILLISEC;

  double tbin = centre_period / (double)nbin;
  double fcentre = archive->get_centre_frequency();
  double bw = archive->get_bandwidth();
  unsigned nchan = archive->get_nchan();
  double bwband = fabs(bw/(double)nchan);

  return tbin / DMCONST / (pow(1.0/(fcentre - 0.5*bwband), 2.0) - (pow(1.0/(fcentre + 0.5*bwband), 2.0)));
}


double getNaturalPeriodStep (const Archive * archive, double halfRange)
{
  if (verbose)
    cerr << "getNaturalPeriodStep: MJD"
      " start=" << archive->start_time().printdays(13) <<
      " end=" << archive->end_time().printdays(13) << endl;

  double tspan = archive->integration_length();
  assert (tspan > 0.0);

  double centre_period = archive->get_Integration(0)->get_folding_period();
  assert (centre_period > 0.0);

  double tbin = centre_period / archive->get_nbin();

  return MICROSEC * centre_period * tbin / tspan;
}


double getNaturalperiodHalfRange(const Archive * archive, double step)  {

	int nbin = archive->get_nbin();
  	double centre_period = archive->get_Integration(FIRST_SUBINT)->get_folding_period() * MICROSEC;
	double tsub = (archive->get_Integration(0))->get_duration() * MICROSEC;
	double tbin = centre_period / (double)nbin; // already in microseconds

	return centre_period * tbin / tsub;
}


// Some hardcoded viewport settings. Makes it easier to
// add things to the window

void goToHeaderViewPort() {
	cpgsvp(0, 1.0, 0.87, 1.0);
	cpgswin(0.0, 1.0, 0.0, 1.0);
	cpgsch(0.7);
	cpgslw(1);

}

void goToDMPViewPort() {
	cpgsvp(0.05, 0.45, 0.68, 0.86);
}

void goToSNRdmViewPort() {
  	cpgsvp(0.55, 0.95, 0.68, 0.86);
	//	cpgswin(0.0-0.05*());
}

void goToPhaseTimeViewPort() {
	cpgsvp(0.05, 0.45, 0.33, 0.56);
}

void goToPhaseTimeTitleViewPort() {
	cpgsvp(0.05, 0.45,0.565, 0.58);
	cpgswin(0.0, 1.0, 0.0, 1.0);
}

void goToPhaseFreqViewPort() {
	cpgsvp(0.55, 0.95, 0.33, 0.56);
}

void goToPhaseFreqTitleViewPort() {
	cpgsvp(0.55, 0.95, 0.565, 0.58);
	cpgswin(0.0, 1.0, 0.0, 1.0);

}

void goToBestValuesViewPort() {
	cpgsvp(0.01, 1.0, 0.185, 0.275);
	cpgswin(0.0, 1.0, 0.0, 1.0);
}


void goToProfileViewPort() {
	cpgsvp(0.05, 0.95, 0.05, 0.205);
}

template<typename T> T sqr (T x) { return x*x; }

float getRMS (const Archive * archive)
{
  if (user_rms > 0)
    return user_rms;

  unsigned nbin = archive->get_nbin();
  unsigned nchan = archive->get_nchan();
  unsigned nsub = archive->get_nsubint();

  double weighted_total_variance = 0;
  double total_weight = 0;

  for (unsigned is = 0; is < nsub ; is++)
  {
    for (unsigned ic = 0; ic < nchan; ic++)
    {
      const Profile* profile = archive->get_Profile(is, FIRST_POL, ic);
      const float* amps = profile->get_amps();

      // find the min mean
      double smin = -1;
      int itmin = 0;

      for (unsigned ib = 0; ib < nbin; ib++)
      {
        double s = 0;
        for (unsigned j = ib; j < ib + nbin/2; j++)
          s += amps[j%nbin];

        if ((s < smin) || (smin < 0))
        {
          smin = s;
          itmin = ib;
        }
      }

      double minMean = smin / (nbin/2);

      double sumsq = 0.0;
      for (unsigned i = itmin; i < itmin + nbin/2; i++)
        sumsq += sqr( amps[i%nbin] - minMean );

      double variance = sumsq / (nbin/2-1);

      double weight = profile->get_weight();

      weighted_total_variance += sqr(weight) * variance;
      total_weight += weight;
    }
  }

  double rms = sqrt( weighted_total_variance / sqr(total_weight) );

  return rms;
}

/* 
float getSNR (const Archive * archive, float rms, int minwidthbins) {

Reference::To<Archive> copy = archive->clone();
	Reference::To<Profile> profile = copy->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);

	snr_obj.set_rms( rms );
	snr_obj.set_minwidthbins (minwidthbins);
	return snr_obj.get_snr (profile);

}
*/

float getSNR(const Profile * p){
  cout << "Calling std profile snr function"<< endl;
  float s = std_obj.get_snr(p);
  cout << "SNR is " << s << endl;
  return s;
}


float getSNR (const Profile * p, float rms, int minwidthbins) {

  // cerr << "getSNR rms=" << rms << " minwidth=" << minwidthbins << endl;

	snr_obj.set_rms( rms );
	snr_obj.set_minwidthbins (minwidthbins);
	snr_obj.set_maxwidthbins (p->get_nbin()/2);
	return snr_obj.get_snr (p);
}

float getSNR (const Profile * p, float rms, int minwidthbins, int maxwidthbins) {

	snr_obj.set_rms( rms );
	snr_obj.set_minwidthbins (minwidthbins);
	snr_obj.set_maxwidthbins (maxwidthbins);
	return snr_obj.get_snr (p);
}

bool isNumber(char * str) {

	for (int i = 0; str[i] != '\0'; i++) {
		for (int j = 0; ; j++) {
			if (str[i] == NUMERIC_CHARS[j])
				break;

			// Reached the end which means it's not a number
			if (j == NUMERIC_CHARS_LEN - 1)
				return false;
		}
	}

	return true;
}

void parseAndValidateDouble(string optionName, char * numberString, double &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}

	value = atof(numberString);
}

void parseAndValidateInt(string optionName, char * numberString, int &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}

	value = atoi(numberString);
}

void parseAndValidatePositiveDouble(string optionName, char * numberString, double &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}

	value = atof(numberString);

	if (value <= 0 ) {
		cerr << endl << "Please provide a positive float for value: " << optionName << endl;
		exit(1);
	}

}
void parseAndValidatePositiveInt(string optionName, char * numberString, int &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}

	value = atoi(numberString);

	if (value <= 0 ) {
		cerr << endl << "Please provide a positive int for value: " << optionName << endl;
		exit(1);
	}

}

double computePeriodError(const Archive * archive) {

	double pulseWidth_ms = 0;
	double error = 0;
	double tspan = archive->integration_length();
	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	double refP_ms = bcPeriod_s * MILLISEC;
	double tsub = archive->get_Integration(0)->get_duration();
	unsigned nsub = (unsigned)(tspan / tsub);
	double integrationLength = tsub * (nsub - 1);
	unsigned nbin = archive->get_nbin();
	double tbin_ms = (refP_ms) / nbin;

	pulseWidth_ms = bestPulseWidth * tbin_ms;
	error = pulseWidth_ms / bestSNR;

	if (error < (bcPeriod_s / nbin ) * 0.5 * MILLISEC) {
		if (verbose) {
			printf("computeDMError: error of %3.10g < %3.10g\n",
			error, (bcPeriod_s / nbin ) * 0.5 * MILLISEC);
		}
		error = (bcPeriod_s / nbin ) * 0.5 * MILLISEC;
	}

	if (integrationLength > 0)
		return (error*refP_ms) / (integrationLength * MILLISEC);
	else if (integrationLength < 0)
		throw "Error: nsub <= 0?";
	else
		return 0;
}


double computeDMError(const Archive * archive) {

	double pulseWidth_ms = 0;
	double error = 0;
	double frequency1 = 0;
	double frequency2 = 0;

	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	unsigned nbin = archive->get_nbin();
	unsigned nchan = archive->get_nchan();

	double channelBW = archive->get_bandwidth() / archive->get_nchan();
	double refP_ms = bcPeriod_s * MILLISEC;
	double tbin_ms = (refP_ms) / nbin;
	pulseWidth_ms = bestPulseWidth * tbin_ms;
	error = pulseWidth_ms / bestSNR;

	if (error < (bcPeriod_s / nbin ) * 0.5 * MILLISEC) {
		if (verbose) {
			printf("computeDMError: error of %3.10g < %3.10g\n",
			error, (bcPeriod_s / nbin ) * 0.5 * MILLISEC);
		}
		error = (bcPeriod_s / nbin ) * 0.5 * MILLISEC;
	}

	frequency1 = archive->get_centre_frequency() + 0.5 * fabs(archive->get_bandwidth()) - fabs(channelBW/2);
	frequency2 = archive->get_centre_frequency() - 0.5 * fabs(archive->get_bandwidth()) + fabs(channelBW/2);

	double deltaf = frequency1 - frequency2;

	if (nchan > 1)
		return error / ( (8.297616/MILLISEC) * deltaf * pow(archive->get_centre_frequency()/(double)1000, -3));
	else if (nchan <= 0)
		throw "nchan <= 0?";
	else
		return 0;

	// This is an error for a curved fit to the phase vs freq plot
	// return fabs(error / (DMCONST * (pow(frequency1, -2) - pow(frequency2, -2))));

}

void slaPvobs ( double p, double h, double stl, double pv[6] )
{
	double SR = 7.292115855306589e-5;  /* Mean sidereal rate (at J2000)
                                    in radians per (UT1) second */

	double r, z, s, c, v;

/* Geodetic to geocentric conversion */
	slaGeoc ( p, h, &r, &z );

/* Functions of ST */
	s = sin ( stl );
	c = cos ( stl );

/* Speed */
	v = SR * r;

/* Position */
	pv[0] = r * c;
	pv[1] = r * s;
	pv[2] = z;

/* Velocity */
	pv[3] = - v * s;
	pv[4] = v * c;
	pv[5] = 0.0;
}

void slaGeoc ( double p, double h, double *r, double *z )
{
	double sp, cp, c, s;

/* Earth equatorial radius (metres) */
	static double a0 = 6378140.0;

/* Reference spheroid flattening factor and useful function thereof */
	static double f = 1.0 / 298.257;
	double b = ( 1.0 - f ) * ( 1.0 - f );

/* Astronomical unit in metres */
	static double au = 1.49597870e11;

/* Geodetic to geocentric conversion */
	sp = sin ( p );
	cp = cos ( p );
	c = 1.0 / sqrt ( cp * cp + b * sp * sp );
	s = b * c;
	*r = ( a0 * c + h ) * cp / au;
	*z = ( a0 * s + h ) * sp / au;
}


double getPeriod(const Archive * data)
{
	const Integration* subint = data->get_Integration (data->get_nsubint()/2);

	if (verbose) {
		MJD start = subint->get_start_time();
		MJD end = subint->get_end_time();

		printf("pdmp: getPeriod: start_time = %3.10g, end_time = %3.10g\n",
		start.intday() + start.fracday(), end.intday() + end.fracday());
	}

	return subint->get_folding_period();
}


double getDM(const Archive * archive) {
	return archive->get_dispersion_measure();
}



double getDopplerFactor(const Archive * archive)
{
	// Astronomical units
	double AUS     = 499.004786;
	double TWOPI   = 2*M_PI;
	double RTOD    = (double)360/TWOPI;

	double dps[3];
	double dvb[3];
	double dpb[3];
	double dvh[3];
	double dph[3];

	int hours, degrees, minutes;
	double seconds, ra_secs, ra_degs, dec_degs;
	double R2000, D2000;

	double start = archive->start_time().intday() + archive->start_time().fracday();
	double end = archive->end_time().intday() + archive->end_time().fracday();

  if (tmjdctr == 0.0) {
    tmjdctr = (archive->start_time().in_days() + 0.5 *
        archive->integration_length()) * 86400.0;
  }

	sky_coord coord = getCoord (archive);
	Angle ra_angle = coord.ra();
	Angle dec_angle = coord.dec();

	ra_angle.getHMS(hours, minutes, seconds);
	if (verbose) {
		printf("pdmp: getDopplerFactor: mjd_start = %3.10g, mjd_end = %3.10g, ra_hours = %d, ra_min = %d, ra_sec = %3.10g\n",
		start,
		end,
		hours, minutes, seconds);
	}
	ra_secs = hours * 3600 + minutes * 60 + seconds;

	dec_angle.getDMS(degrees, minutes, seconds);

	dec_degs = dec_angle.getDegrees();

	if (verbose) {
		printf("pdmp: getDopplerFactor: ra_secs = %3.15g, dec_degs = %d, dec_min = %d, dec_sec = %3.10g, dec.getDegrees() = %3.15g\n",
		ra_secs, degrees, minutes, seconds, dec_degs);
	}

	// Convert it to degrees
	ra_degs = ra_secs / (double)240; // == (ra_secs/(24*3600)) * 360

	// Convert to radians
	R2000 = ra_degs/(double)RTOD;
	D2000 = dec_degs/(double)RTOD;

	if (verbose) {
		printf("pdmp: getDopplerFactor: RTOD = %3.15g, R2000 = %3.15g, D2000 = %3.15g\n",
		RTOD, R2000, D2000 );
	}

	// Get the site index
	//char site_code = copy->get_telescope();

	/*int index = getSiteCodeIndex(site_code);*/

	/*if (verbose)
		cout << "pdmp: getDopplerFactor: Archive observed from the telescope: " << CTEL[index] << endl;*/

	// Get the observatory position wrt the geocentre
	/*double x = X[index]; // in metres
	double y = Y[index];
	double z = Z[index];*/

	double x = 0;
	double y = 0;
	double z = 0;

	try {
		Reference::To<const ITRFExtension> ext = archive->get<ITRFExtension>();
		x = ext->ant_x;
		y = ext->ant_y;
		z = ext->ant_z;
	} catch (Error &error)
        {
	  using Pulsar::Site;
	  const Site* site = Site::location (archive->get_telescope());
	  if (site)
	    site->get_xyz (x, y, z);
	  else
	    {
	      cerr << "pdmp: cannot determine coordinates of antenna" << endl
		   << "      defaulting to geocenter" << endl;
	      x = y = z = 0.0;
	    }
	}

	// Convert to DCS
	slaDcs2c (R2000, D2000, dps);

	// Convert the time to TDT.
	double tdt = tmjdctr + slaDtt (tmjdctr/(double)86400);

	// Convert UTC to TDB.
	double g = (357.53 + 0.9856003 * (tmjdctr/(double)86400 - 51544.5)) / RTOD;
	double tdb = tdt + 0.001658 * sin (g) + 0.000014 * sin(2 * g);

	slaEvp (tdb/(double)86400, (double)2000, dvb, dpb, dvh, dph);

	double sitera = 0;

	// The velocity of the observatory
	double site_vel[3];

	// Cylindrical coordinates of observatory
	double site[3];

	/////////////////////////////////
	///// Based on tempo's setup.f
	////////////
	double erad = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

	double siteLatitude = asin(z/erad);
	double siteLongitude = atan2(-y, x);
	double sphericalRadius = erad/(2.99792458e8*AUS);

	if (verbose) {

		printf("pdmp: getDopplerFactor: tmjdctr = %3.10g, radius = %3.10g, siteLatitude = %3.10g, siteLongitude = %3.10g, sphericalRadius = %3.10g\n" ,
		0.5 * (end + start), erad, siteLatitude, siteLongitude, sphericalRadius);

	}

	/////////////////////////////////
	///// Based on tempo's arrtim.f
	////////////

	site[0] = sphericalRadius * cos(siteLatitude) * AUS;
	site[1] = site[0] * tan(siteLatitude);
	site[2] = siteLongitude;

	if (verbose) {
		printf("pdmp: getDopplerFactor: site[0] = %3.10g, site[1] = %3.10g, site[2] = %3.10g\n" ,
		site[0], site[1], site[2]);
	}

	/////////////////////////////////
	///// Based on tempo's obsite.f
	////////////

	double LST_der;
	double LST;
	// Set longitude to 0 because we want the
	lmst2(0.5 * (end + start), 0, &LST, &LST_der);

	LST *= TWOPI;

	double ph = LST - site[2];

	double EEQ[3];

	EEQ[0] = site[0] * cos(ph);
	EEQ[1] = site[0] * sin(ph);
	EEQ[2] = site[1];

	sitera = atan2(EEQ[1], EEQ[0]);

	if (verbose) {
		printf("pdmp: getDopplerFactor: LST = %3.10g, ph = %3.10g, EEQ[0] = %3.10g, EEQ[1] = %3.10g, EEQ[2] = %3.10g, sitera = %3.10g\n" ,
		LST, ph, EEQ[0], EEQ[1], EEQ[2], sitera);
	}

	double speed = TWOPI*site[0]/((double)86400/ 1.00273);

	site_vel[0] = -speed * sin(sitera);
	site_vel[1] =  speed * cos(sitera);
	site_vel[2] = 0;

	if (verbose) {
		printf("pdmp: getDopplerFactor: site_vel[0] = %3.10g, site_vel[1] = %3.10g, site_vel[2] = %3.10g, speed = %3.10g\n" ,
		site_vel[0], site_vel[1], site_vel[2], speed);
	}

	/////////////////////////////////
	///// Based on psrephd.f
	////////////

	// Add the EC-observatory vectors to the barycentre-EC vectors.
	for (int i = 0; i < 3; i++) {

		// Convert from AUS per second to metres per second
		dvb[i] *= AUS;
		dvb[i] = dvb[i] + site_vel[i];
	}

	double evel = slaDvdv(dvb, dps);

	if (verbose)
		printf("pdmp: getDopplerFactor: dopps = %3.20g\n", 1-evel);

	return 1-evel;
}


// Get the coordinates from the archive
sky_coord getCoord (const Archive* archive) try
{
  if (!use_ephemeris_coordinates)
  {
    // cerr << "getCoord hdr: " << archive->get_coordinates() << endl;
    return archive->get_coordinates();
  }

  const Parameters* params = archive->get_ephemeris();
  const TextParameters* text = dynamic_cast<const TextParameters*> (params);
  if (!text)
  {
    cerr << "getCoord: archive does not contain TextParameters" << endl;
    exit (-1);
  }

  sky_coord result;

  result.ra().setHMS( text->get<string>("RAJ").c_str() );
  result.dec().setDMS( text->get<string>("DECJ").c_str() );

  // cerr << "getCoord eph: " << archive->get_coordinates() << endl;
  return result;
}
catch (Error& error)
{
  cerr << "getCoord: error=" << error << endl;
  exit (-1);
}

double TRUNC(double value) {
	double result = floor(abs(value));
	if (value < 0.0)
		result = (-result);
	return (result);
}

double FRAC(double value) {
	double result = value - TRUNC(value);
	if (result < 0.0)
		result += 1.0;
	return (result);
}


/* ******************************************** */
/* lmst2                                         */
/* Author:  G. Hobbs (06 May 2003)              */
/* Purpose: Calculates the local mean sidereal  */
/*          time (tsid) and its derivative      */
/*                                              */
/* Inputs:                                      */
/* Outputs: tsid, tsid_der                      */
/*                                              */
/* Notes: based on lmst.f                       */
/*        NONE of this has been checked         */
/* Changes:                                     */
/* ******************************************** */

double lmst2(double mjd,double olong,double *tsid,double *tsid_der)
{
  double xlst,sdd;
  double gmst0;
  double a = 24110.54841;
  double b = 8640184.812866;
  double c = 0.093104;
  double d = -6.2e-6;
  double bprime,cprime,dprime;
  double tu0,fmjdu1,dtu,tu,seconds_per_jc,gst;
  int nmjdu1;

  nmjdu1 = (int)mjd;
  fmjdu1 = mjd - nmjdu1;

  tu0 = ((double)(nmjdu1-51545)+0.5)/3.6525e4;
  dtu  =fmjdu1/3.6525e4;
  tu = tu0+dtu;
  gmst0 = (a + tu0*(b+tu0*(c+tu0*d)))/86400.0;
  seconds_per_jc = 86400.0*36525.0;

  bprime = 1.0 + b/seconds_per_jc;
  cprime = 2.0 * c/seconds_per_jc;
  dprime = 3.0 * d/seconds_per_jc;

  sdd = bprime+tu*(cprime+tu*dprime);

  gst = gmst0 + dtu*(seconds_per_jc + b + c*(tu+tu0) + d*(tu*tu+tu*tu0+tu0*tu0))/86400;
  xlst = gst - olong/360.0;

  xlst = fortran_mod2(xlst,1.0);

  if (xlst<0.0)xlst=xlst+1.0;

  *tsid = xlst;
  *tsid_der = sdd;
  return 0.0;
}

/* Emulate the Fortran "mod" operator */
double fortran_mod2(double a,double p)
{
  double ret;
  ret = a - (int)(a/p)*p;
  return ret;
}

//////////////////////////////////////////////////////////////////////////
////////////////
//////////////// Drawing and printing methods
////////////////
//////////////////////////////////////////////////////////////////////////

void printHeader(const Archive * archive,
                 double periodOffset_us,
								 double periodStep_us,
								 double periodHalfRange_us,
								 double dmOffset,
								 double dmStep,
								 double dmHalfRange) {


	/////////////////////////////////////////////////
	// Print the header
	//

	double gal_long;
 	double gal_lat;

	AnglePair glgb = getCoord(archive).getGalactic();
	glgb.getDegrees(&gal_long, &gal_lat);

	if (gal_long < 0)
		gal_long += 360;

	float lineHeight = 0.16;
	char temp [1024];
	string temp_str;
	string space = "  ";

	// Set the view port to the top row to display the header information
	goToHeaderViewPort();

	if (join) {
		temp_str = archive->get_source() + ": ";
		if (filenames.size() > 5) {
			for (unsigned i = 0; i < 6; i++) {
				temp_str = temp_str + " " + filenames[i];
			}
			temp_str = temp_str + " ...";

		} else {
			for (unsigned i = 0; i < filenames.size(); i++) {
				temp_str = temp_str + " " + filenames[i];
			}
		}

	} else {
		temp_str = archive->get_source() + ": " + archive->get_filename();
	}

	// Display the pulsar name and archive file
	cpgtext(linex, 0.85, temp_str.c_str());

	// reset the y ordinate for the text when looping over archives
	liney = HEADER_Y;


	double refP_us = getPeriod(archive) * MICROSEC;
	double refDM = getDM(archive);
	int nBin = archive->get_nbin();
	int nChan = archive->get_nchan();
	int nSub = archive->get_nsubint();
	double tspan = archive->integration_length();
	double tsub = (archive->get_Integration(0))->get_duration();
	double tbin = (refP_us/1000) / nBin;

	Reference::To<const Integration> subint = archive->get_Integration((int)floor(archive->get_nsubint()/(double)2));
	MJD start = subint->get_start_time();

	int hours, degrees, ra_minutes, dec_minutes;
	double ra_seconds, dec_seconds;

        sky_coord coord = getCoord (archive);
        Angle ra_angle = coord.ra();
        Angle dec_angle = coord.dec();

	double dec_degs = dec_angle.getDegrees();

	ra_angle.getHMS(hours, ra_minutes, ra_seconds);
	dec_angle.getDMS(degrees, dec_minutes, dec_seconds);

	if (dec_degs < 0)
		degrees = -degrees;

	////////////////////////////////////////
	/// Print the reference period and dm

	sprintf(temp, "BC P(ms)= %3.9f TC P(ms)= %3.9f DM= %3.3f RAJ= %02d:%02d:%05.2f DecJ= %02d:%02d:%04.1f",
		(refP_us/1000) / dopplerFactor,
		getPeriod(archive) * MILLISEC,
		refDM,
		hours, ra_minutes, ra_seconds,
		degrees, dec_minutes, dec_seconds);

	cpgtext(linex, liney, temp);
	liney -= lineHeight;

	///////////////////////////////
	/// Print the MJD and frequency

	// Newline
	sprintf(temp, "BC MJD = %.6f " ,
	start.intday() + start.fracday());
	temp_str = temp;

	sprintf(temp, "Centre freq(MHz) = %3.3f ", archive->get_centre_frequency());
	temp_str += temp;

	sprintf(temp, "Bandwidth(MHz) = %3.9g ",
	archive->get_bandwidth());
	temp_str += temp;

	sprintf(temp, "l = %.3f b = %.3f",
	gal_long, gal_lat);
	temp_str += temp;

	cpgtext(linex, liney, temp_str.c_str());

	//////////////////////////////////////////
	/// New line for bin counts and bin times

	liney -= lineHeight;

	sprintf(temp, "NBin = %d NChan = %d NSub = %d ", nBin, nChan, nSub);
	temp_str = temp;

    sprintf(temp , "TBin(ms) = %.3f TSub(s) = %.3f TSpan(s) = %.3f", tbin,
        tsub, tspan);

	temp_str += temp;

	// Print the line
	cpgtext(linex, liney, temp_str.c_str());

	liney -= lineHeight;

	////////////////////////////////////////////////////////
	/// Print the Period and DM offset, step and half-range

	// Newline
	sprintf(temp, "P(us): offset = %3.5f, step = %3.5f, range = %3.5f ",
	 	periodOffset_us, periodStep_us, periodHalfRange_us);

	temp_str = temp;

	sprintf(temp, "DM: offset = %3.3f, step = %3.3f, range = %3.3f",
		dmOffset, dmStep, dmHalfRange);

	temp_str += temp;
	cpgtext(linex, liney, temp_str.c_str());
}

void printResults(const Archive * archive) {

	/////////////////////////////////////////////////
	// Print the best values and corrections

	char temp [512];
	Reference::To<const Integration> subint = archive->get_Integration((int)(archive->get_nsubint()/(double)2));

	MJD start = subint->get_start_time();
	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	double refDM = getDM(archive);
	double refP_us = bcPeriod_s * MICROSEC;

	// Now that the best values are discovered
	goToBestValuesViewPort();

	cpgsch(0.7);
	cpgslw(1);
	float lineHeight = 0.2;
	float colWidth = 0.16;

	float bestValues_x = 0;
	float bestValues_y = 1;

	unsigned nbin = archive->get_nbin();
	double tbin = (bestPeriod_bc_us/1000) / nbin;

	const float RIGHT_JUSTIFY = 1;
	const float HORIZONTAL = 0;

	// Firstly print out the first column of labels
	cpgtext(bestValues_x, bestValues_y, "BC prd (ms):");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Corrn (ms):");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Error (ms):");

	// Next, print out all the values
	bestValues_x += colWidth*1.7;
	bestValues_y = 1;

	sprintf(temp, "%3.9f", bestPeriod_bc_us / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", (bestPeriod_bc_us-refP_us) / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", periodError_ms);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;

	cpgtext(bestValues_x, bestValues_y, "TC prd (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Corrn (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Error (ms):");

	bestValues_x += colWidth * 1.7;
	bestValues_y = 1;

	sprintf(temp, "%3.9f", dopplerFactor * bestPeriod_bc_us / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", (dopplerFactor * bestPeriod_bc_us / MILLISEC) - getPeriod(archive) * MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", periodError_ms);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);


	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;
	cpgtext(bestValues_x, bestValues_y, "DM:");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Corrn:");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Error:");

	// New column: Print out all the values
	bestValues_x += colWidth * 0.8;
	bestValues_y = 1;
	sprintf(temp, "%3.3f", bestDM);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", bestDM-refDM);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", dmError);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);


	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;

	cpgtext(bestValues_x, bestValues_y, "BC freq (Hz):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Freq err. (Hz):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Width (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Best S/N:");

	bestValues_x += colWidth * 0.8;
	bestValues_y = 1;

	sprintf(temp, "%-3.9f", bestFreq);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%-3.9f", freqError);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", bestPulseWidth * tbin);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%-3.2f", bestSNR);
	cpgtext(bestValues_x, bestValues_y, temp);


	// Print out the results on the console
	if (!silent) {

		printf("\n\nBest S/N = %.2f\n", bestSNR);

		printf("BC MJD = %.6f\n", start.intday() + start.fracday());

		printf("BC Period (ms) = %3.10g  TC Period (ms) =  %3.10g  DM = %3.3g\n",
						bcPeriod_s * MILLISEC,	getPeriod(archive) * MILLISEC, getDM(archive));

		printf("Best BC Period (ms) = %3.10g  Correction (ms) = %3.10g  Error (ms) = %3.10g\n",
						bestPeriod_bc_us / MILLISEC,	(bestPeriod_bc_us-refP_us) / MILLISEC, periodError_ms / MILLISEC);

		printf("Best TC Period (ms) = %3.10g  Correction (ms) = %3.10g  Error (ms) = %3.10g\n",
						dopplerFactor * bestPeriod_bc_us / MILLISEC,
						(dopplerFactor * bestPeriod_bc_us / MILLISEC) - (getPeriod(archive) * MILLISEC),
						periodError_ms / MILLISEC);

		printf("Best DM = %3.3g  Correction = %3.3g  Error = %3.3g\n",
	        	bestDM, bestDM-refDM, dmError);

		printf("Best BC Frequency (Hz) = %3.10g  Error (Hz) = %3.10g\n",
	        	bestFreq, freqError);

		printf("Pulse width (bins) = %d\n",
	        	bestPulseWidth);
	}

}

void writeResultFiles(Archive * archive, int tScint, int fScint, int tfScint) {

	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	double refP_ms = bcPeriod_s * MILLISEC;
	unsigned nbin = archive->get_nbin();
	double tbin_ms = (refP_ms) / nbin;
	double glong, glat;

	FILE *file;

	AnglePair glgb = getCoord(archive).getGalactic();

	glgb.getDegrees(&glong, &glat);

	if (glong < 0)
		glong += 360;

	file = fopen("pdmp.posn", "at");

	if (file == NULL)
	{
	  perror ("pdmp: Failed to open file pdmp.posn for writing results");
	  exit (-1);
	}

    const MJD start = archive->get_Integration((int)floor(archive->get_nsubint()/(double)2))->get_start_time();

	fprintf(file, " %s\t%3.3f\t%3.3f\t%3.2f\t%3.5f\t%3.10f\t%3.10f\t%3.3f\t%3.3f\t%s\t%d\t%d\t%d\n",
		archive->get_source().c_str(),
		glong,
		glat,
		bestSNR,
		start.intday() + start.fracday(),
		bestPeriod_bc_us/MILLISEC,
		periodError_ms,
		bestDM,
		dmError,
		archive->get_filename().c_str(), tScint, fScint, tfScint);
	
	fclose(file);

	file = fopen("pdmp.per", "at");

	if (file != NULL) {
		fprintf(file, " %3.6f\t%3.10f\t%3.10f\t%3.3f\t%3.3f\t%3.3f\t%3.2f\t%s %d %d %d\n",
		start.intday() + start.fracday(),
		bestPeriod_bc_us/MILLISEC,
		periodError_ms,
		bestDM,
		dmError,
		bestPulseWidth * tbin_ms,
		bestSNR,
			archive->get_filename().c_str(),tScint,fScint,tfScint);
	} else {
		cerr << "pdmp: Failed to open file pdmp.per for writing results\n";
	}
	fclose(file);
}

void plotProfile(const Profile * profile, ProfilePlot* plot, TextInterface::Parser* flui) {

	cpgsch(0.7);
	cpgslw(1);

	flui->set_value("ch", "0.8");
	flui->set_value("x:view", "(0.05, 0.95)");
	flui->set_value("y:view", "(0.05, 0.205)");
	flui->set_value("x:range", "(0, 1.2)");
	plot->plot_profile(profile);
	double min, max;

	min = profile->min();
	max = profile->max();

	int div = 2;

	double tickSpace = cpgrnd((max-min)/(float)(NUM_VERT_PROFILE_TICKS-1), &div);

	cpgbox ("BINTS", 0.0, 0, "BNTSI", tickSpace, 0);
}

void readsum (string & filename, int * ndat, float*dms, float*snrs){

  vector<float> d;
  vector<float> s;

  FILE * f;
  f=fopen(filename.c_str(), "r");
  if (f==NULL){
    fprintf(stderr, "Could not open file %s\n", filename.c_str());
    *ndat=0;
    return;
  }

  char st[200];
  // skip two lines
  fgets(st,200,f);
  fgets(st,200,f);

  // now read in data

  *ndat=0;
  while (fgets(st,200,f)!='\0'){
    int count; float dm,ss;
    sscanf(st,"%d %f %f",&count,&dm,&ss);  
    d.push_back(dm);
    s.push_back(ss);
    //fprintf(stderr, "count = %d\n",count);
    //fprintf(stderr, "dm = %f\n",dm);
    (*ndat)++;
    //fprintf(stderr, "ndat = %d\n",*ndat);
  }
  fclose(f);

  //dms = new float[*ndat];

  //  dms = (float *) malloc(*ndat*sizeof(float));

  //snrs = new float[*ndat];

  // snrs = (float *) malloc(*ndat*sizeof(float));
  for (int i=0;i<*ndat;i++){
    dms[i]=d[i];
    snrs[i]=s[i];
  }
}

void getrange(int n, float * d, float * xmin, float * xmax){
  
  *xmin=d[0];
  for (int i=1;i<n;i++){
    if(d[i]<*xmin){
      *xmin=d[i];
    }
  }
  
  *xmax=d[0];
  for (int i=1;i<n;i++){
    if(d[i]>*xmax){
      *xmax=d[i];
    }
  }

  //fprintf(stderr," xmin = %f, xmax = %f\n", *xmin, *xmax);

}

void plotSNRdm (string & filename, double bestDM){
  int ndat;
  float dms[10000], snrs[10000];

  cout << "fn:" << filename << endl;

  readsum(filename, &ndat, dms, snrs);

  float xmin, xmax, ymin, ymax;
  getrange(ndat, dms, &xmin, &xmax);
  getrange(ndat, snrs, &ymin, &ymax);

  goToSNRdmViewPort();

  //cout << "ndat " << ndat << endl;

  //cpgswin(0.0,1.0,0.0,1.0);
  //  cpgswin((float)0.0-0.05*(xmax),(float)xmax*1.05,(float)0.0,(float)ymax*1.05);
  cpgswin((float)0.0-0.05*(xmax),(float)xmax*1.05,(float)ymin-0.1*(ymin),(float)ymax*1.05);
  cpgbox("BCNST",0.0,0,"BCNST",0.0,0);
  cpgsci(2);
  cpgmove((float) bestDM,0.0);
  cpgdraw((float) bestDM,xmax);
  cpgsci(1);
  cpglab("DM", "SNR", "");
  if (ndat>1) cpgline(ndat,dms,snrs);
  if (ndat==1) cpgpt(1,dms,snrs,17);

}

void plotPhaseFreq (const Archive * archive, 
		    Plot* phase_plot,
		    TextInterface::Parser* fui)
{
	Reference::To<Archive> phase_freq_copy = archive->clone();

	phase_freq_copy->pscrunch();
	phase_freq_copy->tscrunch();
	phase_freq_copy->remove_baseline();

	fui->set_value("x:view", "(0.55, 0.95)");
	fui->set_value("y:view", "(0.33, 0.56)");
	fui->set_value("x:range", "(0, 1.2)");
	fui->set_value("ch", "0.8");
	fui->set_value("above:c", "");
	fui->set_value("below:c", "");
	fui->set_value("y:alt", "0");
	fui->set_value("x:lab", "");
	fui->set_value("above:l", "Phase vs Frequency");
	fui->set_value("x:opt", "BCNTSI");
	fui->set_value("y:opt", "BNTIC");

	phase_plot->plot(phase_freq_copy);

	// Draw the line of best fit of the original
	drawBestFitPhaseFreq(phase_freq_copy);
	cpgsci(6);
	cpgslw(8);
}

void plotPhaseTime(const Archive * archive, Plot* plot, TextInterface::Parser* tui) {

	string y_scale = get_scale(archive);
	Reference::To<Archive> phase_time_copy = archive->clone();

    string yscale = "Elapsed Time (";
    yscale = yscale + y_scale;
    yscale = yscale + ")";

	phase_time_copy->fscrunch();
	phase_time_copy->pscrunch();
	phase_time_copy->remove_baseline();


	tui->set_value("x:view", "(0.05, 0.45)");
	tui->set_value("y:view", "(0.33, 0.56)");
	tui->set_value("x:range", "(0, 1.2)");
	tui->set_value("ch", "0.8");
	tui->set_value("above:c", "");
	tui->set_value("below:c", "");
	tui->set_value("y:alt", "0");
	tui->set_value("x:lab", "");
	tui->set_value("above:l", "Phase vs Time");
	tui->set_value("x:opt", "BCNTSI");
	tui->set_value("y:opt", "BNTIC");
	tui->set_value("y:lab", yscale);

	plot->plot(phase_time_copy);

	// Draw the line of best fit of the original
	drawBestFitPhaseTime(phase_time_copy);

	cpgsci(6);
	cpgslw(1);

	// reset the colour and size of the line back to its original
	cpgslw(1);
	cpgsci(1);
}

void drawBestFitPhaseFreq(const Archive * archive) {

	Reference::To<Profile> profile;
	unsigned nchan = archive->get_nchan();
	double dm = getDM(archive);
	double bw = archive->get_bandwidth();
	double bwband = fabs(bw/(double)nchan);
	double fspan = fabs((nchan) * bwband);
	double centre_freq = archive->get_centre_frequency();
	double deltaDM = bestDM - dm;
	vector<float> xpoints;
	vector<float> ypoints;
	double refPhase = 0.5;

	// Note that DMCONST is in milliseconds
	// Note that this deltaPhase refers to the midpoint of the
	// first channel to the midpoint of the second channel
	double deltaPhase = (deltaDM * DMCONST *
	                     (pow(centre_freq - 0.5*fspan + bwband/2, -2) -
											  pow(centre_freq + 0.5*fspan - bwband/2, -2) ) ) /
												(bestPeriod_bc_us/MILLISEC);
	double gradient;

	if (nchan == 1) {
		deltaPhase = 0;
		gradient = 0;
	} else {

		if (bw > 0) {
			gradient = -(deltaPhase / (nchan - 1));
		} else {
			gradient = deltaPhase / (nchan - 1);
		}
	}

	// get the deltaPhase for the entire length of the freq channels
	// This ensures a nice line going from the top of the phase-freq
	// plot to the bottom instead of midway between the top and bottom
	// channels.
	deltaPhase = gradient * nchan;

	// The first point is the centre of the first subint
	xpoints.push_back(refPhase);
	ypoints.push_back(FIRST_CHAN);

	// Second point is the point on the last subint
	// which is offseted by delta phase
	xpoints.push_back(refPhase + deltaPhase);
	ypoints.push_back(nchan);

	cpgswin(0,1.2,FIRST_CHAN,nchan);
	if (verbose) {
		printf("Drawing line of best fit for phase frequency with coords: (%3.10g, %3.10g) to (%3.10g, %3.10g)\n",
		xpoints[0], ypoints[0], xpoints[1], ypoints[1]);
	}

	cpgsci(bestFitLineColour);
	cpgslw(BEST_FIT_LINE_WIDTH);
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);

	// reset the colour and width back to original
	cpgsci(1);
	cpgslw(1);

}

void drawBestFitPhaseTime(const Archive * archive) {

	string scale = get_scale(archive);
	int divisor = 1;

	if (scale == "days")
		divisor = 60 * 60 * 24;
	else if (scale == "hours")
		divisor = 60 * 60;
	else if (scale == "minutes")
		divisor = 60;

	Reference::To<Profile> profile;

	double intLength = (archive->end_time() - archive->start_time()).in_seconds();
	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	double p = bcPeriod_s*MICROSEC;
	double deltaP;

	if (join) {
		deltaP = bestPeriod_bc_us - p * MICROSEC;
	} else {
		deltaP = bestPeriod_bc_us - p;
	}

	double tspan = archive->integration_length();
	double tsub = archive->get_Integration(0)->get_duration();
	unsigned nsub;

	if (join) {
		nsub = (unsigned)(tspan / tsub);
	} else {
		nsub = archive->get_nsubint();
	}

	vector<float> xpoints;
	vector<float> ypoints;
	double refPhase = 0.5;

	// This equation is derived from the property
	// deltaP / P = deltaT / T
	// Note that this deltaPhase refers to the midpoint of the
	// first subint to the midpoint of the second subint

	double deltaPhase = ((nsub-1) * tsub * (deltaP / MICROSEC)) / pow((bestPeriod_bc_us/MICROSEC), 2);
	double gradient;

	if (nsub == 1) {
		deltaPhase = 0;
		gradient = 0;

	} else {

		if (join) {
			gradient = (bestPeriod_bc_us - bcPeriod_s * MICROSEC) / MILLISEC;
		} else {
			gradient = deltaPhase / (nsub - 1);
		}
	}

	// get the deltaPhase for the entire length of the subints
	// This draws a nice line from top to bottom, instead of midway
	// through the first and last subints

	deltaPhase = gradient * nsub;

	if (verbose)
		printf("deltaPhase = %3.10g, deltaP = %3.10g, bestPeriod_bc_us = %3.10g\n", deltaPhase, deltaP, bestPeriod_bc_us);

	// The first point is the centre of the first subint
	xpoints.push_back(refPhase);
	ypoints.push_back(FIRST_SUBINT);

	// Second point is the point on the last subint
	// which is offseted by delta phase
	xpoints.push_back(refPhase + deltaPhase);
	ypoints.push_back(intLength / divisor);

	if (verbose) {
		printf("Drawing line of best fit for phase time with coords: (%3.10g, %3.10g) to (%3.10g, %3.10g)\n",
		xpoints[0], ypoints[0], xpoints[1], ypoints[1]);
	}

	cpgsci(bestFitLineColour);
	cpgslw(BEST_FIT_LINE_WIDTH);
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);

	// reset the colour and width back to original
	cpgsci(1);
	cpgslw(1);
}

void drawBestValuesCrossHair( const Archive * archive,
     double periodOffset_us, double periodStep_us, double periodHalfRange_us,
     double dmOffset, double dmStep, double dmHalfRange) {

	goToDMPViewPort();
	cpgsci(bestFitLineColour);
	cpgslw(1);

	// In microsec
	double bcPeriod_us = (getPeriod(archive) / dopplerFactor) * MICROSEC;

	vector<float> xpoints;
	vector<float> ypoints;

	double minDM = getDM(archive) + dmOffset -	dmHalfRange;
	double maxDM = getDM(archive) + dmOffset +	dmHalfRange;

	// Get the initial minimum search period as a starting point. The following
	// way of getting minP ensures that the reference P is used to calculate
	// SNR as, in some cases, the optimal SNR is found using the ref P and DM
	double minP = -periodHalfRange_us+ periodOffset_us;
	double maxP = periodHalfRange_us + periodOffset_us;

	cpgswin(minP,maxP,minDM,maxDM);

	// Draw vertical line
	xpoints.push_back((bestPeriod_bc_us - bcPeriod_us));
	ypoints.push_back(minDM);

	xpoints.push_back((bestPeriod_bc_us - bcPeriod_us));
	ypoints.push_back(maxDM);

	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);

	xpoints.clear();
	ypoints.clear();

	// Draw vertical line
	xpoints.push_back(minP);
	ypoints.push_back(bestDM);

	xpoints.push_back(maxP);
	ypoints.push_back(bestDM);

	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);

	// reset the colour back to original
	cpgsci(1);
}

void reorder(Reference::To<Archive> archive)
{
	Reference::To<TimeSortedOrder> tso = new TimeSortedOrder;
	tso->organise(archive,0);
}

void draw_colour_map_only(const int rows, const int columns, const double minx,
    const double maxx, const double miny, const double maxy, const float* trf)
{
  if (columns <= 0 || rows <= 0) {
    throw Error(InvalidRange, "draw_colour_map_only",
        "columns=%d rows=%d", columns, rows);
  }

  if (cpgopen(plot_device.c_str()) < 0) {
    cerr << "Error: Could not open plot device" << endl;
    exit(-1);
  }

  cpgslw(2);    // line width
  cpgsch(1.2);  // character height
  cpgscf(2);    // Roman font
	cpgsci(1);    // colour index 

	pgplot::ColourMap cmap(pgplot::ColourMap::Heat);
	cmap.apply();

	cpgswin(minx, maxx, miny, maxy);

	float min = 0, max = 0;

  if (onlyDisplayDmPlotMin != 0.0 || onlyDisplayDmPlotMax != 0.0) {
    min = onlyDisplayDmPlotMin;
    max = onlyDisplayDmPlotMax;
  } else {
    minmaxval(columns*rows, &SNRs[0], &min, &max);
  }

  cout << "Plotting DM vs period with min=" << min << " max=" << max << endl;

  cpgimag(&SNRs[0], columns, rows, 1, columns, 1, rows, max, min, trf);

  if (drawContourMap) {
    vector<float> contourLevels;
    float contourLevel = contourFirstLevel;

    for (unsigned i = 0; i < numContourLevels; ++i) {
      contourLevels.push_back(contourLevel);
      contourLevel += contourLevelStep;
    }

    cpgcont(&SNRs[0], columns, rows, 1, columns,
        1, rows, &contourLevels[0], numContourLevels, trf);
  }

  cpglab("Delta P (microsec)", "DM (cm\\u-3\\d pc)", "");
  cpgbox("BCNTS", 0.0, 5, "BCNTS", 0.0, 2);

  cpgend();
  exit(0);
}

void draw_colour_map (float *plotarray, int rows, int cols, double minx,
		double maxx, const string& xlabel, double miny, double maxy, const string& ylabel,
		const string& title, float * trf, int numVertTicks) {

	pgplot::ColourMap::Name colour_map = pgplot::ColourMap::Heat;
	colour_map = pgplot::ColourMap::Heat;

	pgplot::ColourMap cmap;
	cmap.set_name (colour_map);
	cmap.apply();

	float min=0, max=0;

	// cerr << "cols=" << cols << " rows=" << rows << endl;
        assert (cols > 0 && rows > 0);

	minmaxval (cols*rows, plotarray, &min, &max);

	cpgslw(1);
	cpgsci(1);

	cpgsch(PLOT_CHAR_HEIGHT);
	cpgscf(1);

	cpgswin(minx,maxx,miny,maxy);

	cpgimag(plotarray, cols, rows, 1, cols,
                1, rows, min, max, trf);

    int div = 5;
    double tickSpace = cpgrnd((maxy-miny)/(float)(numVertTicks-1), &div);

    cpgbox("BCINTS", 0.0, 5, "BCINTS", tickSpace, 2);

	cpglab(xlabel.c_str(), ylabel.c_str(), title.c_str());
}

void minmaxval (int n, const float *thearray, float *min, float *max)
{
	int i = 0;
	double themax = thearray[0];
	double themin = thearray[0];

	for(i = 0; i < n; i++) {
		if(thearray[i] > themax)
			themax = thearray[i];
		else if(thearray[i] < themin)
			themin = thearray[i];
    }

	*max = themax;
	*min = themin;
}

string get_scale(const Archive * archive)
{
    double range = (archive->end_time() - archive->start_time()).in_days();
    const float mjd_hours = 1.0 / 24.0;
    const float mjd_minutes = mjd_hours / 60.0;

    if (range > 1.0)
        return "days";
    else if (range > mjd_hours)
        return "hours";
    else if (range > mjd_minutes)
        return "minutes";
    else
        return "seconds";
}

void setSensibleStepSizes(const Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();

  if (dmStep < 0)
    dmStep = getNaturalDMStep(archive, dmHalfRange);

  if (dmHalfRange < 0)
    dmHalfRange = getNaturalDMHalfRange(archive, dmStep);
  
  if (periodStep_us < 0)
    periodStep_us = getNaturalPeriodStep(archive, periodHalfRange_us);
  
  if (periodHalfRange_us < 0)
    periodHalfRange_us = getNaturalperiodHalfRange(archive, periodStep_us);

  // Make sure that the steps are sensible. Otherwise, just do one step
  if (nchan == 1 || dmStep <= 0)
    dmHalfRange = dmStep/2;
  
  if (nsub == 1 || periodStep_us <= 0)
    periodHalfRange_us = periodStep_us/2;
}

void setCentreMjd(const Archive* archive)
{
  const double mid_length = archive->integration_length() / 2;
  const unsigned nsubint = archive->get_nsubint();
  double accumulated_length = 0.0;

  for (unsigned isub = 0; isub < nsubint; ++isub) {
    const double duration = archive->get_Integration(isub)->get_duration();
    if (accumulated_length + duration > mid_length) {
      tmjdctr = archive->get_Integration(isub)->get_start_time().in_seconds() +
        mid_length - accumulated_length;
      return;
    } else {
      accumulated_length += duration;
    }
  }
}


#ifdef HAVE_PSRXML
void normaliseXmlSubints(float* amps,unsigned int nbin){
	if (normaliseSubints){
		float max=-FLT_MAX;
		float min=FLT_MAX;
		for (int i = 0; i < nbin; i++){
			max = (amps[i] > max) ? amps[i] : max;
			min = (amps[i] < min) ? amps[i] : min;
		}

		for (int i = 0; i < nbin; i++){
			amps[i] = (amps[i] - min) / (max-min);
		}
	}
}
void setInitialXmlCandiateSection(const Archive * archive, float minwidthsecs){
	Reference::To<Archive> total = archive->total();
//	total->remove_baseline();
	const unsigned nchan = archive->get_nchan();
	const unsigned nsubint = archive->get_nsubint();
	const unsigned nbin = total->get_nbin();




	// initialise the section if there is no initial section
	if (xml_candidate->nsections == 0){
		xml_candidate->nsections=1;
		xml_candidate->sections=(phcx_section*)malloc(sizeof(phcx_section));

		xml_candidate->sections[0].subints=NULL;
		xml_candidate->sections[0].subbands=NULL;
		xml_candidate->sections[0].pulseProfile=NULL;
		xml_candidate->sections[0].snrBlock.block=NULL;
		xml_candidate->sections[0].nextrakey=0;
		xml_candidate->sections[0].tsamp=0;


		// insert the initial parameters
		// @todo: How to get the initial params?
		strcpy(xml_candidate->header.sourceID,archive->get_source().c_str());

		if (strcmp(archive->get_telescope().c_str(),"PKS")==0){
			strcpy(xml_candidate->header.telescope,"PARKES");
		} else {
			strcpy(xml_candidate->header.telescope,archive->get_telescope().c_str());
		}
		xml_candidate->header.centreFreq=archive->get_centre_frequency();
		xml_candidate->header.bandwidth=archive->get_bandwidth();
		MJD start = archive->get_Integration(0)->get_start_time();
		xml_candidate->header.mjdStart=(start.intday()+ start.fracday());
		xml_candidate->header.observationLength= (archive->end_time() - archive->start_time()).in_seconds();

		sky_coord coord = getCoord (archive);
		Angle ra_angle = coord.ra();
		Angle dec_angle = coord.dec();

		xml_candidate->header.ra=ra_angle.getDegrees();
		xml_candidate->header.dec=dec_angle.getDegrees();


		xml_candidate->sections[0].name = (char*)malloc(sizeof(char)*80);
		strcpy(xml_candidate->sections[0].name,"User Defined");
		xml_candidate->sections[0].bestWidth=-1;
		float rms = getRMS(archive);
		// rms = rms / sqrt(float(nchan*nsubint));

		int minwidthbins = (int) (minwidthsecs / getPeriod(archive) * nbin);

		if (minwidthbins<1) minwidthbins=1;
		if (minwidthbins>nbin/2) minwidthbins=nbin/2;


		xml_candidate->sections[0].bestSnr=getSNR(total->get_Profile(0,0,0),rms,minwidthbins);
		float period=getPeriod(archive);
		xml_candidate->sections[0].bestTopoPeriod=period;
		xml_candidate->sections[0].bestBaryPeriod=period/dopplerFactor;

		xml_candidate->sections[0].bestDm=getDM(archive);
	}

	phcx_section* section = xml_candidate->sections; // the first section

	if ((section->pulseProfile != NULL) || (section->subbands != NULL) || (section->subints != NULL)){
		// we already have an initial section filled out, so continue without doing anything.
	} else {

		// we put in initial values

		strcpy(xml_candidate->header.sourceID,archive->get_source().c_str());

		if (strcmp(archive->get_telescope().c_str(),"PKS")==0){
			strcpy(xml_candidate->header.telescope,"PARKES");
		} else {
			strcpy(xml_candidate->header.telescope,archive->get_telescope().c_str());
		}
		xml_candidate->header.centreFreq=archive->get_centre_frequency();
		xml_candidate->header.bandwidth=archive->get_bandwidth();
		MJD start = archive->get_Integration(0)->get_start_time();
		xml_candidate->header.mjdStart=(start.intday()+ start.fracday());
		xml_candidate->header.observationLength= (archive->end_time() - archive->start_time()).in_seconds();

		sky_coord coord = getCoord (archive);
		Angle ra_angle = coord.ra();
		Angle dec_angle = coord.dec();

		xml_candidate->header.ra=ra_angle.getDegrees();
		xml_candidate->header.dec=dec_angle.getDegrees();


		// we will memcpy for now, since I am not sure what happens to the 'archives'
		{
			float* amps = total->get_Profile(0,0,0)->get_amps();
			// the profile
			section->nbins=nbin;
			section->pulseProfile = (float*)malloc(sizeof(float)*nbin);
			memcpy(section->pulseProfile,amps,sizeof(float)*nbin);
		}
		{
			// make the freq/phase
			Reference::To<Archive> phase_freq_copy = archive->clone();
			phase_freq_copy->pscrunch();
			phase_freq_copy->tscrunch();
			phase_freq_copy->dedisperse();
			phase_freq_copy->remove_baseline();

			Integration* subint = phase_freq_copy->get_Integration(0);
			section->nsubbands=nchan;
			section->subbands = (float**)malloc(sizeof(float*)*nchan);
			for (unsigned ichan=0; ichan < nchan; ichan++)
			{
				const unsigned ipol = 0; // total intensity

				float* amps = subint->get_Profile( ipol, ichan )->get_amps();
				section->subbands[ichan] = (float*)malloc(sizeof(float)*nbin);
				memcpy(section->subbands[ichan],amps,sizeof(float)*nbin);

				/* amps points to an array of nbin phase bins */
			}
		}
		{
			//make the time/phase
			Reference::To<Archive> phase_time_copy = archive->clone();

			phase_time_copy->fscrunch();
			phase_time_copy->pscrunch();
		//	phase_time_copy->remove_baseline();
			phase_time_copy->dedisperse();



			section->nsubints=nsubint;
			section->subints = (float**)malloc(sizeof(float*)*nsubint);

			for (unsigned isub=0; isub < nsubint; isub++)
			{
				Integration* subint = phase_time_copy->get_Integration (isub);

				const unsigned ipol = 0;  // total intensity
				const unsigned ichan = 0; // fscrunched

				float* amps = subint->get_Profile( ipol, ichan )->get_amps();

				
				normaliseXmlSubints(amps,nbin);

				section->subints[isub] = (float*)malloc(sizeof(float)*nbin);
				memcpy(section->subints[isub],amps,sizeof(float)*nbin);

				/* amps points to an array of nbin phase bins */
			}

		}
	}

}
void addOptimisedXmlCandidateSection(const Archive * archive,double centrePeriod,double centreDm,double centreAccn, double centreJerk, float minwidthsecs){

	 Reference::To<Archive> total = archive->total();
//	 total->remove_baseline();
	 const unsigned nchan = archive->get_nchan();
	 const unsigned nsubint = archive->get_nsubint();
	 const unsigned nbin = total->get_nbin();

	xml_candidate->nsections++;
	xml_candidate->sections = (phcx_section*)realloc(xml_candidate->sections,sizeof(phcx_section)*xml_candidate->nsections);
	phcx_section* section = xml_candidate->sections+xml_candidate->nsections-1;

	section->subints=NULL;
	section->subbands=NULL;
	section->pulseProfile=NULL;
	section->snrBlock.block=NULL;
	section->tsamp=xml_candidate->sections[0].tsamp;

	// insert the optimised parameters
	
	section->name = (char*)malloc(sizeof(char)*strlen(xml_candidate->sections[xml_candidate->nsections-2].name)+20);
	sprintf(section->name,"%s-pdmpd",xml_candidate->sections[xml_candidate->nsections-2].name);
	section->bestWidth=bestPulseWidth/(float)nbin;
	float rms = getRMS(archive);

 	int minwidthbins = (int) (minwidthsecs / getPeriod(archive) * nbin);

	if (minwidthbins<1) minwidthbins=1;
	if (minwidthbins>nbin/2) minwidthbins=nbin/2;


	section->bestSnr=getSNR(total->get_Profile(0,0,0),rms,minwidthbins);
	section->bestTopoPeriod=bestPeriod_bc_us/1000000.0*dopplerFactor;
	section->bestBaryPeriod=bestPeriod_bc_us/1000000.0;
	section->bestDm=bestDM;
	section->bestAccn=0;
	section->bestJerk=0;
	section->nextrakey=0;

	{
		float* amps = total->get_Profile(0,0,0)->get_amps();
		// the profile
		section->nbins=nbin;
		section->pulseProfile = (float*)malloc(sizeof(float)*nbin);
		memcpy(section->pulseProfile,amps,sizeof(float)*nbin);
	}
	{
		// make the freq/phase
		Reference::To<Archive> phase_freq_copy = archive->clone();
		phase_freq_copy->pscrunch();
		phase_freq_copy->tscrunch();
		phase_freq_copy->dedisperse();
		phase_freq_copy->remove_baseline();

		Integration* subint = phase_freq_copy->get_Integration(0);
		section->nsubbands=nchan;
		section->subbands = (float**)malloc(sizeof(float*)*nchan);
		for (unsigned ichan=0; ichan < nchan; ichan++)
		{
			const unsigned ipol = 0; // total intensity

			float* amps = subint->get_Profile( ipol, ichan )->get_amps();
			section->subbands[ichan] = (float*)malloc(sizeof(float)*nbin);
			memcpy(section->subbands[ichan],amps,sizeof(float)*nbin);

			/* amps points to an array of nbin phase bins */
		}
	}
	{
		//make the time/phase
		Reference::To<Archive> phase_time_copy = archive->clone();

		phase_time_copy->fscrunch();
		phase_time_copy->pscrunch();
		//phase_time_copy->remove_baseline();
		phase_time_copy->dedisperse();



		section->nsubints=nsubint;
		section->subints = (float**)malloc(sizeof(float*)*nsubint);

		for (unsigned isub=0; isub < nsubint; isub++)
		{
			Integration* subint = phase_time_copy->get_Integration (isub);

			const unsigned ipol = 0;  // total intensity
			const unsigned ichan = 0; // fscrunched

			float* amps = subint->get_Profile( ipol, ichan )->get_amps();

			normaliseXmlSubints(amps,nbin);

			section->subints[isub] = (float*)malloc(sizeof(float)*nbin);
			memcpy(section->subints[isub],amps,sizeof(float)*nbin);

			/* amps points to an array of nbin phase bins */
		}

	}
	{
		//make the 'SNR block'
		int dmBins, periodBins;

		// Number of bins in the DM axis
		dmBins = (int)ceil( ( fabs(dmHalfRange)*2 ) / dmStep);

		// Number of bins in the Period axis
		periodBins = (int)ceil( ( fabs(periodHalfRange_us)*2 ) / periodStep_us) + 1;

		section->snrBlock.periodIndex = (double*)malloc(sizeof(double)*periodBins);
		section->snrBlock.nperiod=periodBins;
		section->snrBlock.dmIndex = (double*)malloc(sizeof(double)*dmBins);
		section->snrBlock.ndm = dmBins;
		section->snrBlock.accnIndex = (double*)malloc(sizeof(double));
		section->snrBlock.naccn=1;
		section->snrBlock.jerkIndex = (double*)malloc(sizeof(double));
		section->snrBlock.njerk=1;

		double periodTrial = centrePeriod-periodHalfRange_us;
		for(int i = 0; i < periodBins; i++){
			section->snrBlock.periodIndex[i] = periodTrial*1000000;
			periodTrial += periodStep_us;
		}

		double dmTrial = centreDm-dmHalfRange;
                for(int i = 0; i < dmBins; i++){
                        section->snrBlock.dmIndex[i] = dmTrial;
                        dmTrial += dmStep;
                }

		section->snrBlock.accnIndex[0]=centreAccn;
                section->snrBlock.jerkIndex[0]=centreJerk;

		int i=0;
		section->snrBlock.block=(double****)malloc(sizeof(double***)*section->snrBlock.ndm);
		for(int d=0; d < section->snrBlock.ndm; d++){
			section->snrBlock.block[d]=(double***)malloc(sizeof(double**)*section->snrBlock.nperiod);
			for(int p=0; p < section->snrBlock.nperiod; p++){
				section->snrBlock.block[d][p]=(double**)malloc(sizeof(double*)*section->snrBlock.naccn);
				for(int a=0; a < section->snrBlock.naccn; a++){
					section->snrBlock.block[d][p][a]=(double*)malloc(sizeof(double)*section->snrBlock.njerk);
					for(int j=0; j < section->snrBlock.njerk; j++){
						section->snrBlock.block[d][p][a][j]=SNRs[i];
						i++;
					}
				}
			}
		}
	}

}




#endif

