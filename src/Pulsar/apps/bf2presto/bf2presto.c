#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/* NOTE: for old data sets (before PBW5) there is no 512 byte alignment,
 so "pad" needs to be ignored (line 82) and also the data does not need to 
be "floatswapped" (line 119). Same applies for convert_collapse() too.*/

char OUTNAME[99] = "PULSAR_OBS";
int BASESUBBAND = 200;
int BEAMS = 1;
int CHANNELS = 1;
int STOKES = 1;	
int STOKES_SWITCH = 0;			
int collapse = 0;
int writefloats = 0;
int SAMPLESPERSTOKESINTEGRATION = 1;
int SAMPLES = 768;
int AVERAGE_OVER = 600;
int NUM_BLOCKGROUPS = -1;
float N_sigma = 7;
#define INITIALSUBBANDS			1
#define WRITEFUNC(b)			(b[STOKES_SWITCH]) // I=0,Q=1,U/2=2,V/2=3
//#define WRITEFUNC(b)			(b[0]+b[1]) // X power * 2
//#define WRITEFUNC(b)			(b[0]-b[1]) // Y power * 2

#define SCALE				150 /* divide samples by this */
#define CLOCK				200*1000*1000
#define FFTLENGTH			1024
#define SAMPLEDURATION			(SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*CLOCK/FFTLENGTH/CHANNELS))

