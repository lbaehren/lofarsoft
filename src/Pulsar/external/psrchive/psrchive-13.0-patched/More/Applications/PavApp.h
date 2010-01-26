/*
 * Copyright (c) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 */




#ifndef PAV_APP_H_
#define PAV_APP_H_



#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <Pulsar/Integration.h>
#include <Pulsar/IntegrationOrder.h>
#include <Pulsar/PeriastronOrder.h>
#include <Pulsar/BinaryPhaseOrder.h>
#include <Pulsar/BinLngPeriOrder.h>
#include <Pulsar/BinLngAscOrder.h>

#include <Pulsar/StokesPlot.h>
#include <Pulsar/StokesCylindrical.h>

#include "TextInterface.h"
#include "strutil.h"
#include "dirutil.h"
#include "getopt.h"
#include "cpgplot.h"

#include "ColourMap.h"

#include <tostring.h>

#include <cpgplot.h>

#include <sstream>

#include <unistd.h>

using std::string;
using std::vector;
using Pulsar::Archive;
using Pulsar::Option;
using Pulsar::Plot;
using Reference::To;



class PavApp
{
public:
  PavApp();

  int run( int argc, char *argv[] );
private:
  
  static Option<string> default_plot_device;

  struct FilePlots
  {
    string filename;
    Reference::To<Archive> archive;
    vector< Reference::To<Plot> > plots;
  };

  vector< FilePlots > plots;

  template< class PC > void SetPlotOptions( string cmd )
  {
    vector< FilePlots >::iterator fit;
    for( fit = plots.begin(); fit != plots.end(); fit ++ )
    {
      vector< Reference::To<Plot> >::iterator pit;
      for( pit = (*fit).plots.begin(); pit != (*fit).plots.end(); pit ++ )
      {
        Reference::To<PC> sp = dynamic_cast<PC*>( (*pit).get() );
        if( sp )
        {
	  try { sp->configure( cmd ); } catch( Error e ) {}
        }
      }
    }
  }

  std::string GetBandpassOrSpectrumPlot( Reference::To<Archive> archive );
  
  void PrintUsage( void );

  void SetStokesPlotToQU( void );
  void SetPhaseZoom( double min_phase, double max_phase );
  void SetFreqZoom( double min_freq, double max_freq );
  void PavSpecificOptions( void );
  void PavSpecificLabels( Pulsar::Archive* archive );
  void CreatePlotsList( vector< string > filenames, vector< string > plot_ids );
  void SetCmdLineOptions( vector< string > options );
  bool CheckColour( void );
  float PADegreesToTurns( const int deg );

  bool have_colour;
  int ipol;
  int fsub, lsub;
  int isubint;
  int ichan;
  double rot_phase;
  bool svp;
  bool publn;
  bool axes;
  bool labels;
  unsigned int n1, n2;
  double min_freq;
  double max_freq;
  double min_phase;
  double max_phase;
  double reference_position_angle;
  double position_angle_height;
  double border;
  bool dark;
  double y_max;
  double y_min;
  double truncate_amp;
  pgplot::ColourMap::Name colour_map;

  bool cbppo;
  bool cbpao;
  bool cblpo;
  bool cblao;
  
  bool pa_ext;
  
  bool plot_error_box;
  
  bool freq_under_name;
  
  float user_character_height;
  
  float pa_min, pa_max;

  bool label_degrees;
  bool centre_profile;
  bool remove_baseline;

  unsigned int ronsub;
};

#endif






