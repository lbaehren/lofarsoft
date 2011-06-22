#include<iostream>
#include<fstream>
#include <sstream>
#include<cstdlib>
#include<cmath>
#include<iomanip>
#include<vector>
#include<algorithm>
#include<map>
#include<complex>
#include<cstring>
#include<string>
#include <iterator>
// 2011 May 24  James M Anderson  --MPIfR  code modifications for speedup
// 2011 May 25  JMA  --convert more IO to C
// 2011 May 27  JMA  --restrict to common scaling for both real and imag parts
//                     and both X and Y polarizations (or R and L)
// 2011 Jun 03  JMA  --use same min/max range for all real/imaginary values,
//                     for both X and Y simulateously, but different for
//                     different channels
#include <stdint.h>


/* restrict
   This is a really useful modifier, but it is not supported by
   all compilers.  Furthermore, the different ways to specify it
   (double * restrict dp0, double dp1[restrict]) are not available
   in the same release of a compiler.  If you are still using an old
   compiler, your performace is going to suck anyway, so this code
   will only give you restrict when it is fully available.
*/
#ifdef __GNUC__
#  ifdef restrict
/*   Someone else has already defined it.  Hope they got it right. */
#  elif !defined(__GNUG__) && (__STDC_VERSION__ >= 199901L)
/*   Restrict already available */
#  elif !defined(__GNUG__) && (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#    define restrict __restrict
#  elif (__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#    define restrict __restrict
#  else
#    define restrict
#  endif
#else
#  ifndef restrict
#    define restrict
#  endif
#endif




#define NEED_TO_BYTESWAP 1
#define MAX_NFILES 10000
uint32_t CHANNELS = 16;
uint32_t SUBBANDS = 32;
uint32_t SAMPLES = 12208;
uint32_t NPOL = 2;

int verb = 0; // generic verbosity flag
int realv = 0; // float-writer flag
const int_fast32_t INPUT_DATA_BLOCK_HEADER_SIZE = 512;

#define CLOCKRES 0.1953125

using namespace std;

streamsize getSize(const char*); 
inline float FloatRead(int32_t a){
  union{
    float f;
    int32_t i;
  }dat;
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
  a = __builtin_bswap32(a);
#    else
  uint32_t u(a);
  a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
  uint32_t u(a);
  a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
  /* do nothing */
#endif
  dat.i = a;
  return dat.f;
}
inline int32_t byteswap_32_bit_int(int32_t &a){
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
  return __builtin_bswap32(a);
#    else
  uint32_t u(a);
  return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
  uint32_t u(a);
  return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
  /* do nothing */
#endif
  return a;
}
inline uint32_t byteswap_32_bit_int(uint32_t &u){
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
  return uint32_t(__builtin_bswap32(int32_t(u)));
#    else
  return uint32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
  return uint32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
  /* do nothing */
#endif
  return u;
}
void byteswap_32_bit_float_array(const size_t NUM, float* const restrict fp){
#if(NEED_TO_BYTESWAP)
    int32_t* const restrict ip(reinterpret_cast<int32_t* const restrict>(fp));
    for(size_t s=0; s < NUM; s++) {
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
        ip[s] = __builtin_bswap32(ip[s]);
#    else
        uint32_t u(ip[s]);
        ip[s] = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
        uint32_t u(ip[s]);
        ip[s] = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
    }
#else
  /* do nothing */
#endif
  return;
}



//complex<float> convert_complex(complex<float>);

struct samp_t{
  int8_t Xr;
  int8_t Xi;
  int8_t Yr;
  int8_t Yi;
};

class writer{
public:
    writer() : sample_block_offset(0xFFFFFFFFU), blocksamples(0) {}
    ~writer();
  void filesInit();
  void filesClose();
  void writePuMa2Block( const float * const restrict Xsamp,
                        const float * const restrict Ysamp);
  void writePuMa2BlockMissingFill(const int8_t Xfill,
                                  const int8_t Yfill);
  void readParset();
  
  void passFilenames(char* &, char* &, char* &);
  void passSub(uint32_t  &);
  void passChan(uint32_t &);

  uint32_t NCHANNELS;
  uint32_t SUBBSTART;
  uint32_t SUBBEND;
  uint32_t NSUBBANDS;

