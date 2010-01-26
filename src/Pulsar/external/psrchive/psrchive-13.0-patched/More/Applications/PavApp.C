/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "PavApp.h"
#include <tostring.h>
#include <templates.h>
#include <limits>
#include <Pulsar/StokesCylindrical.h>
#include <Pulsar/BandpassChannelWeightPlot.h>
#include <Pulsar/StokesSpherical.h>
#include <Pulsar/PhaseVsTime.h>
#include <Pulsar/PhaseVsFrequency.h>
#include <Pulsar/DynamicSNSpectrumPlot.h>
#include <Pulsar/ProfilePlot.h>
#include <Pulsar/PosAngPlot.h>
#include <Pulsar/PeakConsecutive.h>
#include <Pulsar/PlotFactory.h>
#include <Pulsar/PhaseVsPlot.h>
#include <Pulsar/BandpassPlot.h>
#include <Pulsar/Passband.h>

#define BANDPASSPLOT_ID "d"
#define SPECTRUMPLOT_ID "b"

using std::ios;
using std::cerr;
using std::cout;
using std::endl;
using std::ostringstream;
using Pulsar::PhasePlot;
using Pulsar::MultiPlot;
using Pulsar::StokesCylindrical;
using Pulsar::BandpassChannelWeightPlot;
using Pulsar::PhaseVsPlot;
using Pulsar::StokesSpherical;
using Pulsar::PhaseVsTime;
using Pulsar::PhaseVsFrequency;
using Pulsar::DynamicSNSpectrumPlot;
using Pulsar::ProfilePlot;
using Pulsar::PosAngPlot;
using Pulsar::PeakConsecutive;
using Pulsar::MultiPhase;
using Pulsar::PlotFactory;
using Pulsar::FluxPlot;
using Pulsar::Interpreter;
using Pulsar::RiseFall;
using Pulsar::Profile;
using Pulsar::Integration;
using Pulsar::BandpassPlot;
using Pulsar::Passband;

Pulsar::Option<string> PavApp::default_plot_device
(
  "PlotDevice::name", "/xs",
  "The name of the pgplot device to use for output",
  "The name of the plot device to got to pgopen if none is given by the user\n"
  "on the command line."
);




