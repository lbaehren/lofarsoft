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

#define STATIONS 2
#define CHANNELS 16
#define SAMPLES 12208
#define NPOL 2



#define PERIOD 1292.241530713345
#define CLOCKRES 195.3125


using namespace std;

long getSize(char*); 
void swap_endian(char*);
float FloatSwap(float);
complex<float> convert_complex(complex<float>);
//int FileSize(const char*);

struct rawvoltagedata {
       /* big endian (ppc/sparc) */
       unsigned int  sequence_number;
       char pad[508];
        /* big endian (ppc/sparc) */
       float         samples[STATIONS][CHANNELS][SAMPLES|2][NPOL][2];
};
	


struct samp_t
{
  char Xr;
  char Xi;
  char Yr;
  char Yi;
};

class writer
{
public:
  void fileInit(char* &, unsigned int &);
  void fileClose();
  void writePuMa2Block(rawvoltagedata*);
  void readStations(int, char**);
  void zerovoltages(rawvoltagedata*);

  float pow_I[SAMPLES];
  float pow_Q[SAMPLES];
  float pow_U[SAMPLES];
  float pow_V[SAMPLES];


private:
  ofstream datafile;
  unsigned int the_chan;
  vector<unsigned int> selected_stations; 



};



int main(int argc, char** argv)
{

  rawvoltagedata* bfdata = (rawvoltagedata*) malloc(sizeof(rawvoltagedata));
  rawvoltagedata* padded = (rawvoltagedata*) malloc(sizeof(rawvoltagedata));

  char* filename = argv[1];
  unsigned int allblocks = (unsigned int)(getSize(filename)/sizeof(rawvoltagedata)); 

//  cerr << "File Size: " << getSize(filename) << "  Block Size: " << sizeof(rawvoltagedata) <<endl;
  
  if(argc==1 || argc==2)
  {
  	cout <<endl<<endl
	     << "Usage: beamReader <Input File (raw voltages)> <output file (PuMa2)> [OPTIONS]"  
	     <<endl<<endl;
        cout << "OPTIONS:" <<endl
	<<                 "           -nb  number of blocks to read"; 
	if(argc==2)
	           cout << " (total=" << allblocks << ")";
	cout <<endl<<      "           -s   which stations to combine, e.g. \"2 5 0\" (total=" << STATIONS << ")" 
	     <<endl<<endl;
	return 0;
  }


  
  if (bfdata==NULL) {fputs ("Memory fail",stderr); exit (1);}

  FILE * pfile;
  fpos_t pos;
  
  pfile = fopen ( filename , "rb" );
  if (pfile==NULL) {fputs ("File error",stderr); exit (1);}


  unsigned int nblocks = 0;
  for(int ia=0; ia<argc; ia++)
  	if(string(argv[ia])=="-nb")
	{
                nblocks = atoi(argv[ia+1]);
	        break;
	}	        
  if(nblocks == 0) nblocks = allblocks;


  cout <<endl<< "Reading " << nblocks << " blocks of data:" <<endl;


  writer puma2data;
  
  puma2data.readStations(argc,argv);

  for(unsigned int ichan=0; ichan<CHANNELS; ichan++)
  {
  
	 rewind(pfile);

	 puma2data.zerovoltages(padded);
  
	 puma2data.fileInit(argv[2],ichan);

	 cerr <<"CHANNEL " << ichan+1 <<":   Reading block ";

	 for(unsigned int nonzeroblocks=0, iblock=0; nonzeroblocks<nblocks; nonzeroblocks++, iblock++)
	 {

		 size_t num = fread(bfdata, sizeof(rawvoltagedata), 1, pfile);

		 swap_endian((char*)(&bfdata->sequence_number));
		 unsigned int seqno = bfdata->sequence_number;


		 while(iblock < seqno)
		 {
			       cerr <<"("<< iblock <<")"; 
			       puma2data.writePuMa2Block(padded);
	                       iblock++;
		 }

		 cerr << seqno << ".";

		 puma2data.writePuMa2Block(bfdata);

	 }
	 cerr <<endl;
	 puma2data.fileClose();

  }

  return 0;
}

void writer::readStations(int argc, char** argv)
{
  for(int ia=0; ia<argc; ia++)
  	if(string(argv[ia])=="-s")
	{
		for(int is=ia+1; is<argc; is++)
		{
		        if(string(argv[is])=="-nb") break;
			selected_stations.push_back(atoi(argv[is]));
                }
		break;
	}

   cout << "Combining " << selected_stations.size();
   if(selected_stations.size()>1) cout <<" stations (";
   else                           cout <<" station (";
   
   for(unsigned int is=0; is<selected_stations.size(); is++)
   {
   	cout << selected_stations[is]; 
	if(is<selected_stations.size()-1) cout <<",";
   }
   cout <<")"<<endl; 
}

void writer::fileInit(char* &basename,unsigned int &_the_chan)
{
    
    the_chan = _the_chan;
    
    stringstream the_chan_sstrm;
    the_chan_sstrm << the_chan;    

    string basename_str = basename;
    string filename_str = basename_str + "_" + the_chan_sstrm.str();
    
    datafile.open(filename_str.c_str(), ios::out | ios::binary);

    unsigned char buf[4096];
  
    /* for header */
    memset(buf,'\0',sizeof(buf));
    
    datafile.write((char*)&buf,sizeof(buf));
}

void writer::fileClose(){datafile.close();}

void writer::zerovoltages(rawvoltagedata* datain)
{
	float zero = 000000000000.000000000000;
	memset((char*)&datain->samples,int(FloatSwap(zero)),sizeof(datain->samples));
}