  string DATE;
  string ANTENNA;

private:
  FILE* datafile[MAX_NFILES];
  FILE* realvolt[MAX_NFILES];
  void writeHeader(unsigned int &, unsigned int &);
  char* _basename;
  char* _hdrFilename;
  char* _psetFilename;
  uint32_t _the_chan;  
  uint32_t _the_sub;  
  string getKeyVal(string &);
  double mjdcalc();
  size_t sample_block_offset;
  samp_t* restrict blocksamples;
};
writer::~writer() {
    free(blocksamples);
}

int main(int argc, char** argv){

  if(argc==1){
  	cout << endl << endl
         << "Usage: <executable> -f <input file> -h <header file> -p <parset file> -BG -v "         << endl << endl
         << " --- Example: bf2puma2 -f L00000_SAP000_B000 -h header.puma2 -p L00000.parset -BG -v " << endl << endl; 
         
        return 0;
  }

  char* basename = 0;
  char* hdrFilename = 0;
  char* psetFilename = 0;
  int8_t flag_value(-128); // 0xFF
  int BlueGeneData = 0;

  for(int ia=0; ia<argc; ia++){      
    if(string(argv[ia])=="-f"){
        if(ia+1 < argc) {
            basename = argv[ia+1];
        }
        else {
            cerr << "no argument following '-f'" << endl;
            exit(2);
        }
      continue;
    }else if(string(argv[ia])=="-h"){
        if(ia+1 < argc) {
            hdrFilename = argv[ia+1];
        }
        else {
            cerr << "no argument following '-h'" << endl;
            exit(2);
        }
      continue;
    }else if(string(argv[ia])=="-p"){
        if(ia+1 < argc) {
            psetFilename = argv[ia+1];
        }
        else {
            cerr << "no argument following '-p'" << endl;
            exit(2);
        }
      continue;
    }else if( string(argv[ia])=="-v"){
      // Turn on verbosity
      verb = 1;
      continue;
    }else if( string(argv[ia])=="-t"){
      // Write float complex samples
      realv = 1;
      continue;
    }else if( string(argv[ia])=="-BG"){
      // Turn on Blue Gene block reads
      BlueGeneData = 1;
      continue;
    }else if( string(argv[ia])=="-flag_value"){
      // Write float complex samples
        if(ia+1 < argc) {
            flag_value = int8_t(atoi(argv[ia+1]));
        }
        else {
            cerr << "no argument following '-flag_value'" << endl;
            exit(2);
        }
      continue;
    }
  }
  
  if( verb ){
    cout << "input = " << basename <<endl;
    cout << "header = " << hdrFilename <<endl;
    cout << "parset = " << psetFilename <<endl;
  }
  
  string filenameX_str = string(basename)+"_S0_P000_bf.raw";
  string filenameY_str = string(basename)+"_S1_P000_bf.raw";
  
  const char* filenameX = filenameX_str.c_str();
  const char* filenameY = filenameY_str.c_str();
  
  FILE * pfileX;
  FILE * pfileY;
  
  pfileX = fopen ( filenameX , "rb" );
  if (pfileX==NULL){fputs ("File error",stderr); exit (1);}
  
  pfileY = fopen ( filenameY , "rb" );
  if (pfileY==NULL) {fputs ("File error",stderr); exit (1);}
  
  writer puma2data;

  puma2data.passFilenames(basename,hdrFilename, psetFilename);

  puma2data.readParset();
  
  puma2data.filesInit();


  if(verb)
    cout << "\n\nUsing Channels: " << CHANNELS << "\t and Subbands: " 
         << SUBBANDS << "\t and Samples: " << SAMPLES << endl << endl;

#if(NEED_TO_BYTESWAP)
  size_t Jnumber_floats = size_t(2)*CHANNELS*SUBBANDS*(SAMPLES);
#else
#endif
  size_t Jnumber_floats_read = (BlueGeneData) ?
      size_t(2)*CHANNELS*SUBBANDS*(SAMPLES|2) : size_t(2)*CHANNELS*SUBBANDS*(SAMPLES);
  size_t Jnumber_char = sizeof(float)*Jnumber_floats_read;

  // One-dimensional arrays for temporary storage
  float * restrict XJV;
  XJV = reinterpret_cast<float* restrict>(malloc(Jnumber_char));
  float * restrict YJV;
  YJV = reinterpret_cast<float* restrict>(malloc(Jnumber_char));
  
  if( XJV==0 || YJV==0){
    fputs( "Memory fail", stderr ); 
    exit( 1 );
  }

  // X-pol values:
  uint32_t seqnoX;

  uint32_t Xpad[INPUT_DATA_BLOCK_HEADER_SIZE/sizeof(uint32_t)];

  // Y-pol values:
  uint32_t  seqnoY;

  uint32_t Ypad[INPUT_DATA_BLOCK_HEADER_SIZE/sizeof(uint32_t)];

  if(verb){
    cout << "SUBBANDS = " << puma2data.NSUBBANDS <<endl;
    cout << "CHANNELS = " << puma2data.NCHANNELS <<endl;
  }


  uint32_t iblock = 0;

  while(1){
      size_t num;
      uint32_t nskip(0);
      cerr << "Reading block: " << iblock << endl;


      // Read X data:
      num = fread( Xpad, INPUT_DATA_BLOCK_HEADER_SIZE, 1, pfileX );
      if( num != 1 ) goto end_of_data;

      num = fread( XJV, Jnumber_char, 1, pfileX );
      if( num != 1 ) goto end_of_data;
#if(NEED_TO_BYTESWAP)
      seqnoX = byteswap_32_bit_int(Xpad[0]);
      byteswap_32_bit_float_array(Jnumber_floats, XJV);
#else
      seqnoX = Xpad[0];
#endif

      // Read Y data:
      num = fread( Ypad, INPUT_DATA_BLOCK_HEADER_SIZE, 1, pfileY );
      if( num != 1 ) goto end_of_data;

      num = fread( YJV, Jnumber_char, 1, pfileY );
      if( num != 1 ) goto end_of_data;
#if(NEED_TO_BYTESWAP)
      seqnoY = byteswap_32_bit_int(Ypad[0]);
      byteswap_32_bit_float_array(Jnumber_floats, YJV);
#else
      seqnoY = Ypad[0];
#endif

      if(verb)
        cout <<setw(10)<< "IBLOCK = " <<setw(10)<< iblock <<setw(10)
             << " SEQNO_X = " << seqnoX <<setw(10)
             << " / SEQNO_Y = " << seqnoY <<endl;

      while(seqnoX != seqnoY){
        while(seqnoX < seqnoY){
          if(verb)cerr << "seqnoX < seqnoY" <<endl;
          
          num = fread( Xpad, INPUT_DATA_BLOCK_HEADER_SIZE, 1, pfileX );
          if( num != 1 ) goto end_of_data;

          num = fread( XJV, Jnumber_char, 1, pfileX );
          if( num != 1 ) goto end_of_data;
#if(NEED_TO_BYTESWAP)
          seqnoX = byteswap_32_bit_int(Xpad[0]);
          byteswap_32_bit_float_array(Jnumber_floats, XJV);
#else
          seqnoX = Xpad[0];
#endif
        }

        while(seqnoY < seqnoX){
          if(verb)cerr << "seqnoY < seqnoX" <<endl;
          
          num = fread( Ypad, INPUT_DATA_BLOCK_HEADER_SIZE, 1, pfileY );
          if( num != 1 ) goto end_of_data;

          num = fread( YJV, Jnumber_char, 1, pfileY );
          if( num != 1 ) goto end_of_data;
#if(NEED_TO_BYTESWAP)
          seqnoY = byteswap_32_bit_int(Ypad[0]);
          byteswap_32_bit_float_array(Jnumber_floats, YJV);
#else
          seqnoY = Ypad[0];
#endif
        }
      }

      // do we need to skip blocks?
      while(iblock < seqnoX){
          if(verb){
            cerr << "iblock < seqno: " << iblock << " < " << seqnoX <<endl;
            cerr <<"("<< iblock <<")"; 
          }
          nskip++; 
          iblock++;
      }


      
      //START WRITING
      for(uint32_t isub=0; isub<puma2data.NSUBBANDS; isub++){
	if(verb)cout << "Starting subband: " << isub << endl;
	puma2data.passSub(isub);
	for(uint32_t ichan=0; ichan<puma2data.NCHANNELS; ichan++){
          puma2data.passChan(ichan);

          // WRITE FLAG VALUE IN ALL EMPTY BLOCKS
          for(uint32_t iskip=0; iskip<nskip; iskip++)
            puma2data.writePuMa2BlockMissingFill( flag_value, flag_value );

          puma2data.writePuMa2Block( XJV, YJV );
	} // End of loop over channels

      } // End of loop over subbands
      iblock++;
  } // End of while(1) loop
end_of_data:
  if(verb)cout << " *** Read " << iblock << " blocks." <<endl; 
  if(verb)cout << "Closing output files: " << endl;
  puma2data.filesClose();

  return 0;
}

