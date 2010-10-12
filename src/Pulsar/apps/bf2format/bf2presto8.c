#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "filterbank.h"
#include "makeinf.h"

/* NOTE: for old data sets (before PBW5) there is no 512 byte alignment,
 so "pad" needs to be ignored (line 82) and also the data does not need to 
be "floatswapped" (line 119). Same applies for convert_collapse() too.*/

char OUTNAME[248] = "PULSAR_OBS";
int BASESUBBAND = 0;
int BEAMS = 1;
int CHANNELS = 1;
int STOKES = 1;	
int STOKES_SWITCH = 0;
int TRANSPOSE = 0;
int SUBBANDS=1;
int collapse = 0;
int writefloats = 0;
int writefb     = 0;
int eightBit = 0;
char parsetfile[1000]; // for reading header info for filterbank file
char tmpstr[16];
int SAMPLESPERSTOKESINTEGRATION = 1;
int SAMPLES = 768;
int AVERAGE_OVER = 600;
int NUM_BLOCKGROUPS = -1;
float N_sigma = 7;
int old_bnr = 0;

#define INITIALSUBBANDS			1
#define WRITEFUNC(b)			(b[STOKES_SWITCH]) // I=0,Q=1,U/2=2,V/2=3
//#define WRITEFUNC(b)			(b[0]+b[1]) // X power * 2
//#define WRITEFUNC(b)			(b[0]-b[1]) // Y power * 2

#define SCALE				150 /* divide samples by this */
#define CLOCK				200*1000*1000
#define FFTLENGTH			1024
#define SAMPLEDURATION			(SAMPLESPERSTOKESINTEGRATION*1.0/(1.0*CLOCK/FFTLENGTH/CHANNELS))
#define MAXNOINPUTFILES                 256

