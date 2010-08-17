

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
using std::cout;
using std::cerr;
using std::endl;
#include <cstdlib>

#include "gmrt_corr_lib.h"
#include "gmrt_corr_cuda.h"

#include "kernel_defs.hpp"
#include "maths_functions.hpp"
#include "device_interface.hpp"
#include "fill_device.hpp"
#include "unpack_device.hpp"
#include "phase_shift_device.hpp"
#include "fft_device.hpp"
#include "mac_device.hpp"
#include "stopwatch.hpp"


inline void start_profiling(const cudaEvent_t& start_event,
                            const cudaStream_t& stream=0)
{
#ifdef _PROFILE
  cudaEventRecord(start_event, stream);
#endif

}
inline float stop_profiling(const cudaEvent_t& start_event,
                            const cudaEvent_t& stop_event,
                            const cudaStream_t& stream=0)
{
  float elapsed = 0.f;
#ifdef _PROFILE
  cudaEventRecord(stop_event, stream);
  cudaEventSynchronize(stop_event);
  cudaEventElapsedTime(&elapsed, start_event, stop_event);
#endif
  return elapsed;
}


int gmrt_corr_cuda_create_events(gmrt_corr_profile_t * p)
{
	setDevice(0);
  cudaEventCreate(&(p->start_event));
  cudaEventCreate(&(p->stop_event));
  p->h2d_time = 0;
  p->d2h_time = 0;
  p->unpack_time = 0;
  p->mac_time = 0;
  p->fft_time = 0;
  p->phase_time = 0;
  p->run_count = 0;
  p->io_time = 0;
	return 0;
}

int gmrt_corr_cuda_alloc(gmrt_corr_data_t * d, gmrt_corr_params_t p)
{

	size_t q;
	for( q=0; q<p.max_queue_count; ++q ) {
    createQueue(d->queues[q]);
    mallocHost(  d->h_raw[q], p.max_raw_word_count * p.ant_count * sizeof(RawWord));
    mallocDevice(d->d_raw[q], p.max_raw_word_count * p.ant_count * sizeof(RawWord));
    mallocDevice(d->d_unpacked[q], p.max_unpacked_count*p.ant_count*sizeof(Real));
  }
  mallocDevice(d->d_output, p.output_count*sizeof(Complex));
  mallocHost(  d->h_output, p.output_count*sizeof(Complex));

	return 0;
}

int gmrt_corr_cuda_dealloc(gmrt_corr_data_t * d, gmrt_corr_params_t p)
{
	freeDevice(d->d_output);
  freeHost(d->h_output);
  for( size_t q=0; q<p.max_queue_count; ++q ) {
    freeDevice(d->d_unpacked[q]); 
    freeDevice(d->d_raw[q]);
    freeHost(d->h_raw[q]);
    destroyQueue(d->queues[q]);
  }
	return 0;
}

