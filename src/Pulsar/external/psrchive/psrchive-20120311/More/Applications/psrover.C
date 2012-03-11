/***************************************************************************
 *
 *   Copyright (C) 2009,2010,2011 by Stefan Oslowski,Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include <string.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "BoxMuller.h"
#include "Pulsar/ComponentModel.h"
#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"


#define sub_bin 10

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

using Pulsar::Archive;

class psrover : public Pulsar::Application
{
 public:
  //! Default constructor:
  psrover ();

  //! Main loop
  //  void run ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Initial setup
  void setup ();

  //! Handle all the final actions
  void finalize ();

 protected:

  void add_options (CommandLine::Menu&);
  void set_noise (string);
  void set_fwhms (string);
  void set_bins (string);
  void set_ascii_file (string);
  void set_nbins (int);

  time_t seconds;
  long seed;
  BoxMuller gasdev;
  Pulsar::ComponentModel model;

  string ascii_filename;
  string output_filename;
  string out_path;
  long file_count;
  bool sequential_files;
  std::stringstream ss;

  string save_added_noise_filename;
  string save_added_gauss_filename;
  string save_profile_ascii;
  ofstream outfile1;
  ofstream outfile2;
  ofstream outfile3;

  vector<double> noise_to_add;
  vector<double> fwhms;
  vector<double> bins;
  vector<double> profile_values;
  

  float current_bin;
  float tmp_rand;
  float amplitude;

  int nbin;

  bool got_noises;
  bool got_fwhms;
  bool got_bins;
  bool got_ascii_file;
  bool got_nbins;
  bool data_reset;

  bool draw_amplitude;
  bool use_mises;
};

psrover::psrover()
	: Application ("psrover", "program for adding noise to data")
{
  Pulsar::StandardOptions* preprocessor = new Pulsar::StandardOptions;
  add (preprocessor );

  seed = 0;

  data_reset = use_mises = draw_amplitude = got_nbins = got_ascii_file = got_noises = got_fwhms = got_bins = false;

  output_filename = "temp_archive.ar";
  out_path = ".";
  file_count = 0;
  sequential_files = false;
}

void psrover::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("");

  menu.add("");

  arg = menu.add( this, &psrover::set_noise,'r',"noise amplitudes");
  arg->set_help( "coma separated list of noise amplitudes to be added");

  arg = menu.add( seed, 's',"seed");
  arg->set_help( "set seed used for generating random numbers");

  arg = menu.add( use_mises, "vM", "use van Mises distribution");
  arg->set_help( "Force psrover to use van Mises distribution, instead of gaussian function");

  arg = menu.add( draw_amplitude, "fr", "random amplitude");
  arg->set_long_help( "draw normally distributed amplitude in range [-amp, amp], where amp is defined with -f");

  arg = menu.add( this, &psrover::set_fwhms,'f',"fwhms");
  arg->set_help( "coma separated list of FWHM of gaussian peaks");
  arg->set_long_help( "This option will require also to set the centre bins\n"
		  "Negative value means that the corresponding noise amplitude will be the value of delta function\n"
		  "at the corresponding bin, or gaussian noise, in case the bin is negative as well\n");

  arg = menu.add( this, &psrover::set_bins,'b',"bins");
  arg->set_help ("Coma separated list of centre bins for gaussian peaks");
  arg->set_help ("Negative value means that the corresponding noise amplitude is for random noise");

  arg = menu.add( this, &psrover::set_ascii_file,'a',"ascii file");
  arg->set_help ("ascii file with desired underlying profile");
  arg->set_long_help ("if given, it will overwrite the data from archive");

  arg = menu.add( this, &psrover::set_nbins,'n',"number of bins");
  arg->set_help( "the number of bins in the requested profile");
  arg->set_long_help("This will be overriden by the ascii file or archive, if any given");

  arg = menu.add( output_filename,'o',"output file");
  arg->set_help( "name of the output, defaults to temp_archive.ar");

  arg = menu.add( sequential_files, 'O' );
  arg->set_help( "Use a sequence of number as the output filename" );

  arg = menu.add( out_path, 'u', "path" );
  arg->set_help( "Write files to this location" );

  menu.add("");
  arg = menu.add(save_added_noise_filename, "Snoise", "filename");
  arg->set_help("Save the noise added to each bin in ascii file");

  arg = menu.add(save_added_gauss_filename, "Sgauss", "filename");
  arg->set_help("Save the gaussian / von misses amplitude in ascii file");

  arg = menu.add(save_profile_ascii, "Sascii", "filename");
  arg->set_help("Save the resulting profile in ascii file");


  menu.add("");

  menu.add("For example the command:\npsrover -r 100,3,30,65,142 -f 30,-1,250,-1,-1 -b 250,-1,400,800,124 inp.ar -a inp.txt -o out.ar\n"
		  "will take input from the ascii file inp.txt, containing one column, and top of that it will add:\n"
		  "  - first gaussian compoonent at bin 250 with FWHM 30 and amplitude 100\n"
		  "  - seocnd gaussian compoonent at bin 400 with FWHM 250 and amplitude 30\n"
		  "  - first spike at bin 800 with amplitude of 65\n"
		  "  - second spike at bin 124 with amplitude of 142\n"
		  "  - white noise with amplitude of 3\n"
		  " The result will be stored in the file out.ar\n");
  menu.add("");
}

void psrover::set_nbins (int _nbins)
{
  nbin = _nbins;
  got_nbins = true;
}

void psrover::set_noise(string _noise)
{
  for (string sub=stringtok(_noise,","); !sub.empty(); sub=stringtok(_noise,","))
    noise_to_add.push_back(fromstring<float>(sub));
  got_noises = true;
}

void psrover::set_fwhms(string _fwhms)
{
  for (string sub=stringtok(_fwhms,","); !sub.empty(); sub=stringtok(_fwhms,","))
    fwhms.push_back(fromstring<float>(sub));
  got_fwhms = true;
}

void psrover::set_bins(string _bins)
{
  for (string sub=stringtok(_bins,","); !sub.empty(); sub=stringtok(_bins,","))
    bins.push_back(fromstring<float>(sub));
  got_bins = true;
}

void psrover::set_ascii_file (string _file)
{
  if (verbose)
    cerr << "psrover::set_ascii_file setting to " << _file << endl;
  ascii_filename = _file;
  got_ascii_file = true;
}

void psrover::setup ()
{
  if (got_ascii_file) {
    if (verbose)
      cerr << "psrover::setup reading the ascii file " << endl;
    ifstream in_file(ascii_filename.c_str());
    if (in_file.is_open()) {
      while (!in_file.eof()) {
	float f;
	in_file >> f;
	profile_values.push_back(f);
	nbin ++;
      }
      in_file.close();
    } 
    nbin --;
  } //got_ascii_file
  else {
    if (verbose)
      cerr << "psrover::setup defaulting to an empty profile" << endl;
    if (!got_nbins) {
      if (verbose)
       cerr << "psrover::setup no nbins provided, defaulting to 1024" << endl;
      nbin = 1024;
    }
    for (unsigned i = 0 ; i < nbin ; i++) {
      profile_values.push_back(0.0);
    }
  } // didn't get ascii file

  //set the seed
  if (seed == 0)
  {
    if (verbose)
      cerr << "seed from time " << endl;
    seconds = time(NULL);
    gasdev = BoxMuller(seconds);
  }
  else
  {
    if (verbose)
      cerr << "seed from command line " << seed << endl;
    gasdev = BoxMuller(seed);
  }

  //open file for noise
  if (!save_added_noise_filename.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for noise: " << save_added_noise_filename << endl;
    outfile1.open(save_added_noise_filename.c_str());
  }

  //open file for first gaussian
  if (!save_added_gauss_filename.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for the gaussian von misses amplitudes: " << save_added_gauss_filename << endl;
    outfile2.open(save_added_gauss_filename.c_str(), std::ios::app);
  }
  //open file for the ascii profile
  if (!save_profile_ascii.empty())
  {
    if (verbose)
      cerr << "psrover::setup opening output file for the created profile: " << save_profile_ascii << endl;
    outfile3.open(save_profile_ascii.c_str());
  }
}

void psrover::process (Pulsar::Archive* archive)
{
  // default to adding white noise
  if ( !noise_to_add.empty() && fwhms.empty() && bins.empty() )
  {
    cerr << "No fwhm or bin provided - defaulting to white noise" << endl;
    for ( unsigned i_amp = 0; i_amp < noise_to_add.size (); i_amp++ )
    {
      fwhms.push_back( -1 );
      bins.push_back( -1 );
    }
  }

  nbin = archive-> get_nbin();

  if (verbose)
    cerr << "psrover::process getting the amps from " << archive->get_filename() << endl;

  float* data = new float[nbin];
  for ( unsigned i_sub = 0; i_sub < archive->get_nsubint() ;i_sub ++ )
  {
    data = archive->get_Integration(i_sub)->get_Profile(0, 0)->get_amps();

    if (!noise_to_add.empty()) {
      for (unsigned jcomp = 0; jcomp < fwhms.size(); jcomp++ ) {
	if (fwhms[jcomp] >= 0 && bins[jcomp] >= 0)
	{
	  if (draw_amplitude)
	  {
	    tmp_rand = gasdev();
	    amplitude = tmp_rand * noise_to_add[jcomp];
	  }
	  else
	    amplitude = noise_to_add[jcomp];
	  if (!save_added_gauss_filename.empty())
	    outfile2 << amplitude << endl;
	  if (use_mises) {
	    if (verbose)
	    { 
	      cerr << "Adding a von Mises component:" << endl;
	      cerr << "maximum: " << noise_to_add[jcomp] << " peak at the bin: " << bins[jcomp] << " fwhm: " << fwhms[jcomp] << endl;
	    }
	    model.add_component(bins[jcomp]/nbin, 1.0/fwhms[jcomp]/fwhms[jcomp] * 2.35482 * 2.35482 * nbin * nbin / 4 / M_PI / M_PI, amplitude, "");
	  }
	  else 
	  {
	    if (verbose)
	    {
	      cerr << "Adding a gaussian:" << endl;
	      cerr << "maximum: " << amplitude << " peak at the bin: " << bins[jcomp] << " fwhm: " << fwhms[jcomp] << endl;
	    }
	    for (unsigned ibin = 0; ibin < nbin; ++ibin) {
	      // resolve the gaussian with sub_bin resolution:
	      if (!data_reset)
	      {
		if  (verbose && ibin == 0)
		  cerr << "psrover::process Resetting data when adding gaussian" << endl;
		*data = 0.0;
		if (ibin == nbin-1)
		  data_reset = true;
	      }
	      for (unsigned ksubbin = 0; ksubbin < sub_bin; ksubbin++ ) {
		current_bin = (float)((signed)ibin) + 1.0 / sub_bin * (float)ksubbin;
		*data += amplitude / sub_bin * exp( -pow((signed)current_bin - bins[jcomp],2) / 2 / pow(fwhms[jcomp] / 2.35482, 2)) ;
		//wrap the gaussian around
		*data += amplitude / sub_bin * exp( -pow((signed)(current_bin - 1024)-bins[jcomp],2) / 2 / pow(fwhms[jcomp] / 2.35482, 2)) ;
	      }
	      ++data;
	    }
	  }
	  if (use_mises)
	    model.evaluate(data, archive->get_nbin());
	  else
	  {
	    data -= nbin;
	  } 
	}
	else if (fwhms[jcomp] < 0 && bins[jcomp] < 0)
	{
	  for (unsigned ibin = 0; ibin < nbin; ++ibin)
	  {
	    if (!data_reset && !use_mises)
	    {
	      if (verbose && ibin==0)
		cerr << "psrover::process Resetting data when adding noise" << endl;
	      *data = 0.0;
	      if (ibin == nbin-1)
		data_reset = true;
	    }
	    if (verbose && ibin == 0)
	      cerr << "Adding " << noise_to_add[jcomp] << " noise to each bin" << endl;
	    tmp_rand = gasdev();
	    *data += tmp_rand * (float)noise_to_add[jcomp];
	    if (!save_added_noise_filename.empty())
	      outfile1 << ibin << " " << tmp_rand * (float)noise_to_add[jcomp] << endl;
	    ++data;
	  }
	  data -= nbin;
	}
	else if (fwhms[jcomp] < 0 && bins[jcomp] >= 0)
	{
	  for (unsigned ibin = 0; ibin < nbin; ++ibin)
	  {
	    if (!data_reset)
	    {
	      if (verbose && ibin == 0)
		cerr << "psrover::process Resetting data when adding delta function" << endl;
	      *data = 0.0;
	      if (ibin == nbin - 1)
		data_reset = true;
	    }
	    if (ibin == (unsigned)bins[jcomp])
	    {
	      if (verbose)
		cerr << " Adding " << noise_to_add[jcomp] << " to " << ibin << "-th bin" << endl;
	      *data += noise_to_add[jcomp];
	    }
	    ++data;
	  }
	  data -= nbin;
	}
      }
    }
    if (got_ascii_file)
    {
      if (verbose)
	cerr << "psrover::process adding the values from the ascii file to the profile" << endl;
      for (unsigned ibin = 0; ibin < nbin; ++ibin)
      {
	if (!data_reset && !use_mises)
	{
	  if (verbose && ibin == 0)
	    cerr << "psrover::process Resetting data when adding ascii values" << endl;
	  *data = 0.0;
	  if (ibin == nbin - 1)
	    data_reset = true;
	}
	*data += profile_values[ibin];
	++data;
      }
      data -= nbin;
    }

    if (!save_profile_ascii.empty())
    {
      if (verbose)
	cerr << "psrover::process printing profile to an ascii file" << endl;
      float *amps = archive->get_Profile(0,0,0)->get_amps();
      for (unsigned i = 0 ; i < nbin; i++ )
      {
	outfile3 <<  amps[i] << endl;
      }
    }
    data_reset = false;
  }
  if (verbose)
    cerr << "Unloading " << endl;
  if ( !sequential_files )
    archive->unload(out_path+"/"+output_filename);
  else
  {
    ss << out_path << "/" << file_count << ".ar";
    archive->unload( ss.str() );
    ss.str("");
  }
  file_count++;
}

void psrover::finalize()
{
  if (!save_added_noise_filename.empty())
  {
    if (verbose)
      cerr << "psrover::finalize closing " << save_added_noise_filename << endl;
    outfile1.close();
    outfile2.close();
  }
}

int main (int argc, char** argv) 
{
  psrover program;
  return program.main (argc, argv);
}
