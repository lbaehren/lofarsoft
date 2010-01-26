/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/PlotFactory.h"
#include "Pulsar/PlotLoop.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "TextInterface.h"
#include "strutil.h"

#include <cpgplot.h>

using namespace Pulsar;
using namespace std;

//! Pulsar Archive Zapping application
class psrplot: public Pulsar::Application
{
public:

  //! Default constructor
  psrplot ();

  //! Verify setup
  void setup ();

  //! Very verbose mode
  void set_very_verbose ()
  { Application::set_very_verbose(); Pulsar::Plot::verbose = true; }

  //! Process the given archive
  void process (Pulsar::Archive*);

  // -P help
  void help_plot_types ();

  // -C help
  void help_plot_options (const string& name);

  // -F help
  void help_frame_options (const string& name);

  // -p add plot
  void add_plot (const string& name);

  // -c add plot options
  void add_plot_options (const string& name);

  // -s load plot options
  void load_plot_options (const string& name);

  // -l add loop index
  void add_loop_index (const string& name);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  // Available plots
  PlotFactory factory;

  // Plots
  vector< Reference::To<Plot> > plots;

  // Options to be set
  vector<string> options;

  // Indeces over which to loop
  PlotLoop loop;

  // Allow plot classes to preprocess data before plotting
  bool preprocess;

  // Overlay plots from different files on top of eachother
  bool overlay_files;

  // Overlay plots on top of eachother
  bool overlay_plots;
};

int main (int argc, char** argv)
{
  psrplot program;
  return program.main (argc, argv);
}

psrplot::psrplot () : Pulsar::Application ("psrplot",
					   "pulsar plotting program")
{
  has_manual = true;
  version = "$Id: psrplot.C,v 1.33 2010/01/14 13:14:30 straten Exp $";

  // print angles in degrees
  Angle::default_type = Angle::Degrees;

  // Allow plot classes to preprocess data before plotting
  preprocess = true;

  // Do not overlay plots from different files on top of eachother
  overlay_files = false;

  // Do not overlay plots
  overlay_plots = false;

  add( new Pulsar::PlotOptions );

  Pulsar::StandardOptions* preprocessor = new Pulsar::StandardOptions;
  preprocessor->get_interpreter()->allow_infinite_frequency = true;

  add( preprocessor );
}

void psrplot::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::help_plot_types, 'P');
  arg->set_help ("Help: list available plot types");

  arg = menu.add (this, &psrplot::help_plot_options, 'C', "plot");
  arg->set_help ("Help: list options specific to 'plot'");

  arg = menu.add (this, &psrplot::help_frame_options, 'A', "plot");
  arg->set_help ("Help: list common options for 'plot'");

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::add_plot, 'p', "plot");
  arg->set_help ("add plot type");

  arg = menu.add (this, &psrplot::add_plot_options, 'c', "cfg[s]");
  arg->set_help ("add plot options");

  arg = menu.add (this, &psrplot::load_plot_options, 's', "style");
  arg->set_help ("add plot options in 'style' file");

  // blank line in help
  menu.add ("");

  arg = menu.add (preprocess, 'x');
  arg->set_help ("disable default preprocessing");

  arg = menu.add (overlay_plots, 'O');
  arg->set_help ("overlay plots");

  arg = menu.add (overlay_files, 'F');
  arg->set_help ("overlay plots from multiple files");

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::add_loop_index, 'l', "name=<range>");
  arg->set_help ("loop over the range of the named parameter");
}

void psrplot::add_plot (const string& name)
{
  plots.push_back( factory.construct(name) );
  loop.add_Plot (plots.back());
}

// load the string of options into one of the plots
void specific_options (string optarg, vector< Reference::To<Plot> >& plots);

void psrplot::add_plot_options (const std::string& arg)
{
  if (arg[0] == ':')
    specific_options (arg, plots);
  else
    separate (arg, options, ",");
}
      
// load the style file into one of the plots
void specific_style (string optarg, vector< Reference::To<Plot> >& plots);

// set the option for the specified plot
void set_option (Pulsar::Plot* plot, const string& option);

void psrplot::load_plot_options (const std::string& arg)
{
  if (arg[0] == ':')
  {
    specific_style (arg, plots);
    return;
  }

  vector<string> style;
  loadlines (arg, style);

  Reference::To<Plot> current_plot;

  for (unsigned i=0; i<style.size(); i++)
  {
    string line = style[i];

    string key = stringtok (line, " \t\n");
    if (key == "plot")
    {
      add_plot (line);
      current_plot = plots.back();
    }
    else if (current_plot)
      set_option( current_plot, style[i] );
    else
      options.push_back( style[i] );
  }
}

void psrplot::add_loop_index (const std::string& arg)
{
  loop.add_index( new TextIndex(arg) );
}

// load the vector of options into the specified plot
void set_options (Pulsar::Plot* plot, const vector<string>& options);

void psrplot::setup ()
{
  if (plots.empty())
    throw Error (InvalidState, "psrplot",
		 "please choose at least one plot style");

  if (options.size())
  {
    if (verbose)
      cerr << "psrplot: parsing options" << endl;
    for (unsigned iplot=0; iplot < plots.size(); iplot++)
      set_options (plots[iplot], options);
  }

  loop.set_overlay (overlay_plots);
  loop.set_preprocess (preprocess);
}

void psrplot::process (Pulsar::Archive* archive)
{
  if (verbose)
    cerr << "psrplot: plotting " << archive->get_filename() << endl;

  if( !overlay_files && loop.get_overlay() )
    cpgpage();

  loop.set_Archive (archive);
  loop.plot();
}

void psrplot::help_plot_types ()
{
  cout << "Available Plots:\n" << factory.help() << endl;
  exit (0);
}

void help_options (TextInterface::Parser* tui)
{
  cout << tui->help(true) << endl;
  exit (0);
}

void psrplot::help_plot_options (const string& name)
{
  Plot* plot = factory.construct(name);
  help_options( plot->get_interface() );
}

void psrplot::help_frame_options (const string& name)
{
  Plot* plot = factory.construct (name);
  help_options( plot->get_frame_interface() );
}

void set_option (Pulsar::Plot* plot, const string& option) try
{
  plot->configure (option);
}
catch (Error& error)
{
  cerr << "psrplot: Invalid option '" << option << "' " 
       << error.get_message() << endl;
  exit (-1);
}

void set_options (Pulsar::Plot* plot, const vector<string>& options)
{
  for (unsigned j = 0; j < options.size(); j++)
    set_option (plot, options[j]);
}

// parses index from arg and removes it from the string
unsigned get_index (string& arg, vector< Reference::To<Plot> >& plots)
{
  unsigned index = fromstring<unsigned> ( stringtok (arg, ":") );
  if (index >= plots.size())
  {
    cerr << "psrplot: invalid plot index = " << index
	 << " nplot=" << plots.size() << endl;
    exit(-1);
  }
  return index;
}

// load the string of options into one of the plots
void specific_options (string arg, vector< Reference::To<Plot> >& plots)
{
  // get the plot index
  unsigned index = get_index (arg, plots);

  // separate the options
  vector<string> options;
  separate (arg, options, ",");

  // set them for the specified plot
  set_options (plots[index], options);
}

// load the style file into one of the plots
void specific_style (string arg, vector< Reference::To<Plot> >& plots)
{
  // get the plot index
  unsigned index = get_index (arg, plots);

  // load the options
  vector<string> options;
  loadlines (arg, options);

  // set them for the specified plot
  set_options (plots[index], options);
}