void usage(){
  puts("Syntax: bf2presto8 [options] SB*.MS");
  puts("-8\tOutput 8bit subbands");
  puts("-A\tNumber of blocks to average over when rescaling (Default = 600)");
  puts("-b\tNumber of Beams (Default = 1)");
  puts("-B\tNumber of BlockGroups to convert (Default = All)");
  puts("-c\tNumber of Channels (Default = 1)");
  puts("-C\tCollapse all channels in MS to a single .sub file");
  puts("-f\tNumber of the Base Subband (Default = 200)");
  puts("-F\tWrite Floats");
  puts("-h\tShow this Help Screen");
  puts("-L <parset>\tuse fiLterbank format (8/32 bit, 1 beam, stokes I, no collapse), with header info from <parset>");
  puts("-n\tNumber of Samples per Stokes Integration (Default = 1)");
  puts("-N\tNumber of Samples in block (Default = 768)");
  puts("-o\tOutput Name(Default = PULSAR_OBS)");
  puts("-s\tNumber of Stokes Parameters (Default = 1)");
  puts("-S\tStokes Parameter to write out (Default = StokesI):\n\t 0 = StokesI\n\t 1 = StokesQ\n\t 2 = StokesU\n\t 3 = StokesV");
  puts("-r\tData further than <r>*sigma from the mean will be clipped (Default = 7)");
  puts("-t\t INCOHERENT data in Second Transpose mode");
  puts("-T <number of subbands>\t COHERENT data in Second Transpose mode");
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

void parset_reader(){}

void write_filterbank_header ( int n_infiles, char *parsetfile, int writefloats) {
  /* read values from parset */
  FILE *fp;
  char cmd[1000], tempinffile[1000], script[1000];
  infodata idata;

  int obits=8;
  if (writefloats==1) obits=32;

  /* use parset to inf file convertor */
  sprintf(tempinffile, "/tmp/bf2presto"); // no suffix here
  /* Vlad, Aug 5*/
  /* sprintf(script,      "$LOFARSOFT/src/Pulsar/apps/bf2presto/par2inf.py"); */
  sprintf(script,      "${LOFARSOFT}/release/share/pulsar/bin/par2inf.py");

  printf("Reading parset file %s using %s \n", parsetfile, script);
  sprintf(cmd, "%s -o %s.inf %s; ", script, tempinffile, parsetfile);  
  system(cmd);
  readinf(&idata, tempinffile);
  strcat(tempinffile, ".inf");
  if( remove(tempinffile)!=0 ) printf("Could not remove %s", tempinffile);  

  /* broadcast header */
  send_string("HEADER_START");
  send_string("source_name");  send_string(idata.object);
  send_int("machine_id", 9);
  send_int("telescope_id", 9);
  send_int("data_type",1);
  send_double("fch1", idata.freq  + 0.1953125*n_infiles); //139.56484375 + 0.1953125*n_infiles);
  send_double("foff", -0.1953125/CHANNELS);
  send_int("nchans",  CHANNELS*n_infiles);
  send_int("nbeams",  1);
  send_int("ibeam",   1);
  send_int("nbits",   obits);
  send_double("tstart", idata.mjd_i+idata.mjd_f);
  send_double("tsamp",  idata.dt);
  send_int("nifs", 1);
  send_string("HEADER_END");

 /*
   convert raw data from a pulsar machine into "filterbank data"
   a stream of samples: [s1,c1] [s1,c2]...[s1,cn] [s2,c1]...[s2,cn]..... etc
   where the indices s1, s2.... are individual sample readouts and c1, c2....
   are the individual channels. For each readout, there are "n" channels.
   Before writing out a stream of samples, filterbank sends a header string
   which is intercepted by other programs (dedisperse, fold etc) downstream.
 */
}


void convert_filterBank( FILE **inputfiles, FILE **outputfile, int beamnr, int writefb, int n_infiles )
{
  
  struct stokesdata_struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][CHANNELS][SAMPLES|2][STOKES];
  };
  /* Filterbank data is in a different order. Allocated below  */
  /* [BEAMS][STOKES][SAMPLES*AVERAGE_OVER][CHANNELS*n_infiles] */
  unsigned char *filterbank_char_buffer;
  float         *filterbank_float_buffer;
  
  
  struct stokesdata_struct *stokesdata;
  int prev_seqnr = -1;
  static int current_file=0;
  unsigned time;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale;
  double offset;
  int x = 0;
  int filterbank_buffer_size;
  stokesdata = (struct stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(struct stokesdata_struct) );
  filterbank_buffer_size =  BEAMS * STOKES * (SAMPLES*AVERAGE_OVER) * (CHANNELS*n_infiles);
  if (writefloats == 1) {
    filterbank_float_buffer = (float *) malloc( filterbank_buffer_size * sizeof(float) );
  } else {
    filterbank_char_buffer = (unsigned char *) malloc( filterbank_buffer_size * sizeof(unsigned char) );
    printf("Allocating write buffer of %d MByte\n", filterbank_buffer_size/(1024*1024));
  }
  /* send the filterbank file header if needed */
  output = outputfile[0];
  write_filterbank_header(n_infiles, parsetfile, writefloats);
  if (old_bnr != beamnr) current_file = 0; //fix for FE mode
  old_bnr = beamnr;
  input = inputfiles[current_file]; 
  current_file++;
  fseek( input, 0, SEEK_SET ); 
  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS) break;
    x++;
    unsigned num = 0;
    unsigned i,c,f,n_simult_files;
    int orig_prev_seqnr;
    int written_samples=0;
    
    /* read data */
    n_simult_files=n_infiles;  /* need data from all input files */
    /* zero out array to be safe */
    if (writefloats == 0){
      memset(filterbank_char_buffer, 0, 
	     filterbank_buffer_size * sizeof(filterbank_char_buffer[0]));
    } else {
      memset(filterbank_float_buffer, 0, 
	     filterbank_buffer_size * sizeof(filterbank_float_buffer[0]));
    }
    
    for (f = 0; f < n_simult_files ; f++) {
      /* read data */
      num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, inputfiles[f] );
      
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
	    if( !isnan( value ) ) {
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
	  scale = 128.0/(N_sigma*2*rms); //8-bit dynamic
	  offset = average - (N_sigma*rms);
	}
	
	/* convert and write the data */
	prev_seqnr = orig_prev_seqnr;
	for( i = 0; i < num; i++ ) {
	  int gap;
	  
	  for( gap = prev_seqnr + 1; gap < stokesdata[i].sequence_number; gap++ ) {
	    /* gaps, fill in zeroes */
	    for( time = 0; time < SAMPLES; time++ ) {
	      /* filterbank file need in memory transpose, difficult for gaps */
	      printf("A gap here -- not doing anything about it in filterbank mode ..\n");
	      /* gap zeroes get written to the single filterbank file */
	      /* this is wrong -- need to go into buffer, keep track of variable buffer lenghth .. */
	      
	    }
	    output_samples += SAMPLES;
	    nul_samples += SAMPLES;
	  }
	  stokesdata[i].sequence_number =  prev_seqnr + 1;       
	  prev_seqnr = stokesdata[i].sequence_number;
	  
	  /* process data */
	  float prev = 0;
	  float x = 0;
	  int fb_pos;
	  for( time = 0; time < SAMPLES; time++ ) {
	    float sum;
	    float avr;
	    unsigned char isum; //8-bit
	    sum = WRITEFUNC( stokesdata[i].samples[beamnr][c][time] );
	    
	    if(writefloats==1){
	      sum = isnan(sum) || sum <= 0.01f && sum >= -0.01f ? average : sum;
	      fb_pos = // Assumed beamnr = 0 && stokes = 0 here! XXX
		(i*SAMPLES+time+1) * (CHANNELS*n_infiles) - (f*CHANNELS+c+1);
	      if (fb_pos > filterbank_buffer_size) { printf("Oops, fb_pos overflowing\n"); }
	      /* sample written to reordered filterbank buffer */
	      filterbank_float_buffer[fb_pos] = (float) sum;
	      written_samples++;
	      output_samples++;
	      if( sum == 0 ) nul_samples++;	      
	    } else {
	      /*8-bit*/
	      sum = isnan(sum) ? 0 : floor((sum-offset)*scale); 
	      if( sum >= 256 ) {
		sum = 255;
		toobig++;
	      }	  
	      if( sum < 0    ) {
		sum = 0 ;
		toosmall++;
	      }	
	      isum = sum;
	      fb_pos = // Assumed beamnr = 0 && stokes = 0 here! XXX
		(i*SAMPLES+time+1) * (CHANNELS*n_infiles) - (f*CHANNELS+c+1);
	      if (fb_pos > filterbank_buffer_size) { printf("Oops, fb_pos overflowing\n"); }
	      /* sample written to reordered filterbank buffer */
	      filterbank_char_buffer[fb_pos] = (unsigned char) isum;
	      written_samples++;
	      output_samples++;
	      if( sum == 0 ) nul_samples++;
	    } // if (writefloats) {} else 
	  } // for( time = 0; time < SAMPLES; time++ ) 
	} // for( i = 0; i < num; i++ ) {
      } // for( c = 0; c < CHANNELS; c++ ) {
    } // for (f = 0; f < n_simult_files ; f++) { 
    
    /* now write the aligned buffer to filterbank file */
    if (writefloats==1) { /* float value to buffer */
      fwrite( filterbank_float_buffer, 
	      filterbank_buffer_size/AVERAGE_OVER*sizeof(float), 
	      num, outputfile[0] );
    } else { /* 8-bit value */
      fwrite( filterbank_char_buffer, 
	      filterbank_buffer_size/AVERAGE_OVER*sizeof(unsigned char), 
	      num, outputfile[0] );
    }
    printf("Printing %d values\n", num);
    printf("Wrote %d samples in buffer of size %d\n", written_samples, filterbank_buffer_size );
  
} //  while( !feof( input ) ) {  
fprintf(stderr,"%u samples in output, %.2f%% null values, %.2f%% too big, %.2f%% too small\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples);
fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);