/**
 * Constructor
 *
 *  DOES     - Inititialization
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

PavApp::PavApp()
{
  have_colour = false;
  ipol = 0;
  fsub = 0, lsub = 0;
  isubint = 0;
  ichan = 0;
  rot_phase = 0.0;
  svp = true;
  publn = false;
  axes = true;
  labels = true;
  n1 = 1;
  n2 = 1;
  min_freq = 0.0;
  max_freq = 0.0;
  min_phase = 0.0;
  max_phase = 1.0;
  reference_position_angle = 0.0;
  position_angle_height = 0.25;
  border = 0.1;
  dark = false;
  y_max = 1.0;
  y_min = 0.0;
  truncate_amp = 0.0;
  colour_map = pgplot::ColourMap::Heat;
  centre_profile = false;

  plot_error_box = false;

  freq_under_name = false;

  user_character_height = -1;

  cbppo = false;
  cbpao = false;
  cblpo = false;
  cblao = false;

  pa_min = 0;
  pa_max = 1;

  pa_ext = false;

  label_degrees = false;

  ronsub = 0;
  remove_baseline = true;
}



/**
 * PrintUsage
 *
 *  DOES     - Display the help text to stdout
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::PrintUsage( void )
{
  cout << "Preprocessing options:" << endl;
  cout << " -b scr    Bscrunch scr phase bins together" << endl;
  cout << " -C        Centre the profiles on phase zero" << endl;
  cout << " -d        Dedisperse all channels" << endl;
  cout << " -r phase  Rotate all profiles by phase (in turns)" << endl;
  cout << " -f scr    Fscrunch scr frequency channels together" << endl;
  cout << " -F        Fscrunch all frequency channels" << endl;
  cout << " -t src    Tscrunch scr Integrations together" << endl;
  cout << " -T        Tscrunch all Integrations" << endl;
  cout << " -p        Add all polarisations together" << endl;
  cout << " -x  max   Zoom into the data between 0 and max (normalized coords)" << endl;
  cout << " -c        Do not remove baseline" << endl;
  cout << endl;
  cout << "Configuration options:" << endl;
  cout << " -g dev    Manually specify a plot device" << endl;
  cout << " -M meta   Read a meta-file containing the files to use" << endl;
  cout << endl;
  cout << "Selection options:" << endl;
  cout << " -z x1,x2  Zoom to this pulse phase range (in turns)" << endl;
  cout << " -k f1,f2  Zoom to this frequency range" << endl;
  cout << " -y s1,s2  Zoom to this subint range" << endl;
  cout << " -l a1,a2  Zoom to this position angle range (Degrees)" << endl;
  cout << " -N x,y    Divide the window into x by y panels" << endl;
  cout << " -H chan   Select which frequency channel to display" << endl;
  cout << " -P pol    Select which polarization to display" << endl;
  cout << " -I subint Select which sub-integration to display" << endl;
  cout << endl;
  cout << "Plotting options:" << endl;
  cout << " -B        Display off-pulse bandpass & channel weights" << endl;
  cout << " -D        Plot a single profile (chan 0, poln 0, subint 0 by default)" << endl;
  cout << " -G        Plot an image of amplitude against frequency & phase" << endl;
  cout << " -K        Plot Digitiser Statistics" << endl;
  cout << " -j        Display a simple dynamic spectrum image" << endl;
  cout << " -m        Plot Poincare vector in spherical coordinates" << endl;
  cout << " -n        Plot S/N against frequency" << endl;
  cout << " -R        Plot stacked sub-integration profiles" << endl;
  cout << " -S        Plot polarization parameters (I,L,V,PA)" << endl;
  cout << " -A        Plot Digitiser Counts histogram" << endl;
  cout << " -X        Plot cal amplitude and phase vs frequency channel" << endl;
  cout << " -Y        Plot colour map of sub-integrations against pulse phase" << endl;
  cout << endl;
  cout << "Other plotting options:" << endl;
  cout << " --sl           Stack labels(name,freq) on the left side of the plot" << endl;
  cout << " --publn        Publication quality plot (B&W)  L dashed, V dots" << endl;
  cout << " --publnc       Publication quality plot (keep in colour if device supports it)" << endl;
  cout << " --ch           Set the character height" << endl;
  cout << " --cmap index   Select a colour map for PGIMAG style plots" << endl;
  cout << "                The available indices are: (maybe 4-6 not needed)" << endl;
  cout << "                  0 -> Greyscale" << endl;
  cout << "                  1 -> Inverse Greyscale" << endl;
  cout << "                  2 -> Heat" << endl;
  cout << "                  3 -> Cold" << endl;
  cout << "                  4 -> Plasma" << endl;
  cout << "                  5 -> Forest" << endl;
  cout << "                  6 -> Alien Glow" << endl;
  cout << "                  7 -> Test" << endl;
  cout << endl;
  cout << " --plot_qu      Plot Stokes Q and Stokes U in '-S' option instead of degree of linear" << endl;
  cout << " --ld           Label phase axes in degrees" << endl;
  cout << " --ebox         Plot an error box, for -D and -S " << endl;
  cout << " --pa_ext       Plot position angle plot from -90 deg to 180 deg, instead of -90,90" << endl;
  cout << " --ha           Plot versus Hour Angle in '-Y' mode" << endl;
  cout << endl;
  cout << "Integration re-ordering (nsub = final # of subints) (used with pav Y for binary pulsars):" << endl;
  cout << " --convert_binphsperi   nsub" << endl;
  cout << " --convert_binphsasc    nsub" << endl;
  cout << " --convert_binlngperi   nsub" << endl;
  cout << " --convert_binlngasc    nsub" << endl;
  cout << endl;
  cout << "Utility options:" << endl;
  cout << " -a              Print available plugin information" << endl;
  cout << " -h              Display this useful help page" << endl;
  cout << " -i              Show revision information" << endl;
  cout << " -v              Verbose output" << endl;
  cout << " -V              Very verbose output" << endl;
  cout << endl << endl;
}



/**
 * PADegreesToTurns
 *
 *   DOES     - Converts a value in degrees to the turns value needed for angle plots (0 = -60, 1 = 90)
 *   RECEIVES - The angle in degrees
 *   RETURNS  - The angle in turns
 *   THROWS   - Nothing
 *   TODO     - Nothing
 **/

