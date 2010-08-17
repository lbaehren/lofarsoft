/* 
 *   short test program to test the GMRT delay library
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include "gmrt_delay_fns.h"
#include "gmrt_newcorr.h"

void usage();

int main(int argc, char *argv[])
{

	CorrType        corr;
	SourceParType   source;

	delay_vals_t    delays[NCHAN*NUM_ANT];
	fringe_vals_t   fringes[NCHAN*NUM_ANT];

	int             int_time_offset;
	double          res_time_offset;
	double          BW; 
	char   				  source_name[100];
	struct timeval  timestamp;
	struct timezone noreq;
	double  				time_offset = 0.0;

	if (argc < 3 || argc > 4)
	{
		usage();
		return EXIT_FAILURE;
	}
	if (argc == 4) 
		time_offset=atof(argv[3]);

	int_time_offset = (int) time_offset;
	res_time_offset = time_offset - int_time_offset;

	// use the current time
  gettimeofday(&timestamp, &noreq );
  timestamp.tv_sec+=int_time_offset;
  double diff_usec = (time_offset-int_time_offset) * 1000000.0;
  timestamp.tv_usec+=diff_usec;
  if(timestamp.tv_usec < 0) 
	{
    timestamp.tv_sec-= 1;
    timestamp.tv_usec+=1000000;  
	} else {
    int int_sec = (timestamp.tv_usec/1000000);
    timestamp.tv_sec+= int_sec;
    timestamp.tv_usec-=(int_sec*1000000);
	}

	// read the antennasys header file
	if (read_antenna_file(argv[1], corr.antenna) < 0)
		return EXIT_FAILURE;

	// read the antenna connectivity
	if (read_antenna_connectivity(argv[2], &corr) < 0)
		return EXIT_FAILURE;

	// manually set source parameters
	source.ra_app = 1.498449;
	source.dec_app = 0.870157;
	source.freq[0] = 1190000000;
	source.first_lo[0] = 1340000000.000000;
	source.bb_lo[0] = 70000000.000000;

	if (calculate_delays(&source, &corr, timestamp, delays, fringes) < 0)
		return EXIT_FAILURE;

	print_vals(delays, fringes);

	return 0;	

}

void usage()
{
  fprintf (stdout,
     "Usage: gmrt_delay_test antsys sampler [offset]\n"
     " antsys         antenna definition file\n"
		 " sampler        antenna list file [for query]\n"
     " offset         offset to add to current time [seconds]\n");
}

