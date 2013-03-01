/***************************************************************************
 *
 *   Copyright (C) 2007 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <cpgplot.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "Pulsar/psrchive.h"
#include "Pulsar/PlotFactory.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/LawnMower.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "TextInterface.h"
#include "strutil.h"
#include "BoxMuller.h"
#include "pairutil.h"

#define BOX_ANCHOR 2
#define HORIZONTAL_ANCHOR 3
#define VERTICAL_ANCHOR 4
#define HORIZONTAL_LINE 3
#define VERTICAL_LINE 4

#define ZERO_PAIR std::pair<float, float>(0.0, 0.0)
#define UNDEF_MOUSE std::pair<float, float>(-1.0, -1.0)

typedef std::pair<float, float> MouseType;
typedef std::pair<unsigned, unsigned> RangeType;

using namespace Pulsar;
using namespace std;

static PlotFactory factory;

void usage()
{
  cout << endl <<

    "A user-interactive program for zapping subints, channels and bins.\n"
    "Usage: pazi [filename]\n\n"
    "Options.\n"
    "  -h                         This help page.\n\n"
    "Mouse and keyboard commands.\n"
    "  zoom:                      left click, then left click\n"
    "  reset zoom:                'r'\n"
    "\n"
    "Modes: \n"
    "  phase-vs-frequency:        'f'\n"
    "  phase-vs-time:             't'\n"
    "  binzap-integration:        'b' (must be in phase-vs-time mode)\n"
    "\n"
    "  center pulse:              'c'\n"
    "  toggle dedispersion:       'd'\n"
    "\n"
    "  zap:                       right click \n"
    "  zap (multiple):            left click, then right click \n"
    "  undo last:                 'u'\n"
    "\n"
    "  In binzap mode: \n"
    "    -  zoom and reset zoom as above \n"
    "    -  zap phase range:      left click, then right click \n"
    "    -  mow the lawn:         'm'\n"
    "    -  prune the hedge:      'x' to start box, 'x' to finish\n"
    "\n"
    "  save (<filename>.pazi):    's'\n"
    "  quit:                      'q'\n"
    "  print paz command:         'p'\n"
    "\n"
    
    "See " PSRCHIVE_HTTP "/manuals/pazi for more details\n" << endl;
}

enum PlotType
  {
    PhaseVsTime,
    PhaseVsFrequency,
    FscrunchedSubint
  };

void zap_single ();
void zap_multiple ();

void constrain_range(float& f);

string get_zoom_option(const RangeType& ranges, const unsigned max_value);

RangeType get_range(const MouseType& mouse_ref, const MouseType& mouse,
		    const RangeType& ranges, const bool horizontal);

unsigned get_subint_indexed_value(const MouseType& mouse);

void zap_bin(Pulsar::Archive* arch, Pulsar::Archive* old_arch, const unsigned subint,
	     const unsigned bin);

// Zap bins in polns [0:npol-1] across a range of bins.
void zap_bins(Pulsar::Archive* arch, Pulsar::Archive* old_arch,
    const unsigned subint, const unsigned start_bin, const unsigned end_bin);

unsigned get_max_value(const Pulsar::Archive* archive, PlotType plot_type);

void reorder_ranges(RangeType& p);

unsigned get_indexed_value(const MouseType& mouse);

bool add_zapped_value(const unsigned value, vector<int>& zapped_values);

void time_zap_subint(Pulsar::Archive* archive, const unsigned subint);

void freq_zap_chan(Pulsar::Archive* arch, const unsigned zap_chan);
void freq_unzap_chan(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_chan);
void time_unzap_subint(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_subint);
void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void freq_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void time_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom);
void update_total(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* plot);
bool add_channel(int chan, vector<int>& delete_channels);
bool remove_channel(int chan, vector<int>& delete_channels);
void print_command(vector<int>& freq_chans, vector<int>& subints, string extension, string filename);
void set_dedispersion(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &dedispersed);
//void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, int type, bool dedispersed);

void mowlawn (Pulsar::Archive* arch, Pulsar::Archive* old, int subint);

void prune_hedge (Pulsar::Archive* arch, Pulsar::Archive* old, int subint);
MouseType prune_start = UNDEF_MOUSE;
MouseType prune_end = UNDEF_MOUSE;

static bool dedispersed = true;
static vector<int> channels_to_zap;
static vector<int> subints_to_zap;
static vector<int> bins_to_zap;
static vector<int> subints_to_mow;

RangeType ranges(0, 0);
bool positive_direction = true;

//static bool centered = true;

Reference::To<Pulsar::Archive> base_archive;
Reference::To<Pulsar::Archive> backup_archive;
Reference::To<Pulsar::Archive> mod_archive;
Reference::To<Pulsar::Archive> scrunched_archive;

// by default, phase-vs-time
PlotType plot_type = PhaseVsTime;

// the current sub-integration
int subint = 0;

char ch;
bool zoomed = false;
bool fscrunched = true;

// where the mouse clicks are stored
MouseType mouse_ref(0.0, 0.0);
MouseType mouse(0.0, 0.0);

Reference::To<Plot> time_orig_plot;
Reference::To<Plot> time_mod_plot;
Reference::To<TextInterface::Parser> time_fui;

Reference::To<Plot> freq_orig_plot;
Reference::To<Plot> freq_mod_plot;
Reference::To<TextInterface::Parser> freq_fui;

Reference::To<Plot> total_plot;

Reference::To<ProfilePlot> subint_orig_plot;
Reference::To<ProfilePlot> subint_mod_plot;
Reference::To<TextInterface::Parser> subint_fui;

int main(int argc, char* argv[]) try
{
  if (argc < 2) {
    usage();
    return EXIT_SUCCESS;
  }
  
  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hvV")) != -1) {
    switch (gotc) {
    case 'h':
      usage();
      return EXIT_SUCCESS;
    case 'V':
      Pulsar::Archive::set_verbosity(3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity(2);
      break;
    }
  }
  
  if (optind >= argc)
    {
      cerr << "pazi: please specify filename" << endl;
      return -1;
    }
  
  string filename = argv[optind];
  string extension = filename.substr(filename.length() - 2, 2);
  
  if (extension == "rf")
    extension = "rz";
  else if (extension == "cf")
    extension = "cz";
  else
    extension = "pazi";
  
  string write_filename = filename + ".";
  write_filename += extension;
  
  cerr << "pazi: loading data" << endl;
  base_archive = Archive::load(filename);
  
  if (base_archive->get_npol() == 4)
    base_archive->convert_state( Signal::Stokes );
  
  backup_archive = base_archive->clone();
  
  cerr << "pazi: making fscrunched clone" << endl;
  mod_archive = base_archive->clone();
  mod_archive->pscrunch();
  mod_archive->remove_baseline();
  mod_archive->dedisperse();
  mod_archive->fscrunch();

  scrunched_archive = mod_archive->clone();
  scrunched_archive->tscrunch();

  ranges.second = get_max_value(base_archive, plot_type);
  positive_direction = base_archive->get_bandwidth() < 0.0;

  time_orig_plot = factory.construct("time");
  time_mod_plot = factory.construct("time");
  time_fui = time_mod_plot->get_frame_interface();

  freq_orig_plot = factory.construct("freq");
  freq_mod_plot = factory.construct("freq");
  freq_fui = freq_mod_plot->get_frame_interface();

  total_plot = factory.construct("flux");
  total_plot->configure("info=1");

  subint_orig_plot = new ProfilePlot;
  subint_mod_plot = new ProfilePlot;
  subint_fui = subint_mod_plot->get_frame_interface();

  subint_orig_plot->configure("info=1");
  subint_mod_plot->configure("info=1");

  unsigned window = 0;
  char device [8];

  for (unsigned i=0; i<2; i++) do {
    window ++;
    snprintf (device, 8, "%u/XS", window);
  }
  while ( cpgopen (device) < 0 );

  cpgask(0);

  cerr << endl << "Total S/N = " <<
    scrunched_archive->get_Profile(0,0,0)->snr() << endl << endl;

  total_plot->plot(scrunched_archive);
  cpgslct(1);
  time_orig_plot->plot(mod_archive);

  do
  {
    cpgswin(0, 1, 0, 1);

    // plot:
    //   frequency = horizontal mouse band
    //   time      = horizontal mouse band
    //   profile   = vertical mouse band

    int band = 0;

    if (prune_start != UNDEF_MOUSE)
    {
      band = BOX_ANCHOR;
      mouse_ref = prune_start;
    }
    else if (mouse_ref != UNDEF_MOUSE)
    {
      if (plot_type == FscrunchedSubint)
	band = VERTICAL_ANCHOR;
      else
	band = HORIZONTAL_ANCHOR;
    }
    else
    {
      if (plot_type == FscrunchedSubint)
	band = VERTICAL_LINE;
      else
	band = HORIZONTAL_LINE;
    }


    cpgband(band, 0, mouse_ref.first, mouse_ref.second, &(mouse.first),
	    &(mouse.second), &ch);

    switch (ch)
    {

    case 'A': // zoom
      {
	constrain_range(mouse.first);
	constrain_range(mouse.second);

	if (mouse_ref == UNDEF_MOUSE)
        {
	  mouse_ref = mouse;
	  continue;
	}


	// store the current range so it can be restored if the user selects
	// a zoom region too small
	const RangeType old_ranges = ranges;
	bool horizontal = plot_type == FscrunchedSubint ? false : true;
	ranges = get_range(mouse_ref, mouse, ranges, horizontal);

	// ignore mouse clicks if the index values are too close (< 1)
	if (ranges.first == ranges.second) {
	  ranges = old_ranges;
	  break;
	}

	zoomed = true;

	const unsigned max_value = get_max_value(base_archive, plot_type);
	const string zoom_option = get_zoom_option(ranges, max_value);

	switch (plot_type) {
	case PhaseVsTime:
	  time_fui->set_value("y:range", zoom_option);
	  redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
	  break;
	case PhaseVsFrequency:
	  freq_fui->set_value("y:range", zoom_option);
	  freq_redraw(mod_archive, base_archive, freq_orig_plot,
		      freq_mod_plot, zoomed);
	  break;
	case FscrunchedSubint:
	  subint_fui->set_value("x:range", zoom_option);
	  redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
	  break;
	}
      }
      break; // case 'A'
    case 'h':
      usage();
      break;

    case 'b': // plot specific subint
      if (plot_type == PhaseVsTime) {
	plot_type = FscrunchedSubint;
	zoomed = false;

	*mod_archive = *base_archive;
	mod_archive->set_dispersion_measure(0);
	mod_archive->fscrunch();
	mod_archive->pscrunch();
	mod_archive->remove_baseline();

	subint = get_indexed_value(mouse);

	ranges.first = 0;
	ranges.second = get_max_value(base_archive, plot_type);

	char add[3];
	sprintf(add, "%d", subint);
	string subint_option = "subint=";
	subint_option += add;

	subint_orig_plot->configure(subint_option);
	subint_mod_plot->configure(subint_option);

	cpgeras();
	subint_orig_plot->plot(mod_archive);
	update_total(scrunched_archive, base_archive, total_plot);
      }
      break;

      /*case 'c': // center pulse
	set_centre(mod_archive, base_archive, centered, plot_type, dedispersed);

	if (plot_type == "freq")
	redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
	else if (plot_type == "time")
	redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);

	update_total(scrunched_archive, base_archive, total_plot);
	break;*/

    case 'd': // toggle dedispersion on/off
      set_dedispersion(mod_archive, base_archive, dedispersed);

      if (plot_type == PhaseVsFrequency) {
	mod_archive->tscrunch();
	redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
      } else if (plot_type == PhaseVsTime) {
	mod_archive->fscrunch();
	redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
      }

      update_total(scrunched_archive, base_archive, total_plot);
      break;

    case 'f': // frequency plot
      plot_type = PhaseVsFrequency;
      ranges.first = 0;
      ranges.second = get_max_value(base_archive, plot_type);
      zoomed = false;
      freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);
      break;

    case 'm':

      if (plot_type != FscrunchedSubint)
      {
	cerr << "pazi: can only mow lawn in binzap-subint mode" << endl;
	continue;
      }

      cerr << "pazi: mowing lawn" << endl;
      mowlawn (mod_archive, base_archive, subint);

      cerr << "pazi: replotting" << endl;
      redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
      cerr << "pazi: updating total" << endl;
      update_total(scrunched_archive, base_archive, total_plot);

      break;

    case 'x': // prune

      if (plot_type != FscrunchedSubint)
      {
	cerr << "pazi: can only prune hedge in binzap-subint mode" << endl;
	continue;
      }

      constrain_range(mouse.first);
      constrain_range(mouse.second);

      if (prune_start == UNDEF_MOUSE)
      {
	prune_start = mouse;
	continue;
      }

      prune_end = mouse;

      cerr << "pazi: pruning hedge" << endl;
      prune_hedge (mod_archive, base_archive, subint);

      cerr << "pazi: replotting" << endl;
      redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
      cerr << "pazi: updating total" << endl;
      update_total(scrunched_archive, base_archive, total_plot);

      break;

    case 'o': // toggle frequency scrunching on/off
      if (plot_type == PhaseVsTime)
      {
	if (fscrunched) {
	  fscrunched = false;
	  *mod_archive = *base_archive;
	}
	else {
	  fscrunched = true;
	  mod_archive->fscrunch();
	}
	redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
      }
      break;

    case 'q': // quit
      cpgclos();
      return EXIT_SUCCESS;

    case 'p':
      print_command(channels_to_zap, subints_to_zap, extension, filename);
      break;

    case 'r': // reset zoom
      zoomed = false;
      ranges.first = 0;
      ranges.second = get_max_value(base_archive, plot_type);

      switch (plot_type) {
      case PhaseVsTime:
	redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
	break;
      case PhaseVsFrequency:
	freq_redraw(mod_archive, base_archive, freq_orig_plot,
		    freq_mod_plot, zoomed);
	break;
      case FscrunchedSubint:
	redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
	break;
      }
      break;

    case 's': // save current archive changes:
      {
	Pulsar::ProcHistory* ext = base_archive->get<Pulsar::ProcHistory>();
	if (ext) {
	  ext->set_command_str("pazi");
	}

	base_archive->unload(write_filename);
	break;
      }

    case 't': // time plot
      plot_type = PhaseVsTime;
      ranges.first = 0;
      ranges.second = get_max_value(base_archive, plot_type);
      zoomed = false;
      time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);
      break;

    case 'u': // undo last change

      if (mouse_ref != UNDEF_MOUSE) {
	mouse_ref = UNDEF_MOUSE;
	continue;
      }

      switch (plot_type) {
      case PhaseVsTime:
	{
	  const unsigned value = get_indexed_value(mouse);
	  remove_channel(value, subints_to_zap);
	  time_unzap_subint(base_archive, backup_archive, value);
	  time_redraw(mod_archive, base_archive, time_orig_plot,
		      time_mod_plot, zoomed);
	}
	break;
      case PhaseVsFrequency:
	{
	  const unsigned value = get_indexed_value(mouse);
	  remove_channel(value, channels_to_zap);
	  freq_unzap_chan(base_archive, backup_archive, value);
	  freq_redraw(mod_archive, base_archive, freq_orig_plot,
		      freq_mod_plot, zoomed);
	}
	break;
      case FscrunchedSubint:
	if (bins_to_zap.size()) {
	  bins_to_zap.erase(bins_to_zap.end() - 5, bins_to_zap.end());

	  *base_archive = *backup_archive;
	  *mod_archive = *backup_archive;

	  mod_archive->set_dispersion_measure(0);
	  mod_archive->pscrunch();
	  mod_archive->fscrunch();
	  mod_archive->remove_baseline();
	  redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
	}
	break;
      }
      update_total(scrunched_archive, base_archive, total_plot);
      break;

    case 'z':
    case 'X': // zap single channel

      if (mouse_ref == UNDEF_MOUSE)
	zap_single ();
      else
	zap_multiple ();

      break;
    }

    prune_start = UNDEF_MOUSE;
    mouse_ref = UNDEF_MOUSE;

  } while (ch != 'q');

  return 0;

 } // end main

 catch (Error& error) {
   cerr << "pazi: " << error << endl;
   return -1;
 }

