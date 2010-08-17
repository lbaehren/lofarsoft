#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include "gmrt_corr_lib.h"
#include "ascii_header.h"

/*
 * parse the command line params and fill in the params struct
 */
int parse_command_line(int argc, char ** argv, gmrt_corr_params_t * params) 
{

	int arg = 0;

	params->verbose         = 0;
	params->use_datablock   = 0;
  params->max_queue_count = 4;
  params->do_full_polar   = 0;
  params->gulp_accum_size = 1024;
	params->dada_key_list   = 0;
	params->data_file_list  = 0;
	params->write_output    = 0;
  params->antsys_hdr      = "antsys.hdr";
  params->source_hdr      = "source.hdr";
  params->sampler_hdr     = "sampler.hdr";

	while ((arg=getopt(argc,argv,"f:g:hk:pq:wv")) != -1)
	{
    switch (arg) {

      case 'a':
        if (optarg)
          params->antsys_hdr = strdup(optarg);
        else
        {
          fprintf(stderr,"ERROR: no antsys header file specified");
          return -1;
        }
        break;

			case 'f':
				if (optarg)
					params->data_file_list = strdup(optarg);
				else
				{
					fprintf(stderr,"ERROR: no data file list specified");
					return -1;
				}
        break;

    	case 'k':
				if (optarg)
					params->dada_key_list = strdup(optarg);	
				else
				{
					fprintf(stderr, "ERROR: no datablock key list specified");
					return -1;
				}
				params->use_datablock = 1;
      	break;

    	case 'g':
				if (optarg)
      		params->gulp_accum_size = atoi(optarg);
				else 
				{
					fprintf(stderr, "ERROR: no gulp size specified");
					return -1;
				}
      	break;

      case 'h':
        return -1;

      case 'o':
        if (optarg)
          params->source_hdr = strdup(optarg);
        else
        {
          fprintf(stderr,"ERROR: no source header file specified");
          return -1;
        }
        break;

      case 'p':
        params->do_full_polar = 1;
        break;

    	case 'q':
				if (optarg)
      		params->max_queue_count = atoi(optarg);
				else 
				{
					fprintf(stderr,"ERROR: no queue count specified");
					return -1;
				}
      	break;

      case 's':
        if (optarg)
          params->sampler_hdr = strdup(optarg);
        else
        {
          fprintf(stderr,"ERROR: no sampler header file specified");
          return -1;
        }
        break;

      case 'w':
        params->write_output = 1;
        break;

			case 'v':
				params->verbose += 1;
				break;

    	default:
				fprintf(stderr, "ERROR: unexpected argument %s\n", optarg); 
      	return -1;
		}
	}

	if (argc - optind != 3) 
  {
    fprintf(stderr, "ERROR: not enough arguements [%d], expected 3\n",(argc-optind));
		return -1;
  }

	params->sample_count    = atoi(argv[optind]);//1024*1024;
  params->freq_count      = atoi(argv[optind+1]);//1024;
  params->ant_count       = atoi(argv[optind+2]);//32;
  
  // Derived science parameters
  // Note: sample_count must be a multiple of freq_count
  params->science_accum_size = params->sample_count / (2*params->freq_count);
  params->output_count = params->ant_count * params->ant_count * params->freq_count;
  
  // Technical parameters
  params->max_queue_accum_size = (size_t) (floor(params->gulp_accum_size/params->max_queue_count) + 1);
  params->max_unpacked_count = params->max_queue_accum_size * 2 * params->freq_count;
  params->max_raw_word_count = params->max_unpacked_count / SAMPLES_PER_WORD;

	if (params->verbose) 
	{
		fprintf(stderr,"sample_count         = %d\n", params->sample_count);
		fprintf(stderr,"freq_count           = %d\n", params->freq_count);
		fprintf(stderr,"ant_count            = %d\n", params->ant_count);
		fprintf(stderr,"max_queue_count      = %d\n", params->max_queue_count);
  	fprintf(stderr,"accumulation_size    = %d\n", params->science_accum_size);
		fprintf(stderr,"output_count         = %d\n", params->output_count);
		fprintf(stderr,"gulp_accum_size      = %d\n", params->gulp_accum_size);
		fprintf(stderr,"max_queue_accum_size = %d\n", params->max_queue_accum_size);
		fprintf(stderr,"max_unpacked_count   = %d\n", params->max_unpacked_count);
		fprintf(stderr,"max_raw_word_count   = %d\n", params->max_raw_word_count);
	}

	return 0;
}

