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

#define CHANNELS 16
#define SUBBANDS 32
#define SAMPLES 12208
#define NPOL 2

#define CLOCKRES 0.1953125


using namespace std;

long getSize(const char*); 
void swap_endian(char*);
float FloatSwap(char c[4]);
complex<float> convert_complex(complex<float>);
//int FileSize(const char*);

struct rawvoltagedata {
       /* big endian (ppc/sparc) */
       unsigned int  sequence_number;
       char pad[508];
        /* big endian (ppc/sparc) */
       char         samples[SAMPLES|2][SUBBANDS][CHANNELS][2][4];
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
  void filesInit();
  void filesClose();
  void writePuMa2Block(rawvoltagedata*,rawvoltagedata*);
  void readParset();
  void zerovoltages(rawvoltagedata*);

  void passFilenames(char* &, char* &, char* &);
  void passSub(unsigned int &);
  void passChan(unsigned int &);


  float pow_I[SAMPLES];
  float pow_Q[SAMPLES];
  float pow_U[SAMPLES];
  float pow_V[SAMPLES];

  int NCHANNELS;
  int SUBBSTART;
  int SUBBEND;
  int NSUBBANDS;

  string DATE;

private:
  ofstream datafile[CHANNELS];
  void writeHeader(unsigned int &);
  char* _basename;
  char* _hdrFilename;
  char* _psetFilename;
  unsigned int _the_chan;  
  unsigned int _the_sub;  
  string getKeyVal(string &);
  double mjdcalc();  
};