void usage(){
  puts("Syntax: bf2presto[options] SB*.MS");
  puts("-A\tNumber of blocks to average over when rescaling (Default = 600)");
  puts("-b\tNumber of Beams (Default = 1)");
  puts("-B\tNumber of BlockGroups to convert (Default = All)");
  puts("-c\tNumber of Channels (Default = 1)");
  puts("-C\tCollapse all channels in MS to a single .sub file");
  puts("-f\tNumber of the Base Subband (Default = 200)");
  puts("-F\tWrite Floats");
  puts("-h\tShow this Help Screen");
  puts("-n\tNumber of Samples per Stokes Integration (Default = 1)");
  puts("-N\tNumber of Samples in block (Default = 768)");
  puts("-o\tOutput Name(Default = PULSAR_OBS)");
  puts("-s\tNumber of Stokes Parameters (Default = 1)");
  puts("-S\tStokes Parameter to write out (Default = StokesI):\n\t 0 = StokesI\n\t 1 = StokesQ\n\t 2 = StokesU\n\t 3 = StokesV");
  puts("-r\tData further than <r>*sigma from the mean will be clipped (Default = 7)");
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

 void convert_nocollapse( FILE *input, FILE **output, int beamnr)
{
  struct stokesdata {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][CHANNELS][SAMPLES|2][STOKES];
  };

 struct stokesdata *stokesdata;
 int prev_seqnr = -1;
 unsigned time;
 unsigned output_samples = 0;
 unsigned nul_samples = 0;
 unsigned toobig = 0, toosmall = 0;
 double scale = SCALE;
 double offset = 0;
 int x = 0;
 
 stokesdata = malloc( sizeof *stokesdata * AVERAGE_OVER );

 fseek( input, 0, SEEK_SET );
 //fread( &stokesdata[0], sizeof stokesdata[0], 60, input ); // discard 60s

 while( !feof( input ) ) {
   if (x == NUM_BLOCKGROUPS) break;
   x++;
   unsigned num = 0;
   unsigned i,c;
   int orig_prev_seqnr;

   /* read data */
   num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
   //fprintf(stderr,"read %d blocks\n", num);
   for( i = 0; i < num; i++ ) {
     for( c = 0; c < CHANNELS; c++ ) {
       unsigned b,t,s;

       b = beamnr;

       for( t = 0; t < SAMPLES; t++ ) {
         for( s = 0; s < STOKES; s++ ) {
	   floatSwap( &stokesdata[i].samples[b][c][t][s] );
         }
       }
     }
   }

   if( !num ) {
     break;
   }

   orig_prev_seqnr = prev_seqnr;

   for( i = 0; i < num; i++ ) {
     /* sequence number is big endian */
     swap_endian( (char*)&stokesdata[i].sequence_number );

     /* detect gaps */
     if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
       fprintf(stderr,"num %d gap between sequence numbers %u and %u.\n",i, prev_seqnr, stokesdata[i].sequence_number );
       
     }
     prev_seqnr = stokesdata[i].sequence_number;
   }
   
   for( c = 0; c < CHANNELS; c++ ) {
     float sum = 0.0f, average;
     float ms = 0.0f, rms;
     int N = 0;
     int z = 0;
     unsigned validsamples = 0;
     float prev = WRITEFUNC( stokesdata[0].samples[beamnr][c][0]);
     //fprintf( stderr, "Channel %d\n", c );

     /* compute average */
     for( i = 0; i < num; i++ ) {
       for( time = 0; time < SAMPLES; time++ ) {
         const float value = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
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
     average = sum/N;
     rms = sqrt((ms-(N*average*average))/(N-1));
     //fprintf( stderr, "Average: %.2f rms: %.2f (over %d samples)\n", average, rms, N );

     if (writefloats == 0){
     /* select a scale factor */
     scale = (256*256) / (N_sigma*2*rms);
     offset = average - (N_sigma*rms);
     //fprintf(stderr,"Scale: %f Offset: %f\n",1/scale,offset);
     }

     /* convert and write the data */
     prev_seqnr = orig_prev_seqnr;
     for( i = 0; i < num; i++ ) {
       int gap;

       for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
         /* gaps, fill in zeroes */
         for( time = 0; time < SAMPLES; time++ ) {
	   if (writefloats==1){
           float sum = 0;
           fwrite( &sum, sizeof sum, 1, output[c] );
	   }else{
           short shsum = 0;
           fwrite( &shsum, sizeof shsum, 1, output[c] );
	   }
         }
         output_samples += SAMPLES;
         nul_samples += SAMPLES;
       }

       prev_seqnr = stokesdata[i].sequence_number;
       /* process data */
       float prev = 0;
       float x = 0;
       for( time = 0; time < SAMPLES; time++ ) {
         float sum;
         float avr;
         signed short shsum;
         sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );

	 /*remove wiggles
	   if( fabs(sum - prev) > 1e10 ){
	     printf("%f\n", fabs(sum - prev));
	      x = prev - sum;
	   }
	   sum = sum + x; 
	   prev = sum;
	   /*remove wiggles*/

	 if(writefloats==1){
	   sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? average : sum;
	   fwrite( &sum, sizeof sum, 1, output[c] );
	 }else{
	   sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? 0 : floor((sum-offset)*scale)-128*256;	   
	   //convert to signed short
	   if( sum >= 128*256 ) {
	     sum = 128*256 - 1;
	     toobig++;
	   }	  
	   if( sum < -128*256 ) {
	     sum = -128*256;
	     toosmall++;
	   }	   
	   shsum = sum;
	   fwrite( &shsum, sizeof shsum, 1, output[c] );
	   //fwrite( &sum, sizeof sum, 1, output );
	   output_samples++;
	   if( sum == 0 ) nul_samples++;
	 }
       }
     }
   }
 }
 
 fprintf(stderr,"%u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples);
 fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
 
 free( stokesdata );
}