// ensure p.first < p.second
void reorder_ranges(RangeType& p)
{
  if (p.first > p.second) {
    std::swap(p.first, p.second);
  }
}

unsigned get_indexed_value(const MouseType& mouse)
{
  return static_cast<unsigned>(mouse.second *
			       float(ranges.second - ranges.first) + ranges.first);
}

unsigned get_subint_indexed_value(const MouseType& mouse)
{
  return static_cast<unsigned>(mouse.first *
			       float(ranges.second - ranges.first) + ranges.first);
}

RangeType get_range(const MouseType& mouse_ref, const MouseType& mouse,
		    const RangeType& ranges, const bool horizontal)
{
  RangeType new_ranges;

  const MouseType mouse_values = horizontal ?
    make_pair(mouse_ref.second, mouse.second) :
    make_pair(mouse_ref.first, mouse.first);

  new_ranges.first = static_cast<unsigned>(mouse_values.first *
					   float(ranges.second - ranges.first) + ranges.first);

  new_ranges.second = static_cast<unsigned>(mouse_values.second *
					    float(ranges.second - ranges.first) + ranges.first);

  reorder_ranges(new_ranges);

  return new_ranges;
}

// ensure mouse range [0:1]
void constrain_range(float& f)
{
  if (f < 0.0) {
    f = 0.0;
  } else if (f > 1.0) {
    f = 1.0;
  }
}