void writer::writePuMa2Block(rawvoltagedata* datain)
{

  samp_t blocksamples[SAMPLES];
  
  float BE_val = 0.0;
  float LE_val = 0.0;

///////////////////// STATION SUMMATION //////////////////////////////

  float statsum[SAMPLES][NPOL][2]={0.0,0.0,0.0,0.0};

  for(unsigned int isamp=0; isamp<SAMPLES; isamp++)
  {

	  for(unsigned int istat=selected_stations[0], isss=0; 
		                                              isss<selected_stations.size(); 
				                                                            istat=selected_stations[++isss])
  	  {     
		   BE_val = datain->samples[istat][the_chan][isamp][0][0];
		   LE_val = FloatSwap(BE_val);
		   statsum[isamp][0][0] += LE_val;

		   BE_val = datain->samples[istat][the_chan][isamp][0][1];
		   LE_val = FloatSwap(BE_val);
		   statsum[isamp][0][1] += LE_val;

		   BE_val = datain->samples[istat][the_chan][isamp][1][0];
		   LE_val = FloatSwap(BE_val);
		   statsum[isamp][1][0] += LE_val;

		   BE_val = datain->samples[istat][the_chan][isamp][1][1];
		   LE_val = FloatSwap(BE_val);
		   statsum[isamp][1][1] += LE_val;
	  }

   }

  

///////////////////////////////////////////////////////////////


  float ME_val = 0.0;
  float SD_val = 0.0;
  float DATA_val = 0.0;
  
///////////////////// STATISTICS //////////////////////////////
  float average[NPOL][2]={0.0,0.0,0.0};
  float sspread[NPOL][2]={0.0,0.0,0.0};

  float max00 = -1e10; float min00 = 1e10;
  float max01 = -1e10; float min01 = 1e10;
  float max10 = -1e10; float min10 = 1e10;
  float max11 = -1e10; float min11 = 1e10;

  for(unsigned int isamp=0; isamp<SAMPLES; isamp++)
  {

		 LE_val = statsum[isamp][0][0];
		 if(LE_val>max00) max00 = LE_val; 
		 if(LE_val<min00) min00 = LE_val; 
		 average[0][0] += LE_val/float(SAMPLES);

		 LE_val = statsum[isamp][0][1];
		 if(LE_val>max01) max01 = LE_val; 
		 if(LE_val<min01) min01 = LE_val; 
		 average[0][1] += LE_val/float(SAMPLES);

		 LE_val = statsum[isamp][1][0];
		 if(LE_val>max10) max10 = LE_val; 
		 if(LE_val<min10) min10 = LE_val; 
		 average[1][0] += LE_val/float(SAMPLES);

		 LE_val = statsum[isamp][1][1];
		 if(LE_val>max11) max11 = LE_val; 
		 if(LE_val<min11) min11 = LE_val; 
		 average[1][1] += LE_val/float(SAMPLES);

  }

  sspread[0][0] = max00-min00;
  sspread[0][1] = max01-min01;
  sspread[1][0] = max10-min10;
  sspread[1][1] = max11-min11;




////////////////////////////////////////////////////////////////
///////////////////// REPACKING AND SAVING //////////////////////////////

  unsigned int counter = 0;

  
  for(unsigned int isamp=0; isamp<SAMPLES; isamp++)
  {
    pow_I[isamp] = 0.0;
    pow_Q[isamp] = 0.0;
    pow_U[isamp] = 0.0;
    pow_V[isamp] = 0.0;
  }
  

  for(unsigned int isamp=0; isamp<SAMPLES; isamp++)
  {

		 LE_val = statsum[isamp][0][0];
		 ME_val = average[0][0];
		 SD_val = sspread[0][0];
		 DATA_val = 256*(LE_val-ME_val)/SD_val;
		 blocksamples[counter].Xr = (char)(DATA_val);

                 float xreal = DATA_val;

		 LE_val = statsum[isamp][0][1];
		 ME_val = average[0][1];
		 SD_val = sspread[0][1];
		 DATA_val = 256*(LE_val-ME_val)/SD_val;
		 blocksamples[counter].Xi = (char)(DATA_val);

                 float ximag = DATA_val;

		 LE_val = statsum[isamp][1][0];
		 ME_val = average[1][0];
		 SD_val = sspread[1][0];
		 DATA_val = 256*(LE_val-ME_val)/SD_val;
		 blocksamples[counter].Yr = (char)(DATA_val);

                 float yreal = DATA_val;

		 LE_val = statsum[isamp][1][1];
		 ME_val = average[1][1];
		 SD_val = sspread[1][1];
		 DATA_val = 256*(LE_val-ME_val)/SD_val;
		 blocksamples[counter].Yi = (char)(DATA_val);

                 float yimag = DATA_val;


                 counter++;
  }

  
  datafile.write((char*)&blocksamples, sizeof(blocksamples));

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


float FloatSwap( float f ){

  union{
  
  float f;
  unsigned char b[4];
  
  }dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];

  return dat2.f;
}



// obtaining file size
long getSize(char* filename) 
{
  long begin,end;
  ifstream myfile (filename, ios::binary);
  begin = myfile.tellg();
  myfile.seekg (0, ios::end);
  end = myfile.tellg();
  myfile.close();

  return end-begin;
}


/*int FileSize(const char* sFileName)
{
  std::ifstream f;
  f.open(sFileName, std::ios_base::binary | std::ios_base::in);
  if (!f.good() || f.eof() || !f.is_open()) { return 0; }
  f.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f.tellg();
  f.seekg(0, std::ios_base::end);
  return static_cast<int>(f.tellg() - begin_pos);
}
*/