int main(int argc, char** argv)
{


  if(argc==1)
  {
  	cout <<endl<<endl
	     << "Usage: <executable> -f <input file> -h <header file> -p <parset file>  [OPTIONS]"  
	     <<endl<<endl;
	
	
	return 0;
  }

  rawvoltagedata* bfdataX = (rawvoltagedata*) malloc(sizeof(rawvoltagedata));
  rawvoltagedata* bfdataY = (rawvoltagedata*) malloc(sizeof(rawvoltagedata));
  rawvoltagedata* padded = (rawvoltagedata*) malloc(sizeof(rawvoltagedata));

  char* basename;
  char* hdrFilename;
  char* psetFilename;
  for(int ia=0; ia<argc; ia++)
  {

  	if(string(argv[ia])=="-f")
	{
                basename = argv[ia+1];
	        continue;
	}	        
  	if(string(argv[ia])=="-h")
	{
                hdrFilename = argv[ia+1];
	        continue;
	}	        
  	if(string(argv[ia])=="-p")
	{
                psetFilename = argv[ia+1];
	        continue;
	}	        


  }
  
  cout << "input = " << basename <<endl;
  cout << "header = " << hdrFilename <<endl;
  cout << "parset = " << psetFilename <<endl;

  string filenameX_str = string(basename)+"_S0_P000_bf.raw";
  string filenameY_str = string(basename)+"_S1_P000_bf.raw";
  
  const char* filenameX = filenameX_str.c_str();
  const char* filenameY = filenameY_str.c_str();


  
  if (bfdataX==NULL || bfdataY==NULL) {fputs ("Memory fail",stderr); exit (1);}

  FILE * pfileX;
  FILE * pfileY;

  
  pfileX = fopen ( filenameX , "rb" );
  if (pfileX==NULL) {fputs ("File error",stderr); exit (1);}

  pfileY = fopen ( filenameY , "rb" );
  if (pfileY==NULL) {fputs ("File error",stderr); exit (1);}


  


  writer puma2data;
  
  puma2data.passFilenames(basename,hdrFilename, psetFilename);
  puma2data.zerovoltages(padded);

  puma2data.readParset();

  cout << "SUBBANDS = " << puma2data.NSUBBANDS <<endl;
  cout << "CHANNELS = " << puma2data.NCHANNELS <<endl;

  for(unsigned int isub=0; isub<puma2data.NSUBBANDS; isub++)
  {

	 puma2data.passSub(isub);

	 rewind(pfileX);
	 rewind(pfileY);

	 puma2data.filesInit();

         int iblock = 0;
         bool runout = false;
         
	 while(1)
	 {
		
	        if(runout) 
		{ cout << " *** Read " << iblock << " blocks." <<endl; 
		  break;
                }
		
		size_t num = fread(bfdataX, sizeof(rawvoltagedata), 1, pfileX); if(num == 0) runout = true;


		swap_endian((char*)(&bfdataX->sequence_number));
		unsigned int seqnoX = bfdataX->sequence_number;


		num = fread(bfdataY, sizeof(rawvoltagedata), 1, pfileY); if(num == 0) runout = true;


		swap_endian((char*)(&bfdataY->sequence_number));
		unsigned int seqnoY = bfdataY->sequence_number;

                cerr <<setw(10)<< "IBLOCK = " <<setw(10)<< iblock <<setw(10)<< "   SEQNO_X = " << seqnoX 
                                                                  <<setw(10)<< " / SEQNO_Y = " << seqnoY <<endl;
  	        unsigned int nskip = 0;

                if(seqnoX == seqnoY)
		{
	            if(runout) break;

		    while(iblock < seqnoX)
		    {

				  cerr << "iblock < seqno" <<endl;

				  cerr <<"("<< iblock <<")"; 
				  nskip++;
	                	  iblock++;

		    }
          
                }
		while(seqnoX != seqnoY)
		{
	            if(runout) break;

 		    while(seqnoX < seqnoY)
		    {
                      if(runout) break;

		      cerr << "seqnoX < seqnoY" <<endl;

		      while(iblock < seqnoY)
		      {
			            
				    cerr << "iblock < seqnoY" <<endl;

				    cerr <<"("<< iblock <<")"; 

				    nskip++;
	                	    iblock++;

		      }

		      num = fread(bfdataX, sizeof(rawvoltagedata), 1, pfileX); if(num == 0) runout = true;

		      swap_endian((char*)(&bfdataX->sequence_number));
		      unsigned int seqnoX = bfdataX->sequence_number;

                    }

 		    while(seqnoY < seqnoX)
		    {
	              if(runout) break;
		      
		      cerr << "seqnoY < seqnoX" <<endl;

		      while(iblock < seqnoX)
		      {
			            
				    cerr << "iblock < seqnoX" <<endl;

				    cerr <<"("<< iblock <<")"; 

                                    nskip++;
	                	    iblock++;

		      }

		      num = fread(bfdataY, sizeof(rawvoltagedata), 1, pfileY); if(num == 0) runout = true;
		      
		      swap_endian((char*)(&bfdataY->sequence_number));
		      unsigned int seqnoY = bfdataY->sequence_number;

                    }

		}

//START WRITING
                cerr << "CHANNELS:";
		for(unsigned int ichan=0; ichan<puma2data.NCHANNELS; ichan++)
		{


                      puma2data.passChan(ichan);

		      cerr << ichan+1;


		      // WRITE ZERO IN ALL EMPTY BLOCKS
		      
		      for(unsigned int iskip=0; iskip<nskip; iskip++)
			            puma2data.writePuMa2Block(padded,padded);

		      puma2data.writePuMa2Block(bfdataX,bfdataY);

		}
		cerr <<endl;
		
		iblock++;

         }
	 puma2data.filesClose();

  }


  return 0;

}

void writer::passFilenames(char* &base, char* &hdr, char* &pset)
{
    _basename = base;
    _hdrFilename = hdr;
    _psetFilename = pset;
}

void writer::passSub(unsigned int &sub)
{
    _the_sub = sub;  
}

void writer::passChan(unsigned int &chan)
{
    _the_chan = chan;  
}


void writer::readParset()
{

   
   string tmp = "Observation.channelsPerSubband";

   NCHANNELS = atoi(getKeyVal(tmp).c_str());
   cout << " CHANNELS PER SUBBAND = " << NCHANNELS <<endl;

   string mychar = "";
   int ic = 1;
   while(1)
   {
     tmp = "Observation.subbandList";
     mychar = getKeyVal(tmp).substr(1,ic);
     cout << mychar <<endl;
     
     if(mychar.substr(ic-1,1)==".") break;
     
     ic++;
   }
   SUBBSTART = atoi(mychar.c_str());
   
   cerr << "Got SUBBSTART = " << SUBBSTART <<endl;
   
   mychar = "";
   int nc = ic+2;
   ic = 1;
   while(1)
   {
     tmp = "Observation.subbandList";
     mychar = getKeyVal(tmp).substr(nc,ic);

     if(mychar.substr(ic-1,1)=="]") break;

     ic++;
   }
   SUBBEND = atoi(mychar.c_str());

   cerr << "Got SUBBEND = " << SUBBEND <<endl;


   cout << " SUBBAND RANGE = [" << SUBBSTART << "--" << SUBBEND <<"]" <<endl;

   NSUBBANDS = SUBBEND - SUBBSTART + 1;

   tmp="Observation.startTime";
   DATE = getKeyVal(tmp).c_str();

   mjdcalc();

   cout << "DATE = " << DATE <<endl;
}

