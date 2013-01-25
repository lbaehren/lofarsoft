#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "fitsio.h"
#include "patricklib.h"
#include "patricklib_lofar.c"

#define parsetmaxnrlines     5000
#define parsetmaxlinelength  1000

/* NOTE: for old data sets (before PBW5) there is no 512 byte alignment,
 so "pad" needs to be ignored (line 82) and also the data does not need to 
be "floatswapped" (line 119). Same applies for convert_collapse() too.*/

char OUTNAME[248] = "PULSAR_OBS";
char gainsfile[248] = "";
int BEAMS = 1;
int SUBBANDS = 1;
int CHANNELS = 1;
int CHANNELS_TRES = -1;
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
int zapFirstChannel = 1;   // By default, set first frequency channel of each subband to zero
int IS2 = 0;  // Incoherentstokes 2nd transpose turned on/off phase
int CSIS_MODE = 0;  // CSIS_MODE = 0 for Coherentstokes;  CSIS_MODE = 1 for Incoherentstokes

#define WRITEFUNC(b)			(b[STOKES_SWITCH]) // I=0,Q=1,U/2=2,V/2=3
//#define WRITEFUNC(b)			(b[0]+b[1]) // X power * 2
//#define WRITEFUNC(b)			(b[0]-b[1]) // Y power * 2

#define SCALE				150 /* divide samples by this */
/* #define CLOCK				200*1000*1000 */
#define FFTLENGTH			1024
/*#define SAMPLEDURATION			(SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*CLOCK/FFTLENGTH/CHANNELS))*/

int constructFITSsearchsubint(datafile_definition datafile, float *data, int subintnr, unsigned char **subintdata, float **scales, float **offsets, int alreadyscaled, int allocmem, int destroymem);
int writeFITSsubint(datafile_definition datafile, long subintnr, unsigned char *subintdata, float *scales, float *offsets);
int writePSRFITSHeader(datafile_definition *datafile, int verbose);

/* separator is "=" for presto and ":" for sigproc */
char *get_ptr_entry(char *str, char **txt, int nrlines, char *separator);

// function to create gains table
double *create_gains_table(double *gains, char *gainsfile, long nchan, double fres, double lowfreq) {

 double **amp;
 struct stat info;
 int status, in, ii, kk, jj, jj_start=0;
 int size;
 double freq;

 status = stat(gainsfile, &info);
 if (status != 0) {
   fprintf (stderr, "2f2fits: Can't get stat info!\n");
   return 0;
 }
 size = info.st_size / 8;
 amp = malloc(2 * sizeof (double *));
 for (ii=0; ii<2; ii++) amp[ii] = malloc(size/2 * sizeof(double));

 in = open(gainsfile, O_RDONLY);
 if (in == -1) {
  fprintf (stderr, "2bf2fits: Can't open file '%s' with gains!\n", gainsfile);
  return 0;
 }
 lseek(in, 0, SEEK_SET);
 for (ii=0;ii<size/2;ii++) {
  read(in, &amp[0][ii], 8);
  read(in, &amp[1][ii], 8);
 }
 close(in);

 // determine the closest lowest freq
 if (lowfreq < amp[0][0]) {
  for (jj=0; jj<size/2; jj++) {
   if (lowfreq >= amp[0][jj]) {
    jj_start = jj;
    break;
   }
  }
 } else {
  for (jj=0; jj<size/2; jj++) {
   if (lowfreq < amp[0][jj]) {
    jj_start = jj - 1;
    break;
   }
  }
 }

 // calculating the gain
 for (kk=0; kk<nchan; kk++) {
  freq=lowfreq+kk*fres;
  for (jj=jj_start; jj<size/2; jj++) {
   if (freq > amp[0][jj]) continue;
   gains[kk] = amp[1][jj-1] + (amp[1][jj] - amp[1][jj-1])*((freq-amp[0][jj-1])/(amp[0][jj]-amp[0][jj-1]));
   break;
  }
   // then assigning the gain from the last available frequency
   if (freq > amp[0][size/2-1]) gains[kk] = amp[1][size/2-1];
 }

 for (ii=0; ii<2; ii++) free (amp[ii]);
 free(amp);
 return gains;
}