/* free input and output data */
free(stokesdata);
// if( filterbank_float_buffer != NULL ) 
//   free( (float *)filterbank_float_buffer )
// if( filterbank_char_buffer != NULL ) 
//   free( (unsigned char *)filterbank_char_buffer )

//if (writefloats == 1) {
//   free(filterbank_float_buffer);
// } else {
//   free(filterbank_char_buffer); 
// }

} // void convert_filterBank()


void secondTranspose( FILE **inputfiles, int beamnr)
{
  struct stokesdata_struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][SAMPLES|2][SUBBANDS][CHANNELS][STOKES];
  };
  
  struct stokesdata_struct *stokesdata;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale;
  double offset;
  int x = 0;
  
  stokesdata = (struct stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(struct stokesdata_struct) );
  
  input = inputfiles[0]; 
  fseek( input, 0, SEEK_SET ); 
  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS) break; // switch so you can choose only to read in x blocks
    unsigned num = 0;
    unsigned i,f,n_simult_files, sub;
    char buf[1024];
    int index=0;

    /* read data */
    num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
    
    for( i = 0; i < num; i++ ) { 
      //swap endianness (sequence number is big endian)
      swap_endian( (char*)&stokesdata[i].sequence_number );
    }

    for( sub = 0; sub < SUBBANDS; sub ++ ){
      unsigned t, chan, s = STOKES_SWITCH;
      for( chan = 0; chan < CHANNELS; chan++ ){
	FILE *outputfile[CHANNELS]; 
	sprintf( buf, "%s.sub%04d", OUTNAME, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
	if ( BEAMS > 1 ) {
	  sprintf(buf, "beam_%d/%s", beamnr, buf ); /* prepend beam name */
	} 
	fprintf(stderr,"SUBBAND: %d CHANNEL %d BLOCK: %d -> %s\n", sub, chan, x, buf);
	index++;
	if( x == 0 ){
	  outputfile[chan] = fopen( buf, "wb" );
	} else {
	  outputfile[chan] = fopen( buf, "ab" );
	}
	double average = 0;
	double rms = 0;
	int validsamples = 0;
	int N;
	
	for( i = 0; i < num; i++ ) { 
	  for( t = 0; t < SAMPLES; t++ ){ 
	    //first pass to compute statistics
	    floatSwap( &stokesdata[i].samples[beamnr][t][sub][chan][s] );
	    if( !isnan(stokesdata[i].samples[beamnr][t][sub][chan][s]) ){
	      average += stokesdata[i].samples[beamnr][t][sub][chan][s]; 
	      rms += stokesdata[i].samples[beamnr][t][sub][chan][s]*stokesdata[i].samples[beamnr][t][sub][chan][s];
	      validsamples++;
	    }
	  }	 
	}
	//calculate scale and offset for the block
	N = validsamples?validsamples:1;
	average = average/N;
	rms = sqrt((rms - (N*average*average))/(N-1));
	if( eightBit==1 ){ //8-bit mode
	  scale = 256.0/(N_sigma*2*rms);
	  offset = average;
	} else if( writefloats==1 ){ //float mode (no scale or offset)
	  scale = 1;
	  offset = 0;
	} else { //16-bit scaling
	  scale = 65536.0/(N_sigma*2*rms);
	  offset = average;
	}
	int prev_seqnr = -1 + x*AVERAGE_OVER;
	
	for( i = 0; i < num; i++ ) {    
	  float value;
	  signed short shvalue;
	  signed char ivalue;
	  
	  /* detect gaps and write zeros here*/
	  if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
	    fprintf(stderr,"gap between sequence numbers %d and %d.\n", prev_seqnr, stokesdata[i].sequence_number );
	    
	    //corrupted sequence number, write block of zeros and move to next number
	    if( stokesdata[i].sequence_number - prev_seqnr > 100){
	      fprintf(stderr,"skipping corrupted sequence number: %d.\n", stokesdata[i].sequence_number );
	      for( t=0; t < SAMPLES; t++){
		shvalue =0;
		fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
		output_samples++;
		nul_samples++;
	      }
	      prev_seqnr ++;
	      
	      //dropped packet(s) write zeros until gap is padded
	    } else {
	      int gap, ii;
	      gap = stokesdata[i].sequence_number - (prev_seqnr+1);
	      fprintf(stderr, "gap:%d\n", gap);
	      for( ii=0; ii< gap; ii++){
		for( t=0; t < SAMPLES; t++){
		  shvalue =0;
		  fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
		  output_samples++;
		  nul_samples++;
		}
	      }
	      prev_seqnr = stokesdata[i].sequence_number;  
	    }
	  } else {
	    
	    //write data
	    for( t = 0; t < SAMPLES; t++ ){
	      value = floor((stokesdata[i].samples[beamnr][t][sub][chan][s]-offset)*scale);
	      if( isnan(value)) {
		value = 0;
		nul_samples++;
	      }
	      
	      if( eightBit==1 ){ //8-bit mode
		if( value >= 128 ) {
		  value = 127;
		  toobig++;
		}
		if( value < -128 ) {
		  value =  -128;
		  toosmall++;
		}
		ivalue = value;
		fwrite(&ivalue, sizeof ivalue, 1, outputfile[chan]);
		
	      } else if( writefloats==1 ){ //float mode
		fwrite(&value, sizeof value, 1, outputfile[chan]);
		
	      } else { // 16-bit mode
		if( value >= 32768 ) {
		  value = 32767;
		  toobig++;
		}
		if( value < -32768 ) {
		  value =  -32768;
		  toosmall++;
		}
		shvalue = value;
		fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	      }
	      output_samples++;
	    }//for( t = 0; t < SAMPLES; t++ )
	    prev_seqnr = stokesdata[i].sequence_number;
	  }//if( prev_seqnr + 1 != stokesdata[i].sequence_number ) else{
	}//for( i = 0; i < num; i++ )
	fclose( outputfile[chan] );
      }//for( chan = 0; chan < 1; chan++ )
    }//for( sub = 0; sub < SUBBANDS; subb++ );
    x++;
  }//while( !feof( input ) )
  fprintf(stderr,"%u samples in output, %.2f%% bad samples, %.2f%% too big, %.2f%% too small %d Blocks\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples, x);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free(stokesdata);
}//end of secondTranspose()