void writer::filesInit()
{
    for(unsigned int ichan=0; ichan<NCHANNELS; ichan++)
    {
 
       stringstream the_sub_sstrm;
       the_sub_sstrm << _the_sub;    

       stringstream the_chan_sstrm;
       the_chan_sstrm << ichan;    

       string basename_str = _basename;
       string filename_str = basename_str + "_SB" + the_sub_sstrm.str() + "_CH" + the_chan_sstrm.str();

       datafile[ichan].open(filename_str.c_str(), ios::out | ios::binary);
       
       char buf[4096];

       /* for header */
       memset(buf,'\0',sizeof(buf));

       datafile[ichan].write((char*)&buf,sizeof(buf));

       writeHeader(ichan);

    }

}

void writer::filesClose(){for(unsigned int ichan=0; ichan<NCHANNELS; ichan++) datafile[ichan].close();}

void writer::zerovoltages(rawvoltagedata* datain)
{
	char zero[4] = {0,0,0,0};
	memset((char*)&datain->samples,int(FloatSwap(zero)),sizeof(datain->samples));
}

void writer::writePuMa2Block(rawvoltagedata* datainX, rawvoltagedata* datainY)
{

  samp_t blocksamples[SAMPLES];
  
  char BE_val[4] = {0,0,0,0};
  float LE_val = 0.0;


  float voltage[SAMPLES][NPOL][2]={0.0,0.0,0.0,0.0};

  for(unsigned int isamp=0; isamp<SAMPLES; isamp++)
  {

      for(int ic=0; ic<4; ic++) BE_val[ic] = datainX->samples[isamp][_the_sub][_the_chan][0][ic];
      LE_val = FloatSwap(BE_val);
      voltage[isamp][0][0] = LE_val;

      for(int ic=0; ic<4; ic++) BE_val[ic] = datainX->samples[isamp][_the_sub][_the_chan][1][ic];
      LE_val = FloatSwap(BE_val);
      voltage[isamp][0][1] = LE_val;

      for(int ic=0; ic<4; ic++) BE_val[ic] = datainY->samples[isamp][_the_sub][_the_chan][0][ic];
      LE_val = FloatSwap(BE_val);
      voltage[isamp][1][0] = LE_val;

      for(int ic=0; ic<4; ic++) BE_val[ic] = datainY->samples[isamp][_the_sub][_the_chan][1][ic];
      LE_val = FloatSwap(BE_val);
      voltage[isamp][1][1] = LE_val;

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

		 LE_val = voltage[isamp][0][0];
		 if(LE_val>max00) max00 = LE_val; 
		 if(LE_val<min00) min00 = LE_val; 
		 average[0][0] += LE_val/float(SAMPLES);

		 LE_val = voltage[isamp][0][1];
		 if(LE_val>max01) max01 = LE_val; 
		 if(LE_val<min01) min01 = LE_val; 
		 average[0][1] += LE_val/float(SAMPLES);

		 LE_val = voltage[isamp][1][0];
		 if(LE_val>max10) max10 = LE_val; 
		 if(LE_val<min10) min10 = LE_val; 
		 average[1][0] += LE_val/float(SAMPLES);

		 LE_val = voltage[isamp][1][1];
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

		 LE_val = voltage[isamp][0][0];
		 ME_val = average[0][0];
		 SD_val = sspread[0][0];
		 DATA_val = 256*(LE_val-ME_val)/SD_val; if(DATA_val>128.0) DATA_val = 127.0;
		 blocksamples[counter].Xr = (char)(DATA_val);

                 float xreal = DATA_val;

		 LE_val = voltage[isamp][0][1];
		 ME_val = average[0][1];
		 SD_val = sspread[0][1];
		 DATA_val = 256*(LE_val-ME_val)/SD_val; if(DATA_val>128.0) DATA_val = 127.0;
		 blocksamples[counter].Xi = (char)(DATA_val);

                 float ximag = DATA_val;

		 LE_val = voltage[isamp][1][0];
		 ME_val = average[1][0];
		 SD_val = sspread[1][0];
		 DATA_val = 256*(LE_val-ME_val)/SD_val; if(DATA_val>128.0) DATA_val = 127.0;
		 blocksamples[counter].Yr = (char)(DATA_val);

                 float yreal = DATA_val;

		 LE_val = voltage[isamp][1][1];
		 ME_val = average[1][1];
		 SD_val = sspread[1][1];
		 DATA_val = 256*(LE_val-ME_val)/SD_val; if(DATA_val>128.0) DATA_val = 127.0;
		 blocksamples[counter].Yi = (char)(DATA_val);

                 float yimag = DATA_val;

/*                 if(fabs(LE_val)>1e-9 && DATA_val>127.0) cerr <<endl<<setw(8)<<"LE_val = " <<setw(8)<< LE_val 
		                        <<setw(8)<<"  DATA_val = " <<setw(8)<< DATA_val 
					 <<setw(8)<< " ---> " 
					 <<setw(8)<< static_cast<int>(blocksamples[counter].Yi)
					 << endl;
*/		
                 counter++;
  }

  
  datafile[_the_chan].write((char*)&blocksamples, sizeof(blocksamples));

}


