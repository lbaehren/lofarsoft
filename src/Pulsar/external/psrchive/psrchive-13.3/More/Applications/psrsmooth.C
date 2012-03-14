/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Transformation.h"
#include "Pulsar/AdaptiveSmooth.h"
#include "Pulsar/WaveletSmooth.h"

using namespace Pulsar;

//
//! Profile adaptive smoothing application
//
class psrsmooth : public Pulsar::Application
{
public:

  //! Default constructor
  psrsmooth ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! smoothing method to use
  std::string method;

  //! Extension to append
  std::string ext;

  //! Wavelet class/order
  std::string wavelet_type;

  //! Cutoff (number of sigma)
  float wavelet_cutoff;

  //! Normalize outputs?
  bool normalize;

  //! Harmonics output
  bool print_harm;

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  void use_wavelet () { method = "wavelet"; }
};


psrsmooth::psrsmooth ()
  : Application ("psrsmooth", "Denoise profiles using adaptive smoothing")
{
  method = "sinc";
  ext = "sm";
  normalize = false;
  print_harm = false;
  wavelet_type = "UD8";
  wavelet_cutoff = 1.3;
}

void psrsmooth::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (this, &psrsmooth::use_wavelet, 'W');
  arg->set_help ("Use Wavelet smoothing (default Sinc)");

  arg = menu.add(wavelet_type, 't');
  arg->set_help ("Use specified wavelet type");

  arg = menu.add(wavelet_cutoff, 'c');
  arg->set_help ("Set wavelet cutoff factor");

  arg = menu.add (normalize, 'n');
  arg->set_help ("Normalize smoothed profile");

  arg = menu.add (ext, 'e', "ext");
  arg->set_help ("Append extention to output (default .sm)");

  arg = menu.add (print_harm, 'H');
  arg->set_help ("Only print the smoothed number of harmonics");
}

void psrsmooth::process (Pulsar::Archive* archive)
{

  // We want stokes for 4-pol data, otherwise
  // whatever is input is probably ok.
  if (archive->get_npol()==4)
    archive->convert_state(Signal::Stokes);

  // Set up transformation
  Reference::To< Transformation<Profile> > smooth;
  Reference::To<AdaptiveSmooth> asmooth = NULL;
  Reference::To<WaveletSmooth> wsmooth = NULL;

  // Print harmonics needs Sinc method
  if (print_harm) method = "sinc";

  if (method=="sinc")
    smooth = new AdaptiveSmooth;
  else if (method=="wavelet")
    smooth = new WaveletSmooth;
  else 
    throw Error(InvalidState, "psrsmooth::process",
        "Unrecognized smoothing method (%s) selected", method.c_str());

  asmooth = dynamic_cast<AdaptiveSmooth*>(smooth.get());
  wsmooth = dynamic_cast<WaveletSmooth*>(smooth.get());

  if (wsmooth) {
    wsmooth->set_wavelet(wavelet_type);
    wsmooth->set_cutoff(wavelet_cutoff);
  }

  Reference::To<Integration> subint;

  for (unsigned isub=0; isub<archive->get_nsubint(); isub++) {
    subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<subint->get_nchan(); ichan++) {

      // Smooth pol 0
      (*smooth)(subint->get_Profile(0,ichan));

      // Calc norm factor from pol0
      // Normalize so sum of squares is 1.0 (not counting DC).
      double norm=1.0;
      if (normalize) {
        Profile *p = subint->get_Profile(0,ichan);
        double dc = p->sum() / (double)(p->get_nbin());
        p->offset(-1.0 * dc);
        norm = 1.0 / sqrt(p->sumsq());
        p->offset(dc);
        p->scale(norm);
      }

      if (print_harm) 
        cout << archive->get_filename() << " " << isub << " " 
          << ichan << " " << asmooth->get_max_harm() << endl;

      // if we're using Fourier-domain, apply same filter to all 
      // other pols
      if (asmooth) asmooth->set_hold(true);

      // Loop over other pols
      for (unsigned ipol=1; ipol<subint->get_npol(); ipol++) {
        (*smooth)(subint->get_Profile(ipol,ichan));
        if (normalize) subint->get_Profile(ipol,ichan)->scale(norm);
      }

      // Reset hold if needed
      if (asmooth) asmooth->set_hold(false);
    }
  }

  // Unload archive with .sm extension
  if (!print_harm)
    archive->unload(archive->get_filename() + "." + ext);

}

static psrsmooth program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