unsigned get_max_value(const Pulsar::Archive* archive, PlotType plot_type)
{
  switch (plot_type) {
  case PhaseVsTime:
    return archive->get_nsubint();
  case PhaseVsFrequency:
    return archive->get_nchan();
  case FscrunchedSubint:
    return archive->get_nbin();
  default:
    // throw something
    return 0;
  }
}

// pre: y < y2
string get_zoom_option(const RangeType& ranges, const unsigned max_value)
{
  string option = "(";
  char add[10];

  const float bottom = static_cast<float>(ranges.first) /
    static_cast<float>(max_value);

  const float top = static_cast<float>(ranges.second) /
    static_cast<float>(max_value);

  sprintf(add, "%2.6f", bottom);
  option += add;
  option += ",";
  sprintf(add, "%2.6f", top);
  option += add;
  option += ")";

  return option;
}

void zap_single ()
{
  cerr << "zap single" << endl;

  *backup_archive = *base_archive;

  unsigned value = 0;

  switch (plot_type) {
  case PhaseVsTime:
    value = get_indexed_value(mouse);
    time_zap_subint(base_archive, value);
    time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot,
		zoomed);
    break;

  case PhaseVsFrequency:
    value = get_indexed_value(mouse);
    freq_zap_chan(base_archive, value);
    freq_redraw(mod_archive, base_archive, freq_orig_plot,
		freq_mod_plot, zoomed);
    break;

  case FscrunchedSubint: // subint
    value = get_subint_indexed_value(mouse);
    zap_bin(mod_archive, base_archive, subint, value);
    redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
    break;
  }

  update_total(scrunched_archive, base_archive, total_plot);
}

