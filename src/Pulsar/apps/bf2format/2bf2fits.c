/* To compile as stand alone:

gcc  -I/scratch/windmill_1/wltvrede/packages/include -c -Wall bf2fits.c
g++ -o bf2fits bf2fits.o -L/scratch/windmill_1/wltvrede/packages/lib -lcfitsio 

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "fitsio.h"
#include "patricklib.h"
#include "patricklib_lofar.c"

/* NOTE: for old data sets (before PBW5) there is no 512 byte alignment,
 so "pad" needs to be ignored (line 82) and also the data does not need to 
be "floatswapped" (line 119). Same applies for convert_collapse() too.*/

char OUTNAME[248] = "PULSAR_OBS";
int BEAMS = 1;
int CHANNELS = 1;
int STOKES = 1;	
int STOKES_SWITCH = 0;			
int collapse = 0;
int writefloats = 0;
int writefb     = 0;
int eightBit = 0;
char parsetfile[1000]; // for reading header info for filterbank file
int SAMPLESPERSTOKESINTEGRATION = 1;
int SAMPLES = 768;
int AVERAGE_OVER = 600;
int NUM_BLOCKGROUPS = -1;
float N_sigma = 7;
#define WRITEFUNC(b)			(b[STOKES_SWITCH]) // I=0,Q=1,U/2=2,V/2=3
//#define WRITEFUNC(b)			(b[0]+b[1]) // X power * 2
//#define WRITEFUNC(b)			(b[0]-b[1]) // Y power * 2

#define SCALE				150 /* divide samples by this */
#define CLOCK				200*1000*1000
#define FFTLENGTH			1024
#define SAMPLEDURATION			(SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*CLOCK/FFTLENGTH/CHANNELS))

int constructFITSsearchsubint(datafile_definition datafile, float *data, int subintnr, unsigned char **subintdata, float **scales, float **offsets, int alreadyscaled, int allocmem, int destroymem);
int writeFITSsubint(datafile_definition datafile, long subintnr, unsigned char *subintdata, float *scales, float *offsets);
int writePSRFITSHeader(datafile_definition *datafile, int verbose);

/* separator is "=" for presto and ":" for sigproc */
char *get_ptr_entry(char *str, char **txt, int nrlines, char *separator);

void usage(){
  puts("Syntax: bf2presto8 [options] SB*.MS");
  puts("-A\t\tNumber of blocks to read in at the same time, which affects calc of running mean etc. (Default = 600)");
  puts("-clipav\t\tInstead of normal clipping, write out average");
  puts("-header\t\tChange something in the header");
  puts("-headerlist\tlist available options");
  puts("-nbits\t\tUse this number of bits.");
  puts("-o\t\tOutput Name(Default = PULSAR_OBS)");
  puts("-parset\t\tRead this parset file to obtain header parameters.");
  puts("-sigma\t\tFor the packing, use this sigma limit instead of using the minimum and maximum.");
  puts("\n");
  puts("-debugpacking\tSuper verbose mode to debug packing algoritm.");
  puts("-v\t\tverbose");
}

void swap_endian( char *x )
{
 char c;

 c = x[0];
 x[0] = x[3];
 x[3] = c;


 c = x[1];
 x[1] = x[2];
 x[2] = c;
}

void floatSwap(float *f )
{
union {
  char buffer[ 4 ];
  float f;
} u; 

u.f = *f;
swap_endian(u.buffer);
*f = u.f;
}