void usage(){
  //  puts("Just wanted to make sure the new version is picked up.");
  puts("Syntax: 2bf2fits [options] SB*.MS");
  puts("-A\t\tNumber of blocks to read in at the same time, which affects calc of running mean etc. (Default = 600)");
  puts("-clipav\t\tInstead of normal clipping, write out average");
  puts("-header\t\tChange something in the header");
  puts("-headerlist\tlist available options");
  puts("-nbits\t\tUse this number of bits.");
  puts("-o\t\tOutput Name(Default = PULSAR_OBS)");
  puts("-parset\t\tRead this parset file to obtain header parameters.");
  puts("-sigma\t\tFor the packing, use this sigma limit instead of using the minimum and maximum.");
  puts("-CS\t\tInput data is CS data (one file, total I only)");
  puts("-IS\t\tInput data is IS 2nd transpose data (one file, total I only, process like CS, header IS values)");
  puts("-H\t\tHDF5 Data Mode, only working in combination with -CS");
  puts("-append\t\tfor IS data append output fits files together");
  puts("-skip\t\tSkip this number of blocks in the output. Only works with -CS option.");
  puts("-trunc\t\tTruncate output after this number of blocks. Only works with -CS option.");
  puts("-noZap0\t\tDo NOT zap the first frequency channel in each subband");
  puts("-nsubs\t\tThe number of subbands in the input file.");
  puts("-gains\t\tSpecify the file with the gains");
  puts("\n");
  puts("-debugpacking\tSuper verbose mode to debug packing algoritm.");
  puts("-v\t\tverbose");
  puts("-h\t\tprint this help");
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

int isNum( char c )
{
  if ( c < '0' || c > '9' ) return 0; 
  return 1;
}

/* Returns 1 if successful, 0 on error */
/* Only for IS data */
int convert_nocollapse(datafile_definition fout, FILE *input, int beamnr, datafile_definition *subintdata, int *firstseq, int *lastseq, int findseq, float sigmalimit, int clipav, int verbose, int debugpacking)
{
  typedef struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][STOKES][CHANNELS][SAMPLES|2];
  }stokesdata_struct;
    

  //  time_t t1,t2;
  stokesdata_struct *stokesdata;
 int prev_seqnr = -1;
 unsigned timeT;
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
	   if(zapFirstChannel && CHANNELS > 1 && c == 0) {
	     stokesdata[i].samples[b][s][c][t]  = 0.0;
	   }
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
       fprintf(stderr,"\nconvert_nocollapse: num %d gap between sequence numbers %u and %u.\n",i, prev_seqnr, stokesdata[i].sequence_number );
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
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         const float value = stokesdata[i].samples[beamnr][STOKES_SWITCH][c][timeT];
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
     N = validsamples;
     if(N == 0) {
       average[c] = 0;
       rms[c] = 0;
     }else if(N == 1) {
       average[c] = sum;
       rms[c] = 0;
     }else {
       average[c] = sum/N;
       rms[c] = sqrt((ms-(N*average[c]*average[c]))/(float)(N-1));
     }
   }
 
  /* convert and write the data */
   prev_seqnr = orig_prev_seqnr;
   for( i = 0; i < num; i++ ) {
     int gap;
     /*     fprintf(stderr, "XXXXXXXX %d - 1\n", i); */
     for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
       /* gaps, fill in zeroes */
       for( c = 0; c < CHANNELS; c++ ) {
	 for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	   /*	     if (writefloats==1){ */
	   float sum = 0;
	   /* patrick: put polarization to 0, assume only one pol
	      written out. */
	   if(writePulsePSRData(*subintdata, 0, 0, c, timeT, 1, &sum) == 0) { 
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
     // (void) time(&t1);
     // printf("Just after initial time\n");
     for( c = 0; c < CHANNELS; c++ ) {
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         float sum;
         sum = stokesdata[i].samples[beamnr][STOKES_SWITCH][c][timeT] ;
	 
	 /* not sure; replacing sum by average if (NaN or within 0.01 of zero)? */
	 sum = (isnan(sum) || (sum <= 0.01f && sum >= -0.01f)) ? average[c] : sum;
	 if(sigmalimit > 0) {
	   if(debugpacking)
	     printf("2 value %e ", sum);
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
	 // (void) time(&t2);
	 // printf("Time to do process loop is %d seconds\n",(int)(t2-t1));
	 

	 /* patrick: put polarization to 0, assume only one pol
	    written out. Think x is the current bin nr */
	 if(writePulsePSRData(*subintdata, 0, 0, c, timeT, 1, &sum) == 0) { 
	   fprintf(stderr, "Error writing to temporary memory");
	   return 0;
	 }
	   /*	   fwrite( &sum, sizeof sum, 1, outputfile[c] ); */ /* single sample written to channel file*/
	 output_samples++;
	 if( sum == 0 ) nul_samples++;

       } // for( timeT = 0; timeT < SAMPLES; timeT++ ) 
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


void *stokesdata_h5_ptr = NULL;

  double time_read = 0;
  double time_swapfloats = 0;
  double time_calcrms = 0;
  double time_calcscales = 0;
  double time_clipscale = 0;
  double time_packing = 0;
  double time_writing = 0;

/* Returns 1 if successful, 0 on error */
/* Only for H5 data */
int convert_nocollapse_H5(datafile_definition fout, FILE *input, int beamnr, int subbandnr, datafile_definition *subintdata, int *firstseq, int *lastseq, int findseq, float sigmalimit, int clipav, int verbose, int debugpacking, int is_append, long skipNrBlocks, long truncNrBlocks, double *gains)
{
  /* Used to be SAMPLES|2, but that is apparently no longer the case
     for the new H5 format. */
  typedef struct {
    float samples[BEAMS][SAMPLES][SUBBANDS][CHANNELS][STOKES];
  }stokesdata_h5_struct;

  
  stokesdata_h5_struct *stokesdata_h5;

  //  time_t t1,t2;  
  unsigned timeT;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  /* unsigned toobig = 0, toosmall = 0; */
  unsigned num;
  int x;
  long filesize, currentblock;
  int doneprocessing = 0;
  int skip_calc_rms = 0;
  float *average, *rms;
  
  clock_t tstart, tend;

  //  fprintf(stderr, "Entering convert_nocollapse_H5 and start memory allocation\n");
  if(stokesdata_h5_ptr == NULL) {
    //   printf("XXXXX Alloc %ld\n",AVERAGE_OVER * sizeof(stokesdata_h5_struct) );
    stokesdata_h5 = (stokesdata_h5_struct *) malloc( AVERAGE_OVER * sizeof(stokesdata_h5_struct) );
    if(stokesdata_h5 == NULL) {
      fprintf(stderr, "convert_nocollapse_H5: Memory allocation error\n");
      perror("");
      return 0;
    }
    stokesdata_h5_ptr = (void *)stokesdata_h5;
  }else {
    stokesdata_h5 = (stokesdata_h5_struct *)stokesdata_h5_ptr;
  }
  if(is_append) {
    average = calloc(SUBBANDS*CHANNELS, sizeof(float));
    rms = calloc(SUBBANDS*CHANNELS, sizeof(float));
  }else {
    average = calloc(CHANNELS, sizeof(float));
    rms = calloc(CHANNELS, sizeof(float));
  }
  if(average == NULL || rms == NULL) {
    fprintf(stderr, "convert_nocollapse_H5: Memory allocation error\n");
    perror("");
    return 0;
  }
  //  fprintf(stderr, "  memory allocation done\n");

 currentblock = 0;
 /* Only find out how many blocks (including gaps) are in the data, then quit function. */
 if(findseq) {
   //   fprintf(stderr, "  Finding number of blocks in file\n");
   fseek( input, 0, SEEK_SET );
   num = fread( &stokesdata_h5[0], sizeof stokesdata_h5[0], 1, input );
   if(num != 1) {
     fprintf(stderr, "No data?\n");
     return 0;
   }
   /* No sequence numbers anymore in H5 format */
   *firstseq = 0;
   fseek(input, 0, SEEK_END);
   filesize = ftell(input);
   if(verbose) printf("  File size %ld bytes = %ld blocks of %ld bytes\n", filesize, filesize/sizeof(stokesdata_h5_struct), sizeof(stokesdata_h5_struct));
   filesize /= sizeof(stokesdata_h5_struct);
   fseek(input, (filesize-1)*sizeof(stokesdata_h5_struct), SEEK_SET);
   num = fread( &stokesdata_h5[0], sizeof stokesdata_h5[0], 1, input );
   if(num != 1) {
     fprintf(stderr, "Error reading file.\n");
     return 0;
   }
   /* No sequence numbers anymore in H5 format */
   *lastseq = filesize-1;
   
   //   fprintf(stderr, "  Finding number of blocks in file done\n");
   return 1;
 }


 /* Allocate temporary memory for packed data */
 // fprintf(stderr, "  Allocate temporary memory for packed data\n");
 unsigned char *packeddata;
 float *scales, *offsets;
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 1, 0) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }
 // fprintf(stderr, "  Allocate temporary memory for packed data done\n");

 /* 
    - Reads AVERAGE_OVER (10) blocks of data at the time from the input data. 
    - Swap some bytes around
 */

 long subbandnr_start, subbandnr_end, nsub;
 if(is_append == 0) {
   subbandnr_start = subbandnr;
   subbandnr_end = subbandnr;
 }else {
   subbandnr_start = 0;
   subbandnr_end = SUBBANDS-1;
 }


/*
 float *spectrum;
 spectrum = (float *)malloc((subbandnr_end - subbandnr_start + 1)*CHANNELS*sizeof(float));
 if (!spectrum) { printf("not enough memory!\n"); exit(1); }
 memset(spectrum, 0, (subbandnr_end - subbandnr_start + 1)*CHANNELS*sizeof(float));
*/

 x = 0;
 currentblock = 0;
 fseek( input, 0, SEEK_SET );
 // fprintf(stderr, "  Entering data read loop\n");
 while( !feof( input ) ) {
   if (x == NUM_BLOCKGROUPS) break;
   x++;
   unsigned i,c;


   //   fprintf(stderr, "  Read data\n");
   tstart = clock();
   /* read data */
   num = fread( &stokesdata_h5[0], sizeof stokesdata_h5[0], AVERAGE_OVER, input );
   tend = clock()-tstart;
   time_read += tend/(double)CLOCKS_PER_SEC;
   currentblock += num;

   //   fprintf(stderr, "  Float swap\n");
   tstart = clock();
   /* i loops over the blocks of data read in */
   for( i = 0; i < num; i++ ) {
     for(nsub = subbandnr_start; nsub <= subbandnr_end; nsub++) {
       for( c = 0; c < CHANNELS; c++ ) {
	 unsigned b,t,s;
	 b = beamnr;
	 for( t = 0; t < SAMPLES; t++ ) {
	   for( s = 0; s < STOKES; s++ ) {
	     floatSwap( &stokesdata_h5[i].samples[b][t][nsub][c][s] );
	     if(zapFirstChannel && CHANNELS > 1 && c == 0) {
	       stokesdata_h5[i].samples[b][t][nsub][c][s] = 0.0;
	     }
	   }
//           if (isfinite(stokesdata_h5[i].samples[beamnr][t][nsub][c][0]))
//            spectrum[(nsub-subbandnr_start)*CHANNELS+c] += (float)stokesdata_h5[i].samples[beamnr][t][nsub][c][0] / gains[(nsub-subbandnr_start)*CHANNELS+c];
	 }
       }
     }
   }
   tend = clock()-tstart;
   time_swapfloats += tend/(double)CLOCKS_PER_SEC;
   //   fprintf(stderr, "  Float swap done\n");

/*
  int oo;
  oo = open("spectrum-selfgains.dat", O_CREAT | O_RDWR | O_TRUNC, 0644);
  lseek(oo, 0, SEEK_SET);
//  for (i=0; i<(subbandnr_end-subbandnr_start); i++)
//    spectrum[i] /= gains[i*CHANNELS];
//   printf("%f\n", spectrum[i]);
  write(oo, spectrum, (subbandnr_end - subbandnr_start + 1)*CHANNELS*sizeof(float));
  close(oo);

  free (spectrum);
  exit (0);
*/

   
   /* If no more data, sto while loop */
   if( !num ) {
     break;
   }


   /* Calculate average. RMS not used right now. */
   //   float average[CHANNELS], rms[CHANNELS];
 
   //(void) time(&t1);
   //   fprintf(stderr, "  Calculate rms\n");
   tstart = clock();
   if(skip_calc_rms == 0) {
     for(nsub = subbandnr_start; nsub <= subbandnr_end; nsub++) {
       for( c = 0; c < CHANNELS; c++ ) {
	 float sum = 0.0f;
	 float ms = 0.0f;
	 int N = 0;
	 //	 int l;
	 unsigned validsamples = 0;
	 //	 float median = 0.0f;
	 /* compute average */
	 for( i = 0; i < num; i++ ) {
	   //	   float mvalue[SAMPLES];
	   /*	 for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	     mvalue[timeT] = stokesdata_h5[i].samples[beamnr][timeT][nsub][c][STOKES_SWITCH];
	     }*/
	   
	   /* Massive kludge - BWS - Skip first 4 samples always due to distortion of rms */
	   /* Added 2011-11-16 */
	   for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	     float value;
	     //	     float mtemp;
	     value = stokesdata_h5[i].samples[beamnr][timeT][nsub][c][STOKES_SWITCH] / gains[(nsub-subbandnr_start)*CHANNELS+c];
	     // if (c == 8) printf("BWS AVG time %d num %d channel %d nsub %ld value %f prev %f sum %f\n",time,i,c,nsub,value,prev,sum);
	     
	     // Try median
	     /* for (l=timeT+1;l<SAMPLES;l++)
		{
		if (!isnan(mvalue[l]) && !isnan(mvalue[timeT])) 
		{
		if (mvalue[timeT] > mvalue[l])
		{
		mtemp = mvalue[l];
		mvalue[l] = mvalue[timeT];
		mvalue[timeT] = mtemp;
		}
		}
		}
		if (SAMPLES%2 == 0) median = (mvalue[SAMPLES/2]+mvalue[SAMPLES/2-1])/2;
		else median = mvalue[SAMPLES/2];*/
	     
	     // Mean method 
	     if( !isnan( value ) ) {
	       sum += value;
	       ms += value*value;
	       validsamples++;
	     }
	   }
	 }
	 
	 N = validsamples;
	 if(is_append) {
	   if(N == 0) {
	     average[nsub*CHANNELS+c] = 0;
	     rms[nsub*CHANNELS+c] = 0;
	   }else if(N == 1) {
	     average[nsub*CHANNELS+c] = sum;
	     rms[nsub*CHANNELS+c] = 0;
	   }else {
	     average[nsub*CHANNELS+c] = sum/N;
	     rms[nsub*CHANNELS+c] = sqrt((ms-(N*average[nsub*CHANNELS+c]*average[nsub*CHANNELS+c]))/(N-1));
	   }
	   if (debugpacking) printf("N = %d average[%ld] = %lf rms[%ld] = %lf\n",N,nsub*CHANNELS+c,average[nsub*CHANNELS+c],nsub*CHANNELS+c,rms[nsub*CHANNELS+c]);
	 }else {
	   if(N == 0) {
	     average[c] = 0;
	     rms[c] = 0;
	   }else if(N == 1) {
	     average[c] = sum;
	     rms[c] = 0;
	   }else {
	     average[c] = sum/N;
	     rms[c] = sqrt((ms-(N*average[c]*average[c]))/(N-1));
	   }
	   if (debugpacking) printf("N = %d average[%d] = %lf rms[%d] = %lf\n",N,c,average[c],c,rms[c]);
	 }

	 /* BWS: Ideal loop would count how many samples over the limit, but that requires more passes through
	    the data, so for now, just do it brute force, with three loops.... */
	 
	 int avgloops = 20;
	 int al;
	 float previous_rms,current_rms;
	 for ( al = 0; al < avgloops; al++)
	   {
	     sum = 0.0f;
	     ms = 0.0f;
	     validsamples = 0;
	     
	     for( i = 0; i < num; i++ ) {
	       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
		 float value;
		 value = stokesdata_h5[i].samples[beamnr][timeT][nsub][c][STOKES_SWITCH] / gains[(nsub-subbandnr_start)*CHANNELS+c];
		 // if (nsub == 237 && c == 1) printf("BWS AVG timeT %d num %d channel %d nsub %ld value %f sum %f %f %f\n",timeT,i,c,nsub,value,sum,average[nsub*CHANNELS+c],rms[nsub*CHANNELS+c]);
		 if(is_append) {
		   if (!isnan( value ) && value > average[nsub*CHANNELS+c] - 3.0 * rms[nsub*CHANNELS+c] && value < average[nsub*CHANNELS+c] + 3.0 *rms[nsub*CHANNELS+c]) 
		     {
		       sum += value;
		       ms += value*value;
		       validsamples++;
		     }
		 }else {
		   if (!isnan( value ) && value > average[c] - 3.0 * rms[c] && value < average[c] + 3.0 *rms[c]) 
		     {
		       sum += value;
		       ms += value*value;
		       validsamples++;
		     }
		 }
	       }
	     }
	     
	     N = validsamples;
	     if(is_append) {
	       previous_rms = rms[nsub*CHANNELS+c];
	       if(N == 0) {
		 average[nsub*CHANNELS+c] = 0;
		 rms[nsub*CHANNELS+c] = 0;
	       }else if(N == 1) {
		 average[nsub*CHANNELS+c] = sum;
		 rms[nsub*CHANNELS+c] = 0;
	       }else {
		 average[nsub*CHANNELS+c] = sum/N;
		 rms[nsub*CHANNELS+c] = sqrt((ms-(N*average[nsub*CHANNELS+c]*average[nsub*CHANNELS+c]))/(N-1));
	       }
	       if (debugpacking) printf("N %d avloop %d average[%ld] = %lf rms[%ld] = %lf\n",N,al,nsub*CHANNELS+c,average[nsub*CHANNELS+c],nsub*CHANNELS+c,rms[nsub*CHANNELS+c]);
	       current_rms = rms[nsub*CHANNELS+c];
	     }else {
	       previous_rms = rms[c];
	       if(N == 0) {
		 average[c] = 0;
		 rms[c] = 0;
	       }else if(N == 1) {
		 average[c] = sum;
		 rms[c] = 0;
	       }else {
		 average[c] = sum/N;
		 rms[c] = sqrt((ms-(N*average[c]*average[c]))/(N-1));
	       }
	       if (debugpacking) printf("N %d avloop %d average[%d] = %lf rms[%d] = %lf\n",N,al,c,average[c],c,rms[c]);
	       current_rms = rms[c];
	     }
	     if ( previous_rms / current_rms < 1.05 || current_rms == 0.0 ) break;
	   }
       }
       //(void) time(&t2);
       //   printf("nsub = %ld Time to process is %lf seconds\n",nsub,(double)(t2-t1));
     }
   }  /* End of skip_calc_rms */
   /* If only once want to calc rms'es etc, set flag to 1 */
   skip_calc_rms = 0;
   tend = clock()-tstart;
   time_calcrms += tend/(double)CLOCKS_PER_SEC;
   //   fprintf(stderr, "  Calculate rms done\n");

  /* convert and write the data */
   //    fprintf(stderr, "  Starting converting/writing\n");
    float powerNbits = (float)(pow(2,subintdata->NrBits)-1);
    for( i = 0; i < num; i++ ) {
     // (void) time(&t1);

     /*     fprintf(stderr, "XXXXXXXX %d - 2\n", i); */
   
       /* patrick */
      tstart = clock();
    if(sigmalimit > 0 && i == 0) {
       for(nsub = subbandnr_start; nsub <= subbandnr_end; nsub++) {
	 for( c = 0; c < CHANNELS; c++ ) {
	   if(is_append == 0) {
	     offsets[c]                    = average[c];
	     scales[c]  = (sigmalimit*rms[c])/powerNbits;
	     /*     if(debugpacking)
		    fprintf(stderr, "num %d nsub %ld ch %d scale=%e offset=%e (av=%e  rms=%e)\n", i, nsub, c, scales[c], offsets[c], average[c], rms[c]); */
	   }else {
	     offsets[nsub*CHANNELS+c]      = average[nsub*CHANNELS+c];
	     scales[nsub*CHANNELS+c]  = (sigmalimit*rms[nsub*CHANNELS+c])/powerNbits;
	     /*	     if(debugpacking)
		     fprintf(stderr, "num %d nsub %ld ch %d scale=%e offset=%e (av=%e  rms=%e)\n", i, nsub, c, scales[nsub*CHANNELS+c], offsets[nsub*CHANNELS+c], average[nsub*CHANNELS+c], rms[nsub*CHANNELS+c]);*/
	   }
	 }
       }
     }
    tend = clock()-tstart;
    time_calcscales += tend/(double)CLOCKS_PER_SEC;

     /* process data */
    //     fprintf(stderr, "  Start clipping/scaling\n");
    tstart = clock();
    float sum, gainfac, pwr_2_nbitsMin1, pwr_2_max, scalefac, leveloffset, channelaverage;
    float min_sum, max_sum;
    long channel_idx, cur_idx;
    pwr_2_nbitsMin1 = pow(2,subintdata->NrBits-1);
    pwr_2_max = pow(2,subintdata->NrBits)-1;
    for(nsub = subbandnr_start; nsub <= subbandnr_end; nsub++) {
      for( c = 0; c < CHANNELS; c++ ) {
	/* Calculate some constants that are used in the time-critical time loop. */
	gainfac = 1.0/gains[(nsub-subbandnr_start)*CHANNELS+c];
	if(is_append) {
	  channel_idx = nsub*CHANNELS+c;
	}else {
	  channel_idx = c;
	}
	scalefac = 1.0/scales[channel_idx];
	leveloffset = offsets[channel_idx];
	channelaverage = average[channel_idx];
	if(clipav) {
	  min_sum = (channelaverage - leveloffset)*scalefac;
	}else {
	  min_sum = 0;
	}
	if(clipav) {
	  max_sum = (channelaverage - leveloffset)*scalefac;
	}else {
	  max_sum = pwr_2_max;
	}
	cur_idx = (subintdata->NrBins)*((subintdata->NrPols)*(channel_idx));
	for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	 
	  sum = gainfac*stokesdata_h5[i].samples[beamnr][timeT][nsub][c][STOKES_SWITCH];
	  /* not sure; replacing sum by average if (NaN or within 0.01 of zero)? */
	  sum = (isnan(sum) || (sum <= 0.01f && sum >= -0.01f)) ? channelaverage : sum;
	   
	  if(sigmalimit > 0) {
	    /*	    if(debugpacking && nsub == 237)
	      printf("printf channel %d time %d value %e ", c, timeT, sum);
	    */
	    sum = (sum - leveloffset)*scalefac + pwr_2_nbitsMin1;
	    /*	    if(debugpacking && nsub == 237)
	      printf(" %e ", sum);*/

	    if(sum < 0) {
	      sum = min_sum;
	    }else if(sum > pwr_2_max) {
	      sum = max_sum;
	    }
	    /*	    if(debugpacking && nsub == 237)
	      printf(" %e \n", sum);
	    */
	  }

	  //     (void) time(&t2);
	  // printf("Time to process num %d is %lf seconds\n",i,(double)(t2-t1));
	  
	  /* BWS */
	  // if (nsub*CHANNELS+c == 20 && i == 0 ) printf("BWS %d %d %f %f %f %f\n",c,timeT,sum,stokesdata_h5[i].samples[beamnr][timeT][nsub][c][STOKES_SWITCH],offsets[nsub*CHANNELS+c], scales[nsub*CHANNELS+c]);
	  
	  
	  
	  
	  /* patrick: put polarization to 0, assume only one pol
	     written out. Think x is the current bin nr */
	  
	  /* Replace function calls with the actual code 
	  if(writePulsePSRData(*subintdata, 0, 0, channel_idx, timeT, 1, &sum) == 0) { 
	    fprintf(stderr, "Error writing to temporary memory\n");
	    return 0;
	  }
	  */	  
	  subintdata->data[cur_idx++] = sum;

	  /*	   fwrite( &sum, sizeof sum, 1, outputfile[c] ); */ /* single sample written to channel file*/
	  output_samples++;
	  if( sum == 0 ) nul_samples++;
	  
	} // for( timeT = 0; timeT < SAMPLES; timeT++ ) 
      } // Loop over channels
    } // Loop over subbands 
    tend = clock()-tstart;
    time_clipscale += tend/(double)CLOCKS_PER_SEC;
     //     fprintf(stderr, "  clipping/scaling done\n");
 

   
     /*     fprintf(stderr, "XXXXXXXX %d - 3\n", i); */
     if(verbose) {
       //       printf(".");
       if(is_append) {
	 printf("\rProcessed block %ld/%ld: %.3f%%", currentblock, fout.NrPulses, 100.0*((currentblock/((float)fout.NrPulses))));
       }else {
	 printf("\rProcessed block %ld/%ld of subband %d/%d: %.3f%%", currentblock, fout.NrPulses, subbandnr, SUBBANDS, 100.0*(subbandnr+(currentblock/((float)fout.NrPulses)))/((float)SUBBANDS));
       }
       fflush(stdout);
     }
     
     tstart = clock();
     //     fprintf(stderr, "  start packing data\n");
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
     tend = clock()-tstart;
     time_packing += tend/(double)CLOCKS_PER_SEC;
    //     fprintf(stderr, "  packing data done\n");


     /*     fprintf(stderr, "XXXXXXXX %d - 4\n", (x-1)*AVERAGE_OVER+i); */
     /* Write out subint */
     //     fprintf(stderr, "  start writing\n");
     tstart = clock();
     if((x-1)*AVERAGE_OVER+i - skipNrBlocks >= truncNrBlocks && truncNrBlocks > 0) {
       doneprocessing = 1;
     }else {
       if((x-1)*AVERAGE_OVER+i - skipNrBlocks >= 0) {
	 if(writeFITSsubint(fout, (x-1)*AVERAGE_OVER+i- skipNrBlocks, packeddata, scales, offsets) != 1) {
	   fprintf(stderr, "ERROR: Cannot write subint data               \n");      
	   return 0;
	 }
       }
     }
     //     fprintf(stderr, "  writing done\n");
     tend = clock()-tstart;
     time_writing += tend/(double)CLOCKS_PER_SEC;
     /*     fprintf(stderr, "XXXXXXXX %d - 5\n", (x-1)*AVERAGE_OVER+i); */
     if(doneprocessing)
       break;
     
   } /* End of process loop: for( i = 0; i < num; i++ ) { */
    //    fprintf(stderr, "  Converting/writing done\n");

   /*   fprintf(stderr, "XXXXXXXX X - 6\n"); */

   if(doneprocessing)
     break;
 } //  while( !feof( input ) ) {
 printf("\n");
 // printf("Wait before destruction\n");  scanf("%d", &x);
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 0, 1) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }
 // printf("Wait after destruction\n");  scanf("%d", &x);

 /* fprintf(stderr,"  %u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples); */
 
 // printf("XXXXX Free\n");
 // free(stokesdata_h5);    //  Patrick: No longer do the free, as the memory will be re-used in later calls. Freeing&reallocating the memory leads to big problems with large data sets.
 return 1;
} // void convert_nocollapse_H5()