void zap_multiple ()
{
  cerr << "zap multiple" << endl;

  *backup_archive = *base_archive;

  const bool horizontal = plot_type == FscrunchedSubint ? false : true;
  RangeType range_to_zap = get_range(mouse_ref, mouse, ranges, horizontal);

  switch (plot_type)
  {

  case PhaseVsTime: // time

    for (unsigned i = range_to_zap.first; i <= range_to_zap.second; ++i)
      time_zap_subint(base_archive, i);
      
    time_redraw(mod_archive, base_archive, time_orig_plot,
		time_mod_plot, zoomed);
    
    break;

  case PhaseVsFrequency: // freq
    
    for (unsigned i = range_to_zap.first; i <= range_to_zap.second; ++i)
      freq_zap_chan(base_archive, i);
    
    freq_redraw(mod_archive, base_archive, freq_orig_plot,
		freq_mod_plot, zoomed);

    break;

  case FscrunchedSubint:

    zap_bins(mod_archive, base_archive, subint, range_to_zap.first,
        range_to_zap.second);
  
    redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);

    break;
  }
  
  mouse_ref = ZERO_PAIR;
  update_total(scrunched_archive, base_archive, total_plot);
}

void freq_zap_chan(Pulsar::Archive* arch, const unsigned zap_chan)
{
  const unsigned npol = arch->get_npol();
  const unsigned nsubint = arch->get_nsubint();
  const unsigned nchan = arch->get_nchan();

  add_zapped_value(zap_chan, channels_to_zap);

  for (unsigned ipol = 0; ipol < npol; ++ipol) {
    for (unsigned isub = 0; isub < nsubint; ++isub) {
      for (unsigned ichan = 0; ichan < nchan; ++ichan) {
	if (ichan == zap_chan) {
	  arch->get_Profile(isub, ipol, ichan)->set_weight(0);
	}
      }
    }
  }
}

