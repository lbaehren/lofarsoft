#include <cuda_runtime.h>

#include "dada_def.h"
#include "dada_hdu.h"

#ifndef __gmrt_corr_lib_h
#define __gmrt_corr_lib_h

#ifdef __cplusplus
extern "C" {
#endif

// Note: What we refer to as "queues" are really CUDA streams
typedef cudaStream_t   Queue;
typedef float          Real;
typedef float2         Complex;
typedef unsigned char  RawWord; // Implies 4-bit complex components

typedef struct 
{
  size_t sample_count;
  size_t freq_count;
  size_t ant_count;
  size_t max_queue_count;
  int    do_full_polar;
  size_t science_accum_size;
  size_t output_count;
  size_t gulp_accum_size;
  size_t max_queue_accum_size;
  size_t max_unpacked_count;
  size_t max_raw_word_count;
  int    verbose;
  int    use_datablock;
  int    write_output;
  char * dada_key_list;
  char * data_file_list;
  char * antsys_hdr;
  char * sampler_hdr;
  char * source_hdr;
} gmrt_corr_params_t;


typedef struct
{
  cudaEvent_t start_event;
  cudaEvent_t stop_event;
  float       h2d_time;
  float       d2h_time;
  float       unpack_time;
  float       mac_time;
  float       fft_time;
  float       phase_time;
  float       run_count;
  float       io_time;
} gmrt_corr_profile_t;

typedef struct gmrt_corr_data
{
  size_t *      queue_accum_size;
  size_t *      unpacked_count;
  size_t *      raw_word_count;
  Queue *       queues;
  RawWord **    h_raw;
  RawWord **    d_raw;
  Real **       d_unpacked;
  Complex *     d_output;
  Complex *     h_output;
  int *         input_sources;
  key_t *       input_keys;
  dada_hdu_t ** hdus;
  size_t (*io_function) (struct gmrt_corr_data *, unsigned ant, char * address, size_t size);
} gmrt_corr_data_t;

enum { SAMPLES_PER_WORD = 1 };

void usage();

int parse_command_line(int argc, char ** argv, gmrt_corr_params_t * params);

int gmrt_corr_data_init(gmrt_corr_data_t * d, gmrt_corr_params_t p);

int gmrt_corr_data_dealloc(gmrt_corr_data_t * d, gmrt_corr_params_t p);

size_t gmrt_corr_file_read(gmrt_corr_data_t * d, unsigned ant, char *address, size_t size);

int gmrt_corr_dada_open(gmrt_corr_data_t * d, gmrt_corr_params_t p);

size_t gmrt_corr_dada_read(gmrt_corr_data_t * d, unsigned ant, char * address, size_t size);

#ifdef __cplusplus
}
#endif

#endif