/* Returns 1 if successful, 0 on error */
/* Only for CS data */
int convert_nocollapse_CS(datafile_definition fout, FILE *input, int beamnr, datafile_definition *subintdata, int *firstseq, int *lastseq, int findseq, float sigmalimit, int clipav, int verbose, int debugpacking, long skipNrBlocks, long truncNrBlocks, double *gains)
{
  typedef struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[SAMPLES|2][SUBBANDS][CHANNELS];
  } stokesdata_struct;
    
 stokesdata_struct *stokesdata;
 int prev_seqnr = -1;
 unsigned timeT;
 unsigned output_samples = 0;
 unsigned nul_samples = 0;
 /* unsigned toobig = 0, toosmall = 0; */
 unsigned num;
 int doneprocessing = 0;
 int x;
 long filesize;


 /* Only find out how many blocks (including gaps) are in the data, then quit function. */
 if(findseq) {

   stokesdata = (stokesdata_struct *) malloc(sizeof(stokesdata_struct) );
   if(stokesdata == NULL) {
     fprintf(stderr, "Memory allocation error\n");
     return 0;
   }
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
   if(verbose) printf("  Last sequence number: %d\n", *lastseq);
 
   //   fseek(input, (filesize-1)*sizeof(stokesdata_struct), SEEK_SET);
   /*
   fseek(input, 0, SEEK_SET);
   long curblock = 0;
   while( !feof( input ) ) {
     num = fread( &stokesdata[0], sizeof stokesdata[0], 1, input );
     if(num == 1) {
       unsigned i,c,s;
       i = 0;
       for( s = 0; s <= 0; s++ ) {
	 unsigned t;
	 float value;
	 for( t = 0; t < SAMPLES; t++ ) {
	   for( c = 100; c <= 100; c++ ) {
	     floatSwap( &stokesdata[i].samples[t][s][c] );
	     value = stokesdata[i].samples[t][s][c];
	     if(isnan( value )) {
	       fprintf(stderr, "Found a NAN (block: %ld)\n", curblock);
	     }
	     if(fabs(value) < 1e-3) {
	       fprintf(stderr, "Found a 0 (block: %ld sample %d)\n", curblock, t);
	     }
		//	     printf("%f XXXXX\n", stokesdata[i].samples[t][s][c]);
	   }
	 }
       }
     }
     printf("Block %ld done\n", curblock);
     curblock++;
   }
   exit(0);
   */

   free(stokesdata);
   return 1;
 }

 stokesdata = (stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(stokesdata_struct) );
 if(stokesdata == NULL) {
   fprintf(stderr, "Memory allocation error\n");
   return 0;
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
   unsigned i,c,s,ac;
   int orig_prev_seqnr;

   /* read data */
   num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );


   /* i loops over the blocks of data read in */
   for( i = 0; i < num; i++ ) {
     for( s = 0; s < SUBBANDS; s++ ) {
       unsigned t;
       for( t = 0; t < SAMPLES; t++ ) {
         for( c = 0; c < CHANNELS; c++ ) {
	   floatSwap( &stokesdata[i].samples[t][s][c] );
	   if(zapFirstChannel && CHANNELS > 1 && c == 0) {
	     stokesdata[i].samples[t][s][c] = 0.0;
	   }
         }
       }
     }
   }
 
   /*   fprintf(stderr, "XXXX %d\n", stokesdata[1].sequence_number);
   swap_endian( (char*)&stokesdata[1].sequence_number );
   fprintf(stderr, "XXXX %d\n", stokesdata[1].sequence_number);
   */

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
       fprintf(stderr,"\nconvert_nocollapse_CS: num %d gap between sequence numbers %u and %u.\n",i, prev_seqnr, stokesdata[i].sequence_number );
     }
     prev_seqnr = stokesdata[i].sequence_number;
   }

   /* Calculate average. RMS not used right now. */
   float *average = (float *)calloc (SUBBANDS * CHANNELS, sizeof(float));
   float *rms = (float *)calloc (SUBBANDS * CHANNELS, sizeof(float));
   if (!average || !rms) { perror("Out of memory: average or rms"); }
   
   for (s = 0; s < SUBBANDS; s++ ) {
   for( c = 0; c < CHANNELS; c++ ) {
     float sum = 0.0f;
     float ms = 0.0f;
     int N = 0;
     unsigned validsamples = 0;
     float prev = stokesdata[0].samples[0][s][c] / gains[s*CHANNELS+c];
     /* compute average */
     for( i = 0; i < num; i++ ) {
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         const float value = stokesdata[i].samples[timeT][s][c] / gains[s*CHANNELS+c];
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
   
     ac=s*CHANNELS+c;

     N = validsamples;
     if(N == 0) {
       average[ac] = 0;
       rms[ac] = 0;
     }else if(N == 1) {
       average[ac] = sum;
       rms[ac] = 0;
     }else {
       average[ac] = sum/N;
       rms[ac] = sqrt((ms-(N*average[ac]*average[ac]))/(N-1));
     }
   } // CHANNELS
  } // SUBBANDS
 
  /* convert and write the data */
   prev_seqnr = orig_prev_seqnr;
   for( i = 0; i < num; i++ ) {
     int gap;
     /*     fprintf(stderr, "XXXXXXXX %d - 1\n", i); */
     for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
       /* gaps, fill in zeroes */
       for( s = 0; s < SUBBANDS; s++ ) {
       for( c = 0; c < CHANNELS; c++ ) {
         ac=s*CHANNELS+c;
	 for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	   /*	     if (writefloats==1){ */
	   float sum = 0;
	   /* patrick: put polarization to 0, assume only one pol
	      written out. */
	   if(writePulsePSRData(*subintdata, 0, 0, ac, timeT, 1, &sum) == 0) { 
	     fprintf(stderr, "Error writing to temporary memory\n");
	     return 0;
	   }
	 }
       }
      } // SUBBANDS
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
       if(gap+1 - skipNrBlocks >= 0 && (gap+1 - skipNrBlocks <= truncNrBlocks || truncNrBlocks <= 0)) {
	 if(writeFITSsubint(fout, gap+1 - skipNrBlocks, packeddata, scales, offsets) != 1) {
	   fprintf(stderr, "ERROR: Cannot write subint data               \n");      
	   return 0;
	 }
	 output_samples += SAMPLES;
	 nul_samples += SAMPLES;
       }
     }  /* End of for(gap loop */
     stokesdata[i].sequence_number =  prev_seqnr + 1;       
     prev_seqnr = stokesdata[i].sequence_number;
   
       /* patrick */
     if(sigmalimit > 0 && i == 0) {
       for( s = 0; s < SUBBANDS; s++ ) {
       for( c = 0; c < CHANNELS; c++ ) {
         ac=s*CHANNELS+c;
	 /*
	 0                         = ivalue(average[c]-sigmalimit*rms[c]);
	 0                         = (average[c]-sigmalimit*rms[c] - offset)/scale;
	 0                         = (average[c]-sigmalimit*rms[c] - offset);
	 */
	 offsets[ac]                    = average[ac]-sigmalimit*rms[ac];

	 /*
	 pow(2,subintdata.NrBits)-1  = ivalue(average[c]+sigmalimit*rms[c]);
	 pow(2,subintdata.NrBits)-1  = (average[c]+sigmalimit*rms[c]-offset)/scale;
	 */
	 scales[ac]  = (average[ac]+sigmalimit*rms[ac]-offsets[ac])/(float)(pow(2,subintdata->NrBits)-1);
	 if(debugpacking)
	   fprintf(stderr, "scale=%e offset=%e (av=%e  rms=%e)\n", scales[ac], offsets[ac], average[ac], rms[ac]);
       }
      } // SUBBANDS
     }
     /* process data */
     for( s = 0; s < SUBBANDS; s++ ) {
     for( c = 0; c < CHANNELS; c++ ) {
       ac=s*CHANNELS+c;
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         float sum;
         sum = stokesdata[i].samples[timeT][s][c] / gains[s*CHANNELS+c];
	 
	 /* not sure; replacing sum by average if (NaN or within 0.01 of zero)? */
	 sum = (isnan(sum) || (sum <= 0.01f && sum >= -0.01f)) ? average[ac] : sum;
	 if(sigmalimit > 0) {
	   if(debugpacking)
	     printf("value %e ", sum);
	   sum = (sum - offsets[ac])/scales[ac];
	   if(debugpacking)
	     printf(" %e ", sum);
	   if(sum < 0) {
	     if(clipav)
	       sum = (average[ac] - offsets[ac])/scales[ac];
	     else
	       sum = 0;
	     /*	     printf("Underflow! %e\n", sum); */
	   }
	   if(sum > pow(2,subintdata->NrBits)-1) {
	     if(clipav)
	       sum = (average[ac] - offsets[ac])/scales[ac];
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
	 if(writePulsePSRData(*subintdata, 0, 0, ac, timeT, 1, &sum) == 0) { 
	   fprintf(stderr, "Error writing to temporary memory");
	   return 0;
	 }
	   /*	   fwrite( &sum, sizeof sum, 1, outputfile[c] ); */ /* single sample written to channel file*/
	 output_samples++;
	 if( sum == 0 ) nul_samples++;

       } // for( timeT = 0; timeT < SAMPLES; timeT++ ) 
     }
   } // SUBBANDS

     /*     fprintf(stderr, "XXXXXXXX %d - 3\n", i); */
   

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
     if((x-1)*AVERAGE_OVER+i - skipNrBlocks >= truncNrBlocks && truncNrBlocks > 0) {
       doneprocessing = 1;
     }else {
       if((x-1)*AVERAGE_OVER+i - skipNrBlocks >= 0) {
	 if(writeFITSsubint(fout, (x-1)*AVERAGE_OVER+i - skipNrBlocks, packeddata, scales, offsets) != 1) {
	   fprintf(stderr, "ERROR: Cannot write subint data               \n");      
	   return 0;
	 }
       }
     }
   
     if(verbose) {
       printf("\rProcessed block %d/%ld (%.3f%%)", (x-1)*AVERAGE_OVER+i, fout.NrPulses, 100.0*((x-1)*AVERAGE_OVER+i)/(float)fout.NrPulses);
       fflush(stdout);
     }
     if(doneprocessing)
       break;
   }

  free (average);
  free (rms);

  if(doneprocessing)
    break;
 } //  while( !feof( input ) ) {
 printf("\n");
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 0, 1) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }

 free(stokesdata);
 return 1;
} // int convert_nocollapse_CS()