void secondTranspose_inc( FILE **inputfiles, FILE **outputfile, int beamnr, int writefb, int n_infiles)
{
  struct stokesdata_struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][STOKES][SAMPLES|2][CHANNELS];
  };
  
  struct stokesdata_struct *stokesdata;
  static int current_file=0;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale;
  double offset;
  int x = 0;
  
  stokesdata = (struct stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(struct stokesdata_struct) );
  
  input = inputfiles[current_file];
  current_file++;
  fseek( input, 0, SEEK_SET ); 
  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS) break; // switch so you can choose only to read in x blocks
    x++;
    unsigned num = 0;
    unsigned i,f,n_simult_files;
    
    /* read data */
    num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
    unsigned t, chan, s = STOKES_SWITCH;
    for( chan = 0; chan < CHANNELS; chan++ ){
      double average = 0;
      double rms = 0;
      int validsamples = 0;
      int N;
      
      for( i = 0; i < num; i++ ) { 
	//swap endianness (sequence number is big endian)
	if( chan == 0){
	  swap_endian( (char*)&stokesdata[i].sequence_number );
	}
	
	for( t = 0; t < SAMPLES; t++ ){ 
	  //first pass to compute statistics
	  floatSwap( &stokesdata[i].samples[beamnr][s][t][chan]);
	  if( !isnan(stokesdata[i].samples[beamnr][s][t][chan]) ){
	   average += stokesdata[i].samples[beamnr][s][t][chan]; 
	   rms += stokesdata[i].samples[beamnr][s][t][chan]*stokesdata[i].samples[beamnr][s][t][chan];
	   validsamples++;
	  }
	}	 
      }

      //calculate scale and offset for the block
      N = validsamples?validsamples:1;
      average = average/N;
      rms = sqrt((rms - (N*average*average))/(N-1));
      if( eightBit==1 ){ //8-bit mode
	scale = 256.0/(N_sigma*2*rms);
	offset = average;
      } else if( writefloats==1 ){ //float mode (no scale or offset)
	scale = 1;
	offset = 0;
      } else { //16-bit scaling
	scale = 65536.0/(N_sigma*2*rms);
	offset = average;
      }
      int prev_seqnr = -1 + (x-1)*AVERAGE_OVER;
      
      for( i = 0; i < num; i++ ) {    
	float value;
	signed short shvalue;
	signed char ivalue;
	
	/* detect gaps and write zeros here*/
	if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
	  fprintf(stderr,"gap between sequence numbers %d and %d.\n", prev_seqnr, stokesdata[i].sequence_number );
	  
	  //corrupted sequence number, write block of zeros and move to next number
	  if( stokesdata[i].sequence_number - prev_seqnr > 100){
	    fprintf(stderr,"skipping corrupted sequence number: %d.\n", stokesdata[i].sequence_number );
	    for( t=0; t < SAMPLES; t++){
	      shvalue =0;
	      fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	      output_samples++;
	      nul_samples++;
	    }
	    prev_seqnr ++;
	    
	    //dropped packet(s) write zeros until gap is padded
	  } else {
	    int gap, ii;
	    gap = stokesdata[i].sequence_number - (prev_seqnr+1);
	    fprintf(stderr, "gap:%d\n", gap);
	    for( ii=0; ii< gap; ii++){
	      for( t=0; t < SAMPLES; t++){
		shvalue =0;
		fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
		output_samples++;
		nul_samples++;
	      }
	    }
	    prev_seqnr = stokesdata[i].sequence_number;  
	  }
	} else {
	  
	  //write data
	  for( t = 0; t < SAMPLES; t++ ){
	    value = floor((stokesdata[i].samples[beamnr][s][t][chan]-offset)*scale);
	    if( isnan(value)) {
	      value = 0;
	      nul_samples++;
	    }
	    
	    if( eightBit==1 ){ //8-bit mode
	      if( value >= 128 ) {
		value = 127;
		toobig++;
	      }
	      if( value < -128 ) {
		value =  -128;
		toosmall++;
	      }
	      ivalue = value;
	      fwrite(&ivalue, sizeof ivalue, 1, outputfile[chan]);
	      
	    } else if( writefloats==1 ){ //float mode
	      fwrite(&value, sizeof value, 1, outputfile[chan]);

	    } else { // 16-bit mode
	      if( value >= 32768 ) {
		value = 32767;
		toobig++;
	      }
	      if( value < -32768 ) {
		value =  -32768;
		toosmall++;
	      }
	      shvalue = value;
	      fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	    }
	    output_samples++;
	  }//for( t = 0; t < SAMPLES; t++ )
	  prev_seqnr = stokesdata[i].sequence_number;
	}//if( prev_seqnr + 1 != stokesdata[i].sequence_number ) else{
      }//for( i = 0; i < num; i++ )
    }//for( chan = 0; chan < 1; chan++ )
  }//while( !feof( input ) )
  fprintf(stderr,"%u samples in output, %.2f%% bad samples, %.2f%% too big, %.2f%% too small %d Blocks\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples, x);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free(stokesdata);
}//end of secondTranspose_inc()