int gmrt_corr_data_init(gmrt_corr_data_t * d, gmrt_corr_params_t p) 
{

 	d->queue_accum_size = (size_t *) malloc(sizeof(size_t) * p.max_queue_count);
  d->unpacked_count = (size_t *) malloc(sizeof(size_t) * p.max_queue_count);
  d->raw_word_count = (size_t *) malloc(sizeof(size_t) * p.max_queue_count);
	d->queues = (Queue *) malloc(sizeof(Queue) * p.max_queue_count);
	d->h_raw = (RawWord **) malloc(sizeof(RawWord *) * p.max_queue_count);
	d->d_raw = (RawWord **) malloc(sizeof(RawWord *) * p.max_queue_count);
	d->d_unpacked = (Real **) malloc(sizeof(Complex *) * p.max_queue_count);
  if (!d->queue_accum_size || !d->unpacked_count || !d->raw_word_count || 
      !d->queues ||  !d->h_raw || !d->d_raw || !d->d_unpacked)
  {
    fprintf(stderr, "gmrt_corr_data_init: malloc failed!\n");
    return -1;
  }

	if (p.use_datablock) 
	{
    if (p.verbose)
      fprintf(stderr, "gmrt_corr_data_init: using datablock\n");

		// set the IO function for datablock mode
		d->io_function = gmrt_corr_dada_read;

		// we should be parsing ant_count hdu keys from the file
		d->hdus = (dada_hdu_t **) malloc(sizeof(dada_hdu_t *) * p.ant_count);
		d->input_keys = (key_t *) malloc(sizeof(key_t) * p.ant_count);
    if (!d->hdus || !d->input_keys)
    {
      fprintf(stderr, "gmrt_corr_data_init: malloc failed!\n");
      return -1;
    }

		if (p.verbose)
			fprintf(stdout, "gmrt_corr_data_init: opening dada key list file %s\n", p.dada_key_list);

		FILE *fptr = fopen(p.dada_key_list, "r");
		if (!fptr)
		{
			fprintf(stderr, "ERROR: could not open dada key list [%s]\n", p.dada_key_list);
			return -1;
		}

    char line[64];
    unsigned i=0;

		// parse the dada keys from the file
		while (	fgets( line, 64, fptr )	&& i < p.ant_count)
		{
			if (p.verbose)
				fprintf(stdout, "gmrt_corr_data_init: parsed '%s'", line);

			if (sscanf (line, "%x\n", &(d->input_keys[i])) != 1)
			{
				fprintf (stderr,"ERROR: could not parse dada key from '%s'\n",line);
        return -1;
			} 
			i++;
		}
    fclose(fptr);

		// create HDU's for each datablock key
    size_t ant;
		for (ant=0; ant<p.ant_count; ++ant)
		{
			d->hdus[ant] = dada_hdu_create(0);
		  dada_hdu_set_key(d->hdus[ant], d->input_keys[ant]);

			if (p.verbose)
				fprintf(stdout,"gmrt_corr_data_init: connecting to datablock %d\n", ant);
			if (dada_hdu_connect (d->hdus[ant]) < 0)
				return -1;

			if (p.verbose)
				fprintf(stdout,"gmrt_corr_data_init: locking read on datablock %d\n", ant);
			if (dada_hdu_lock_read (d->hdus[ant]) < 0)
				return -1;
		}
	} 
	else
	{
		d->io_function = gmrt_corr_file_read;
    d->input_sources = (int *) malloc(sizeof(int) * p.ant_count);
    if (!d->input_sources)
    {
      fprintf(stderr, "gmrt_corr_data_init: malloc failed\n");
      return -1;
    }
    
    char line[256];
    unsigned i=0;
    unsigned last = 0;

    if (p.verbose)
      fprintf(stderr, "gmrt_corr_data_init: opening data file list %s\n", p.data_file_list);
    FILE *fptr = fopen(p.data_file_list, "r");
    if (!fptr)
    { 
      fprintf(stderr, "ERROR: could not open dada key list [%s]\n", p.dada_key_list);
      return -1;
    } 

    int fd = 0;
    while ( fgets( line, 256, fptr ) && i < p.ant_count)
    {
      last = strlen(line)-1;
      line[last] = '\0';
      if (p.verbose)
        fprintf(stdout, "gmrt_corr_data_init: parsing line '%s' into input_source[%d]\n", line, i);

      d->input_sources[i] = open(line, O_RDONLY);

      if (d->input_sources[i] < 0) {
        fprintf(stderr, "gmrt_corr_data_init: failed to open %s [%d]: %s\n", line, i, strerror(errno));
        return -1;
      }
      i++;
    }
    fclose(fptr);
	}

	return 0;
}

