#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#define PULSARNAME			"B0809+74"
/*#define PULSARNAME			"B0921+62_BLANK"*/
/*#define PULSARNAME			"B1919+21"*/
/*#define PULSARNAME			"B0329+54"*/
/*#define PULSARNAME			"B0531+21"*/
/*#define PULSARNAME			"B1237+25"*/
/*#define PULSARNAME			"B1508+55"*/
/*#define PULSARNAME			"SGR0418+5729"*/
#define OBSDATE				"091029"
#define BASESUBBAND			153
#define INITIALSUBBANDS			1
#define BEAMS				5
#define CHANNELS			1
#define STOKES				1
#define SAMPLESPERSTOKESINTEGRATION	1

#define WRITEFUNC(b)			(b[0]) // StokesI
//#define WRITEFUNC(b)			(b[1]) // StokesQ
//#define WRITEFUNC(b)			(b[2]) // StokesU/2
//#define WRITEFUNC(b)			(b[3]) // StokesV/2
//#define WRITEFUNC(b)			(b[0]+b[1]) // X power * 2
//#define WRITEFUNC(b)			(b[0]-b[1]) // Y power * 2

#define AVERAGE_OVER			600 /* blocks of SAMPLES samples */
#define SCALE				250 /* divide samples by this */

#define ORIGCHANNELS			128
#define SAMPLES				768//((int)(round(CLOCK/FFTLENGTH/ORIGCHANNELS/16))*16/SAMPLESPERSTOKESINTEGRATION)
//#define SAMPLES				((int)(floor(1.0*CLOCK/FFTLENGTH/ORIGCHANNELS/16+0.5))*16/SAMPLESPERSTOKESINTEGRATION)

#define CLOCK				200*1000*1000
#define FFTLENGTH			1024
#define SAMPLEDURATION			(SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*CLOCK/FFTLENGTH/CHANNELS))

#undef WRITEFLOATS

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

/*
class Converter {
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
}
*/
void convert( FILE *input, FILE **output, int beamnr )
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

 stokesdata = malloc( sizeof *stokesdata * AVERAGE_OVER );

 fseek( input, 0, SEEK_SET );
 //fread( &stokesdata[0], sizeof stokesdata[0], 60, input ); // discard 60s

 while( !feof( input ) ) {
   unsigned num = 0;
   unsigned i,c;
   int orig_prev_seqnr;

   /* read data */
   num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
   printf("read %d blocks\n", num);
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

   for( c = 0; c < CHANNELS; c++ ) {
     float sum = 0.0f, average;
     unsigned validsamples = 0;

     fprintf( stderr, "Channel %d\n", c );

     /* compute average */
     for( i = 0; i < num; i++ ) {
       for( time = 0; time < SAMPLES; time++ ) {
         const float value = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );

         if( !isnan( value ) ) {
           sum += value;
           validsamples++;
         }
       }
     }
     average = sum / (validsamples?validsamples:1);
     fprintf( stderr, "Average: %.2f (over %d samples)\n", average, validsamples );


#ifndef WRITEFLOATS
     /* select a scale factor */
     int changescale;
     unsigned clipped;
     int changedir = 0;
     do {
       changescale = 0;
       clipped = 0;

       /* process data */
       for( i = 0; i < num; i++ ) {
         for( time = 0; time < SAMPLES; time++ ) {
           float sum;
           float avr;

           sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
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
#endif
       /* compute nr of out of bound samples */

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
     /* convert and write the data */
     prev_seqnr = orig_prev_seqnr;
     for( i = 0; i < num; i++ ) {
       int gap;

       for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
         /* gaps, fill in zeroes */
         for( time = 0; time < SAMPLES; time++ ) {
#ifdef WRITEFLOATS
           float sum = 0;
           fwrite( &sum, sizeof sum, 1, output[c] );
#else
           short shsum = 0;
           fwrite( &shsum, sizeof shsum, 1, output[c] );
#endif
         }
         output_samples += SAMPLES;
         nul_samples += SAMPLES;
       }

       prev_seqnr = stokesdata[i].sequence_number;
       /* process data */
       for( time = 0; time < SAMPLES; time++ ) {
         float sum;
         float avr;
         signed short shsum;

         sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
#ifdef WRITEFLOATS
         sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? average : sum;
         fwrite( &sum, sizeof sum, 1, output[c] );
#else
         sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? 0 : floor( (sum-average)/scale + 0.5 );

         /* convert to signed short */
         if( sum >= 128*256 ) {
           sum = 128*256 - 1;
           toobig++;
         }

         if( sum <  -128*256 ) {
           sum = -128*256;
           toosmall++;
         }

         shsum = sum;
         fwrite( &shsum, sizeof shsum, 1, output[c] );
         /*fwrite( &sum, sizeof sum, 1, output );*/
         output_samples++;
         if( sum == 0 ) nul_samples++;
#endif
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
 FILE *input, *output[CHANNELS];
 int f,b,c;
 char buf[1024];

 printf( "%d channels %d beams %d samples %d stokes\n", CHANNELS, BEAMS, SAMPLES, STOKES );

 /* command line verification */
 if( argc < 2 ) {
   puts("Syntax: convert SB*.MS\n");
   exit(1);
 }

 if( BEAMS > 1 ) {
   for( b = 0; b < BEAMS; b++ ) {
     sprintf( buf, "beam_%d", b );
     mkdir( buf, 0775 );
   }
 }

 for( b = 0; b < BEAMS; b++ ) {
   int index = 0;
   for( f = 1; f < argc; f++ ) {
     input = fopen( argv[f], "rb" );
     if( !input ) {
       perror( argv[f] );
       exit(1);
     }

     for( c = 0; c < CHANNELS; c++ ) {
       if( BEAMS > 1 ) {
         sprintf( buf, "beam_%d/%s_%s.sub%03d", b, PULSARNAME, OBSDATE, index || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
       } else {
         sprintf( buf, "%s_%s.sub%03d", PULSARNAME, OBSDATE, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
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
     convert( input, output, b );

     for( c = 0; c < CHANNELS; c++ ) {
       fclose( output[c] );
     }

     fclose( input );
   }
 }


 return 0;
}