float PavApp::PADegreesToTurns( const int deg )
{
  return ((float)deg / 180.0) + 0.5;
}




/**
 * SetPhaseZoom
 *
 *   DOES     - creates a command string that sets the x zoom in normalised coords.
 *   RECEIVES - Nothing
 *   RETURNS  - Nothing
 *   THROWS   - Nothing
 *   TODO     - Nothing
 **/

void PavApp::SetPhaseZoom( double min_phase, double max_phase  )
{
  string range_str = string("(") +
                     tostring< double >(min_phase ) +
                     string(",") +
                     tostring< double >( max_phase ) +
                     string( ")");

  // Now set the range for all the plots that have phase on the x axis.

  SetPlotOptions<PhasePlot>( string("x:range=") + range_str );
  SetPlotOptions<MultiPhase>( string("x:range=") + range_str );
}



/**
 * SetFreqZoom
 *
 *  DOES     - creates a command string that sets the y zoom in normalised coords based
 *             on a target frequency range in MHz.
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::SetFreqZoom( double min_freq, double max_freq )
{
  for( unsigned i = 0; i < plots.size(); i ++ )
  {
    Reference::To<Archive> archive = plots[i].archive;

    // get the centre frequency and bandwidth

    double ctr_freq = archive->get_centre_frequency();
    double bw = archive->get_bandwidth();
    double bw_abs = bw;

    if( bw < 0 )
    {
      bw_abs = 0 - bw;
    }

    double actual_min = ctr_freq - bw_abs / 2.0;

    // convert the min and max freq values to (0-1)
    double norm_min_freq = (min_freq-actual_min)/bw_abs;
    double norm_max_freq = (max_freq-actual_min)/bw_abs;

    if( bw < 0 )
    {
      double tmp = 1 - norm_min_freq;
      norm_min_freq = 1 - norm_max_freq;
      norm_max_freq = tmp;
    }

    // construct the command to set the range, the same for all plots (for current plots)

    string range_cmd = "y:range=(";
    range_cmd += tostring<double>( norm_min_freq );
    range_cmd += string(",");
    range_cmd += tostring<double>( norm_max_freq );
    range_cmd += string(")" );

    SetPlotOptions<Plot>( range_cmd );
  }
}


/**
 * PavSpecificLabels
 *
 *  DOES     - Calculates integration duration, SNR and after the jobs have 
 *             been executed. Displays the appropriate frequency depending on
 *             whether the archive has been dedispersed.
 *  RECEIVES - Archive
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Display the decimal points for the duration and SNR.
 **/

void PavApp::PavSpecificLabels( Pulsar::Archive* archive)
{
  Reference::To<Integration> integ = archive->get_Integration(0);

  // fully scrunch archive to get the correct SNR
  Reference::To<Archive> copy = archive->clone();
  copy->tscrunch();
  copy->fscrunch();
  copy->pscrunch();

  string snr = tostring( copy->get_Profile(0,0,0)->snr() );
  string frequency;

  /*
     If the archive has been dedispsersed, then the effective centre
     frequency associated with pulse phase is that of the header, not
     the centre frequency stored in the profile.
  */

  if ( archive->get_dedispersed() )
    frequency = tostring( archive->get_centre_frequency() );
  else
    frequency = tostring( integ->weighted_frequency(0, archive->get_nchan()) );

  const string duration = tostring<double>( archive->integration_length() );

  SetPlotOptions<Plot>( "above:c=$name $file\n Freq: " + frequency +
          " MHz BW: $bw Length: " + duration + " S/N: " + snr );
}