void time_zap_subint(Pulsar::Archive* archive, const unsigned subint)
{
  add_zapped_value(subint, subints_to_zap);

  const unsigned npol = archive->get_npol();
  const unsigned nchan = archive->get_nchan();

  for (unsigned ipol = 0; ipol < npol; ++ipol) {
    for (unsigned ichan = 0; ichan < nchan; ++ichan) {
      archive->get_Profile(subint,ipol,ichan)->set_weight(0);
    }
  }
}

bool is_zapped (vector<int>& zapped, int index)
{
  for (unsigned i=0; i<zapped.size(); i++)
    if (zapped[i] == index)
      return true;
  return false;
}

void freq_unzap_chan(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_chan)
{
  int npol = arch->get_npol();
  int nsub = arch->get_nsubint();
  int nchan = arch->get_nchan();

  for (int pol = 0; pol < npol; pol++ ) {
    for (int sub = 0; sub < nsub; sub++) {
      if (is_zapped(subints_to_zap, sub))
	continue;

      for (int chan = 0; chan < nchan; chan++) {
	if (chan == zap_chan)
	  arch->get_Profile(sub,pol,chan)->set_weight(backup->get_Profile(sub,pol,chan)->get_weight());
      }
    }
  }
}

void time_unzap_subint(Pulsar::Archive* arch, Pulsar::Archive* backup, int zap_subint)
{
  int npol = arch->get_npol();
  int nsub = arch->get_nsubint();
  int nchan = arch->get_nchan();

  for (int pol = 0; pol < npol; pol++ )
    for (int sub = 0; sub < nsub; sub++)
      for (int chan = 0; chan < nchan; chan++)
	if (!is_zapped(channels_to_zap,chan))
	  arch->get_Profile(sub,pol,chan)->set_weight(backup->get_Profile(sub,pol,chan)->get_weight());
}