void convert_nocollapse( FILE **inputfiles, FILE **outputfile, int beamnr, int writefb, int n_infiles )
{
  struct stokesdata_struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][CHANNELS][SAMPLES|2][STOKES];
  };
  
  struct stokesdata_struct *stokesdata;
  static int current_file=0;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale;
  double offset;
  int x = 0;
  
  stokesdata = (struct stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(struct stokesdata_struct) );
  input = inputfiles[current_file];
  current_file++;
  fseek( input, 0, SEEK_SET ); 
  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS) break;
    x++;
    unsigned num = 0;
    unsigned i,c;

    /* read data */
    num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
        
    unsigned t, chan, s = STOKES_SWITCH;
    for( chan = 0; chan < CHANNELS; chan++ ){
      double average = 0;
      double rms = 0;
      int validsamples = 0;
      int N;
      
      for( i = 0; i < num; i++ ) { 
	//swap endianness (sequence number is big endian)
	if( chan == 0){
	  swap_endian( (char*)&stokesdata[i].sequence_number );
	}
	
	for( t = 0; t < SAMPLES; t++ ){ 
	  //first pass to compute statistics
	  floatSwap( &stokesdata[i].samples[beamnr][chan][t][s] );
	  if( !isnan(stokesdata[i].samples[beamnr][chan][t][s]) ){
	    average += stokesdata[i].samples[beamnr][chan][t][s]; 
	    rms += stokesdata[i].samples[beamnr][chan][t][s]*stokesdata[i].samples[beamnr][chan][t][s];
	    validsamples++;
	  }
	}	 
      }

      //calculate scale and offset for the block
      N = validsamples?validsamples:1;
      average = average/N;
      rms = sqrt((rms - (N*average*average))/(N-1));
      if( eightBit==1 ){ //8-bit mode
	scale = 256.0/(N_sigma*2*rms);
	offset = average;
      } else if( writefloats==1 ){ //float mode (no scale or offset)
	scale = 1;
	offset = 0;
      } else { //16-bit scaling
	scale = 65536.0/(N_sigma*2*rms);
	offset = average;
      }
   
      int prev_seqnr = -1 + (x-1)*AVERAGE_OVER;
      
      for( i = 0; i < num; i++ ) {    
	float value;
	signed short shvalue;
	signed char ivalue;
	
	/* detect gaps and write zeros here*/
	if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
	  fprintf(stderr,"gap between sequence numbers %d and %d.\n", prev_seqnr, stokesdata[i].sequence_number );
	  
	  //corrupted sequence number, write block of zeros and move to next number
	  if( stokesdata[i].sequence_number - prev_seqnr > 100){
	    fprintf(stderr,"skipping corrupted sequence number: %d.\n", stokesdata[i].sequence_number );
	    for( t=0; t < SAMPLES; t++){
	      shvalue =0;
	      fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	      output_samples++;
	      nul_samples++;
	    }
	    prev_seqnr ++;
	    
	    //dropped packet(s) write zeros until gap is padded
	  } else {
	    int gap, ii;
	    gap = stokesdata[i].sequence_number - (prev_seqnr+1);
	    fprintf(stderr, "gap:%d\n", gap);
	    for( ii=0; ii< gap; ii++){
	      for( t=0; t < SAMPLES; t++){
		shvalue =0;
		fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
		output_samples++;
		nul_samples++;
	      }
	    }
	    prev_seqnr = stokesdata[i].sequence_number;  
	  }
	} else {
	  //write data
	  for( t = 0; t < SAMPLES; t++ ){
	    value = floor((stokesdata[i].samples[beamnr][chan][t][s]-offset)*scale);
	    if( isnan(value)) {
	      value = 0;
	      nul_samples++;
	    }
	    
	    if( eightBit==1 ){ //8-bit mode
	      if( value >= 128 ) {
		value = 127;
		toobig++;
	      }
	      if( value < -128 ) {
		value =  -128;
		toosmall++;
	      }
	      ivalue = value;
	      fwrite(&ivalue, sizeof ivalue, 1, outputfile[chan]);
	      
	    } else if( writefloats==1 ){ //float mode
	      fwrite(&value, sizeof value, 1, outputfile[chan]);

	    } else { // 16-bit mode
	      if( value >= 32768 ) {
		value = 32767;
		toobig++;
	      }
	      if( value < -32768 ) {
		value =  -32768;
		toosmall++;
	      }
	      shvalue = value;
	      fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	    }
	    output_samples++;
	  }//for( t = 0; t < SAMPLES; t++ )
	  prev_seqnr = stokesdata[i].sequence_number;
	}//if( prev_seqnr + 1 != stokesdata[i].sequence_number ) else{
      }//for( i = 0; i < num; i++ )
    }//for( chan = 0; chan < 1; chan++ )
  }//while( !feof( input ) )
  fprintf(stderr,"%u samples in output, %.2f%% bad samples, %.2f%% too big, %.2f%% too small %d Blocks\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples, x);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free(stokesdata);
}//end of convert_nocollapse