void writer::passFilenames(char* &base, char* &hdr, char* &pset){
  _basename = base;
  _hdrFilename = hdr;
  _psetFilename = pset;
}

void writer::passSub(unsigned int &sub){
  _the_sub = sub;  
}

void writer::passChan(unsigned int &chan){
  _the_chan = chan;  
}

void writer::readParset(){

  // Determine the type of antenna:
  vector<string> tmps;
  tmps.push_back("Observation.antennaArray");
  tmps.push_back("Observation.antennaSet");
  
  ANTENNA = "UNKNOWN";
  for(unsigned int it=0; it<tmps.size(); it++)
  {
    string key = getKeyVal(tmps[it]).substr(0,3);
    if(key=="HBA" || key=="LBA")
    {
      ANTENNA = key;
      break;
    }
  }
  if(verb)cout << " ANTENNA TYPE = " << ANTENNA <<endl;


  // Determine number of channels:
  string tmp = "Observation.channelsPerSubband";
  NCHANNELS = atoi(getKeyVal(tmp).c_str());
  CHANNELS = NCHANNELS;
  if(verb)cout << " CHANNELS PER SUBBAND = " << NCHANNELS <<endl;

  // Determine number of polarisations:
  tmp = "Observation.nrPolarisations";
  NPOL = atoi(getKeyVal(tmp).c_str());
  if(verb)cout << " Number of Polarisations = " << NPOL << endl;
  if((NPOL < 1) || (NPOL > 4)) {
      cerr << "NPOL has bad value " << NPOL << endl;
      exit(2);
  }
  
  // Determine starting subband:
  string mychar = "";
  int ic = 1;
  while(1){
    tmp = "Observation.subbandList";
    mychar = getKeyVal(tmp).substr(1,ic);

    if(mychar.substr(ic-1,1)==".") break;
    ic++;
  }
  SUBBSTART = atoi(mychar.c_str());
  if(verb)cerr << "Got SUBBSTART = " << SUBBSTART <<endl;

  // Determine ending subband:
  mychar = "";
  int nc = ic+2;
  ic = 1;
  while(1){
    tmp = "Observation.subbandList";
    mychar = getKeyVal(tmp).substr(nc,ic);
    
    if(mychar.substr(ic-1,1)=="]") break;
    
    ic++;
  }
  SUBBEND = atoi(mychar.c_str());

  if(verb){
    cerr << "Got SUBBEND = " << SUBBEND <<endl;
    cout << " SUBBAND RANGE = [" << SUBBSTART << "--" << SUBBEND <<"]" <<endl;
  }

  // Determine number of subbands:
  NSUBBANDS = SUBBEND - SUBBSTART + 1;
  SUBBANDS = NSUBBANDS;
  // Determine Number of samples:
  tmp = "OLAP.CNProc.integrationSteps";
  SAMPLES = atoi(getKeyVal(tmp).c_str());
  if(verb)cout << "Have SAMPLES: " << SAMPLES << endl;
  if(SAMPLES < 1) {
      cerr << "SAMPLES has bad value " << SAMPLES << endl;
      exit(2);
  }

  free(blocksamples);
  blocksamples = reinterpret_cast<samp_t* restrict>(malloc(sizeof(samp_t)*SAMPLES));
  
  // Determine observing epoch:
  tmp="Observation.startTime";
  DATE = getKeyVal(tmp).c_str();
  
  mjdcalc();
  cout << "DATE = " << DATE <<endl;

  // determine the sample block offset within the loops for
  // writePuMa2Block.  This is the number of subbands times the
  // number of channels per subband, times the number of words
  // per complex voltage
  sample_block_offset = size_t(NSUBBANDS) * NCHANNELS * 2;
}

