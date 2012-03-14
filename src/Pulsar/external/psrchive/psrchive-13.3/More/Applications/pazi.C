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
#include "Pulsar/Plot.h"
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

#define HORIZONTAL_BAND 3
#define VERTICAL_BAND 4
#define ZERO_PAIR std::pair<float, float>(0.0, 0.0)

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
    "  zap:                       right click\n"
    "  zap (multiple):            left click, then 'z'\n"
    "  reset zoom:                'r'\n"
    "  frequency:                 'f'\n"
    "  time:                      't'\n"
    "  save (<filename>.pazi):    's'\n"
    "  quit:                      'q'\n"
    "  print paz command:         'p'\n"
    "  center pulse:              'c'\n"
    "  undo last:                 'u'\n"
    "  toggle dedispersion:       'd'\n"
    "  binzap:                    'b'\n"
    "  mow the lawn:              'm'\n"
    "\n"
    
    "See "PSRCHIVE_HTTP"/manuals/pazi for more details\n" << endl;
}

void constrain_range(float& f);

string get_zoom_option(const RangeType& ranges, const unsigned max_value);

RangeType get_range(const MouseType& mouse_ref, const MouseType& mouse,
    const RangeType& ranges, const bool horizontal);

unsigned get_subint_indexed_value(const MouseType& mouse);

void zap_bin(Pulsar::Archive* arch, Pulsar::Archive* old_arch, const unsigned subint,
    const unsigned bin);

unsigned get_max_value(const Pulsar::Archive* archive, const int plot_type);

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
void mowlawn (Pulsar::Archive* arch, Pulsar::Archive* old_arch, int subint);

static bool dedispersed = true;
static vector<int> channels_to_zap;
static vector<int> subints_to_zap;
static vector<int> bins_to_zap;
static vector<int> subints_to_mow;

RangeType ranges(0, 0);
bool positive_direction = true;