void convert_collapse( FILE *input, FILE **outputfile, int beamnr )
{
 
  struct stokesdata_struct {
    unsigned	sequence_number;
    char        pad[508];
    float	samples[BEAMS][CHANNELS][SAMPLES|2][STOKES];
  };
  
  struct stokesdata_struct *stokesdata;
  unsigned output_samples = 0;
  unsigned nul_samples = 0;
  unsigned toobig = 0, toosmall = 0;
  double scale;
  double offset;
  int x = 0;
  
  stokesdata = (struct stokesdata_struct *) malloc( AVERAGE_OVER * sizeof(struct stokesdata_struct) );
  fseek( input, 0, SEEK_SET ); 
  while( !feof( input ) ) {
    if (x == NUM_BLOCKGROUPS) break;
    x++;
    unsigned num = 0;
    unsigned i,c;
    double average = 0;
    double rms = 0;
    int validsamples = 0;
    int N;
    /* read data */
    num = fread( &stokesdata[0], sizeof stokesdata[0], AVERAGE_OVER, input );
    
    unsigned t, chan, s = STOKES_SWITCH;
    for( chan = 0; chan < CHANNELS; chan++ ){
      for( i = 0; i < num; i++ ) { 
	//swap endianness (sequence number is big endian)
	if( chan == 0){
	  swap_endian( (char*)&stokesdata[i].sequence_number );
	}
	
	for( t = 0; t < SAMPLES; t++ ){ 
	  //first pass to compute statistics
	  floatSwap( &stokesdata[i].samples[beamnr][chan][t][s] );
	  if( !isnan(stokesdata[i].samples[beamnr][chan][t][s]) ){
	    average += stokesdata[i].samples[beamnr][chan][t][s]; 
	    rms += stokesdata[i].samples[beamnr][chan][t][s]*stokesdata[i].samples[beamnr][chan][t][s];
	    validsamples++;
	  }
	}	 
      }
    }
    //calculate scale and offset for the block
    N = validsamples?validsamples:1;
    average = average/N;
    rms = sqrt((rms - (N*average*average))/(N-1));
    if( eightBit==1 ){ //8-bit mode
      scale = 256.0/(N_sigma*8*rms);
      offset = average;
    } else if( writefloats==1 ){ //float mode (no scale or offset)
      scale = 1;
      offset = 0;
    } else { //16-bit scaling
      scale = 65536.0/(N_sigma*8*rms);
      offset = average;
    }
    int prev_seqnr = -1 + (x-1)*AVERAGE_OVER;
    for( i = 0; i < num; i++ ) {    
      double value;
      signed short shvalue;
      signed char ivalue;
      /* detect gaps and write zeros here*/
      if( prev_seqnr + 1 != stokesdata[i].sequence_number ) {
	fprintf(stderr,"gap between sequence numbers %d and %d.\n", prev_seqnr, stokesdata[i].sequence_number );
	
	//corrupted sequence number, write block of zeros and move to next number
	if( stokesdata[i].sequence_number - prev_seqnr > 100){
	  fprintf(stderr,"skipping corrupted sequence number: %d.\n", stokesdata[i].sequence_number );
	  for( t=0; t < SAMPLES; t++){
	    shvalue =0;
	    fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	    output_samples++;
	    nul_samples++;
	  }
	  prev_seqnr ++;
	  
	  //dropped packet(s) write zeros until gap is padded
	} else {
	  int gap, ii;
	  gap = stokesdata[i].sequence_number - (prev_seqnr+1);
	  fprintf(stderr, "gap:%d\n", gap);
	  for( ii=0; ii< gap; ii++){
	    for( t=0; t < SAMPLES; t++){
	      shvalue =0;
	      fwrite(&shvalue, sizeof shvalue, 1, outputfile[chan]);
	      output_samples++;
	      nul_samples++;
	    }
	  }
	  prev_seqnr = stokesdata[i].sequence_number;  
	}
      } else {
	//write data
	for( t = 0; t < SAMPLES; t++ ){
	  value = 0;
	  for( chan = 0; chan < CHANNELS; chan++ ){ 
	    value += floor((stokesdata[i].samples[beamnr][chan][t][s]-offset)*scale);
	  }
	  if( isnan(value)) {
	    value = 0;
	    nul_samples++;
	  }
	  if( eightBit==1 ){ //8-bit mode
	    if( value >= 128 ) {
	      value = 127;
	      toobig++;
	    }
	    if( value < -128 ) {
	      value =  -128;
	      toosmall++;
	    }
	    ivalue = value;
	    fwrite(&ivalue, sizeof ivalue, 1, outputfile[chan]);
	    
	  } else if( writefloats==1 ){ //float mode
	    fwrite(&value, sizeof value, 1, outputfile[chan]);
	    
	  } else { // 16-bit mode
	    if( value >= 32768 ) {
	      value = 32767;
	      toobig++;
	    }
	    if( value < -32768 ) {
	      value =  -32768;
	      toosmall++;
	    }
	    shvalue = value;
	    fwrite(&shvalue, sizeof shvalue, 1, outputfile[0]);
	  }
	  output_samples++;
	}//for( t = 0; t < SAMPLES; t++ )
	prev_seqnr = stokesdata[i].sequence_number;
      }//if( prev_seqnr + 1 != stokesdata[i].sequence_number ) else{
    }//for( i = 0; i < num; i++ )
  }//while( !feof( input ) )
  fprintf(stderr,"%u samples in output, %.2f%% bad samples, %.2f%% too big, %.2f%% too small %d Blocks\n",output_samples,100.0*nul_samples/output_samples,100.0*toobig/output_samples,100.0*toosmall/output_samples, x);
  fprintf(stderr,"%.10f seconds per sample\n",SAMPLEDURATION);
  free(stokesdata);
}//end of convert_collapse()