void convert_collapse( FILE *input, FILE **output, int beamnr )
{
  struct stokesdata {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][CHANNELS][SAMPLES|2][STOKES];
  };

  struct stokesdata *stokesdata;
  int prev_seqnr = -1;
  unsigned time;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale = SCALE;
  double offset = 0;
  int x=0;

  stokesdata = malloc( sizeof *stokesdata * AVERAGE_OVER );
  
  fseek( input, 0, SEEK_SET );
  //fread( &stokesdata[0], sizeof stokesdata[0], 60, input ); // discard 60s

  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS)break;
    x++;
    unsigned num = 0;
    unsigned i,c;
    int orig_prev_seqnr;

    /* read data */
    num=fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
    //fprintf(stderr,"read %d blocks\n", num);
    for( i = 0; i < num; i++ ) {
      for( c = 0; c < CHANNELS; c++ ) {
        unsigned b,t,s;

        b = beamnr;

        for( t = 0; t < SAMPLES; t++ ) {
          for( s = 0; s < STOKES; s++ ) {
            floatSwap( &stokesdata[i].samples[b][c][t][s] );
          }
        }
      }
    }
    
    if( !num ) {
      break;
    }

    orig_prev_seqnr = prev_seqnr;

    for( i = 0; i < num; i++ ) {
      /* sequence number is big endian */
      swap_endian( (char*)&stokesdata[i].sequence_number );

      /* detect gaps */ 
      if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
        fprintf(stderr,"gap between sequence numbers %u and %u.\n", prev_seqnr, stokesdata[i].sequence_number );
      }
      prev_seqnr = stokesdata[i].sequence_number;
    }
   

    c = 0; {    
      float sum = 0.0f, average;
      float ms = 0.0f, rms;
      unsigned validsamples = 0;
      int N = 0;
      float prev = WRITEFUNC( stokesdata[0].samples[beamnr][c][0]);
      /* compute average */
      for( i = 0; i < num; i++ ) {
	for( c = 0; c < CHANNELS; c++ ) {	  
	  for( time = 0; time < SAMPLES; time++ ) {
	    const float value = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );	    
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
      }

      N = validsamples?validsamples:1;
      average = sum / N;
      rms = sqrt((ms-(N*average*average))/(N-1));
      //fprintf( stderr, "Average: %.2f (over %d samples)\n", average, validsamples );   
      
	
      if (writefloats == 0){
	/* select a scale factor */
	scale = (256*256*CHANNELS/2) / (N_sigma*2*rms);
	offset = average - (N_sigma*rms*2/CHANNELS);
	//fprintf(stderr,"Scale: %f Offset: %f\n",1/scale,offset);
      }
      
      /* convert and write the data */
      prev_seqnr = orig_prev_seqnr;
      for( i = 0; i < num; i++ ) {
        int gap;
	
        for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
          /* gaps, fill in zeroes*/ 
          for( time = 0; time < SAMPLES; time++ ) {
	    if(writefloats==1){
	      float sum = 0;
	      fwrite( &sum, sizeof sum, 1, output[0] );
	    }else{
	      short shsum = 0;
	      fwrite( &shsum, sizeof shsum, 1, output[0] );
	    }
	  }
          output_samples += SAMPLES;
          nul_samples += SAMPLES;
	}
	
        prev_seqnr = stokesdata[i].sequence_number;
        /* process data */
        for( time = 0; time < SAMPLES; time++ ) {
          float sum = 0;
          float avr;
          signed short shsum;
	  //
	  float prev = 0;
	  float x = 0;
	  //
	  
	  /* getting sum while summing all channels */
          for( c = 0; c < CHANNELS; c++ ) {
            float s = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
	    //PRINT CHANNEL0
	    //if(c==0) printf("%f/n",s);
	    //
	    if(writefloats==1){
	      s = isnan(s) || s <= 0.01f && s >= -0.01f ? average : s;
	    }else{
	      s = isnan(s) || s <= 0.01f && s >= -0.01f ? 0 : floor((s-offset)*scale)-128*256;
	    }
	    /*remove wiggles
	    if( fabs(s - prev) > 1000000 ){
	      x = prev - s;
	    }
	    s = s + x;
	    prev = s;
	    /*remove wiggles*/
	    sum += s;
          }
	  sum = (sum/CHANNELS);

	  if(writefloats==1){
	    fwrite( &sum, sizeof sum, 1, output[0] );
	  }else{
	    /* convert to signed short */
	    if( sum >= 128*256 ) {
	      sum = 128*256 - 1;
	      toobig++;
	    }
	    /* to remove oscillations? 
	    if( sum >  32700) sum = 0;
	    if( sum < -32700) sum = 0;
	    /**************************/

	    if( sum < -128*256 ) {
	      sum = -128*256;
	      toosmall++;
	    }
	    
	    shsum = sum;
	    fwrite( &shsum, sizeof shsum, 1, output[0] );
	    output_samples++;
	    if( sum == 0 ) nul_samples++;
	  }
	}
      }
    }
  }
  
  fprintf(stderr,"%u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free( stokesdata );
}