int gmrt_corr_data_dealloc(gmrt_corr_data_t * d, gmrt_corr_params_t p)
{
  free(d->queue_accum_size);
  free(d->unpacked_count);
  free(d->raw_word_count);
  free(d->queues);
  free(d->h_raw);
  free(d->d_raw);
  free(d->d_unpacked);

	if (p.use_datablock) 
	{
		unsigned i;
		for (i=0; i<p.ant_count; i++)
		{

      fprintf(stdout, "gmrt_corr_data_dealloc: ipcbuf_reset [%d]\n", i);
      ipcbuf_reset ((ipcbuf_t*)d->hdus[i]->data_block);

			//if (p.verbose)
				fprintf(stdout, "gmrt_corr_data_dealloc: unlocking read on datablock %d\n", i);
			if (dada_hdu_unlock_read (d->hdus[i]) < 0)
    		return -1;

			//if (p.verbose)
				fprintf(stdout, "gmrt_corr_data_dealloc: disconnecting from datablock %d\n", i);
  		if (dada_hdu_disconnect (d->hdus[i]) < 0)
    		return -1;
		}
	} 
	else 
	{
		// Close input sources and output dest
    size_t ant;
		for( ant=0; ant<p.ant_count; ++ant ) {
      close(d->input_sources[ant]);
		}
	}
	return 0;
}

int gmrt_corr_dada_open(gmrt_corr_data_t * d, gmrt_corr_params_t p)
{
	unsigned ant = 0;
	for (ant=0; ant<p.ant_count; ant++)
	{
		if (p.verbose)
			fprintf(stdout, "gmrt_corr_dada_open: ppening datablock %d\n", ant);

	 	if (gmrt_corr_get_header(d->hdus[ant]) < 0)
   	{
     	fprintf(stderr,"ERROR: could not get header from datablock %d\n",ant);
     	return -1;
   	} 

    if (p.verbose)
    {
   	  fprintf(stdout,"gmrt_corr_dada_open: DADA header[%d]\n", ant);
      fprintf(stdout, "===========================================\n");
      fprintf(stdout, "%s", d->hdus[ant]->header);
      fprintf(stdout, "===========================================\n");
    }
	}
}

// Load data from datablock into host memory
size_t gmrt_corr_dada_read(gmrt_corr_data_t * d, unsigned ant, char * address, size_t size)
{
	return (size_t) gmrt_corr_dada_ipcio_read(d->hdus[ant], address, size);
}

// Load data from file into host memory
size_t gmrt_corr_file_read(gmrt_corr_data_t * d, unsigned ant, char * address, size_t size)
{
  return (size_t) read(d->input_sources[ant], address, size);
}

void usage() 
{
	fprintf(stdout,"Usage: gmrt_correlator [options] sample_count freq_count ant_count\n");
  fprintf(stdout," -a antsys    antsys.hdr file [default `pwd`/antsys.hdr]\n");
	fprintf(stdout," -f file      file contain list of raw data files\n");
	fprintf(stdout," -k file      file contain list of dada keys\n");
	fprintf(stdout," -g bytes     gulp size [default 128]\n");
  fprintf(stdout," -o source    source.hdr file [default `pwd`/source.hdr]\n");
	fprintf(stdout," -p           do full polar\n");
	fprintf(stdout," -q count     queue count [default 2]\n");
  fprintf(stdout," -a sampler   sampler.hdr file [default `pwd`/sampler.hdr]\n");
	fprintf(stdout," -w           write output files\n");
	fprintf(stdout," -v           verbose output\n");
}