//static bool centered = true;

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

  // where the mouse clicks are stored
  MouseType mouse(0.0, 0.0);
  MouseType mouse_ref(0.0, 0.0);

	int subint = 0;
	char ch;
	bool zoomed = false;
	bool fscrunched = true;
	string filename = argv[optind];
	string extension = filename.substr(filename.length() - 2, 2);

	int plot_type = 0; // 0 = time, 1 = freq, 2 = subint

	if ((extension != "rf") && (extension != "cf")) {
		extension = "pazi";
	} else {
		if (extension == "rf") {
			extension = "rz";
		} else {
			extension = "cz";
		}
	}

	string write_filename = filename + ".";
	write_filename += extension;

	cerr << "pazi: loading data" << endl;
	Reference::To<Pulsar::Archive> base_archive = Archive::load(filename);

	if (base_archive->get_npol() == 4)
	  base_archive->convert_state( Signal::Stokes );

	Reference::To<Pulsar::Archive> backup_archive = base_archive->clone();

	cerr << "pazi: making fscrunched clone" << endl;
	Reference::To<Pulsar::Archive> mod_archive = base_archive->clone();
	mod_archive->pscrunch();
	mod_archive->remove_baseline();
	mod_archive->dedisperse();
	mod_archive->fscrunch();

	Reference::To<Pulsar::Archive> scrunched_archive = mod_archive->clone();
	scrunched_archive->tscrunch();

  ranges.second = get_max_value(base_archive, plot_type);
  positive_direction = base_archive->get_bandwidth() < 0.0;

	Plot *time_orig_plot = factory.construct("time");
	Plot *time_mod_plot = factory.construct("time");
	TextInterface::Parser* time_fui = time_mod_plot->get_frame_interface();

	Plot *freq_orig_plot = factory.construct("freq");
	Plot *freq_mod_plot = factory.construct("freq");
	TextInterface::Parser* freq_fui = freq_mod_plot->get_frame_interface();

	Plot *total_plot = factory.construct("flux");
	total_plot->configure("info=1");

	Plot *subint_orig_plot = factory.construct("flux");
	Plot *subint_mod_plot = factory.construct("flux");
	TextInterface::Parser* subint_fui = subint_mod_plot->get_frame_interface();
	subint_orig_plot->configure("info=1");
	subint_mod_plot->configure("info=1");

	cpgopen("1/XS");
	cpgopen("2/XS");
	cpgask(0);

	cerr << endl << "Total S/N = " <<
    scrunched_archive->get_Profile(0,0,0)->snr() << endl << endl;

	total_plot->plot(scrunched_archive);
	cpgslct(1);
	time_orig_plot->plot(mod_archive);

	do {
    cpgswin(0, 1, 0, 1);

    // plot:
    //   frequency = horizontal mouse band
    //   time      = horizontal mouse band
    //   profile   = vertical mouse band
    const int band = plot_type == 2 ? VERTICAL_BAND : HORIZONTAL_BAND;
    cpgband(band, 0, mouse_ref.first, mouse_ref.second, &(mouse.first),
        &(mouse.second), &ch);

		switch (ch) {
      case 'A': // zoom
        {
          constrain_range(mouse.first);
          constrain_range(mouse.second);

          if (mouse_ref == ZERO_PAIR) { // if position anchor has not been set,
            mouse_ref = mouse; // set it
            continue;
          }


          // store the current range so it can be restored if the user selects
          // a zoom region too small
          const RangeType old_ranges = ranges;
          bool horizontal = plot_type == 2 ? false : true;
          ranges = get_range(mouse_ref, mouse, ranges, horizontal);

          // ignore mouse clicks if the index values are too close (< 1)
          if (ranges.first == ranges.second) {
            mouse_ref = ZERO_PAIR;
            ranges = old_ranges;
            continue;
          }

          zoomed = true;

          const unsigned max_value = get_max_value(base_archive, plot_type);
          const string zoom_option = get_zoom_option(ranges, max_value);

          switch (plot_type) {
            case 0: // time
              time_fui->set_value("y:range", zoom_option);
              redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
              break;
            case 1: // freq
              freq_fui->set_value("y:range", zoom_option);
              freq_redraw(mod_archive, base_archive, freq_orig_plot,
                  freq_mod_plot, zoomed);
              break;
            case 2: // subint
              subint_fui->set_value("x:range", zoom_option);
              redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
              break;
          }
          mouse_ref = ZERO_PAIR;
        }
        break; // case 'A'
		case 'h':
		  usage();
		  break;

    case 'b': // plot specific subint
      if (plot_type == 0) {
        plot_type = 2;
        zoomed = false;
        mouse_ref = ZERO_PAIR;

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

				if (plot_type == 1) {
					mod_archive->tscrunch();
					redraw(mod_archive, freq_orig_plot, freq_mod_plot, zoomed);
				} else if (plot_type == 0) {
					mod_archive->fscrunch();
					redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
				}

				update_total(scrunched_archive, base_archive, total_plot);
				break;

			case 'f': // frequency plot
				plot_type = 1;
        ranges.first = 0;
        ranges.second = get_max_value(base_archive, plot_type);
				zoomed = false;
				freq_redraw(mod_archive, base_archive, freq_orig_plot, freq_mod_plot, zoomed);
				break;

      case 'm':
        if (plot_type != 2) {
          cerr << "pazi: can only mow lawn of subint" << endl;
          continue;
        }

        cerr << "pazi: mowing lawn" << endl;
        mowlawn (mod_archive, base_archive, subint);

        cerr << "pazi: replotting" << endl;
        redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
        cerr << "pazi: updating total" << endl;
        update_total(scrunched_archive, base_archive, total_plot);

        break;

			case 'o': // toggle frequency scrunching on/off
				if (plot_type == 0) {
					if (fscrunched) {
						fscrunched = false;
						*mod_archive = *base_archive;
					} else {
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
          case 0:
            redraw(mod_archive, time_orig_plot, time_mod_plot, zoomed);
            break;
          case 1:
            freq_redraw(mod_archive, base_archive, freq_orig_plot,
                freq_mod_plot, zoomed);
            break;
          case 2:
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
				plot_type = 0;
        ranges.first = 0;
        ranges.second = get_max_value(base_archive, plot_type);
				zoomed = false;
				time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot, zoomed);
				break;

      case 'u': // undo last change
        if (mouse_ref != ZERO_PAIR) {
          mouse_ref = ZERO_PAIR;
          continue;
        }

        switch (plot_type) {
          case 0:
            {
              const unsigned value = get_indexed_value(mouse);
              remove_channel(value, subints_to_zap);
              time_unzap_subint(base_archive, backup_archive, value);
              time_redraw(mod_archive, base_archive, time_orig_plot,
                  time_mod_plot, zoomed);
            }
            break;
          case 1:
            {
            const unsigned value = get_indexed_value(mouse);
            remove_channel(value, channels_to_zap);
            freq_unzap_chan(base_archive, backup_archive, value);
            freq_redraw(mod_archive, base_archive, freq_orig_plot,
                freq_mod_plot, zoomed);
            }
            break;
          case 2:
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

      case 'X': // zap single channel
        if (mouse_ref != ZERO_PAIR) {
          mouse_ref = ZERO_PAIR;
          continue;
        }

        *backup_archive = *base_archive;

        switch (plot_type) {
          case 0:
            {
              const unsigned value = get_indexed_value(mouse);
              time_zap_subint(base_archive, value);
              time_redraw(mod_archive, base_archive, time_orig_plot, time_mod_plot,
                  zoomed);
              break;
            }
          case 1:
            {
              const unsigned value = get_indexed_value(mouse);
              freq_zap_chan(base_archive, value);
              freq_redraw(mod_archive, base_archive, freq_orig_plot,
                  freq_mod_plot, zoomed);
              break;
            }
          case 2: // subint
            {
              const unsigned value = get_subint_indexed_value(mouse);
              zap_bin(mod_archive, base_archive, subint, value);
              redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
              break;
            }
        }

        update_total(scrunched_archive, base_archive, total_plot);
        break;
			case 'z': // zap multiple channels
        if (mouse_ref == ZERO_PAIR) {
          continue;
        }

        *backup_archive = *base_archive;

        const bool horizontal = plot_type == 2 ? false : true;
        RangeType range_to_zap = get_range(mouse_ref, mouse, ranges, horizontal);

        switch (plot_type) {
          case 0: // time
            {
              for (unsigned i = range_to_zap.first; i <= range_to_zap.second; ++i) {
                time_zap_subint(base_archive, i);
              }

              time_redraw(mod_archive, base_archive, time_orig_plot,
                  time_mod_plot, zoomed);
            }
            break;
          case 1: // freq
            {
              for (unsigned i = range_to_zap.first; i <= range_to_zap.second; ++i) {
                freq_zap_chan(base_archive, i);
              }

              freq_redraw(mod_archive, base_archive, freq_orig_plot,
                  freq_mod_plot, zoomed);
            }
            break;
          case 2:
            {
              for (unsigned i = range_to_zap.first; i <= range_to_zap.second; ++i) {
                zap_bin(mod_archive, base_archive, subint, i);
              }

              redraw(mod_archive, subint_orig_plot, subint_mod_plot, zoomed);
            }
            break;
        }

        mouse_ref = ZERO_PAIR;
        update_total(scrunched_archive, base_archive, total_plot);
        break;
		}
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

unsigned get_max_value(const Pulsar::Archive* archive, const int plot_type)
{
  switch (plot_type) {
    case 0:
      return archive->get_nsubint();
    case 1:
      return archive->get_nchan();
    case 2:
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

void zap_bin(Pulsar::Archive* arch, Pulsar::Archive* old_arch, const unsigned subint,
    const unsigned bin)
{
  const unsigned npol = old_arch->get_npol();
  const unsigned nchan = old_arch->get_nchan();

  for (unsigned ipol = 0; ipol < npol; ++ipol) {
    for (unsigned ichan = 0; ichan < nchan; ++ichan) {

      const double baseline_mean =
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

static Pulsar::LawnMower* mower = 0;
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
  if (!mower) {
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