void redraw(Pulsar::Archive* arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
  /*if (centered)
    arch->centre();*/

  cpgeras();
  if (zoom)
    mod_plot->plot(arch);
  else
    orig_plot->plot(arch);
}

void freq_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
  *arch = *old_arch;
  if (!dedispersed)
    arch->set_dispersion_measure(0);
  else
    arch->dedisperse();

  arch->pscrunch();
  arch->remove_baseline();
  arch->tscrunch();
  redraw(arch, orig_plot, mod_plot, zoom);
}

void time_redraw(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* orig_plot, Plot* mod_plot, bool zoom)
{
  *arch = *old_arch;
  if (!dedispersed)
    arch->set_dispersion_measure(0);

  arch->pscrunch();
  arch->remove_baseline();
  arch->fscrunch();
  redraw(arch, orig_plot, mod_plot, zoom);
}

void update_total(Pulsar::Archive* arch, Pulsar::Archive* old_arch, Plot* plot)
{
  *arch = *old_arch;
  if (!dedispersed)
    arch->set_dispersion_measure(0);

  arch->pscrunch();
  arch->remove_baseline();
  arch->tscrunch();
  arch->fscrunch();

  /*if (centered)
    arch->centre();*/

  cpgslct(2);
  cpgeras();
  plot->plot(arch);
  cpgslct(1);
}

bool add_zapped_value(const unsigned value, vector<int>& zapped_values)
{
  if (!is_zapped(zapped_values, value)) {
    zapped_values.push_back(value);
    return true;
  }
  return false;
}


bool add_channel(int chan, vector<int>& delete_channels)
{
  if (!is_zapped(delete_channels, chan)) {
    delete_channels.push_back(chan);
    return true;
  }
  return false;
}

