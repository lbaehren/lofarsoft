#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/* NOTE: for old data sets (before PBW5) there is no 512 byte alignment,
 so "pad" needs to be ignored (line 82) and also the data does not need to 
be "floatswapped" (line 119). Same applies for convert_collapse() too.*/

char OUTNAME[248] = "PULSAR_OBS";
int BASESUBBAND = 0;
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
 double offset=0;
 double max = -1e9, min= 1e9;
 int x = 0;
 
 stokesdata = malloc( sizeof *stokesdata * AVERAGE_OVER );

 fseek( input, 0, SEEK_SET );

 while( !feof( input ) ) {
   if (x == NUM_BLOCKGROUPS) break;
   x++;
   unsigned num = 0;
   unsigned i,c;
   int orig_prev_seqnr;

   /* read data */
   num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
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

     if (writefloats == 0){
       /* select a scale factor */
       //scale = 128/(N_sigma*2*rms); //8-bit dynamic
       scale = (256*256)/(N_sigma*2*rms); //16-bit dynamic
       offset = average - (N_sigma*rms);
       //scale = 1.0/(65536); //16-bit static
       //scale = 1.0/(256*256*128); //8-bit static (sketchy)
       //if ( x == 1 && c == 0 ) {
       //offset = average;
       //fprintf(stderr,"Scale: %f Offset: %f\n",c, 1/scale,offset);
       //}
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
	   //int8_t shsum = 0; // 8-bit
	   short shsum = 0; // 16-bit
           fwrite( &shsum, sizeof shsum, 1, output[c] );
	   }
	 }
         output_samples += SAMPLES;
         nul_samples += SAMPLES;
       }
       stokesdata[i].sequence_number =  prev_seqnr + 1;       
       prev_seqnr = stokesdata[i].sequence_number;

       /* process data */
       float prev = 0;
       float x = 0;
       for( time = 0; time < SAMPLES; time++ ) {
         float sum;
         float avr;
         //int8_t shsum; //8-bit
	 signed short shsum; //16-bit
         sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );

	 if(writefloats==1){
	   sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? average : sum;
	   fwrite( &sum, sizeof sum, 1, output[c] );
	 }else{
	   	   
	   /*convert to 16-bit*/
	   sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? 0 : floor((sum-offset)*scale - 128*256);
	   if( sum >= 256*128 ) {
	     sum = 256*128 - 1;
	     toobig++;
	   }	  
	   if( sum < -256*128 ) {
	     sum = -256*128;
	     toosmall++;
	   }	   
	   /******************/
	   
	   /*8-bit
	   sum = isnan(sum) ? 0 : floor((sum-offset)*scale)-128;
	   if( sum >= 128 ) {
	     sum = 127;
	     toobig++;
	   }	  
	   if( sum < -128 ) {
	     sum = -128;
	     toosmall++;
	   }	
	   /*******/

	   shsum = sum;
	   //printf("%d\n",shsum );
	   fwrite( &shsum, sizeof shsum, 1, output[c] );
	   output_samples++;
	   if( sum == 0 ) nul_samples++;
	 }
       }
     }
   }
 }

 /*add zeros to make subbands an equal length*/
 if (NUM_BLOCKGROUPS > 0 && writefloats==0) {
   printf("adding zeros");
   unsigned c,t,a;
   t=NUM_BLOCKGROUPS*SAMPLES*AVERAGE_OVER - output_samples/CHANNELS;
   for (a=0; a < t; a++){
     for (c = 0; c < CHANNELS; c++){
       //int8_t shsum = 0; //8 bit
       signed short shsum = 0; //16 bit
       fwrite( &shsum, sizeof shsum, 1, output[c] );
       output_samples++;
     }
   }
 }
 /**********************************************/

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
  double offset=0;
  int x=0;

  stokesdata = malloc( sizeof *stokesdata * AVERAGE_OVER );
  
  fseek( input, 0, SEEK_SET );

  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS)break;
    x++;
    unsigned num = 0;
    unsigned i,c;
    int orig_prev_seqnr;

    /* read data */
    num=fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
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
	
      if (writefloats == 0){
	/* select a scale factor */
	scale = (256*256*CHANNELS/2) / (N_sigma*2*rms);
	offset = average - (N_sigma*rms*2/CHANNELS);
	//scale = 1.0/(65536);
	//if ( x == 1 ) {
	// offset = average;
	//fprintf(stderr,"Scale: %f Offset: %f\n", 1/scale,offset);
	//}
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
	  float prev = 0;
	  float x = 0;
	  
	  /* getting sum while summing all channels */
          for( c = 0; c < CHANNELS; c++ ) {
            float s = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
	    //PRINT CHANNEL0
	    //if(c==0) printf("%f/n",s);
	    //
	    if(writefloats==1){
	      s = isnan(s) || s <= 0.01f && s >= -0.01f ? average : s;
	    }else{
	      s = isnan(s) || s <= 0.01f && s >= -0.01f ? 0 : floor((s-offset)*scale);
	    }
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
  
  fprintf(stderr,"%u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small %d Blocks\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples, x);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free( stokesdata );
}

int main( int argc, char **argv ) {
 float avr;
 int f,b,c,y;
 char buf[1024];

 // for( y = 0; y < argc; y++ ){
 // printf("%s ",argv[y]);
 //}
 //printf("\n");
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
	  if (BASESUBBAND > 99999 || BASESUBBAND < 0){
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