void writer::filesInit(){
  for(unsigned int isub=0; isub<NSUBBANDS; isub++){

    for(unsigned int ichan=0; ichan<NCHANNELS; ichan++){
    
       stringstream the_sub_sstrm;
       the_sub_sstrm << isub;    

       stringstream the_chan_sstrm;
       the_chan_sstrm << ichan;    

       string basename_str = _basename;
       string filename_str = basename_str + "_SB" + the_sub_sstrm.str() + "_CH" + 
	 the_chan_sstrm.str();
       string filename2_str = filename_str+".rv";

       datafile[isub*NCHANNELS+ichan] = fopen(filename_str.c_str(), "wb");
       if(datafile[isub*NCHANNELS+ichan] == NULL) {
           cerr << "Unable to open file '" << filename_str << "' for writing" << endl;
           exit(2);
       }
       if(realv) {
           realvolt[isub*NCHANNELS+ichan] = fopen(filename2_str.c_str(), "wb");
           if(realvolt[isub*NCHANNELS+ichan] == NULL) {
               cerr << "Unable to open file '" << filename2_str << "' for writing" << endl;
               exit(2);
           }
       }
       writeHeader(isub,ichan);
    }
    
  }

}

void writer::filesClose(){
    for(unsigned int isub=0; isub<NSUBBANDS; isub++)
      for(unsigned int ichan=0; ichan<NCHANNELS; ichan++)
      { 
          fclose(datafile[isub*NCHANNELS+ichan]);
          if(realv) fclose(realvolt[isub*NCHANNELS+ichan]);
      }
}