bool remove_channel(int chan, vector<int>& delete_channels)
{
  for (vector<int>::iterator it = delete_channels.begin(); it != delete_channels.end(); ++it) {
    if (*it == chan) {
      it = delete_channels.erase(it);
      return true;
    }
  }
  return false;
}

void print_command (vector<int>& freq_chans, vector<int>& subints, 
		    string extension, string filename)
{
  if( freq_chans.size() || subints.size() ||
      bins_to_zap.size() || subints_to_mow.size() ) {

    cout << "paz ";

    if (freq_chans.size()) {
      cout << "-z \"";

      for (unsigned i = 0; i < freq_chans.size() - 1; i++) {
	cout << freq_chans[i] << " ";
      }
      cout << freq_chans[freq_chans.size()-1] << "\" ";
    }

    if (subints.size()) {
      cout << "-w \"";

      for (unsigned i = 0; i< subints.size() - 1; i++) {
	cout << subints[i] << " ";
      }
      cout << subints[subints.size() - 1] << "\" ";
    }

    if (bins_to_zap.size()) {
      cout << "-B \"";

      for (unsigned i = 0; i < bins_to_zap.size() - 1; i++) {
	cout << bins_to_zap[i] << " ";
      }
      cout << bins_to_zap[bins_to_zap.size() - 1] << "\" ";
    }

    if (subints_to_mow.size()) {

      for (unsigned i = 0; i < subints_to_mow.size(); i++)
	cout << "-l " << subints_to_mow[i] << " ";

    }

    cout << "-e " << extension << " " << filename << endl;
  }
}

void set_dedispersion(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &dedispersed)
{
  if (!dedispersed)
    {
      dedispersed = true;
      *arch = *old_arch;
      arch->dedisperse();
    }
  else
    {
      dedispersed = false;
      *arch = *old_arch;
      arch->set_dispersion_measure(0);
      arch->dedisperse();
      arch->set_dispersion_measure(old_arch->get_dispersion_measure());
      arch->set_dedispersed (false);
    }

  arch->remove_baseline();
}

/*void set_centre(Pulsar::Archive* arch, Pulsar::Archive* old_arch, bool &centered, int type, bool dedispersed)
  {
  if (type != 2) {
  if (!centered) {
  arch->centre();
  centered = true;
  } else {
  centered = false;
  *arch = *old_arch;
  arch->pscrunch();
  arch->remove_baseline();

  if (type == 1) {
  arch->tscrunch();
  } else {
  arch->fscrunch();
  }
  }

  if (dedispersed) {
  arch->dedisperse();
  }
  }
  }*/

void zap_bin (Pulsar::Archive* arch, Pulsar::Archive* old_arch, 
	      const unsigned subint,
	      const unsigned bin)
{
  const unsigned npol = old_arch->get_npol();
  const unsigned nchan = old_arch->get_nchan();

  for (unsigned ipol = 0; ipol < npol; ++ipol)
  {
    for (unsigned ichan = 0; ichan < nchan; ++ichan)
    {    
      double baseline_mean =
        old_arch->get_Profile(subint,ipol,ichan)->baseline()->get_mean().get_value();

      float* bins = old_arch->get_Profile(subint,ipol,ichan)->get_amps();
      bins[bin] = baseline_mean;
    }
  }

  *arch = *old_arch;
  arch->set_dispersion_measure(0);
  arch->pscrunch();
  arch->fscrunch();
  arch->remove_baseline();
}

/**
 * Zap a specified range of bins by substituting their values with the
 * baseline mean for each profile.
 */