int main( int argc, char **argv ) {
 float avr;
 int f,b,c,y;
 char buf[1024];

 for( y = 0; y < argc; y++ ){
   printf("%s ",argv[y]);
 }
 printf("\n");
 int i=0;
 while (( c = getopt(argc, argv, "r:b:B:n:N:A:c:s:p:o:f:S:hCF")) != -1)
    {
      i++;
      switch (c)
	{
	case 'b':
	  if (sscanf(optarg, "%d", &BEAMS) != 1) {
	    fprintf(stderr,"ERROR: Number of Beams = %s\n", optarg);
	    exit(-1);
	  }
	  break;
	  
	case 'B':
	  if (sscanf(optarg, "%d", &NUM_BLOCKGROUPS) != 1) {
	    fprintf(stderr,"ERROR: Number of BlockGroups to convert = %s\n", optarg);
	    exit(-1);
	  }
	  break;

	case 'C':
	  collapse = 1;
	  break;
	  
	case 'n':
	  if (sscanf(optarg, "%d", &SAMPLESPERSTOKESINTEGRATION) != 1) {
	    fprintf(stderr,"ERROR: Number of Samples per Stokes = %s\n", optarg);
	    exit(-1);
	  }
	  break;
	  
	case'N':
	   if (sscanf(optarg, "%d", &SAMPLES) != 1) {
	    fprintf(stderr,"ERROR: Number of Samples per Block = %s\n", optarg);
	    exit(-1);
	  }
	   break;

	case 'A':
	  if (sscanf(optarg, "%d", &AVERAGE_OVER) != 1) {
	    fprintf(stderr,"ERROR: Number of Samples to average over = %s\n", optarg);
	    exit(-1);
	  }
	  break;

	case 'F':
	  writefloats = 1;
	  break;

      	case 'c':
	  if (sscanf(optarg, "%d", &CHANNELS) != 1) {
	    fprintf(stderr,"ERROR: Number of Channels = %s\n", optarg);
	    exit(-1);
	  }
	  break;

	case 's':
	  if (sscanf(optarg, "%d", &STOKES) != 1) {
	    fprintf(stderr,"ERROR: Number of Stokes = %s\n", optarg);
	    exit(-1);
	  }
	  if (STOKES > 4 || STOKES < 1){
	    printf("ERROR: Number of Stokes = %d\n", STOKES);
	    exit(-1);
	  }
	  break;

	case 'o':
	  if (sscanf(optarg, "%99s", &OUTNAME) != 1) {
	    printf("ERROR: Output Name = %s\n", optarg);
	    exit(-1);
	  }
	  if (sscanf(OUTNAME,optarg)==-1){
	      printf("Output Name shortened to: %s\n",OUTNAME);
	    }
 	  break;

	case 'f':
	  if (sscanf(optarg, "%d", &BASESUBBAND) != 1) {
	    printf("ERROR: Basesubband = %d\n", optarg);
	    exit(-1);
	  }
	  if (BASESUBBAND > 99999 || BASESUBBAND < 1){
	    printf("ERROR: Lowest subband = %d\n", BASESUBBAND);
	    exit(-1);
	  }

 	  break;

	case 'S':
	  if (sscanf(optarg, "%d", &STOKES_SWITCH) != 1){
	    printf("ERROR: Please choose stokes parameter to output\n 0 = StokesI\n 1 = StokesQ\n 2 = StokesU\n 3 = StokesV\n ", optarg);
	    exit(-1);
	  }
	  if (STOKES_SWITCH>4||STOKES_SWITCH<0){
	      printf("ERROR: Please choose stokes parameter to output\n 0 = StokesI\n 1 = StokesQ\n 2 = StokesU\n 3 = StokesV\n");
	      exit(-1);
	    }
 	  break;
	  
	case 'r':
	   if (sscanf(optarg, "%f", &N_sigma) != 1){
	     printf("ERROR: Please number of sigma to map: %s\n", optarg);
	     exit(-1);
	   }
	   if (N_sigma < 0){
	     printf("Data above %d*sigma cannot be clipped... Please select a new value.\n", N_sigma);
	     exit(-1);
	   }
	   break;

	 case 'h':
	   usage();
	   exit(1);
	   break;
	}
    }
 FILE *input, *output[CHANNELS];
 c=0;

 /* command line verification */
 if( argc -optind < 1 ) {
   usage();
   exit(1);
 }

 printf( "%d channels %d beams %d samples %d stokes\n", CHANNELS, BEAMS, SAMPLES, STOKES );
 printf("Output Name: %s\n", OUTNAME);
 printf("Lowest Subband: %d\n",BASESUBBAND);
 printf("Stokes Parameter: %d\n",STOKES_SWITCH);
 
 if( BEAMS > 1 ) {
   for( b = 0; b < BEAMS; b++ ) {
     sprintf( buf, "beam_%d", b );
     mkdir( buf, 0775 );
   }
 }

 for( b = 0; b < BEAMS; b++ ) {
   int index = 0;
   for( f = optind; f < argc; f++ ) {
     fprintf(stderr,"Starting Work on %s\n", argv[f]);
     input = fopen( argv[f], "rb" );
     if( !input ) {
       perror( argv[f] );
       exit(1);
     }


     if(collapse ==1){
       c = 0; {
	 if( BEAMS > 1 ) {
	   sprintf( buf, "beam_%d/%s.sub%04d", b, OUTNAME, index || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
	 } else {
	   sprintf( buf, "%s.sub%04d", OUTNAME, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
	 }
	 fprintf(stderr,"%s -> %s\n", argv[f], buf);
	 
	 index++;
	 output[c] = fopen( buf, "wb" );
	 if( !output[c] ) {
	   perror( buf );
	   exit(1);
	 }
       }
     
       /* convert */
       convert_collapse( input, output, b );
       
       fclose( output[0] );
       
       fclose( input );
     }
   
     if (collapse==0){
     for( c = 0; c < CHANNELS; c++ ) {
       if( BEAMS > 1 ) {
         sprintf( buf, "beam_%d/%s.sub%04d", b, OUTNAME, index || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
       } else {
         sprintf( buf, "%s.sub%04d", OUTNAME, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
       }
       fprintf(stderr,"%s -> %s\n", argv[f], buf);

       index++;
       output[c] = fopen( buf, "wb" );
       if( !output[c] ) {
         perror( buf );
         exit(1);
       }
     }

     /* convert */ 
     convert_nocollapse( input, output, b);

     for( c = 0; c < CHANNELS; c++ ) {
       fclose( output[c] );
     }

     fclose( input );
     }
   }
 }


 return 0;
}


// BITS WHICH I HAVE REMOVED:

     /* subtract running average 
     for( i = 0; i < num; i++ ) {
       for( c = 0; c < CHANNELS; c++ ) {
	 for( time = 0; time < SAMPLES; time++ ) {
	   float sum;
	   float avr;
	   signed short shsum;
	   int ii=0;
	   int num_mean=0;
	   int MAX = SAMPLES;
	   float sum2=0;
	   if(i==0){
	     for (ii=0;ii<SAMPLES; ii=ii+24){
	       if ((WRITEFUNC( stokesdata[i].samples[beamnr][c][time+ii]) <3*rms+average)&&(WRITEFUNC( stokesdata[i].samples[beamnr][c][time+ii]) > (average - 3*rms))){
		 sum2 +=  WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
		 num_mean++;
		   }
	     }
	   }else{
	     for( ii = 0; ii < MAX; ii=ii+24){ 
	       if((time + ii) < MAX/2){
		 if ((WRITEFUNC( stokesdata[i-1].samples[beamnr][c][SAMPLES+time+ii-MAX/2] ) < 3*rms+average)&&(WRITEFUNC( stokesdata[i-1].samples[beamnr][c][SAMPLES+time+ii-MAX/2] ) > (average - 3*rms))){
		   sum2 += WRITEFUNC( stokesdata[i-1].samples[beamnr][c][SAMPLES+time+ii-MAX/2]);
		   num_mean++;
		 }
	       } else if((time + ii) > (MAX/2 + SAMPLES)){
		 if (( WRITEFUNC( stokesdata[i+1].samples[beamnr][c][time+ii-MAX/2-SAMPLES]) < 3*rms+average)&& ( WRITEFUNC( stokesdata[i+1].samples[beamnr][c][time+ii-MAX/2-SAMPLES]) > (average-3*rms))){
		   sum2 += WRITEFUNC( stokesdata[i+1].samples[beamnr][c][time+ii-MAX/2-SAMPLES]);
		   num_mean++;
		 }
	       } else{
		 if ((WRITEFUNC( stokesdata[i].samples[beamnr][c][time+ii-(MAX/2)]) <3*rms+average)&&(WRITEFUNC( stokesdata[i].samples[beamnr][c][time+ii-(MAX/2)]) > (average - 3*rms))){
		   sum2 += WRITEFUNC( stokesdata[i].samples[beamnr][c][time+ii-(MAX/2)]);
		   num_mean++; 
		 }
	       }
	     }
	   }
	   sum2 = sum2/num_mean;
	   sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
	   printf("%d ",stokesdata[i].samples[beamnr][c][time]);
	   if(writefloats==1){
	     WRITEFUNC(stokesdata[i].samples[beamnr][c][time]) = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? average : sum;
	   }else{
	     WRITEFUNC(stokesdata[i].samples[beamnr][c][time]) = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? 0 : sum-sum2;	
	   }   
	   printf("%d\n",stokesdata[i].samples[beamnr][c][time]);
	 }
       }
     }
     /*************************/


/*class Converter {
 public:
   Converter( const unsigned channels, const unsigned beams, const unsigned samples, const unsigned stokes );
   void convert( FILE *input, FILE **output, int beamnr );
 private:
   const unsigned itsChannels;
   const unsigned itsBeams;
   const unsigned itsSamples;
   const unsigned itsStokes;
};

Converter::Converter( const unsigned channels, const unsigned beams, const unsigned samples, const unsigned stokes ):
 itsChannels( channels ),
 itsBeams( beams ),
 itsSamples( samples ),
 itsStokes( stokes )
{
}*/

   /* check all beams */
/*
   unsigned different = 0;
   for( i = 0; i < num; i++ ) {
     for( time = 0; time < SAMPLES; time++ ) {
       float channelsum = 0.0f;

       for( channel = 0; channel < CHANNELS; channel++ ) {
         float first = stokesdata[i].samples[channel][0][time][0];
         unsigned j;

         for( j = 1; j < BEAMS; j++ ) {
           if( stokesdata[i].samples[channel][j][time][0] != first ) {
             different++;
           }
         }
       }
     }
   }
   fprintf( stderr, "Differences: %u (over %d samples)\n", different, SAMPLES*num*CHANNELS*(BEAMS-1) );
*/


     /* JAN DAVID'S SCALING CODE:
     do {
       changescale = 0;
       clipped = 0;

       // process data 
       float prev = 0;
       float x = 0;
       for( i = 0; i < num; i++ ) {
         for( time = 0; time < SAMPLES; time++ ) {
           float sum;
           float avr;

	   sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
	   /*remove wiggles
	   if( fabs(sum - prev) > 1e10){
	     x = prev-sum;
	   }
	   sum = sum + x;
	   prev = sum;
	   /*remove wiggles
	   
	   sum = isnan(sum) || sum <= 0.01f ? 0 : floor( (sum-average)/scale + 0.5 );
           if( sum >= 128*256 || sum < -128*256 ) {
             clipped++;
           }
         }
       }

       if( 100.0*clipped / validsamples > 5.0 && changedir >= 0 ) {
         scale *= 1.5;
         changescale = 1;
         changedir = +1;
         fprintf( stderr, "Increased scaling factor to %.3f\n", scale );
       } else if( average > 0.001 && 100.0*clipped / validsamples < 0.1 && changedir <= 0 ) {
         scale /= 1.5;
	 changescale = 1;
         changedir = -1;
         fprintf( stderr, "Decreased scaling factor to %.3f\n", scale );
       } 
     } while( changescale );
     }*/