void writer::writePuMa2Block(const float * const restrict Xsamp,
                             const float * const restrict Ysamp) restrict {

    // to convert from float value f to int8_t value i, we want to do
    // i = int8_t( 127.5 * f / diff )
    // where diff is the maximum (absolute) difference away from 0 found
    // in the values.  Rewrite this as
    // i = int8_t( lrintf( f * multiplier ) )
    // where multiplier = 127.5 / diff
    // and lrintf rounds to the nearest integer.  In order to prevent
    // going out of range, instead of exactly 127.5 we use 127.495 to
    // prevent rounding errors from exceeding the int8_t range.

  static const float int8_max_float = 127.495;

  const size_t start = (size_t(_the_sub) * NCHANNELS + _the_chan)*2;

  ///////////// STATISTICS ////////////////////
  float multiplier;

  float max00, min00;
  float max01, min01;
  float max10, min10;
  float max11, min11;

  if(SAMPLES > 0) {
      max00 = min00 = Xsamp[start];
      max01 = min01 = Xsamp[start+1];
      max10 = min10 = Ysamp[start];
      max11 = min11 = Ysamp[start+1];
  }
  else {
      max00 = min00 = 0.0f;
      max01 = min01 = 0.0f;
      max10 = min10 = 0.0f;
      max11 = min11 = 0.0f;
  }      
  int iv = 0;
  int navg = 100;
  float vsum = 0.0f;

  size_t offset = 0;
  for( uint_fast32_t isamp = 0; isamp < SAMPLES; isamp++,
           offset += sample_block_offset ){

    float reX = Xsamp[start+offset];
    float imX = Xsamp[start+offset+1];
    float reY = Ysamp[start+offset];
    float imY = Ysamp[start+offset+1];

    if     ( reX > max00 ) max00 = reX;
    else if( reX < min00 ) min00 = reX;

    if     ( imX > max01 ) max01 = imX;
    else if( imX < min01 ) min01 = imX;

    if     ( reY > max10 ) max10 = reY;
    else if( reY < min10 ) min10 = reY; 

    if     ( imY > max11 ) max11 = imY;
    else if( imY < min11 ) min11 = imY;
    
    if(realv)
    {
        iv++;
        vsum += reX*reX+imX*imX+reY*reY+imY*imY;
        if(iv == navg) {
            if(fwrite(&vsum, sizeof(vsum), 1, realvolt[_the_sub*NCHANNELS+_the_chan]) != 1) {
                cerr << "Error writing to realvolt" << endl;
            }
            iv = 0;
            vsum = 0.0f;
        }
    }


  }
  // Now find the biggest absolute difference
  {
      max00 = fabsf(max00);
      max01 = fabsf(max01);
      max10 = fabsf(max10);
      max11 = fabsf(max11);
      min00 = fabsf(min00);
      min01 = fabsf(min01);
      min10 = fabsf(min10);
      min11 = fabsf(min11);

      float diff00 = (max00 > min00) ? max00 : min00;
      float diff01 = (max01 > min01) ? max01 : min01;
      float diff10 = (max10 > min10) ? max10 : min10;
      float diff11 = (max11 > min11) ? max11 : min11;

      float diff0 = (diff00 > diff01) ? diff00 : diff01;
      float diff1 = (diff10 > diff11) ? diff10 : diff11;

      float diff = (diff0 > diff1) ? diff0 : diff1;

      if(diff == 0.0f) {
          diff = 1.0f;
      }

      multiplier = int8_max_float / diff;
  }

  ////////////////REPACKING AND SAVING//////////////////////

  offset = 0;
  for( uint_fast32_t isamp = 0; isamp < SAMPLES; isamp++,
           offset += sample_block_offset ){
    float reX = Xsamp[start+offset];
    float imX = Xsamp[start+offset+1];
    float reY = Ysamp[start+offset];
    float imY = Ysamp[start+offset+1];

    blocksamples[isamp].Xr = int8_t(lrintf( reX * multiplier));
    blocksamples[isamp].Xi = int8_t(lrintf( imX * multiplier));
    blocksamples[isamp].Yr = int8_t(lrintf( reY * multiplier));
    blocksamples[isamp].Yi = int8_t(lrintf( imY * multiplier));
  }

  if(fwrite(blocksamples, sizeof(samp_t)*SAMPLES, 1, datafile[_the_sub*NCHANNELS+_the_chan]) != 1) {
      cerr << "Error writing to datafile for blocksamples" << endl;
      exit(2);
  }
}
void writer::writePuMa2BlockMissingFill(const int8_t Xfill,
                                        const int8_t Yfill) restrict {
    // write out fill data for missing samples
    if(realv)
    {
        int iv = 0;
        int navg = 100;
        float vsum = 0.0f;
        
        for( uint_fast32_t isamp = 0; isamp < SAMPLES; isamp++){
            if(++iv == navg) {
                if(fwrite(&vsum, sizeof(vsum), 1, realvolt[_the_sub*NCHANNELS+_the_chan]) != 1) {
                    cerr << "Error writing to realvolt" << endl;
                }
                iv=0;
            }
        }
    }

  ////////////////REPACKING AND SAVING//////////////////////

  for( uint_fast32_t isamp = 0; isamp < SAMPLES; isamp++ ){
    blocksamples[isamp].Xr = Xfill;
    blocksamples[isamp].Xi = Xfill;
    blocksamples[isamp].Yr = Yfill;
    blocksamples[isamp].Yi = Yfill;
  }

  if(fwrite(blocksamples, sizeof(samp_t)*SAMPLES, 1, datafile[_the_sub*NCHANNELS+_the_chan]) != 1) {
      cerr << "Error writing to datafile for blocksamples" << endl;
      exit(2);
  }
}