/* Returns 1 if successful, 0 on error */
int convert_nocollapse(datafile_definition fout, FILE *input, int beamnr, datafile_definition *subintdata, int *firstseq, int *lastseq, int findseq, float sigmalimit, int clipav, int verbose, int debugpacking)
{
  typedef struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][STOKES][CHANNELS][SAMPLES|2];
  }stokesdata_struct;

    
 stokesdata_struct *stokesdata;
 int prev_seqnr = -1;
 unsigned time;
 unsigned output_samples = 0;
 unsigned nul_samples = 0;
 /* unsigned toobig = 0, toosmall = 0; */
 unsigned num;
 int x;
 long filesize;

 stokesdata = (stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(stokesdata_struct) );
 if(stokesdata == NULL) {
   fprintf(stderr, "Memory allocation error\n");
   return 0;
 }

 /* Only find out how many blocks (including gaps) are in the data, then quit function. */
 if(findseq) {
   fseek( input, 0, SEEK_SET );
   num = fread( &stokesdata[0], sizeof stokesdata[0], 1, input );
   if(num != 1) {
     fprintf(stderr, "No data?\n");
     return 0;
   }
   swap_endian( (char*)&stokesdata[0].sequence_number );
   *firstseq = stokesdata[0].sequence_number;
   fseek(input, 0, SEEK_END);
   filesize = ftell(input);
   if(verbose) printf("  File size %ld bytes = %ld blocks of %ld bytes\n", filesize, filesize/sizeof(stokesdata_struct), sizeof(stokesdata_struct));
   filesize /= sizeof(stokesdata_struct);
   fseek(input, (filesize-1)*sizeof(stokesdata_struct), SEEK_SET);
   num = fread( &stokesdata[0], sizeof stokesdata[0], 1, input );
   if(num != 1) {
     fprintf(stderr, "Error reading file.\n");
     return 0;
   }
   swap_endian( (char*)&stokesdata[0].sequence_number );
   *lastseq = stokesdata[0].sequence_number;
   
   return 1;
 }


 /* Allocate temporary memory for packed data */
 unsigned char *packeddata;
 float *scales, *offsets;
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 1, 0) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }

 /* 
    - Reads AVERAGE_OVER (10) blocks of data at the time from the input data. 
    - Swap some bytes around
    - Look if there is a gap in the data (sequence numbers are missing) and write zero's in gaps.
 */
 x = 0;
 fseek( input, 0, SEEK_SET );
 while( !feof( input ) ) {
   if (x == NUM_BLOCKGROUPS) break;
   x++;
   unsigned i,c;
   int orig_prev_seqnr;

   /* read data */
   num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );

   /* i loops over the blocks of data read in */
   for( i = 0; i < num; i++ ) {
     for( c = 0; c < CHANNELS; c++ ) {
       unsigned b,t,s;
       b = beamnr;
       for( t = 0; t < SAMPLES; t++ ) {
         for( s = 0; s < STOKES; s++ ) {
	   floatSwap( &stokesdata[i].samples[b][s][c][t] );
         }
       }
     }
   }
   
   /* If no more data, sto while loop */
   if( !num ) {
     break;
   }


   /* Print if there are gaps in the data. It doesn't deal with it here though. */
   orig_prev_seqnr = prev_seqnr;
   for( i = 0; i < num; i++ ) {
     /* sequence number is big endian */
     swap_endian( (char*)&stokesdata[i].sequence_number );

     /* detect gaps */
     if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
       fprintf(stderr,"\nnum %d gap between sequence numbers %u and %u.\n",i, prev_seqnr, stokesdata[i].sequence_number );
     }
     prev_seqnr = stokesdata[i].sequence_number;
   }


   /* Calculate average. RMS not used right now. */
   float average[CHANNELS], rms[CHANNELS];
   for( c = 0; c < CHANNELS; c++ ) {
     float sum = 0.0f;
     float ms = 0.0f;
     int N = 0;
     unsigned validsamples = 0;
     float prev = stokesdata[0].samples[beamnr][STOKES_SWITCH][c][0];
     /* compute average */
     for( i = 0; i < num; i++ ) {
       for( time = 0; time < SAMPLES; time++ ) {
         const float value = stokesdata[i].samples[beamnr][STOKES_SWITCH][c][time];
	 if(prev < 1){
	   prev = value;
	 } else if( !isnan( value ) && value > 0.5*prev && value < 2*prev ) {
           sum += value;
	   ms += value*value;
	   prev = value;
           validsamples++;
         }
       }
     }
   
     N = validsamples?validsamples:1;
     average[c] = sum/N;
     rms[c] = sqrt((ms-(N*average[c]*average[c]))/(N-1));
   }
 
  /* convert and write the data */
   prev_seqnr = orig_prev_seqnr;
   for( i = 0; i < num; i++ ) {
     int gap;
     /*     fprintf(stderr, "XXXXXXXX %d - 1\n", i); */
     for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
       /* gaps, fill in zeroes */
       for( c = 0; c < CHANNELS; c++ ) {
	 for( time = 0; time < SAMPLES; time++ ) {
	   /*	     if (writefloats==1){ */
	   float sum = 0;
	   /* patrick: put polarization to 0, assume only one pol
	      written out. */
	   if(writePulsePSRData(*subintdata, 0, 0, c, time, 1, &sum) == 0) { 
	     fprintf(stderr, "Error writing to temporary memory\n");
	     return 0;
	   }
	 }
       }
       if(verbose) {
	 printf(".");
	 fflush(stdout);
       }
       /* Pack data */
       if(constructFITSsearchsubint(fout, subintdata->data, gap+1, &packeddata, &scales, &offsets, 0, 0, 0) != 1) {
	 fprintf(stderr, "ERROR: Cannot construct subint data               \n");      
	 return 0;
       }
       /* Write out subint */
       if(writeFITSsubint(fout, gap+1, packeddata, scales, offsets) != 1) {
	 fprintf(stderr, "ERROR: Cannot write subint data               \n");      
	 return 0;
       }
       output_samples += SAMPLES;
       nul_samples += SAMPLES;
     }
     stokesdata[i].sequence_number =  prev_seqnr + 1;       
     prev_seqnr = stokesdata[i].sequence_number;

     /*     fprintf(stderr, "XXXXXXXX %d - 2\n", i); */
   
       /* patrick */
     if(sigmalimit > 0 && i == 0) {
       for( c = 0; c < CHANNELS; c++ ) {
	 /*
	 0                         = ivalue(average[c]-sigmalimit*rms[c]);
	 0                         = (average[c]-sigmalimit*rms[c] - offset)/scale;
	 0                         = (average[c]-sigmalimit*rms[c] - offset);
	 */
	 offsets[c]                    = average[c]-sigmalimit*rms[c];

	 /*
	 pow(2,subintdata.NrBits)-1  = ivalue(average[c]+sigmalimit*rms[c]);
	 pow(2,subintdata.NrBits)-1  = (average[c]+sigmalimit*rms[c]-offset)/scale;
	 */
	 scales[c]  = (average[c]+sigmalimit*rms[c]-offsets[c])/(float)(pow(2,subintdata->NrBits)-1);
	 if(debugpacking)
	   fprintf(stderr, "scale=%e offset=%e (av=%e  rms=%e)\n", scales[c], offsets[c], average[c], rms[c]);
       }
     }
     /* process data */
     for( c = 0; c < CHANNELS; c++ ) {
       for( time = 0; time < SAMPLES; time++ ) {
         float sum;
         sum = stokesdata[i].samples[beamnr][STOKES_SWITCH][c][time] ;
	 
	 /* not sure; replacing sum by average if (NaN or within 0.01 of zero)? */
	 sum = (isnan(sum) || (sum <= 0.01f && sum >= -0.01f)) ? average[c] : sum;
	 if(sigmalimit > 0) {
	   if(debugpacking)
	     printf("value %e ", sum);
	   sum = (sum - offsets[c])/scales[c];
	   if(debugpacking)
	     printf(" %e ", sum);
	   if(sum < 0) {
	     if(clipav)
	       sum = (average[c] - offsets[c])/scales[c];
	     else
	       sum = 0;
	     /*	     printf("Underflow! %e\n", sum); */
	   }
	   if(sum > pow(2,subintdata->NrBits)-1) {
	     if(clipav)
	       sum = (average[c] - offsets[c])/scales[c];
	     else
	       sum = pow(2,subintdata->NrBits)-1;
	     /*	     printf("Overflow! %e\n", sum); */
	   }
	   if(debugpacking)
	     printf(" %e \n", sum);

	   /*	   printf("value %e\n", sum); */
	 }
	 
	 /* patrick: put polarization to 0, assume only one pol
	    written out. Think x is the current bin nr */
	 if(writePulsePSRData(*subintdata, 0, 0, c, time, 1, &sum) == 0) { 
	   fprintf(stderr, "Error writing to temporary memory");
	   return 0;
	 }
	   /*	   fwrite( &sum, sizeof sum, 1, outputfile[c] ); */ /* single sample written to channel file*/
	 output_samples++;
	 if( sum == 0 ) nul_samples++;

       } // for( time = 0; time < SAMPLES; time++ ) 
     }

     /*     fprintf(stderr, "XXXXXXXX %d - 3\n", i); */
     if(verbose) {
       printf(".");
       fflush(stdout);
     }
     /* Pack data */
     if(sigmalimit > 0) {
       if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 1, 0, 0) != 1) {
	 fprintf(stderr, "ERROR: Cannot construct subint data               \n");      
	 return 0;
       }
       /*     for( c = 0; c < CHANNELS; c++ ) {
       printf("XXXXX %e\n", scales[c]);
       }*/
     }else {
       if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 0, 0) != 1) {
	 fprintf(stderr, "ERROR: Cannot construct subint data               \n");      
	 return 0;
       }
     }
     /*     fprintf(stderr, "XXXXXXXX %d - 4\n", (x-1)*AVERAGE_OVER+i); */
     /* Write out subint */
     if(writeFITSsubint(fout, (x-1)*AVERAGE_OVER+i, packeddata, scales, offsets) != 1) {
       fprintf(stderr, "ERROR: Cannot write subint data               \n");      
       return 0;
     }
     /*     fprintf(stderr, "XXXXXXXX %d - 5\n", (x-1)*AVERAGE_OVER+i); */
   }
   /*   fprintf(stderr, "XXXXXXXX X - 6\n"); */

 } //  while( !feof( input ) ) {
 printf("\n");
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 0, 1) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }

 /* fprintf(stderr,"  %u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples); */
 

 free(stokesdata);
 return 1;
} // void convert_nocollapse()


