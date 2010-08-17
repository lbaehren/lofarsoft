
/*
  GPU correlator
  By Ben Barsdell (2010)
  Modified Andrew Jameson
  
  Features: Arbitrary antenna, freq and sample counts
            Overlapped memory copies and computing [BROKEN!]

  TODO: Replace *accum_size with *accum_size

*/

#include <iostream>
#include <stdexcept>
#include <string>
using std::cout;
using std::cerr;
using std::endl;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "gmrt_delay_fns.h"
#include "gmrt_corr_lib.h"
#include "gmrt_corr_dada.h"
#include "gmrt_corr_cuda.h"
#include "maths_functions.hpp"

#include "ascii_header.h"

int main(int argc, char* argv[])
{

  // Input parameters
	gmrt_corr_params_t params;

	// Profiling variables
	gmrt_corr_profile_t prof;

  // Data arrays
	gmrt_corr_data_t dat;

  // Correlator struct [for delay library]
  CorrType corr;

  // Source struct [for delay library]
  SourceParType   source;

  // Delay and fringe structs [for delay library]
  delay_vals_t    delays[NCHAN*NUM_ANT];
  fringe_vals_t   fringes[NCHAN*NUM_ANT];

  // Source BW
  double BW;

  // Source name
  char source_name[100];

	// Parse command line, fill in params
	if (parse_command_line(argc, argv, &params) < 0) 
	{
		usage();
		return 1;
	}

  // Initialise the delay code
	cout << "Reading antsys header" << endl;
  if (read_antenna_file(params.antsys_hdr, corr.antenna) < 0)
    return EXIT_FAILURE;

  // read the antenna connectivity
	cout << "Reading sampler header" << endl;
  if (read_antenna_connectivity(params.sampler_hdr, &corr) < 0)
    return EXIT_FAILURE;

  // read the source file
	cout << "Reading source header" << endl;
  if (read_source_file(params.source_hdr, &source, &BW, source_name) < 0)
    return EXIT_FAILURE;

	// Initialize cuda events
	cout << "Initialising cuda events" << endl;
	gmrt_corr_cuda_create_events(&prof);

	// initialise data arrays
	cout << "Initialising data arrays" << endl;
	if (gmrt_corr_data_init(&dat, params) < 0) 
	{
		cerr << "Failed to intialize data structures" << endl;
		return EXIT_FAILURE;
	}

	// Allocate CUDA memory
  cout << "Allocating CUDA memory" << endl;
	gmrt_corr_cuda_alloc(&dat, params);

	if (params.use_datablock)
  {
    cout << "Opening DADA datablocks" << endl;
		gmrt_corr_dada_open(&dat, params);
  }

  cout << "Runnning correlator" << endl;
	float time = gmrt_corr_cuda_run(&dat, &params, &prof, &source, &corr);
	
	// Free memory
	gmrt_corr_cuda_dealloc(&dat, params);
	gmrt_corr_data_dealloc(&dat, params);

#ifdef _PROFILE
	cout << "h2d time: " << prof.h2d_time << "ms" << endl;
	cout << "upk time: " << prof.unpack_time << "ms" << endl;
	cout << "fft time: " << prof.fft_time << "ms" << endl;
	cout << "phs time: " << prof.phase_time << "ms" << endl;
	cout << "mac time: " << prof.mac_time << "ms" << endl;
	cout << "io  time: " << prof.io_time << "ms" << endl;
	cout << "  => performance: "
       << prof.run_count*8*params.ant_count*(params.ant_count+1)/2*params.science_accum_size*params.freq_count / prof.mac_time / 1e6
	     << " GFLOP/s" << endl;
	cout << "d2h time: " << prof.d2h_time << "ms" << endl;
	float tot_time = prof.h2d_time+prof.unpack_time+prof.phase_time+/*prof.fft_time+*/prof.mac_time+prof.d2h_time;
	cout << "tot time: " << tot_time << "ms" << endl;
  cout << "run_count: " << prof.run_count << endl;
  cout << "science_accum_size: " << params.science_accum_size << endl;
  cout << "freq_count: " << params.freq_count << endl;
	cout << "  => science bandwidth (no fft): "
	     << prof.run_count*params.science_accum_size*params.freq_count/tot_time/1e3 << " MHz" << endl;
	cout << "  => science bandwidth:          "
	     << prof.run_count*params.science_accum_size*params.freq_count/(tot_time+prof.fft_time)/1e3 << " MHz" << endl;
  cout << "  => science bandwidth (no h2d): "
       << prof.run_count*params.science_accum_size*params.freq_count/(tot_time+prof.fft_time-prof.h2d_time)/1e3
       << " MHz" << endl;

#endif
	
	cout << "CPU tot time: " << time << "s" << endl;
	cout << "  => science bandwidth:          "
       << prof.run_count*params.science_accum_size*params.freq_count/time/1e6 << " MHz" << endl;

	
	cout << "Done." << endl;
	return 0;
}