void writer::writeHeader(unsigned int &isub, unsigned int &ichan){

    const int HEADER_SIZE = 4096;
    char buf[HEADER_SIZE];

    /* for header */
    memset(buf,'\0',sizeof(buf));

    string line;
    stringstream the_freq,the_mjd,the_bw,the_tsamp;
    ifstream headerfile;
    headerfile.open(_hdrFilename);

    while(1){
       getline(headerfile,line,'\n');

       if(line.substr(0,4)=="FREQ"){
         if(ANTENNA=="HBA")
             the_freq <<setprecision(20)<< (100.0+SUBBSTART*CLOCKRES)+
             (isub*NCHANNELS+ichan)*(CLOCKRES/NCHANNELS);
         else
         if(ANTENNA=="LBA")
            the_freq <<setprecision(20)<< (SUBBSTART*CLOCKRES)+
           (isub*NCHANNELS+ichan)*(CLOCKRES/NCHANNELS);
         else
            the_freq <<setprecision(20)<< (100.0+SUBBSTART*CLOCKRES)+
             (isub*NCHANNELS+ichan)*(CLOCKRES/NCHANNELS);        

         line="FREQ " + the_freq.str();
       }else if(line.substr(0,9)=="MJD_START"){

         the_mjd <<setprecision(20) << mjdcalc();
         line="MJD_START " + the_mjd.str();
       }else if(line.substr(0,9)=="UTC_START"){
         line="UTC_START " + DATE.substr(1,19);
       }else if(line.substr(0,2)=="BW"){
        the_bw <<setprecision(20)<< CLOCKRES/NCHANNELS;
        line="BW " + the_bw.str().substr();
       }else if(line.substr(0,5)=="TSAMP"){
        the_tsamp <<setprecision(8)<< (1.0/CLOCKRES)*NCHANNELS;
        line="TSAMP " + the_tsamp.str().substr();
       }

       strncat(buf, (line + "\n").c_str(), HEADER_SIZE-strlen(buf)-1);
       if(headerfile.eof()) break;
    }

    if(fwrite(buf, HEADER_SIZE, 1, datafile[isub*NCHANNELS+ichan]) != 1) {
        cerr << "Error writing to datafile header for isub " << isub << " ichan " << ichan << endl;
        exit(2);
    }
    headerfile.close();
}