float gmrt_corr_cuda_run(gmrt_corr_data_t * d, gmrt_corr_params_t * p, gmrt_corr_profile_t * prof,
                         SourceParType *source, CorrType * corr)
{
  Complex zero;
  zero.x = 0.f;
  zero.y = 0.f;
  
  Stopwatch timer;
  Stopwatch iotimer;
  timer.start();

  // Delay and fringe structs [for delay library]
  //delay_vals_t    delays[NCHAN*NUM_ANT];
  //fringe_vals_t   fringes[NCHAN*NUM_ANT];

  size_t bytes_to_read;
  size_t bytes_read;
  
  bool finished = false;
  // Begin run loop     
  while( !finished ) {
    prof->run_count++;
    if (p->verbose)
    {
      cout << "Beginning new accumulation..." << endl;
      cout << "=============================" << endl;
    }
    // Zero the accumulation result
    fill_device(d->d_output, d->d_output+p->output_count, zero);
    // Iterate over accumulation gulps
    for( size_t accum=0; accum<p->science_accum_size; accum+=p->gulp_accum_size ) {
      // Store gulp size, dealing with incomplete final gulp
      p->gulp_accum_size = math::min(p->gulp_accum_size, p->science_accum_size-accum);
      if (p->verbose)
      {
        cout << "Beginning new gulp accumulating " << p->gulp_accum_size << " cross correlations" << endl;
        cout << "---------------------------------------------------" << endl;
      }
        
      size_t queue_count = 0;
      // Iterate over queues in the gulp
      for( size_t queue=0; queue<p->gulp_accum_size; queue+=p->max_queue_accum_size ) {
        // Store queue's size, dealing with incomplete final queue
        d->queue_accum_size[queue_count] = math::min(p->max_queue_accum_size, p->gulp_accum_size-queue);
        // Calculate the unpack count and raw word count for this queue
        d->unpacked_count[queue_count] = d->queue_accum_size[queue_count] * 2*p->freq_count;
        d->raw_word_count[queue_count] = d->unpacked_count[queue_count] / SAMPLES_PER_WORD;
        if (p->verbose)
        {
          cout << "Using new queue accumulating " << d->queue_accum_size[queue_count] << " cross correlations" << endl;
          cout << "  => " << d->unpacked_count[queue_count] << " samples" << endl;
          cout << "  => " << d->raw_word_count[queue_count] << " raw words" << endl;
        }
        // Track the number of queues used for this gulp
        ++queue_count;       
      }
      if (p->verbose)
      {
        cout << "Total queues used in this gulp: " << queue_count << endl;
        cout << "Beginning data load..." << endl;
      }
      // Load data
      for( size_t q=0; q<queue_count; ++q ) {
        if (p->verbose)
          cout << "Queue " << q << " reading data from source..." << endl;
        iotimer.start();
        for( size_t ant=0; ant<p->ant_count; ++ant ) {
          // Read data from source
          bytes_to_read = d->raw_word_count[q];
          //bytes_to_read = d->raw_word_count[q]*sizeof(RawWord);
          bytes_read = d->io_function(d, ant, (char *) (d->h_raw[q] + ant*d->raw_word_count[q]), bytes_to_read);
          //fprintf(stderr, "ant=%d, bytes_to_read=%d\n", ant, bytes_to_read);
          //bytes_read = d->io_function(d, ant, (char *) (d->h_raw[q]+ant*d->raw_word_count[q]), bytes_to_read);
          if (bytes_to_read != bytes_read) 
          {
            if (p->verbose)
              cerr << "EOD detected maybe??" << endl;
            finished = true;
          }
        }
        iotimer.stop();
        if (p->verbose)
          cout << "Queue " << q << " copying data to device..." << endl;
        // Copy to device
        start_profiling(prof->start_event, d->queues[q]);
        copyHostToDevice(d->d_raw[q], d->h_raw[q],
                         d->raw_word_count[q]*p->ant_count*sizeof(RawWord),
                         d->queues[q]);
        prof->h2d_time += stop_profiling(prof->start_event, prof->stop_event, d->queues[q]);
      }
      if (p->verbose)
        cout << "Beginning compute..." << endl;
      // Compute
      for( size_t q=0; q<queue_count; ++q ) {
        if (p->verbose)
          cout << "Queue " << q << " unpacking data..." << endl;
        // Unpack
        start_profiling(prof->start_event, d->queues[q]);
        // TODO: Check this!
        /* // This unpacks antenna-interlaced data from the IBOB
        unpack_to_real_ibob_device((unsigned int*)d->d_raw[q],
                              (float4*)d->d_unpacked[q],
                              d->raw_word_count[q],
                              p->ant_count,
                              math::log2(p->ant_count),
                              d->unpacked_count[q]);
        */
        // This unpacks separate time series from each antenna
        //unpack_to_complex_device(d->d_raw[q], d->d_unpacked[q],
        //                         d->raw_word_count[q]*p->ant_count, d->queues[q]);
        unpack_to_real_device(d->d_raw[q], d->d_unpacked[q], 
                              d->raw_word_count[q]*p->ant_count,
                              sizeof(RawWord)*8/SAMPLES_PER_WORD,
                              d->queues[q]);
        
        /*
        // HACK to write unpacked first antenna to file
        copyDeviceToHost(h_unpacked[q], d->d_unpacked[q],
                         d->unpacked_count[q]*sizeof(Real),
                         d->queues[q]);
        synchronize(d->queues[q]);
        std::ofstream upk_file("unpacked_0.dat");
        for( size_t u=0; u<d->unpacked_count[q]; ++u ) {
          upk_file << h_unpacked[q][u] << "\n";
        }
        upk_file.close();
        */
        prof->unpack_time += stop_profiling(prof->start_event, prof->stop_event, d->queues[q]);
       
        if (p->verbose)
        {  
          cout << "Queue " << q << " FFTing data" << endl;
          cout << "  batch: " << d->queue_accum_size[q] * p->ant_count << endl;        
        }
        // FFT
        //if( d->queue_accum_size[q] != p->max_queue_accum_size ) {
        //  cout << "**** ERROR: Need to implement on-the-fly fft plan creation!" << endl;  
        //}
        start_profiling(prof->start_event, d->queues[q]);
        fft_r2c_short_device(d->d_unpacked[q], (Complex*)d->d_unpacked[q],
                             2*p->freq_count,
                             d->queue_accum_size[q] * p->ant_count,
                             d->queues[q]);
        prof->fft_time += stop_profiling(prof->start_event, prof->stop_event, d->queues[q]);
        
        // Phase shift
        /*
        start_profiling(prof->start_event, d->queues[q]);
       
        // need to calculate the fractional delay and apply  
        phase_shift_device((Complex*)d->d_unpacked[q],
                           (Complex*)d->d_unpacked[q],
                           d->unpacked_count[q]/2 * p->ant_count,
                           0.1f, // TODO: Insert delay here
                           d->queues[q]);
        prof->phase_time += stop_profiling(prof->start_event, prof->stop_event, d->queues[q]);
        */
        if (p->verbose)
          cout << "Queue " << q << " MACing data..." << endl;
        // MAC
        start_profiling(prof->start_event, d->queues[q]);
        size_t in_stride1  = p->freq_count;
        size_t in_stride2  = d->unpacked_count[q] / 2; // Complex samples
        size_t out_stride1 = p->freq_count;
        size_t out_stride2 = p->ant_count * out_stride1;
        mac_device((Complex*)d->d_unpacked[q], in_stride1, in_stride2,
                   p->ant_count, p->freq_count, d->queue_accum_size[q],
                   d->d_output, out_stride1, out_stride2, p->do_full_polar,
                   d->queues[q]);
        
        // HACK for testing half_floats
        /*mac_device((ushort2*)d->d_unpacked[q], in_stride1, in_stride2,
                   p->ant_count, p->freq_count, d->queue_accum_size[q],
                   (ushort2*)d_output, out_stride1, out_stride2, p->do_full_polar,
                   d->queues[q]);
        */
        prof->mac_time += stop_profiling(prof->start_event, prof->stop_event, d->queues[q]);
      }
    } // End of iteration over accumulation gulps
    
    if (p->verbose)
    {
      cout << "===========================" << endl;
      cout << "End of accumulation reached" << endl;
    }
      
    // TODO: Could compute second half of spectrum here before
    //       copying back to host.
   
    if (p->verbose) 
      cout << "Retrieving results from device..." << endl;
    // Copy back to host
    start_profiling(prof->start_event);
    copyDeviceToHost(d->h_output, d->d_output,
                     p->output_count*sizeof(Complex));
    prof->d2h_time += stop_profiling(prof->start_event, prof->stop_event);
      
    if (p->verbose)
      cout << "Writing output to destination..." << endl;
    // Output
    synchronize(); // Wait for device->host copy to finish

    if (p->write_output && !finished) {
      // HACK to avoid waiting ages when benchmarking
      if( p->ant_count <= 16 ) {
        size_t ij = 0;
        for( size_t i=0; i<p->ant_count; ++i ) {
          // Note: This is a little trick to divide by either 1 (single-pol)
          //       or 2 (full-polar) when skipping terms near the diagonal.
          size_t b = p->do_full_polar+1;
          for( size_t j=i/b*b; j<p->ant_count; ++j ) {
            ++ij;
            // Open the output destinations
            std::stringstream ss1, ss2;
            //ss1 << "spec_" << ij;
            ss1 << "spec_" << i << "_" << j;
            std::ofstream spectrum_out(ss1.str().c_str());
            spectrum_out.precision(5); // sig figs
            //ss2 << "phase_" << ij;
            ss2 << "phase_" << i << "_" << j;
            std::ofstream phase_out(ss2.str().c_str());
            phase_out.precision(5); // sig figs
            // Iterate over each frequency channel
            for( size_t f=0; f<p->freq_count; ++f ) {
              Complex cij = d->h_output[f+p->freq_count*(i+j*p->ant_count)];
              // Normalise the accumulations
              cij.x /= p->science_accum_size;
              cij.y /= p->science_accum_size;
              // Write the amplitude and phase
              spectrum_out << sqrt(cij.x*cij.x + cij.y*cij.y) << endl;
              // HACK TODO: Work out why this is needed to match old results
              cij.y *= -1;
              phase_out    << atan2(cij.y, cij.x) << endl;
            }
          }
        }
      }
    }
    // TODO: Remove this to run continuously
    //finished = true;

  } // End of run loop
  prof->io_time = iotimer.getTime() * 1000;
  timer.stop();

  return timer.getTime();
}