/* Returns 1 if successful, 0 on error */
/* Only for IS data in appending mode*/
int convert_nocollapse_ISappend(datafile_definition fout, int beamnr, datafile_definition *subintdata, int *seqseek, int *firstseq, int *lastseq, int findseq, float sigmalimit, int clipav, int verbose, int debugpacking, char **isfiles, double *gains)
{
  typedef struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][STOKES][CHANNELS][SAMPLES|2];
  }stokesdata_struct;
    
 stokesdata_struct *stokesdata[SUBBANDS];

 FILE *input[SUBBANDS]; // file descriptors for each subband
 int prev_seqnr = -1;
 unsigned timeT;
 unsigned output_samples = 0;
 unsigned nul_samples = 0;
 /* unsigned toobig = 0, toosmall = 0; */
 unsigned num, ss, seq;
 int x;
 long filesize;
 int min_lastseq, max_firstseq;

 for (ss=0; ss<SUBBANDS; ss++) {
   stokesdata[ss] = (stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(stokesdata_struct) );
   if(stokesdata[ss] == NULL) {
     fprintf(stderr, "Memory allocation error\n");
     return 0;
   }
   /* open file */
   input[ss] = fopen(isfiles[ss], "rb");
   if(input == NULL) {
     fprintf(stderr, "2bf2fits: Cannot open input file\n");
     return 0;
   }
 } // s

 /* Only find out how many blocks (including gaps) are in the data, then quit function. */
 if(findseq) {
   fseek( input[0], 0, SEEK_SET );
   num = fread( &stokesdata[0][0], sizeof(stokesdata_struct), 1, input[0] );
   if(num != 1) {
     fprintf(stderr, "convert_nocollapse_ISappend: No data in first subband file?\n");
     return 0;
   }
   swap_endian( (char*)&stokesdata[0][0].sequence_number );
   *firstseq = stokesdata[0][0].sequence_number;
   seqseek[0] = *firstseq;
   max_firstseq = *firstseq;
   fseek(input[0], 0, SEEK_END);
   filesize = ftell(input[0]);
   if(verbose) printf("  File size %ld bytes = %ld blocks of %ld bytes\n", filesize, filesize/sizeof(stokesdata_struct), sizeof(stokesdata_struct));
   filesize /= sizeof(stokesdata_struct);
   fseek(input[0], (filesize-1)*sizeof(stokesdata_struct), SEEK_SET);
   num = fread( &stokesdata[0][0], sizeof(stokesdata_struct), 1, input[0] );
   if(num != 1) {
     fprintf(stderr, "Error reading file.\n");
     return 0;
   }
   swap_endian( (char*)&stokesdata[0][0].sequence_number );
   *lastseq = stokesdata[0][0].sequence_number;
   min_lastseq = *lastseq;

   for (ss=1; ss<SUBBANDS; ss++) {
     fseek( input[ss], 0, SEEK_SET );
     num = fread( &stokesdata[ss][0], sizeof(stokesdata_struct), 1, input[ss] );
     if(num != 1) {
       fprintf(stderr, "convert_nocollapse_ISappend: No data in subband file %d\n", ss);
       seqseek[ss] = -1;   // Set a flag so we know it shouldn't be processed
     }else {
       swap_endian( (char*)&stokesdata[ss][0].sequence_number );
       seqseek[ss] = stokesdata[ss][0].sequence_number;
       if (stokesdata[ss][0].sequence_number > max_firstseq) {
	 max_firstseq = stokesdata[ss][0].sequence_number;
       }
       fseek(input[ss], 0, SEEK_END);
       filesize = ftell(input[ss]);
       if(verbose) printf("  File size %ld bytes = %ld blocks of %ld bytes\n", filesize, filesize/sizeof(stokesdata_struct), sizeof(stokesdata_struct));
       filesize /= sizeof(stokesdata_struct);
       fseek(input[ss], (filesize-1)*sizeof(stokesdata_struct), SEEK_SET);
       num = fread( &stokesdata[ss][0], sizeof(stokesdata_struct), 1, input[ss] );
       if(num != 1) {
	 fprintf(stderr, "Error reading file.\n");
	 return 0;
       }
       swap_endian( (char*)&stokesdata[ss][0].sequence_number );
       if (stokesdata[ss][0].sequence_number < min_lastseq) {
	 min_lastseq = stokesdata[ss][0].sequence_number;
       }
     }
   } // ss
 
   if (max_firstseq > *firstseq) {
    if (verbose) {
     printf ("First sequence number of subband 0 = %d, the maximum among all subband files = %d\n", *firstseq, max_firstseq);
    }
    *firstseq = max_firstseq;
   }
   if (min_lastseq < *lastseq) {
    if (verbose) {
     printf ("Last sequence number of subband 0 = %d, the minimum among all subband files = %d\n", *lastseq, min_lastseq);
    }
    *lastseq = min_lastseq;
   } 

   printf ("Number of blocks to be converted: %d\n", *lastseq - *firstseq + 1);
  
   //adjusting seek offsets
   for (ss=0; ss<SUBBANDS; ss++) {
     if(seqseek[ss] != -1)
       seqseek[ss] = max_firstseq - seqseek[ss];
   }
   if (verbose) {
     for (ss=0; ss<SUBBANDS; ss++) {
       if (seqseek[ss] != 0 && seqseek[ss] != -1) printf ("The first seq number of subband %d is %d blocks smaller!\n", ss, seqseek[ss]);
     }
   }

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
    - Reads AVERAGE_OVER (10) blocks of data at the timeT from the input data. 
    - Swap some bytes around
    - Look if there is a gap in the data (sequence numbers are missing) and write zero's in gaps.
 */

 // positioning the start pointer for each file depending on its sequence number
 for (ss=0; ss<SUBBANDS; ss++) 
   fseek(input[ss], seqseek[ss] * sizeof(stokesdata_struct), SEEK_SET);

 printf ("\nProcessing...\n");

 x = 0;
 // loop on reading sequence blocks from subband files
 for (seq=0; seq < *lastseq - *firstseq + 1; seq+=AVERAGE_OVER) {

   printf ("   blocks %d-%d (%d)\n", seq + 1, seq+AVERAGE_OVER, *lastseq - *firstseq + 1);

   int exit_flag = 0;
   if (x == NUM_BLOCKGROUPS) break;
   x++;
   unsigned i,c, ac;
   int orig_prev_seqnr;

   /* read data */
   for (ss=0; ss<SUBBANDS; ss++) {
     if(seqseek[ss] != -1) {    /* There is data in the file */
       num = fread( &stokesdata[ss][0], sizeof(stokesdata_struct), AVERAGE_OVER, input[ss] );
       /* If no more data, sto while loop */
       if( !num ) { exit_flag = 1; break; }
     
       /* i loops over the blocks of data read in */
       for( i = 0; i < num; i++ ) {
	 for( c = 0; c < CHANNELS; c++ ) {
	   unsigned b,t,s;
	   b = beamnr;
	   for( t = 0; t < SAMPLES; t++ ) {
	     for( s = 0; s < STOKES; s++ ) {
	       floatSwap( &stokesdata[ss][i].samples[b][s][c][t] );
	       if(zapFirstChannel && CHANNELS > 1 && c == 0) {
		 stokesdata[ss][i].samples[b][s][c][t]  = 0.0;
	       }
	     }
	   }
	 }
       }
     }else {
       for( i = 0; i < AVERAGE_OVER; i++ ) {
	 for( c = 0; c < CHANNELS; c++ ) {
	   unsigned b,t,s;
	   b = beamnr;
	   for( t = 0; t < SAMPLES; t++ ) {
	     for( s = 0; s < STOKES; s++ ) {
	       stokesdata[ss][i].samples[b][s][c][t]  = 0.0;
	     }
	   }
	 }
       }
     }
     
   } // SUBBANDS
   if (exit_flag == 1) break;
 

   /* Print if there are gaps in the data. It doesn't deal with it here though. */
   orig_prev_seqnr = prev_seqnr;
   for (ss=0; ss<SUBBANDS; ss++) {
     for( i = 0; i < num; i++ ) {
       /* sequence number is big endian */
       if(seqseek[ss] != -1) {    /* There is data in the file */
	 swap_endian( (char*)&stokesdata[ss][i].sequence_number );
       
	 /* detect gaps */
	 if( prev_seqnr + 1 != stokesdata[ss][i].sequence_number ) {
	   fprintf(stderr,"\nconvert_nocollapse_ISappend: num %d gap between sequence numbers %u and %u.\n",i, prev_seqnr, stokesdata[ss][i].sequence_number );
	 } 
	 prev_seqnr = stokesdata[ss][i].sequence_number;
       }
     }
     prev_seqnr = orig_prev_seqnr;
   } // SUBBANDS
 
   /* Calculate average. RMS not used right now. */
   float *average = (float *)calloc (SUBBANDS * CHANNELS, sizeof(float));
   float *rms = (float *)calloc (SUBBANDS * CHANNELS, sizeof(float));
   if (average == NULL || rms == NULL) { perror("Out of memory: average or rms"); exit(0); }
 
   for (ss=0; ss<SUBBANDS; ss++) {
   for( c = 0; c < CHANNELS; c++ ) {
     float sum = 0.0f;
     float ms = 0.0f;
     int N = 0;
     unsigned validsamples = 0;
     float prev = stokesdata[ss][0].samples[beamnr][STOKES_SWITCH][c][0] / gains[ss*CHANNELS+c];
     /* compute average */
     for( i = 0; i < num; i++ ) {
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         const float value = stokesdata[ss][i].samples[beamnr][STOKES_SWITCH][c][timeT] / gains[ss*CHANNELS+c];
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
   
     ac = ss * CHANNELS + c;

     N = validsamples;
     if(N == 0) {
       average[ac] = 0;
       rms[ac] = 0;
     }else if(N == 1) {
       average[ac] = sum;
       rms[ac] = 0;
     }else {
       average[ac] = sum/N;
       rms[ac] = sqrt((ms-(N*average[ac]*average[ac]))/(N-1));
     }
   }
   } // SUBBANDS

  /* convert and write the data */
   prev_seqnr = orig_prev_seqnr;
   for( i = 0; i < num; i++ ) {
     int gap;

// here I should check next sequence number among all subbands file and choose the largest one
// and then do fseek (...SEEK_CUR) for the files with smaller seq numbers...
// for now, we assume that even if there are gaps in the middle of data streams, they are the same for all subbands....

     for( gap = prev_seqnr + 1; gap < stokesdata[0][i].sequence_number; gap++ ) {
       /* gaps, fill in zeroes */
       for (ss=0; ss<SUBBANDS; ss++) {
	 for( c = 0; c < CHANNELS; c++ ) {
	   for( timeT = 0; timeT < SAMPLES; timeT++ ) {
	     /*	     if (writefloats==1){ */
	     float sum = 0;
	     /* patrick: put polarization to 0, assume only one pol
		written out. */
	     ac = ss * CHANNELS + c;
	     if(writePulsePSRData(*subintdata, 0, 0, ac, timeT, 1, &sum) == 0) { 
	       fprintf(stderr, "Error writing to temporary memory\n");
	       return 0;
	     }
	   }
	 }
       } // SUBBANDS
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
     for (ss=0; ss<SUBBANDS; ss++) {
       stokesdata[ss][i].sequence_number =  prev_seqnr + 1;       
     }
       prev_seqnr = stokesdata[0][i].sequence_number;
     

     /*     fprintf(stderr, "XXXXXXXX %d - 2\n", i); */
   
       /* patrick */
     if(sigmalimit > 0 && i == 0) {
       for (ss=0; ss<SUBBANDS; ss++) {
       for( c = 0; c < CHANNELS; c++ ) {
	 /*
	 0                         = ivalue(average[c]-sigmalimit*rms[c]);
	 0                         = (average[c]-sigmalimit*rms[c] - offset)/scale;
	 0                         = (average[c]-sigmalimit*rms[c] - offset);
	 */
         ac = ss * CHANNELS + c; 
	 offsets[ac]                    = average[ac]-sigmalimit*rms[ac];

	 /*
	 pow(2,subintdata.NrBits)-1  = ivalue(average[c]+sigmalimit*rms[c]);
	 pow(2,subintdata.NrBits)-1  = (average[c]+sigmalimit*rms[c]-offset)/scale;
	 */
	 scales[ac]  = (average[ac]+sigmalimit*rms[ac]-offsets[ac])/(float)(pow(2,subintdata->NrBits)-1);
	 if(debugpacking)
	   fprintf(stderr, "scale=%e offset=%e (av=%e  rms=%e)\n", scales[ac], offsets[ac], average[ac], rms[ac]);
       }
      } // SUBBANDS
     }
     /* process data */
     for (ss=0; ss<SUBBANDS; ss++) {
     for( c = 0; c < CHANNELS; c++ ) {
       for( timeT = 0; timeT < SAMPLES; timeT++ ) {
         float sum;
         ac = ss * CHANNELS + c;
         sum = stokesdata[ss][i].samples[beamnr][STOKES_SWITCH][c][timeT] / gains[ss*CHANNELS+c];
	 
	 /* not sure; replacing sum by average if (NaN or within 0.01 of zero)? */
	 sum = (isnan(sum) || (sum <= 0.01f && sum >= -0.01f)) ? average[ac] : sum;
	 if(sigmalimit > 0) {
	   if(debugpacking)
	     printf("value %e ", sum);
	   sum = (sum - offsets[ac])/scales[ac];
	   if(debugpacking)
	     printf(" %e ", sum);
	   if(sum < 0) {
	     if(clipav)
	       sum = (average[ac] - offsets[ac])/scales[ac];
	     else
	       sum = 0;
	     /*	     printf("Underflow! %e\n", sum); */
	   }
	   if(sum > pow(2,subintdata->NrBits)-1) {
	     if(clipav)
	       sum = (average[ac] - offsets[ac])/scales[ac];
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
	 if(writePulsePSRData(*subintdata, 0, 0, ac, timeT, 1, &sum) == 0) { 
	   fprintf(stderr, "Error writing to temporary memory");
	   return 0;
	 }
	   /*	   fwrite( &sum, sizeof sum, 1, outputfile[c] ); */ /* single sample written to channel file*/
	 output_samples++;
	 if( sum == 0 ) nul_samples++;

       } // for( timeT = 0; timeT < SAMPLES; timeT++ ) 
     }
    } // SUBBANDS

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

   } // loop on num

   /*   fprintf(stderr, "XXXXXXXX X - 6\n"); */

  free (average);
  free (rms);

 } // for (seq=0; .)

 printf("\n");
 if(constructFITSsearchsubint(fout, subintdata->data, 0, &packeddata, &scales, &offsets, 0, 0, 1) != 1) {
   fprintf(stderr, "ERROR writeFITSfile: Cannot allocate temporary memory               \n");      
   return 0;
 }

 /* fprintf(stderr,"  %u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples); */
 
 for (ss=0; ss<SUBBANDS; ss++) { 
  fclose(input[ss]);
  free(stokesdata[ss]);
 }
 return 1;
} // int convert_nocollapse_ISappend()






/*
 *  ====================== M A I N ====================================
 */
int main( int argc, char **argv ) 
{
  int i,j,b, index, firstseq, lastseq, readparset, totalnrchannels, subbandnr, nrbits, debugpacking;
  //  int *seqseek; // only for IS data in append mode
                // array that will keep the seek offsets for input files due to possible differences 
                // between firstseq (same for all subbands) and actual first sequence number of each file
  int is_CS = 0; // if 1 then input file is CS data
  int is_H5 = 0; // if 1 then input file is H5 data
  int is_append = 0;  // if 1 then the single fits-file will be written for IS data
  int is_gains = 0;   // if 1 then raw data will be bandpass corrected
  char buf[1024], *filename, *dummy_ptr;
  datafile_definition subintdata, fout;
  patrickSoftApplication application;
  FILE *fin;
  char *s_ptr, dummy_string[parsetmaxlinelength], dummy_string2[parsetmaxlinelength];
  int channellist[1000];
  char tmpname[40];
  //  char channellist_name[100];
  int nrlines, ret;
  int blocksperStokes, integrationSteps, clockparam, subbandFirst, nsubbands, clipav;
  float lowerBandFreq, lowerBandEdge, subband_width, bw, sigma_limit;
  double lofreq;
  long skipNrBlocks, truncNrBlocks;
  //  int  is_IS2 = 0; // 0 for old/orginal names ; 1 = Incoherentstokes 2nd transpose parset names 
  char IncoherentStokesAreTransposed[6]="";  // exists and True for Incoherentstokes 2nd transpose parset names
  double *gains;   // array with gains, if gains are not given then has all ones...

  //  char header_txt[parsetmaxnrlines][1001], *txt[parsetmaxnrlines],;
  char *header_txt[parsetmaxnrlines];

  clock_t tstart, tend;
  tstart = clock();
		



  for(i = 0; i < parsetmaxnrlines; i++) {
    header_txt[i] = calloc(parsetmaxlinelength+1, 1);
    if(header_txt[i] == NULL) {
      fprintf(stderr, "2bf2fits: Cannot allocate memory\n");
      return 0;
    }
  }

  initApplication(&application, "2bf2fits", "[options] inputfiles");
  application.switch_headerlist = 1;
  application.switch_header = 1;
  application.switch_verbose = 1;
  readparset = 0;
  nrbits = 8;
  subbandnr = 0;
  skipNrBlocks = 0;
  truncNrBlocks = 0;
  subband_width = 0;
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
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-h") == 0) {
        usage();
        return 0;
      }else if(strcmp(argv[i], "-nbits") == 0) {
	j = sscanf(argv[i+1], "%d", &nrbits);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-skip") == 0) {
	j = sscanf(argv[i+1], "%ld", &skipNrBlocks);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-trunc") == 0) {
	j = sscanf(argv[i+1], "%ld", &truncNrBlocks);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-sigma") == 0) {
	j = sscanf(argv[i+1], "%f", &sigma_limit);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-clipav") == 0) {
	clipav = 1;
      }else if(strcmp(argv[i], "-CS") == 0) {
	is_CS = 1;
      }else if(strcmp(argv[i], "-H") == 0) {
	puts("Enable H5 format.");
	is_H5 = 1;
      }else if(strcmp(argv[i], "-IS") == 0) {
	puts("Post-IS 2nd transpose;  this is really an IS beam for parset reading (processed as CS data)");
	CSIS_MODE = 1;
      }else if(strcmp(argv[i], "-append") == 0) {
	is_append = 1;
      }else if(strcmp(argv[i], "-o") == 0) {
	j = sscanf(argv[i+1], "%s", OUTNAME);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-gains") == 0) {
	j = sscanf(argv[i+1], "%s", gainsfile);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        is_gains = 1;
        i++;
      }else if(strcmp(argv[i], "-nsubs") == 0) {
	j = sscanf(argv[i+1], "%d", &SUBBANDS);
	if(j != 1) {
	  fprintf(stderr, "2bf2fits: Error parsing %s option\n", argv[i]);
	  return 0;
	}
        i++;
      }else if(strcmp(argv[i], "-noZap0") == 0) {
	zapFirstChannel = 0;
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
    fprintf(stderr, "2bf2fits: No input files specified\n");
    return 0;
  }


  lofreq = 0;
  if(readparset) {
    fin = fopen(argv[readparset], "r");
    if(fin == NULL) {
      fprintf(stderr, "2bf2fits: Cannot open %s\n", argv[readparset]);
      return 0;      
    }

    nrlines = 0;
    ret = 0;

    //    for(i = 0; i < parsetmaxnrlines; i++)
    //      txt[i] = header_txt[i];
    do {
      if(fgets(header_txt[nrlines], parsetmaxlinelength, fin) != NULL)
	nrlines++;
      else
	ret = 1;
      if(nrlines >= parsetmaxnrlines-1) {
	fprintf(stderr, "2bf2fits: Too many lines in parset file (%d)\n", nrlines);
	return 0;
      }
    }while(ret == 0);
    fclose(fin);    
    if(application.verbose) printf("Read %d lines from header.\n", nrlines);

    // first check whether 2nd transpose IS has been set;  parset names vary before and after this transition
    s_ptr = get_ptr_entry("OLAP.IncoherentStokesAreTransposed", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", IncoherentStokesAreTransposed);
    }else {
      strcpy(IncoherentStokesAreTransposed, "False");
    }
    if((strcmp(IncoherentStokesAreTransposed, "True") == 0) || (strcmp(IncoherentStokesAreTransposed, "true") == 0) || (strcmp(IncoherentStokesAreTransposed, "T") == 0)){
      puts("Post-IS 2nd transpose;  reading new keywords");
      IS2 = 1;
    }else if(strcmp(IncoherentStokesAreTransposed, "False") == 0) {
      IS2 = 0;
    }

    if(IS2 == 0) {   
//      s_ptr = get_ptr_entry("OLAP.Storage.subbandsPerPart", header_txt, nrlines, "=");
//      if(s_ptr != NULL) {
//        sscanf(s_ptr, "%d", &(SUBBANDS));
//      }else {
//        fprintf(stderr, "2bf2fits: OLAP.Storage.subbandsPerPart not set\n");
//        return 0; 
//      }    
      s_ptr = get_ptr_entry("OLAP.Stokes.integrationSteps", header_txt, nrlines, "=");
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(integrationSteps));
      }else {
        fprintf(stderr, "2bf2fits: OLAP.Stokes.integrationSteps not set\n");
        return 0;     
      }
      s_ptr = get_ptr_entry("Observation.channelsPerSubband", header_txt, nrlines, "=");
      if(application.verbose) printf("Got %s channels from Observation.channelsPerSubband\n", s_ptr);
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(CHANNELS));
      }else {
        fprintf(stderr, "2bf2fits: Observation.channelsPerSubband not set\n");
        return 0;     
      }
      s_ptr = get_ptr_entry("Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps", header_txt, nrlines, "=");
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(SAMPLESPERSTOKESINTEGRATION));
      }else {
        fprintf(stderr, "2bf2fits: Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps not set\n");
        return 0;     
      }
    }else{  /* so IS2 != 0 */
//      if (CSIS_MODE == 0) { // was called OLAP.Storage.subbandsPerPart
//         s_ptr = get_ptr_entry("OLAP.CNProc_CoherentStokes.subbandsPerFile", header_txt, nrlines, "=");
//      }else {
//         s_ptr = get_ptr_entry("OLAP.CNProc_IncoherentStokes.subbandsPerFile", header_txt, nrlines, "=");
//      }
//      if(s_ptr != NULL) {
//        sscanf(s_ptr, "%d", &(SUBBANDS));
//      }else {
//        fprintf(stderr, "2bf2fits: subbandsPerFile not set\n");
//        return 0;
//      }
      if (CSIS_MODE == 0) { // was called OLAP.Stokes.integrationSteps
         s_ptr = get_ptr_entry("OLAP.CNProc_CoherentStokes.timeIntegrationFactor", header_txt, nrlines, "=");
      }else {
         s_ptr = get_ptr_entry("OLAP.CNProc_IncoherentStokes.timeIntegrationFactor", header_txt, nrlines, "=");
      }
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(integrationSteps));
      }else {
        fprintf(stderr, "2bf2fits: OLAP timeIntegrationFactor not set\n");
        return 0;      
      }
      if (CSIS_MODE == 0) { // was called Observation.channelsPerSubband
	s_ptr = get_ptr_entry("Observation.channelsPerSubband", header_txt, nrlines, "=");
	if(application.verbose) printf("Got %s channels from Observation.channelsPerSubband for time resolution calculation\n", s_ptr);
	if(s_ptr != NULL) {
	  sscanf(s_ptr, "%d", &(CHANNELS_TRES));
	}else {
	  fprintf(stderr, "2bf2fits: Observation.channelsPerSubband not set\n");
	  return 0;     
	}
	s_ptr = get_ptr_entry("OLAP.CNProc_CoherentStokes.channelsPerSubband", header_txt, nrlines, "=");
	 if(application.verbose) printf("Got %s channels from OLAP.CNProc_CoherentStokes.channelsPerSubband\n", s_ptr);
      }else {
         s_ptr = get_ptr_entry("OLAP.CNProc_IncoherentStokes.channelsPerSubband", header_txt, nrlines, "=");
	 if(application.verbose) printf("Got %s channels from OLAP.CNProc_IncoherentStokes.channelsPerSubband\n", s_ptr);
      }
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(CHANNELS));
      }else {
        fprintf(stderr, "2bf2fits: channelsPerSubband not set\n");
        return 0;   
      } 
      if (CSIS_MODE == 0) { // was called Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps
         s_ptr = get_ptr_entry("Observation.ObservationControl.OnlineControl.OLAP.CNProc_CoherentStokes.timeIntegrationFactor", header_txt, nrlines, "=");
      }else {
         s_ptr = get_ptr_entry("Observation.ObservationControl.OnlineControl.OLAP.CNProc_IncoherentStokes.timeIntegrationFactor", header_txt, nrlines, "=");
      }
      if(s_ptr != NULL) {
        sscanf(s_ptr, "%d", &(SAMPLESPERSTOKESINTEGRATION));
      }else {
        // transition case, try again with old name Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps
        s_ptr = get_ptr_entry("Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps", header_txt, nrlines, "=");
        if(s_ptr != NULL) {
          sscanf(s_ptr, "%d", &(SAMPLESPERSTOKESINTEGRATION));
        }else {
	  fprintf(stderr, "2bf2fits: Observation timeIntegrationFactor not set\n");
          return 0; 
	}  
      } 
    }

    s_ptr = get_ptr_entry("OLAP.CNProc.integrationSteps", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(blocksperStokes));
    }else {
      fprintf(stderr, "2bf2fits: OLAP.CNProc.integrationSteps not set\n");
      return 0;     
    }


    SAMPLES = blocksperStokes/integrationSteps;


    s_ptr = get_ptr_entry("Observation.Beam[0].angle1", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%f", &(subintdata.ra));
    }else {
      fprintf(stderr, "2bf2fits: Observation.Beam[0].angle1 not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.Beam[0].angle2", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%f", &(subintdata.dec));
    }else {
      fprintf(stderr, "2bf2fits: Observation.Beam[0].angle1 not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.Beam[0].target", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", (subintdata.psrname));
    }else {
      fprintf(stderr, "2bf2fits: Observation.Beam[0].target not set\n");
      return 0;     
    }

    s_ptr = get_ptr_entry("Observation.sampleClock", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%d", &(clockparam));
    }else {
      fprintf(stderr, "2bf2fits: Observation.sampleClock not set\n");
      return 0;     
    }
    s_ptr = get_ptr_entry("Observation.bandFilter", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", subintdata.instrument);
    }else {
      fprintf(stderr, "2bf2fits: Observation.bandFilter not set\n");
      return 0;     
    }
    strcpy(buf, subintdata.instrument);
    dummy_ptr = strchr(buf, '_');
    if(dummy_ptr != NULL)
      dummy_ptr[0] = 0;
    sscanf(buf+4, "%f", &lowerBandFreq);

    s_ptr = get_ptr_entry("Observation.Beam[0].subbandList", header_txt, nrlines, "=");
    if(s_ptr != NULL) {
      sscanf(s_ptr, "%s", dummy_string);
    }else {
      fprintf(stderr, "2bf2fits: Observation.Beam[0].subbandList not set\n");
      return 0;     
    }
    /* Get the list of subbands which should be there. */
    tmpnam(tmpname);
    sprintf(dummy_string2, "echo \"%s\" | awk '{print substr($1,2,length($1)-2)}' | awk -F\\, '{for (i=1; i <= NF ; i++)  print $i}' | awk -F\\. '{var=$1; while (var <=$NF) { print var; var +=1 }}' > \"%s\"\n", dummy_string, tmpname);
    /*    printf(dummy_string2); */
    system(dummy_string2);
    fin = fopen(tmpname, "r");
    if(fin == NULL) {
      fprintf(stderr, "2bf2fits: Error generating channel list\n");
      perror("");
      return 0;
    } 
    for(totalnrchannels = 0; totalnrchannels < 1000; totalnrchannels++) {
      ret = fscanf(fin, "%d", &channellist[totalnrchannels]);
      if(ret != 1)
	break;
    }
    fclose(fin);
    
    sprintf(dummy_string2, "rm \"%s\"\n", tmpname);
    system(dummy_string2);
    subbandFirst = channellist[0];

    nsubbands = 512;
  

    printf("This is a %s obs at %f MHz (clock=%d subbandFirst=%d totalnrchannels=%d)\n", subintdata.instrument, lowerBandFreq, clockparam, subbandFirst, totalnrchannels);

    if (clockparam == 200) {
      bw = 100.0;
      if (lowerBandFreq >= 200.0) lowerBandEdge = 200.0;
      else if (lowerBandFreq < 200.0 && lowerBandFreq >= 100.0) lowerBandEdge = 100.0;
      else lowerBandEdge = 0.0;
    } else { // 160 MHz clock
      bw = 80.0;
      if (lowerBandFreq >= 160.0) lowerBandEdge = 160.0;
      else if (lowerBandFreq < 160.0 && lowerBandFreq >= 80.0) lowerBandEdge = 80.0;
      else lowerBandEdge = 0.0;
    }

    subband_width = bw / nsubbands;
    /*    printf("XXXXX %f = %f / %d\n", subband_width, bw, nsubbands); */
    lofreq = (double) lowerBandEdge + (subbandFirst*subband_width) - (subband_width/(2*CHANNELS));

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
    long double sec_mid;
    s_ptr = get_ptr_entry("Observation.startTime", header_txt, nrlines, "=");
    y = 0;
    if(s_ptr != NULL) {
      /*      sscanf(s_ptr, "%d-%d-%d %d:%d:%d", &y, &m, &d, &hour, &min, &sec);*/
      sscanf(s_ptr, "'%d-%d-%d %d:%d:%d'", &y, &m, &d, &hour, &min, &sec);
    }else {
      fprintf(stderr, "2bf2fits: Observation.startTimeT not set\n");
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
    subintdata.mjd = (subintdata.mjd - 2400000.5) + sec_mid / (long double)86400;

    printf("start=%d-%d-%d %d:%d:%d (%s %Lf)\n", y, m, d, hour, min, sec, s_ptr, subintdata.mjd);
    /*Observation.startTime = '2010-05-12 08:58:25' */

  }  /* End of read parset */


  if (is_CS == 1) printf ("CS data: %d subbands\n", SUBBANDS);
  if (is_append == 1) printf ("append mode (%d subbands), single output file will be written\n", SUBBANDS);
  printf( "%d channels %d beams %d samples %d stokes\n", CHANNELS, BEAMS, SAMPLES, STOKES );
  printf("Output Name: %s\n", OUTNAME);
  printf("Stokes Parameter: %d\n\n",STOKES_SWITCH);


  subintdata.NrBins = SAMPLES;
  if ((is_CS == 0 && is_append == 0) || (is_H5 && is_append == 0))
    subintdata.nrFreqChan = CHANNELS;
  else 
    subintdata.nrFreqChan = CHANNELS * SUBBANDS;
  subintdata.NrPols = 1;
  subintdata.NrBits = nrbits;
  /* Actual number of channels is not always the parameter to use for time resolution calculation. */
  if(CHANNELS_TRES >= 0) {
    subintdata.SampTime = (SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*clockparam*1000*1000/FFTLENGTH/CHANNELS_TRES));
  }else {
    subintdata.SampTime = (SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*clockparam*1000*1000/FFTLENGTH/CHANNELS));
  }
  subintdata.NrPulses = 1;   
  subintdata.format = MEMORY_format;
  subintdata.Period = -1;   
  subintdata.bw = subband_width;
  subintdata.channelbw = subband_width/(float)CHANNELS;
  subintdata.fd_type = 1;  /* Not sure, guessing something. Not used by dspsr I think */
  subintdata.poltype = 1; /* Stokes data */

  /* Search commandline for header parameters to overwrite header
     parameters. */
  if(PSRDataHeader_parse_commandline(&subintdata, argc, argv, application.verbose) == 0)
    return 0;

  // check if observatory is empty, and write LOFAR as default observatory
  if (!strcmp(subintdata.observatory, "")) strncpy(subintdata.observatory, "LOFAR", 100);

  subintdata.data = (float *)malloc(subintdata.NrBins*subintdata.nrFreqChan*subintdata.NrPols*sizeof(float));
  if(subintdata.data == NULL) {
    fprintf(stderr, "2bf2fits: Cannot allocate memory\n");
    return 0;
  }
 
  // initializing gains array
  gains = (double *)malloc(subintdata.nrFreqChan * sizeof(double));
  if (gains == NULL) {
   fprintf (stderr, "2bf2fits: Cannot allocate memory\n");
   return 0;
  }
  if (is_gains == 1) {
   gains = create_gains_table(gains, gainsfile, subintdata.nrFreqChan, subintdata.channelbw, lofreq);
  } else {
   for (j=0; j<subintdata.nrFreqChan; j++) gains[j] = 1.0;
  }

  /* make beam dirs */ 
  if( BEAMS > 1 ) {
    for( b = 0; b < BEAMS; b++ ) {
      sprintf( buf, "beam_%d", b );
      mkdir( buf, 0775 );
    }
  }

  // processing with appending (only for one beam)
  if (is_append == 1 && is_H5 == 0) {
   int seqseek[SUBBANDS]; // only for IS data in append mode
                          // array that will keep the seek offsets for input files due to possible differences 
                          // between firstseq (same for all subbands) and actual first sequence number of each file
   b = 0; // beam = 0 

   // forming the list of input files
   char *sbpointer;
   char *isfiles[SUBBANDS];
   for (j=0; j<SUBBANDS; j++) {
    isfiles[j] = (char *)malloc(1024);
    if (!isfiles[j]) { perror("isfiles malloc"); exit (1); }
    /* Patrick: Corrected - Should be '\0' or simply 0 */
    /*    memset (isfiles[j], "\0", 255); */
    memset (isfiles[j], 0, 255);
   }
   j=0;
   while((filename = getNextFilenameFromList(&application, argv)) != NULL) {
    sbpointer = strstr (filename, "_SB");
    sscanf(sbpointer, "%*3c%d", &subbandnr);
    // here we place the each filename in the proper order (so we don't need to sort the list out)
    sprintf (isfiles[subbandnr], "%s", filename);
    j++;
   }
   if (j != SUBBANDS) {
    fprintf (stderr, "Number of input files (%d) is not equal to the number of subbands (%d)!\n", j, SUBBANDS);
    return 0;
   }

   if (CHANNELS > 1) {
      subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS-1) * subintdata.bw;
   } else {  // bypass 2PPF, freq_cent calculation changes
      subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS) * subintdata.bw;
   }

   // outname
   sprintf(buf, "%s.fits", OUTNAME);
   printf("Output file \"%s\" at center frequency %f MHz\n", buf, subintdata.freq_cent);

   if (convert_nocollapse_ISappend(fout, b, &subintdata, seqseek, &firstseq, &lastseq, 1, sigma_limit, clipav, application.verbose, debugpacking, isfiles, gains) == 0)
    return 0;

   if(application.verbose) printf("  File contains sequence numbers %d - %d\n", firstseq, lastseq);

   if(application.verbose)  printf ("Allocating memory for packed data...\n");

   /* Copy header parameters to output header */
   copy_paramsPSRData(subintdata, &fout);
   fout.NrPulses = lastseq+1;
   if(application.verbose) {
    printf("  Header dump:\n");
    printHeaderPSRData(fout, 4);
   }
   if(skipNrBlocks != 0 || truncNrBlocks != 0) {
     fprintf(stderr, "2bf2fits: Skipping/truncating blocks is not implemented for this input data format.\n");
     exit(-1);
   }

   if(!openPSRData(&fout, buf, FITS_format, 1, 0, application.verbose)) return 0;
   if(!writePSRFITSHeader(&fout, application.verbose)) return 0;

   if(convert_nocollapse_ISappend(fout, b, &subintdata, seqseek, &firstseq, &lastseq, 0, sigma_limit, clipav, application.verbose, debugpacking, isfiles, gains) == 0)
    return 0;

   closePSRData(&fout);

   for (j=0; j<SUBBANDS; j++) free (isfiles[j]);

  } // if (is_append == 1)

  if (is_append == 0 || is_H5) {
  /* open files */
  char *sbpointer;
  long subbandnr_h5, subbandnr_h5_first, subbandnr_h5_last;
  if(is_H5 && is_append == 0) {
    subbandnr_h5_first = 0;
    subbandnr_h5_last = SUBBANDS - 1;
    fprintf(stderr, "Going to process subbands %ld to %ld\n", subbandnr_h5_first, subbandnr_h5_last);
  }else {
    subbandnr_h5_first = 0;  /* Ignore loop if not H5 */
    subbandnr_h5_last = 0;
  }
  for( b = 0; b < BEAMS; b++ ) {
   /* loop over input files */
    while((filename = getNextFilenameFromList(&application, argv)) != NULL) {
      printf("Processing %s\n", filename);
      for(subbandnr_h5 = subbandnr_h5_first; subbandnr_h5 <= subbandnr_h5_last; subbandnr_h5++) {

      if(is_H5) {
	if(is_append == 0){
	  subintdata.freq_cent = (double) lofreq + subbandnr_h5 * subintdata.bw;
        }
	else {
          if (CHANNELS > 1) {
	     subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS-1) * subintdata.bw;
           } else {  // bypass 2PPF, freq_cent calculation changes
	     subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS) * subintdata.bw;
           }
        }
      }else if (is_CS == 0) { // IS data
         sbpointer = strstr (filename, "_SB");
         sscanf(sbpointer, "%*3c%d", &subbandnr);
         subintdata.freq_cent = (double) lofreq + subbandnr * subintdata.bw;
      } else { // CS data
        if (CHANNELS > 1) {
	   subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS-1) * subintdata.bw;
        } else { // bypass 2PPF, freq_cent calculation changes
	   subintdata.freq_cent = (double) lofreq + 0.5 * (SUBBANDS) * subintdata.bw;
        }
      }
      if(is_H5) {
	if(is_append == 0) {
	  printf("  This is file number %d at centre frequency %f MHz\n", subbandnr, subintdata.freq_cent);
	}
      }else {
	printf("  This is subband number %ld at centre frequency %f MHz\n", subbandnr_h5, subintdata.freq_cent);
      }
      /* create names */
      if (is_CS == 0) 
	sprintf( buf, "%s.sub%04d", OUTNAME, subbandnr);
      else if(is_H5 && is_append == 0) {
	sprintf( buf, "%s.sub%04ld", OUTNAME, subbandnr_h5);
      }else 
	sprintf( buf, "%s.fits", OUTNAME);
      if ( BEAMS > 1  ) sprintf( buf, "beam_%d/%s", b, buf ); /* prepend beam name */
      fprintf(stderr,"  %s -> %s\n", filename, buf); 

      /* open file */
      fin = fopen(filename, "rb");
      if(fin == NULL) {
	fprintf(stderr, "2bf2fits: Cannot open input file\n");
	return 0;
      }
      if (is_CS == 0) {
	if(convert_nocollapse(fout, fin, b, &subintdata, &firstseq, &lastseq, 1, sigma_limit, clipav, application.verbose, debugpacking) == 0)
	  return 0;
      }
      if (is_CS == 1) {
        if(is_H5) {
	  if(convert_nocollapse_H5(fout, fin, b, subbandnr_h5, &subintdata, &firstseq, &lastseq, 1, sigma_limit, clipav, application.verbose, debugpacking, is_append, skipNrBlocks, truncNrBlocks, gains) == 0)
	    return 0;
	}else {
	  if(convert_nocollapse_CS(fout, fin, b, &subintdata, &firstseq, &lastseq, 1, sigma_limit, clipav, application.verbose, debugpacking, skipNrBlocks, truncNrBlocks, gains) == 0)
	    return 0;
	}
      }
      if(application.verbose) printf("  File contains sequence numbers %d - %d\n", firstseq, lastseq);

      /* Copy header parameters to output header */
      copy_paramsPSRData(subintdata, &fout);
      fout.NrPulses = lastseq+1;

      if(skipNrBlocks != 0) {
	if(is_CS == 0) {
	  fprintf(stderr, "2bf2fits: Skipping blocks is not implemented for this input data format.\n");
	  exit(-1);
	}
	fout.NrPulses -= skipNrBlocks;
	if(fout.NrPulses <= 0) {
	  fprintf(stderr, "2bf2fits: You appear to would like to skip more data than there is!\n");
	  exit(-1);
	}
      }
      if(truncNrBlocks != 0) {
	if(is_CS == 0) {
	  fprintf(stderr, "2bf2fits: Truncating output is not implemented for this input data format.\n");
	  exit(-1);
	}
	if(truncNrBlocks >= fout.NrPulses) {
	  fprintf(stderr, "2bf2fits: You appear to request to write out more data than there is!\n");
	  exit(-1);
	}	
	fout.NrPulses = truncNrBlocks;
	if(fout.NrPulses <= 0) {
	  fprintf(stderr, "2bf2fits: You appear to request to write out no data!\n");
	  exit(-1);
	}	
      }

      if(application.verbose) {
	printf("  Header dump:\n");
	printHeaderPSRData(fout, 4);
      }
      if(!openPSRData(&fout, buf, FITS_format, 1, 0, application.verbose))
	return 0;
      if(!writePSRFITSHeader(&fout, application.verbose))
	return 0;

      /* convert */
      if (is_CS == 0) {
	if(convert_nocollapse(fout, fin, b, &subintdata, &firstseq, &lastseq, 0, sigma_limit, clipav, application.verbose, debugpacking) == 0)
	  return 0;
      }
      if (is_CS == 1) {
	if(is_H5 == 0) {
	  if(convert_nocollapse_CS(fout, fin, b, &subintdata, &firstseq, &lastseq, 0, sigma_limit, clipav, application.verbose, debugpacking, skipNrBlocks, truncNrBlocks, gains) == 0)
	    return 0;
	}else {
	  if(convert_nocollapse_H5(fout, fin, b, subbandnr_h5, &subintdata, &firstseq, &lastseq, 0, sigma_limit, clipav, application.verbose, debugpacking, is_append, skipNrBlocks, truncNrBlocks, gains) == 0)
	    return 0;
	}
      }

      fclose(fin);
      closePSRData(&fout);
      }// Subband_h5 loop
    } //  Input filename loop
  }   // BEAMS loop
  } // if (is_append == 0)

  closePSRData(&subintdata);
  free (gains);
  tend = clock();


  if(application.verbose) {
    tend -= tstart;
    double tottime = tend/(double)CLOCKS_PER_SEC;
    printf("\n\n");
    printf("Total execution time: %10.2lf seconds\n", tottime);
    printf("Data reading:         %10.2lf seconds (%6.3lf%%)\n", time_read, 100.0*time_read/tottime);
    printf("Swapping floats:      %10.2lf seconds (%6.3lf%%)\n", time_swapfloats, 100.0*time_swapfloats/tottime);
    printf("Calculating rms:      %10.2lf seconds (%6.3lf%%)\n", time_calcrms, 100.0*time_calcrms/tottime);
    printf("Calculating scales:   %10.2lf seconds (%6.3lf%%)\n", time_calcscales, 100.0*time_calcscales/tottime);
    printf("Clipping and scaling: %10.2lf seconds (%6.3lf%%)\n", time_clipscale, 100.0*time_clipscale/tottime);
    printf("Packing:              %10.2lf seconds (%6.3lf%%)\n", time_packing, 100.0*time_packing/tottime);
    printf("Writing:              %10.2lf seconds (%6.3lf%%)\n", time_writing, 100.0*time_writing/tottime);
    double overhead = tottime - time_read - time_swapfloats - time_calcrms - time_calcscales - time_clipscale - time_packing - time_writing;
    printf("Overhead:             %10.2lf seconds (%6.3lf%%)\n", overhead, 100.0*overhead/tottime);
  }

  return 0;
}