/**
 * PavSpecificOptions
 *
 *  DOES     - Sets all the options that are only for pav, ie blanks out labels that we don't want to see here
 *             but still want them present in psrplot and other programs.
 *  RECEIVES - Nothing
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::PavSpecificOptions( void )
{
  // There are a few things we can set for publication regardless of what we are plotting.

  if( publn )
  {
    tostring_precision = 1;
    SetPlotOptions<Plot>( "ch=1.2" );
    if( freq_under_name )
      SetPlotOptions<Plot>( "below:l=$name.$freq MHz" );
    else
    {
      SetPlotOptions<Plot>( "below:l=$name" );
      SetPlotOptions<Plot>( "below:r=$freq MHz" );
    }
  }
  else
  {
    SetPlotOptions<Plot>( "above:c=$name $file\n Freq: $freq MHz BW: $bw Length: $length S/N: $snr" );
    SetPlotOptions<Plot>( "below:l=" );
  }

  // bandpass channel weights plot config.

  if( !publn )
  {
    SetPlotOptions<BandpassChannelWeightPlot>( "band:below:l=" );
    SetPlotOptions<BandpassChannelWeightPlot>( "band:below:r=" );
    SetPlotOptions<BandpassChannelWeightPlot>( "above:c=$name $file\n Freq: $freq MHz BW: $bw Length: $length Rcvr: $rcvr:name" );
  }
  else
  {
    SetPlotOptions<BandpassChannelWeightPlot>( "band:below:l=$name" );
    SetPlotOptions<BandpassChannelWeightPlot>( "band:below:r=$freq MHz" );
    SetPlotOptions<BandpassChannelWeightPlot>( "band:above:c=" );
  }

  // stokes cylindrical plot config

  SetPlotOptions<StokesCylindrical>( "pa:below:l=" );
  SetPlotOptions<StokesCylindrical>( "flux:below:l=" );
  SetPlotOptions<StokesCylindrical>( "flux:y:buf=0.07" );
  SetPlotOptions<StokesCylindrical>( "pa:mark=dot+tick" );
  if( !publn )
  {
    SetPlotOptions<StokesCylindrical>( "pa:above:c=$name $file\n Freq: $freq MHz BW: $bw Length: $length S/N: $snr" );
  }
  else
  {
    SetPlotOptions<StokesCylindrical>( "pa:above:c=" );
    if( freq_under_name )
      SetPlotOptions<StokesCylindrical>( "flux:below:l=$name.$freq MHz" );
    else
    {

      SetPlotOptions<StokesCylindrical>( "flux:below:l=$name" );
      SetPlotOptions<StokesCylindrical>( "flux:below:r=$freq MHz" );
    }
  }

  // PhasVsTime plot config

  SetPlotOptions<PhaseVsTime>( "below:l=" );
  SetPlotOptions<PhaseVsTime>( "below:r=" );
  if( publn )
  {
    SetPlotOptions<PhaseVsTime>( "above:l=$name" );
    SetPlotOptions<PhaseVsTime>( "above:r=$freq MHz" );
  }

  // PhaseVsFrequency plot config

  SetPlotOptions<PhaseVsFrequency>( "below:l=" );
  SetPlotOptions<PhaseVsFrequency>( "below:r=" );
  if( publn )
  {
    SetPlotOptions<PhaseVsFrequency>( "above:l=$name" );
    SetPlotOptions<PhaseVsFrequency>( "above:r=$freq MHz" );
  }

  // DynamicSNSpectrum plot config

  SetPlotOptions<DynamicSNSpectrumPlot>( "below:l=" );
  SetPlotOptions<DynamicSNSpectrumPlot>( "below:r=" );
  if( publn )
  {
    SetPlotOptions<DynamicSNSpectrumPlot>( "above:l=$name" );
    SetPlotOptions<DynamicSNSpectrumPlot>( "above:r=$freq MHz" );
  }

  // StokesSpherical plot config

  SetPlotOptions<StokesSpherical>( "flux:below:l=" );
  if( publn )
  {
    SetPlotOptions<StokesSpherical>( "ell:above:c=" );
    if( freq_under_name )
      SetPlotOptions<StokesSpherical>( "flux:below:l=$name.$freq MHz" );
    else
    {
      SetPlotOptions<StokesSpherical>( "flux:below:l=$name" );
      SetPlotOptions<StokesSpherical>( "flux:below:r=$freq MHz" );
    }
  }
  else
  {
    SetPlotOptions<StokesSpherical>( "ell:above:c=$name $file\n Freq: $freq MHz BW: $bw Length: $length S/N: $snr" );
  }

  // BandpassTable plot config
  SetPlotOptions<BandpassPlot>( "above:c=$name $file Freq: $freq MHz" );

  // if we want to plot error bars

  if( plot_error_box )
  {
    SetPlotOptions<ProfilePlot>( "ebox=1" );
    SetPlotOptions<StokesCylindrical>( "flux:ebox=1" );
  }

  // if we are plotting position angles between -90 and 180, set  config for PosAnglePlot
  if( pa_ext )
  {
    SetPlotOptions<StokesCylindrical>( "pa:y:range=(0,1.5)" );
  }

  // if we have a range of position angles
  if( pa_min != 0.0 || pa_max != 1.0 )
  {
    SetPlotOptions<StokesCylindrical>( string("pa:y:range=(") +
				       tostring<float>(pa_min,3,ios::fixed) +
				       string(",") +
				       tostring<float>(pa_max,3,ios::fixed) +
				       string(")" ) );
  }

  if( user_character_height != -1 )
  {
    SetPlotOptions<Plot>( string("ch=")
			  + tostring(user_character_height,3,ios::fixed) );
  }
}




/**
 * CreatePlotsList
 *
 *  DOES     - given lists of filenames and plot identifiers, create a plotlist
 *  RECEIVES - vector of filenames, vector of plot ids
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::CreatePlotsList( vector< string > filenames,   vector< string > plot_ids )
{
  PlotFactory factory;

  for( unsigned i = 0; i < filenames.size(); i ++ )
  {
    try
    {
      FilePlots new_fplot;
      new_fplot.filename = filenames[i];
      new_fplot.archive = Archive::load( filenames[i] );
      for( unsigned p = 0; p < plot_ids.size(); p ++ )
      {
        string plot_id = plot_ids[p];
        if (plot_ids[p] == "B")
          plot_id = GetBandpassOrSpectrumPlot( new_fplot.archive );

        new_fplot.plots.push_back( factory.construct( plot_id ) );
      }
      plots.push_back( new_fplot );
    }
    catch ( Error e )
    {
      cerr << "Unable to load archive " << filenames[i] << endl;
      cerr << "The following exception was encountered" << endl;
      cerr << e << endl;
    }
  }
}



/**
 * SetCmdLineOptions
 *
 *  DOES     - Set all of the options given on all the plots
 *  RECEIVES - vector of option strings
 *  RETURNS  - Nothing
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

void PavApp::SetCmdLineOptions( vector< string > options )
{
  vector<string>::iterator it;
  for( it = options.begin(); it != options.end(); it ++ )
    SetPlotOptions<Plot>( (*it) );
}



/**
 * CheckColour
 *
 *  DOES     - Probe the device to determine if it suppots colour.
 *  RECEIVES - Nothing
 *  RETURNS  - Boolean true for colour support, false otherwise
 *  THROWS   - Nothing
 *  TODO     - Nothing
 **/