void writer::writeHeader(unsigned int &ichan)
{

//Observation.subbandList = [200..261]
//Observation.channelsPerSubband = 16

        datafile[ichan].seekp(0, ios::beg);

	string line;
        stringstream the_freq,the_mjd;
        ifstream headerfile;
	headerfile.open(_hdrFilename);
	
	while(1)
	{
	   getline(headerfile,line,'\n');

           if(line.substr(0,4)=="FREQ") 
           {                                
              the_freq <<setprecision(20)<< (100.0+SUBBSTART*CLOCKRES)+(_the_sub*NCHANNELS+ichan)*(CLOCKRES/NCHANNELS);
              
	      line="FREQ " + the_freq.str();
         
              cout << line <<endl;
           }
           else
           if(line.substr(0,9)=="MJD_START")
           {

              the_mjd <<setprecision(20) << mjdcalc();

              line="MJD_START " + the_mjd.str();

              cout << line <<endl;
           }
           else
           if(line.substr(0,9)=="UTC_START")
           {

              line="UTC_START " + DATE.substr(1,19);

              cout << line <<endl;
           }


	   datafile[ichan] << line <<endl;
	   if(headerfile.eof()) break;
	
	}
	
	headerfile.close();
	
        datafile[ichan].seekp(0, ios::end);
        	

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


float FloatSwap( char a[4] ){

  union{
  
    float f;
    char b[4];
  
  }dat1, dat2;

  for(int ic=0; ic<4; ic++) dat1.b[ic] = a[ic];
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];

  return dat2.f;
}



// obtaining file size
long getSize(const char* filename) 
{
  long begin,end;
  ifstream myfile (filename, ios::binary);
  begin = myfile.tellg();
  myfile.seekg (0, ios::end);
  end = myfile.tellg();
  myfile.close();

  return end-begin;
}



string writer::getKeyVal(string &key)
{

	string line;
	int start;
	string value;

        ifstream parsetfile;
	parsetfile.open(_psetFilename);
	
	while(1)
	{
	  getline(parsetfile,line,'\n');

          if(parsetfile.eof()) break;
	  
	  for(int i = line.find(key, 0); i != string::npos; i = line.find(key, i))
	  {
    		  

		  istringstream ostr(line);
		  istream_iterator<string> it(ostr);
		  istream_iterator<string> end;

		  size_t nwords = 0;
		  while (it++ != end) nwords++;

		  
		  string temp;
		  istringstream words(line);
		  vector<string> parsed;
		  while(words)
		  {
        	      words >> temp;
		      if(parsed.size()>=nwords) break;
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

double writer::mjdcalc()
{
  
  int yy = atoi(DATE.substr(1,4).c_str());
  int mm = atoi(DATE.substr(6,2).c_str());
  int dd = atoi(DATE.substr(9,2).c_str());

  int hh = atoi(DATE.substr(12,2).c_str());
  int mi = atoi(DATE.substr(15,2).c_str());
  int ss = atoi(DATE.substr(18,2).c_str());

  cout << " DATE = " <<setw(20) << DATE <<endl;  
  cout << " READ = " <<setw(6) << yy <<setw(6) << mm<<setw(6) << dd<<setw(6) << hh<<setw(6) << mi<<setw(6) << ss<<endl;

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

    cout << "MJD0=" <<mjd0<<endl;

    return mjd0;
}