void zap_bins(Pulsar::Archive* arch, Pulsar::Archive* old_arch,
    const unsigned s, const unsigned start_bin, const unsigned end_bin)
{
  const unsigned npol  = old_arch->get_npol();
  const unsigned nchan = old_arch->get_nchan();

  // Get the baseline region from what is displayed on screen (pscrunched
  // and fscrunched integration).
  Reference::To<Pulsar::Archive> copy = old_arch->clone();
  copy->pscrunch();
  copy->fscrunch();

  Reference::To<Pulsar::PhaseWeight> baseline_weights =
    copy->get_Profile(s,0,0)->baseline();

  vector<float> weights;
  baseline_weights->get_weights(weights);

  for (unsigned ipol = 0; ipol < npol; ++ipol) {
    for (unsigned ichan = 0; ichan < nchan; ++ichan) {
      // Apply the baseline region to each profile and get the mean.
      baseline_weights->set_Profile(old_arch->get_Profile(s,ipol,ichan));
      const double baseline_mean = baseline_weights->get_mean().get_value();
      float* bins = old_arch->get_Profile(s,ipol,ichan)->get_amps();

      // Set all the bins between start_bin and end_bin to the mean of the
      // baseline.
      for (unsigned ibin = start_bin; ibin <= end_bin; ++ibin) {
        bins[ibin] = baseline_mean;
      }
    }
  }

  *arch = *old_arch;
  arch->set_dispersion_measure(0);
  arch->pscrunch();
  arch->fscrunch();
  arch->remove_baseline();
}

Reference::To<Pulsar::LawnMower> mower;
unsigned mowing_subint = 0;

bool accept_mow (Pulsar::Profile* profile, Pulsar::PhaseWeight* weight)
{
  Pulsar::ProfilePlot plotter;

  cerr << "mow";

  for (unsigned i=0; i<weight->get_nbin(); i++)
    if ( (*weight)[i] )
      cerr << " " << i;

  cerr << endl;

  plotter.set_selection (weight);

  cpgeras ();
  plotter.plot_profile (profile);

  cerr << "agreed? (y/n)" << endl;

  float x = 0;
  float y = 0;
  char c = 0;
  cpgcurs (&x, &y, &c);

  if (c == 'y') {
    subints_to_mow.push_back( mowing_subint );
    return true;
  }
  else
    return false;
}

void mowlawn (Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint)
{
  if (!mower)
  {
    mower = new Pulsar::LawnMower;
    
    mower->add_precondition
      ( Functor<bool(Pulsar::Profile*,Pulsar::PhaseWeight*)>( &accept_mow ) );
  }

  mowing_subint = subint;
  mower->transform( old_arch->get_Integration(subint) );

  *arch = *old_arch;
  arch->set_dispersion_measure(0);
  arch->pscrunch();
  arch->remove_baseline();
  arch->fscrunch();
}

static Reference::To<Pulsar::LawnMower> prune_mower;

void prune_hedge (Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint)
{
  if (!prune_mower)
    prune_mower = new Pulsar::LawnMower;

  ProfilePlot* use = (zoomed) ? subint_mod_plot : subint_orig_plot;

  PlotFrame* frame = use->get_frame();
  PlotScale* xscale = frame->get_x_scale();
  PlotScale* yscale = frame->get_y_scale();

  MouseType xrange ( std::min(prune_start.first, prune_end.first),
		     std::max(prune_start.first, prune_end.first) );

  MouseType yrange ( std::min(prune_start.second, prune_end.second),
		     std::max(prune_start.second, prune_end.second) );

  std::pair<float,float> prune_x = xscale->viewport_to_world ( xrange );
  std::pair<float,float> prune_y = yscale->viewport_to_world ( yrange );

  unsigned nbin = old_arch->get_nbin();

  PhaseWeight prune_mask (nbin, 0.0);

  cerr << "prune";

  constrain_range( prune_x.first );
  constrain_range( prune_x.second );

  unsigned ibin_start = unsigned (prune_x.first * nbin);
  unsigned ibin_end = unsigned (prune_x.second * nbin);

  assert( use->get_plotter()->profiles.size() == 1 );
  const float* amps = use->get_plotter()->profiles[0]->get_amps();

  for (unsigned i=ibin_start; i<=ibin_end; i++)
    if ( amps[i] > prune_y.first && amps[i] < prune_y.second )
    {
      prune_mask[i] = 1.0;
      cerr << " " << i;
    }

  cerr << endl;

  prune_mower->set_prune( &prune_mask );
  prune_mower->transform( old_arch->get_Integration(subint) );

  *arch = *old_arch;
  arch->set_dispersion_measure(0);
  arch->pscrunch();
  arch->remove_baseline();
  arch->fscrunch();
}