bool PavApp::CheckColour( void )
{
  bool have_colour = false;

  int first_index, last_index;
  cpgqcol( &first_index, &last_index );
  if( first_index != last_index )
  {
    float red,green,blue;
    cpgqcr( 2, &red, &green, &blue );
    if( !( red == green && green == blue ) )
      have_colour = true;
  }

  return have_colour;
}

string PavApp::GetBandpassOrSpectrumPlot( Reference::To<Archive> archive )
{
    Reference::To<Passband> passband = archive->get<Passband>();
    if (passband) {
        archive->pscrunch();
        archive->fscrunch();
        archive->tscrunch();
        return BANDPASSPLOT_ID;
    } else {
        return SPECTRUMPLOT_ID;
    }
}



/**
 * run
 *
 *  DOES     - equivelent of main, so testing programs can create multiple instances of
 *             PavApp and test them with different arguments.
 *  RECEIVES - argc, cargv
 *  RETURNS  - 1 if error, 0 otherwise
 *  THROWS   - Nothing, catches any error and returns 1 on error.
 *  TODO     - Nothing
 **/

int PavApp::run( int argc, char *argv[] )
{
  vector< string > filenames;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // PGPLOT device name
  string plot_device = default_plot_device;

  // Options to be set
  vector<string> options;

  // Preprocessing jobs
  vector<string> jobs;

  bool plot_qu = false;

  bool clear_labels = true;

  bool force_bw = false;

  vector< string > plot_ids;

  int option_index;

  const int PLOT_QU          = 1001;
  const int CMAP_IND         = 1002;
  const int BINPHSP          = 1003;
  const int BINPHSA          = 1004;
  const int BINPHLP          = 1005;
  const int BINPHLA          = 1006;
  const int PUBLN            = 1007;
  const int PUBLNC           = 1008;
  const int LAB_DEG          = 1009;
  const int EBOX             = 1010;
  const int PAEXT            = 1011;
  const int STACKLEFT        = 1012;
  const int CH               = 1013;
  const int USE_HA           = 1014;

  static struct option long_options[] =
    {
      { "convert_binphsperi", 1, 0, BINPHSP },
      { "convert_binphsasc",  1, 0, BINPHSA },
      { "convert_binlngperi", 1, 0, BINPHLP },
      { "convert_binlngasc",  1, 0, BINPHLA },
      { "plot_qu",            0, 0, PLOT_QU },
      { "cmap",               1, 0, CMAP_IND },
      { "publn",              0, 0, PUBLN },
      { "publnc",             0, 0, PUBLNC },
      { "ld",                 0, 0, LAB_DEG },
      { "ebox",               0, 0, EBOX },
      { "pa_ext",             0, 0, PAEXT },
      { "sl",                 0, 0, STACKLEFT },
      { "ch",                 1, 0, CH },
      { "ha",                 0, 0, USE_HA},
      { 0,0,0,0 }
    };

  float clip_value = 0.0;

  char valid_args[] = "Az:hb:M:KDcCdr:f:Ft:TGYSXBRmnjpP:y:H:I:N:k:ivVax:g:l:";
  opterr = 0;

  int c = '\0';
  while( (c = getopt_long( argc, argv, valid_args, long_options, &option_index )) != -1 )
  {
    switch( c )
    {
    case '?':
      if( optopt == 'g' )
        plot_device = "?";
      break;
    case 'a':
      Pulsar::Archive::agent_report ();
      return 0;
    case 'h':
      PrintUsage();
      return 0;
    case 'b':
      jobs.push_back( "bscrunch x" + string(optarg) );
      break;
    case 'i':
      cout << 
        "pav VERSION $Id: PavApp.C,v 1.70 2010/01/22 00:25:44 jonathan_khoo Exp $" << 
        endl << endl;
      return 0;
    case 'M':
      metafile = optarg;
      break;
    case 'g':
      plot_device = optarg;
      break;
    case 'c':
      remove_baseline = false;
      break;
    case 'C':
      //jobs.push_back( "centre" );
      centre_profile = true;
      break;
    case 'd':
      jobs.push_back( "dedisperse" );
      break;
    case 'V':
      Pulsar::Plot::verbose = true;
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'r':
      {
        ostringstream s_job;
        s_job << "rotate " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'f':
      {
        ostringstream s_job;
        s_job << "fscrunch " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'F':
      jobs.push_back( "fscrunch" );
      break;
    case 't':
      {
        ostringstream s_job;
        s_job << "tscrunch " << optarg;
        jobs.push_back( s_job.str() );
        break;
      }
    case 'T':
      jobs.push_back( "tscrunch" );
      break;
    case 'p':
      jobs.push_back( "pscrunch" );
      break;
    case 'S':
      clear_labels = false;
    case 'A':
    case 'X':
    case 'D':
    case 'G':
    case 'K':
    case 'Y':
    case 'R':
    case 'n':
    case 'B':
    case 'j':
      plot_ids.push_back( tostring<char>(c) );
      break;
    case 'm':
      plot_ids.push_back( tostring<char>(c) );
      break;
    case 'P':
      ipol = fromstring<int>( optarg );
      options.push_back( string("pol=") + string(optarg) );
      break;
    case 'y':
      {
	string range_string = string("(") + string(optarg) + string(")");
	options.push_back( string( "srange=" ) + range_string );
      }
      break;
    case 'H':
      ichan = fromstring<int>( optarg );
      options.push_back( string("chan=") + string( optarg ) );
      break;
    case 'I':
      isubint = fromstring<unsigned int>( optarg );
      options.push_back( string("subint=" ) + string( optarg ) );
      break;
    case 'N':
      {
        string s1,s2;
        string_split( optarg, s1, s2, "," );
        n1 = fromstring<unsigned int>( s1 );
        n2 = fromstring<unsigned int>( s2 );
      }
      break;
    case 'k':
      {
        string s1, s2;
        string_split( optarg, s1, s2, "," );
        min_freq = fromstring<double>( s1 );
        max_freq = fromstring<double>( s2 );
      }
      break;
    case 'l':
      {
        string s1, s2;
        string_split( optarg, s1, s2, "," );
        int d1 = fromstring<int>(s1);
        int d2 = fromstring<int>(s2);
        pa_min = PADegreesToTurns( int(floor(d1 + 0.0001)) );
        pa_max = PADegreesToTurns( int(ceil(d2 - 0.0001)) );
      }
      break;
    case 'z':
      {
        string s1, s2;
        string_split( optarg, s1, s2, "," );
        min_phase = fromstring<double>( s1 );
        max_phase = fromstring<double>( s2 );
      }
      break;
    case PLOT_QU:
      plot_qu = true;
      break;
    case CMAP_IND:
      switch( fromstring< unsigned int>( optarg ) )
      {
      case 0:
        colour_map = pgplot::ColourMap::GreyScale;
        options.push_back("cmap:map=grey");
        break;
      case 1:
        colour_map = pgplot::ColourMap::Inverse;
        options.push_back("cmap:map=inv");
        break;
      case 2:
        colour_map = pgplot::ColourMap::Heat;
        options.push_back("cmap:map=heat");
        break;
      case 3:
        colour_map = pgplot::ColourMap::Cold;
        options.push_back("cmap:map=cold");
        break;
      case 4:
        colour_map = pgplot::ColourMap::Plasma;
        options.push_back("cmap:map=plasma");
        break;
      case 5:
        colour_map = pgplot::ColourMap::Forest;
        options.push_back("cmap:map=forest");
        break;
      case 6:
        colour_map = pgplot::ColourMap::AlienGlow;
        options.push_back("cmap:map=alien");
        break;
      case 7:
        colour_map = pgplot::ColourMap::Test;
        options.push_back("cmap:map=test");
        break;
      default:
        cerr << "Unknown colour map, use (0-7)" << endl;
      };
      break;
    case BINPHLA:
      cbpao = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHLP:
      cbppo = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHSA:
      cblao = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case BINPHSP:
      cblpo = true;
      ronsub = fromstring<unsigned int>( optarg );
      break;
    case PUBLN:
      force_bw = true;
      publn = true;
      options.push_back( "set=pub" );
      break;
    case PUBLNC:
      publn = true;
      options.push_back( "set=pub" );
      break;
    case LAB_DEG:
      label_degrees = true;
      break;
    case EBOX:
      plot_error_box = true;
      break;
    case PAEXT:
      pa_ext = true;
      break;
    case STACKLEFT:
      freq_under_name = true;
      break;
    case CH:
      user_character_height = fromstring<float>( optarg );
      break;
    case USE_HA:
      options.push_back("use_ha=1");
      break;
    case 'x':
      clip_value = fromstring<float>( optarg );
      break;
    };
  }

  // Create a list of FilePlots with the archives and a vector of plots for each

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  // If we still don't have any filenames
  //   output an error and exit

  if (filenames.empty())
  {
    cout << "pav: please specify filename[s]" << endl;
    return -1;
  }

  // If we can't open the pgplot device
  //   output an error and exit

  if ( cpgopen( plot_device.c_str() ) < 0 )
  {
    cout << "pav: Could not open plot device" << endl;
    return -1;
  }

  // Determine if the device supports colour or not
  // TODO: in future, might check that it has enough colours, not just that it has colour

  if( !force_bw )
  {
    have_colour = CheckColour();
  }

  CreatePlotsList( filenames, plot_ids );

  // If we haven't put any plots into the plot array, output an error.

  if (plots.empty() )
  {
    cout << "pav: please choose at least one plot style" << endl;
    return -1;
  }

  // set options for all the plots

  SetCmdLineOptions( options );

  // Adjust the labels etc to the way we want to see them in pav.

  PavSpecificOptions();

  // Set the clip value for flux plots -x on the command line

  if( clip_value != 0.0 )
  {
    SetPlotOptions<FluxPlot>( string("crop=") + tostring<float>(clip_value) );
    SetPlotOptions<StokesCylindrical>( string("flux:crop=") + tostring<float>(clip_value) );
    SetPlotOptions<PhaseVsPlot>( string("crop=") + tostring<float>(clip_value) );
    SetPlotOptions<BandpassPlot>( string("crop=") + tostring<float>(clip_value) );
  }

  if( label_degrees )
  {
    //SetPlotOptions<StokesCylindrical>( "x:unit=deg" );
    SetPlotOptions<PhasePlot>( "x:unit=deg" );
    SetPlotOptions<MultiPhase>( "x:unit=deg" );
  }


  // If we received a -N option, divide the pgplot window into n1,n2 panels.

  if (n1 > 1 || n2 > 1)
    cpgsubp(n1,n2);

  // if we are plotting qu with -S, go through each of the plots and find the stokes plot
  // and set it to plot QU

  if( plot_qu )
  {
    SetPlotOptions<StokesCylindrical>( "flux:val=IQUV" );

    if( !have_colour )
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=1111" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=1234" );
    }
    else
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=1264" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=1111" );
    }
  }
  else
  {
    if( !have_colour )
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=111" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=124" );
    }
    else
    {
      SetPlotOptions<StokesCylindrical>( "flux:ci=124" );
      SetPlotOptions<StokesCylindrical>( "flux:ls=111" );
    }
  }

  SetPhaseZoom( min_phase, max_phase );

  if( min_freq != max_freq )
  {
    SetFreqZoom( min_freq, max_freq );
  }

  pgplot::ColourMap cmap;
  cmap.set_name ( colour_map );
  cmap.apply();

  Interpreter preprocessor;

  for (unsigned i = 0; i < plots.size(); i++)
  {
    try
    {
      if (jobs.size())
      {
        preprocessor.set( plots[i].archive );
        preprocessor.script( jobs );
      }

      if( centre_profile )
      {
        Functor< std::pair<int,int> (const Profile*) > old_strat = Profile::peak_edges_strategy;
        Functor< std::pair<int,int> (const Profile*) > consecutive_functor;

        if (!consecutive_functor)
          consecutive_functor.set( new PeakConsecutive, &RiseFall::get_rise_fall );

        Profile::peak_edges_strategy = consecutive_functor;

        Reference::To<Archive> copy = plots[i].archive->clone();
        copy->dedisperse();

        int nbin = copy->get_nbin();

        int p1, p2;
        copy->find_peak_edges( p1, p2 );

        float first = float(p1) / float(nbin);
        float second = float(p2) / float(nbin);
        float centre = first + (second - first) / 2.0;

        if( centre > 0.5 )
        {
          float rot = (centre - 0.5);
          plots[i].archive->rotate_phase( rot );
        }
        else
          plots[i].archive->rotate_phase( .5 - centre );

        Profile::peak_edges_strategy = old_strat;
      }


      if (cbppo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cbpao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cblpo)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngPeriOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      if (cblao)
      {
        Pulsar::IntegrationOrder* myio = new Pulsar::BinLngAscOrder();
        plots[i].archive->add_extension(myio);
        myio->organise(plots[i].archive, ronsub);
      }

      // set the precision that plot will use for labels
      tostring_setf = ios::fixed;
      if( !publn )
        tostring_precision = 3;
      else
        tostring_precision = 1;
      for (unsigned p=0; p < plots[i].plots.size(); p++)
      {
        cpgpage ();
        if( remove_baseline )
          plots[i].plots[p]->preprocess( plots[i].archive );

        if( !publn )
          PavSpecificLabels( plots[i].archive );

        plots[i].plots[p]->plot ( plots[i].archive );
      }
    }
    catch( Error e )
    {
      cerr << "Unable to plot " << plots[i].filename << endl;
      cerr << "The following exception was encountered" << endl;
      cerr << e << endl;
    }
  }


  cpgclos();

  return 0;
}