int main( int argc, char **argv ) {
  float avr;
  int f,b,c,y,n_outfiles,n_infiles;
  char buf[1024];
  int i=0;

  while (( c = getopt(argc, argv, "r:b:B:n:N:A:c:s:p:o:f:S:L:T:t8hCF")) != -1)
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
	    fprintf(stderr, "ERROR: Number of Stokes = %d\n", STOKES);
	    exit(-1);
	  }
	  break;
	  
	case 'L':
	  writefb = 1;
	  if (sscanf(optarg, "%1000s", &parsetfile) != 1) {
	    fprintf(stderr, "ERROR: Could not set parsetfile = %s\n", optarg);
	    exit(-1);
	  }
	  break;
	  
	case '8':
	  eightBit = 1;
	  break;
	  
	case 'T':
	  if (sscanf(optarg, "%d", &SUBBANDS) != 1) {
	    fprintf(stderr,"ERROR: Number of Subbands = %s\n", optarg);
	    exit(-1);
	  }
	  TRANSPOSE = 1;
	  fprintf(stderr,"COHERENT SECOND TRANSPOSE MODE\n");
	  break; 
	  
	case 't':
	  TRANSPOSE = 2;
	  fprintf(stderr,"INCOHERENT SECOND TRANSPOSE MODE\n");
	  break; 

	case 'o':
	  if (sscanf(optarg, "%99s", &OUTNAME) != 1) {
	    fprintf(stderr, "ERROR: Output Name = %s\n", optarg);
	    exit(-1);
	  }
	  if (sscanf(OUTNAME,optarg)==-1){
	    fprintf(stderr, "Output Name shortened to: %s\n",OUTNAME);
	  }
 	  break;
	  
	case 'f':
	  if (sscanf(optarg, "%d", &BASESUBBAND) != 1) {
	    fprintf(stderr, "ERROR: Basesubband = %d\n", optarg);
	    exit(-1);
	  }
	  if (BASESUBBAND > 99999 || BASESUBBAND < 0){
	    fprintf(stderr, "ERROR: Lowest subband = %d\n", BASESUBBAND);
	    exit(-1);
	  }
	  
 	  break;
	  
	case 'S':
	  if (sscanf(optarg, "%d", &STOKES_SWITCH) != 1){
	    fprintf(stderr, "ERROR: Please choose stokes parameter to output\n 0 = StokesI\n 1 = StokesQ\n 2 = StokesU\n 3 = StokesV\n ", optarg);
	    exit(-1);
	  }
	  if (STOKES_SWITCH>4||STOKES_SWITCH<0){
	    fprintf(stderr, "ERROR: Please choose stokes parameter to output\n 0 = StokesI\n 1 = StokesQ\n 2 = StokesU\n 3 = StokesV\n");
	    exit(-1);
	  }
 	  break;
	  
	case 'r':
	  if (sscanf(optarg, "%f", &N_sigma) != 1){
	    fprintf(stderr, "ERROR: Please number of sigma to map: %s\n", optarg);
	    exit(-1);
	  }
	  if (N_sigma < 0){
	    fprintf(stderr, "Data above %d*sigma cannot be clipped... Please select a new value.\n", N_sigma);
	    exit(-1);
	  }
	  break;
	  
	case 'h':
	  usage();
	  exit(1);
	  break;
	}
    } 
  FILE *input[MAXNOINPUTFILES], *outputfile[CHANNELS]; 
  c=0;
  
  /* command line verification */
  if( argc -optind < 1 ) {
    usage();
    exit(1);
  }
  fprintf(stderr,  "%d channels %d beams %d samples %d stokes\n", CHANNELS, BEAMS, SAMPLES, STOKES );
  fprintf(stderr, "Output Name: %s\n", OUTNAME);
  fprintf(stderr, "Lowest Subband: %d\n",BASESUBBAND);
  fprintf(stderr, "Stokes Parameter: %d\n",STOKES_SWITCH);
  
  /* make beam dirs*/ 
  if( BEAMS > 1 ) {
    for( b = 0; b < BEAMS; b++ ) {
      sprintf( buf, "beam_%d", b );
      mkdir( buf, 0775 );
    }
  }
  
  /* open files */
  for( b = 0; b < BEAMS; b++ ) {
    /* how many files to open for output? */
    if (collapse == 1 || writefb == 1) {
      n_outfiles = 1;        /* one file only */
    }else if ( TRANSPOSE==1 ){
      n_outfiles = CHANNELS; /* one file per channel */
    } else {
      n_outfiles = CHANNELS; /* one file per channel */
    }
    
    /* loop over input files */
    int index = 0;
    n_infiles = argc-optind;
    if (n_infiles > MAXNOINPUTFILES) { 
      printf("n_infiles (%d) > MAXNOINPUTFILES (%d), recompile needed, aborting.",n_infiles, MAXNOINPUTFILES);
      exit(1);
    }
    
    for( f = 0; f < n_infiles; f++ ) {
     fprintf(stderr,"Starting Work on %s\n", argv[optind+f]);
     input[f] = fopen( argv[optind+f], "rb" );
     if( !input[f] ) {
    	perror( argv[f] );
    exit(1);
     }
    }
    

    /* open output file(s), do conversion */
    
    //FILTERBANK MODE
    if ( writefb==1 ){ 
      /* create single filterbank file*/
      sprintf( buf, "%s.fil", OUTNAME );  /* append filterbank suffix */
      fprintf( stderr,"Writing to file %s\n", buf);
      /* open file */
      outputfile[0] = fopen( buf, "wb" );
      if( !outputfile[0] ) {
	perror( buf );
	exit(1);
      }
      /* do the conversion */     
      convert_filterBank( input, outputfile, b, writefb, n_infiles ); // where writefb==1
      /* close outfile */
      fclose( outputfile[0] );
      

    //COHERENT SECOND TRANSPOSE MODE
    } else if ( TRANSPOSE==1 ){ 
      /* do the conversion */     
      secondTranspose( input, b ); // takes file **input; writefb==0
    
    //INCOHERENT SECOND TRANSPOSE MODE
    } else if ( TRANSPOSE==2 ){ 
      for( f = 0; f < n_infiles; f++ ) { /* loop over input files */
	for ( c = 0; c < n_outfiles; c++ ) { /* make CHANNEL output files */
	  sprintf( buf, "%s.sub%04d", OUTNAME, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
	  if ( BEAMS > 1  ) {
	    sprintf(buf, "beam_%d/%s", b, buf ); /* prepend beam name */
	  } else {
	    sprintf(buf, "%s", buf);
	  }
	  fprintf(stderr,"%s -> %s\n", argv[optind+f], buf); 
	  /*open file */
	  index++;
	  outputfile[c] = fopen( buf, "wb" );
	  if( !outputfile[c] ) {
	    perror( buf );
	    exit(1);
	  }
	}
	/* do the conversion */     
	secondTranspose_inc( input, outputfile, b, writefb, n_infiles); // takes file **input; writefb==0
	for ( c = 0; c < n_outfiles; c++ ) { /* close CHANNEL output files */
	  fclose( outputfile[c] );
	}
      }
    
      //NORMAL MODE
    } else {
      /* create CHANNELS subband files*/
      for( f = 0; f < n_infiles; f++ ) { /* loop over input files */
	for ( c = 0; c < n_outfiles; c++ ) { /* make CHANNEL output files */
	  sprintf( buf, "%s.sub%04d", OUTNAME, index  || !INITIALSUBBANDS ? index + BASESUBBAND : 0 );
	  if ( BEAMS > 1  ) {
	    sprintf(buf, "beam_%d/%s", b, buf ); /* prepend beam name */
	  } else {
	    sprintf(buf, "%s", buf);
	  }
	  fprintf(stderr,"%s -> %s\n", argv[optind+f], buf); 
	  /* open file */
	  index++;
	  outputfile[c] = fopen( buf, "wb" );
	  if( !outputfile[c] ) {
	    perror( buf );
	    exit(1);
	  }
	}
	/* do the conversion */     
	if (collapse==0) { /* default, write all channels */
	  convert_nocollapse( input, outputfile, b, writefb, n_infiles ); // takes file **input; writefb==0
	} else {
	  convert_collapse( input[f], outputfile, b );	// takes file *input for input
	}
	for ( c = 0; c < n_outfiles; c++ ) { /* close CHANNEL output files */
	  fclose( outputfile[c] );
	}
      }
    }
  }
  return 0;
}