int isNum( char c )
{
  if ( c < '0' || c > '9' ) return 0; 
  return 1;
}

int main( int argc, char **argv ) 
{
  int i,j,b, index, firstseq, lastseq, readparset, totalnrchannels, subbandnr, nrbits, debugpacking;
  char buf[1024], *filename, *dummy_ptr;
  datafile_definition subintdata, fout;
  patrickSoftApplication application;
  FILE *fin;
  char header_txt[1000][1000], *s_ptr, *txt[1000], dummy_string[1000], dummy_string2[1000];
  int channellist[1000];
  int nrlines, ret;
  int blocksperStokes, integrationSteps, clockparam, subbandFirst, nsubbands, clipav;
  float lowerBandFreq, lowerBandEdge, subband_width, bw, lofreq, sigma_limit;

  initApplication(&application, "bf2fits", "[options] inputfiles");
  application.switch_headerlist = 1;
  application.switch_header = 1;
  application.switch_verbose = 1;
  readparset = 0;
  nrbits = 8;

  cleanPRSData(&subintdata);

  sigma_limit = -1;
  debugpacking = 0;
  clipav = 0;
  if(argc < 2) {
    usage();
    return 0;
  }else {
    for(i = 1; i < argc; i++) {
      index = i;
      if(processCommandLine(&application, argc, argv, &index)) {
	i = index;
      }else if(strcmp(argv[i], "-A") == 0) {
	j = sscanf(argv[i+1], "%d", &AVERAGE_OVER);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-nbits") == 0) {
	j = sscanf(argv[i+1], "%d", &nrbits);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-sigma") == 0) {
	j = sscanf(argv[i+1], "%f", &sigma_limit);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-clipav") == 0) {
	clipav = 1;
      }else if(strcmp(argv[i], "-o") == 0) {
	j = sscanf(argv[i+1], "%s", OUTNAME);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-debugpacking") == 0) {
	debugpacking = 1;
	/*
      }else if(strcmp(argv[i], "-c") == 0) {
	j = sscanf(argv[i+1], "%d", &CHANNELS);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-n") == 0) {
	j = sscanf(argv[i+1], "%d", &SAMPLESPERSTOKESINTEGRATION);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-N") == 0) {
	j = sscanf(argv[i+1], "%d", &SAMPLES);
	if(j != 1) {
	  fprintf(stderr, "bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
*/
      }else if(strcmp(argv[i], "-parset") == 0) {
	readparset = i+1;
        i++;
      }else {
	/* If the option is not recognized, assume it is a filename */
	if(argv[i][0] == '-') {
	  printf("Unknown option: %s\n", argv[i]);
	  return 0;
	}else {
	  if(applicationAddFilename(i) == 0)
	    return 0;
	}
      }
    }
  }

  if(applicationFilenameList_checkConsecutive(argv) == 0) {
    return 0;
  }
  if(numberInApplicationFilenameList() == 0) {
    fprintf(stderr, "bf2fits: No input files specified\n");
    return 0;
  }

  lofreq = 0;
  if(readparset) {
    fin = fopen(argv[readparset], "r");
    if(fin == NULL) {
      fprintf(stderr, "bf2fits: Cannot open %s\n", argv[readparset]);
      return 0;      
    }

    nrlines = 0;
    ret = 0;
    for(i = 0; i < 1000; i++)
      txt[i] = header_txt[i];
    do {
      if(fgets(header_txt[nrlines], 1000, fin) != NULL)
	nrlines++;
      else
	ret = 1;
      if(nrlines > 999) {
	fprintf(stderr, "bf2fits: Too many lines in parset file\n");
	return 0;
      }
    }while(ret == 0);
    fclose(fin);    
    if(application.verbose) printf("Read %d lines from header.\n", nrlines);

    s_ptr = get_ptr_entry("OLAP.CNProc.integrationSteps", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(blocksperStokes));
    }else {
      fprintf(stderr, "bf2fits: OLAP.CNProc.integrationSteps not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("OLAP.Stokes.integrationSteps", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(integrationSteps));
    }else {
      fprintf(stderr, "bf2fits: OLAP.Stokes.integrationSteps not set\n");
      return 0;     
    }
    SAMPLES = blocksperStokes/integrationSteps;
    s_ptr = get_ptr_entry("Observation.channelsPerSubband", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(CHANNELS));
    }else {
      fprintf(stderr, "bf2fits: Observation.channelsPerSubband not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(SAMPLESPERSTOKESINTEGRATION));
    }else {
      fprintf(stderr, "bf2fits: Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.Beam[0].angle1", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%f", &(subintdata.ra));
    }else {
      fprintf(stderr, "bf2fits: Observation.Beam[0].angle1 not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.Beam[0].angle2", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%f", &(subintdata.dec));
    }else {
      fprintf(stderr, "bf2fits: Observation.Beam[0].angle1 not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.Beam[0].target", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", (subintdata.psrname));
    }else {
      fprintf(stderr, "bf2fits: Observation.Beam[0].target not set\n");
      return 0;     
    }



    s_ptr = get_ptr_entry("Observation.sampleClock", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(clockparam));
    }else {
      fprintf(stderr, "bf2fits: Observation.sampleClock not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.bandFilter", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", subintdata.instrument);
    }else {
      fprintf(stderr, "bf2fits: Observation.bandFilter not set\n");
      return 0;     
    }
    strcpy(buf, subintdata.instrument);
    dummy_ptr = strchr(buf, '_');
    if(dummy_ptr != NULL)
      dummy_ptr[0] = 0;
    sscanf(buf+4, "%f", &lowerBandFreq);

    s_ptr = get_ptr_entry("Observation.Beam[0].subbandList", txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", dummy_string);
    }else {
      fprintf(stderr, "bf2fits: Observation.Beam[0].subbandList not set\n");
      return 0;     
    }
    /* Get the list of subbands which should be there. */
    sprintf(dummy_string2, "echo \"%s\" | awk '{print substr($1,2,length($1)-2)}' | awk -F\\, '{for (i=1; i <= NF ; i++)  print $i}' | awk -F\\. '{var=$1; while (var <=$NF) { print var; var +=1 }}' > /tmp/bf2fits.channellist\n", dummy_string);
    /*    printf(dummy_string2); */
    system(dummy_string2);
    fin = fopen("/tmp/bf2fits.channellist", "r");
    if(fin == NULL) {
      fprintf(stderr, "bf2fits: Error generating channel list\n");
      perror("");
      return 0;
    } 
    for(totalnrchannels = 0; totalnrchannels < 1000; totalnrchannels++) {
      ret = fscanf(fin, "%d", &channellist[totalnrchannels]);
      if(ret != 1)
	break;
    }
    fclose(fin);
    
    system("rm /tmp/bf2fits.channellist");
    subbandFirst = channellist[0];

    nsubbands = 512;
 

    printf("This is a %s obs at %f MHz (clock=%d subbandFirst=%d totalnrchannels=%d)\n", subintdata.instrument, lowerBandFreq, clockparam, subbandFirst, totalnrchannels);
    if (lowerBandFreq < 150.0 && lowerBandFreq > 100.0 && clockparam == 200) {
      lowerBandEdge = 100.0;
      bw = 100.0;
    }else {
      lowerBandEdge = 0.0;
      bw = 100.0;
    }
    subband_width = bw / nsubbands;
    lofreq = lowerBandEdge + (subbandFirst -0.5) * subband_width;

    /*

if (lowerBandFreq < 150.0 and lowerBandFreq > 100.0 and par.clock == "200"):
    Nyquist = 2
    bw = 100.0
    id.chan_width = bw / nsubbands / int(par.channelsPerSubband)
    lowerBandEdge = 100.0
elif (lowerBandFreq < 40.0 and par.clock == "200"):
    Nyquist = 1
    bw = 100.0
    id.chan_width = bw / nsubbands / int(par.channelsPerSubband)
    lowerBandEdge = 0.0

        id.lofreq = lowerBandEdge + (float(par.subbandFirst) -0.5) * subband_width
        id.BW = (lowerBandEdge  + (float(par.subbandLast)+ 0.5) * subband_width ) - id.lofreq
    */

    int y, m, d, hour, min, sec;
    float sec_mid;
    s_ptr = get_ptr_entry("Observation.startTime", txt, nrlines, "=");
    y = 0;
    if(s_ptr != NULL) {
      /*      sscanf(s_ptr, "%d-%d-%d %d:%d:%d", &y, &m, &d, &hour, &min, &sec);*/
      sscanf(s_ptr, "'%d-%d-%d %d:%d:%d'", &y, &m, &d, &hour, &min, &sec);
    }else {
      fprintf(stderr, "bf2fits: Observation.startTime not set\n");
      return 0;     
    }
    sec_mid = hour * 3600.0 + min * 60.0 + sec;
    i = (m+9)/12;
    i = 7*(y+i)/4;
    j = (y+(m-9)/7)/100;
    j = 3*(j+1)/4;
    i += j;
    j = 275*m/9;
    subintdata.mjd = (367.0 * y) - i + j + d + 1721028.5;
    subintdata.mjd = (subintdata.mjd - 2400000.5) + sec_mid / 86400.0;

    printf("start=%d-%d-%d %d:%d:%d (%s %f)\n", y, m, d, hour, min, sec, s_ptr, subintdata.mjd);
    /*Observation.startTime = '2010-05-12 08:58:25' */

  }

  printf( "%d channels %d beams %d samples %d stokes\n", CHANNELS, BEAMS, SAMPLES, STOKES );
  printf("Output Name: %s\n", OUTNAME);
  printf("Stokes Parameter: %d\n\n",STOKES_SWITCH);


  subintdata.NrBins = SAMPLES;
  subintdata.nrFreqChan = CHANNELS;
  subintdata.NrPols = 1;
  subintdata.NrBits = nrbits;
  subintdata.SampTime = SAMPLEDURATION;
  subintdata.NrPulses = 1;   
  subintdata.format = MEMORY_format;
  subintdata.Period = -1;   
  subintdata.bw = subband_width;
  subintdata.channelbw = subband_width/(float)subintdata.nrFreqChan;
  subintdata.fd_type = 1;  /* Not sure, guessing something. Not used by dspsr I think */
  subintdata.poltype = 1; /* Stokes data */

  /* Search commandline for header parameters to overwrite header
     parameters. */
  if(PSRDataHeader_parse_commandline(&subintdata, argc, argv, application.verbose) == 0)
    return 0;

  subintdata.data = (float *)malloc(subintdata.NrBins*subintdata.nrFreqChan*subintdata.NrPols*sizeof(float));
  if(subintdata.data == NULL) {
    fprintf(stderr, "bf2fits: Cannot allocate memory\n");
    return 0;
  }

  /* make beam dirs */ 
  if( BEAMS > 1 ) {
    for( b = 0; b < BEAMS; b++ ) {
      sprintf( buf, "beam_%d", b );
      mkdir( buf, 0775 );
    }
  }

  /* open files */
  for( b = 0; b < BEAMS; b++ ) {

   /* loop over input files */
    while((filename = getNextFilenameFromList(&application, argv)) != NULL) {
      printf("Processing %s\n", filename);

      for(i = 0; i < strlen(filename); i++) {
	if(isNum(filename[i]))
	  break;
      }
      sscanf(filename+i, "%*8c%d", &subbandnr);
      subintdata.freq_cent = lofreq + subbandnr*subintdata.bw + 0.5*(subintdata.nrFreqChan-1)*subintdata.channelbw;
      printf("  This is file number %d at centre frequency %f MHz\n", subbandnr, subintdata.freq_cent);

      /* create names */
      sprintf( buf, "%s.sub%04d", OUTNAME, subbandnr);
      if ( BEAMS > 1  ) sprintf( buf, "beam_%d/%s", b, buf ); /* prepend beam name */
      fprintf(stderr,"  %s -> %s\n", filename, buf); 

      /* open file */
      fin = fopen(filename, "rb");
      if(fin == NULL) {
	fprintf(stderr, "bf2fits: Cannot open input file\n");
	return 0;
      }
      if(convert_nocollapse(fout, fin, b, &subintdata, &firstseq, &lastseq, 1, sigma_limit, clipav, application.verbose, debugpacking) == 0)
	return 0;
      if(application.verbose) printf("  File contains sequence numbers %d - %d\n", firstseq, lastseq);

      /* Copy header parameters to output header */
      copy_paramsPSRData(subintdata, &fout);
      fout.NrPulses = lastseq+1;
      if(application.verbose) {
	printf("  Header dump:\n");
	printHeaderPSRData(fout, 4);
      }


      if(!openPSRData(&fout, buf, FITS_format, 1, 0, application.verbose))
	return 0;
      if(!writePSRFITSHeader(&fout, application.verbose))
	return 0;

      /* convert */
      if(convert_nocollapse(fout, fin, b, &subintdata, &firstseq, &lastseq, 0, sigma_limit, clipav, application.verbose, debugpacking) == 0)
	return 0;

      fclose(fin);
    }
  }

  closePSRData(&subintdata);
  closePSRData(&fout);

  return 0;
}