// obtaining file size
streamsize getSize(const char* filename){
  streamsize begin,end;
  ifstream myfile (filename, ios::binary);
  begin = myfile.tellg();
  myfile.seekg (0, ios::end);
  end = myfile.tellg();
  myfile.close();
  
  return end-begin;
}

string writer::getKeyVal(string &key){
  
	string line;
	//int start;
	string value;
  
  ifstream parsetfile;
	parsetfile.open(_psetFilename);
	
	while(1){
    getline(parsetfile,line,'\n');
    
    if(parsetfile.eof()) break;
	  
	  for(int i = line.find(key, 0); 
              i != int(string::npos); 
              i = line.find(key, i)){    		  
      
		  istringstream ostr(line);
		  istream_iterator<string> it(ostr);
		  istream_iterator<string> end;
      
		  size_t nwords = 0;
		  while (it++ != end) nwords++;
		  
		  string temp;
		  istringstream words(line);
		  vector<string> parsed;
		  while(words){
        words >> temp;
        if(parsed.size()>=nwords) 
          break;
        parsed.push_back(temp);
		  }
		  
      if(nwords>3){
        value = parsed[nwords-2]+"-"+parsed[nwords-1];
		  }else{
        value = parsed[nwords-1];
		  }
		  i++;
	  }
  }
	parsetfile.close();
  return value;
}

double writer::mjdcalc(){
  
  int yy = atoi(DATE.substr(1,4).c_str());
  int mm = atoi(DATE.substr(6,2).c_str());
  int dd = atoi(DATE.substr(9,2).c_str());
  
  int hh = atoi(DATE.substr(12,2).c_str());
  int mi = atoi(DATE.substr(15,2).c_str());
  int ss = atoi(DATE.substr(18,2).c_str());
  
  int    m, y, ia, ib, ic;
  double jd, mjd0;
  
  if (mm <= 2) {
    y = yy - 1;
    m = mm + 12;
  } else {
    y = yy;
    m = mm;
  }
  ia = y / 100;
  ib = 2 - ia + ia / 4;
  if (y + m / 100. + dd / 1e4 < 1582.1015)  ib = 0;
  ia = (int) ((m + 1) * 30.6001);
  ic = (int) (y * 365.25);
  jd = dd + ia + ib + ic + 1720994.5;
  mjd0 = jd - 2400000.5;
  
  mjd0 += ((double)(hh)+(double)(mi)/60.0+(double)(ss)/3600.0)/24.0;
  
  return mjd0;
}
