//#  FRATcoincidence.cc: Coincidence and subband scan for FRAT project
//#
//#  Copyright (C) 2007
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: FRATcoincidence.cc 2010-05-06 14:22:43Z veen $

#ifndef FRAT_COINCIDENCE_H
#define FRAT_COINCIDENCE_H

//# Common Includes
//#include <Common/LofarLogger.h>
//#include <Common/lofar_string.h>
//#include <Common/lofar_datetime.h>

//# local includes
//#include "TBBTrigger.h"
//#include "TBBReadCmd.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
// forward declaration
#define FRAT_TASK_BUFFER_LENGTH (20000)
#define FRAT_TRIGGER_PORT_0 (0x7BA0)
#define FRAT_TRIGGER_PORT_1 (31661)
#define DM_CONSTANT (4.148808e-3)
#define FRAT_HOSTNAME ("127.0.0.1")
//#define FRAT_HOSTNAME ("10.135.252.101")

using namespace std;



struct chanIdVal {
    int id;
    int val;
};

struct triggerEvent {
	unsigned long int time;
    unsigned long int utc_second;
    unsigned long int utc_nanosecond;
	int length;	
	int sample;
	int block;
	int subband;
	float sum;
	float max;
	int obsID;
    int beam;
	float DM;
};

struct TBBtriggerMessage {
    char Magic0;
    char Magic1;
    
    unsigned char utc_second[4];
    unsigned char utc_nanosecond[4];

    char MagicProject;
};

inline void swap_endian( char *x )
{
	char c;
	
	c = x[0];
	x[0] = x[3];
	x[3] = c;
	
	
	c = x[1];
	x[1] = x[2];
	x[2] = c;
}

inline float FloatSwap( float f )
{
	union
	{
		float f;
		unsigned char b[4];
	} dat1, dat2;
	
	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

inline float SubbandToFreq(int subbandnr, int HBAmode=1, int samplefreq=200) {
    //returns frequency in GHz
	//takes samplefreq in MHz
	return subbandnr*samplefreq/1024000.0+(samplefreq/2000.0)*HBAmode;
}

inline float CalcFreqResolution(int channels, int frequencyIntegration=1, int samplefreq=200) {
	return samplefreq/1024000.0/channels*frequencyIntegration;
}

inline float CalcTimeResolution(int channels,  int timeintegration=1, int samplefreq=200){
    return 1e-6/samplefreq*1024*channels*timeintegration;
}

inline bool SumArray(float *array, float* summedaxis, int dimArray1, int dimArray2, int dimVector, uint sumaxis) {
    if(sumaxis==0){
        if(dimArray1!=dimVector){
            cerr << "You are trying to sum the wrong axis!" << endl;
            return false;
        }
        for(int i=1;i<dimArray1;i++){
            for(int j=1;j<dimArray2;j++){
                summedaxis[i]+=array[i*dimArray2+j];
            }
        }
    } else if(sumaxis==1){
        if(dimArray2!=dimVector){
            cerr << "You are trying to sum the wrong axis!" << endl;
            return false;
        }
        for(int i=1;i<dimArray1;i++){
            for(int j=1;j<dimArray2;j++){
                summedaxis[j]+=array[i*dimArray2+j];
            }
        }
        
    } else {
        cerr << "Axis should be 0 or 1 in SumArray";
        return false; 
    }
    return true;
}


namespace FRAT {
  namespace coincidence {
    
    class CoinCheck
    {
    public:
      CoinCheck();
      ~CoinCheck();
      
      // define responsefunctionType
      //void readTBBdata(std::vector<TBBReadCmd>	cmdVector);
      //void addTrigger(const TBBTrigger&	trigger);
     
		
		unsigned int add2buffer(const triggerEvent& trigger);
		
		bool coincidenceCheck(unsigned int latestindex, unsigned int nChannles, int timeWindow);	
		
		CoinCheck(const CoinCheck&);
		CoinCheck& operator=(const CoinCheck&);
		
        std::string printEvents(int reftime, int timewindow);
    private:


      int itsNoCoincidenceChannels;
      double itsCoincidenceTime;

      // avoid defaultconstruction and copying

      
      // remote function to call for saving triggers
      unsigned int itsNrTriggers;	// just for statistics
      bool   itsInitialized;
     // std::vector<TBBReadCmd> itsCommandVector;    // used as temporarely buffer
      

      // Buffer for trigger messages

      // Single element in the buffer
      struct triggerBuffElem {
	  unsigned int	next;
	  unsigned int	prev;	
      unsigned int	SBnr; //RcuNr;
	  //unsigned int	SeqNr;
	  unsigned long int	Time;
	  unsigned int	SampleNr;
	  float	Sum;
	  unsigned int	NrSamples;
	  float	PeakValue;
	  double  date;
	  unsigned int	meanval;
	  unsigned int	afterval;
	};
		

		
		


      // All buffered triggers
      struct triggerBuffElem trigBuffer[FRAT_TASK_BUFFER_LENGTH];

      // Index of first and last element in buffer
      unsigned int first, last;
	
      // Insert element into buffer, returns index of inserted element


    };
  };
	namespace analysis {
	 
      class UDPsend
         {
            public:
              ~UDPsend();
              UDPsend();
			  bool SendTriggerMessage(struct triggerEvent trigger);
            private:
              const char* send_data;
              struct sockaddr_in server_addr;
			  struct hostent *host;
			  const char* hostname;
              int sock;
         };


	  class SubbandTrigger 
		  {
		  public:	  
			  //SubbandTrigger();
			  ~SubbandTrigger();
			  SubbandTrigger(int StreamID, int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, long startBlock=0, int IntegrationLength=1, bool InDoPadding=true, bool InUseSamplesOr2=true, bool verbose=false, bool doSend=false, int obsID=0, int beam=0);
              SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock=0, int IntegrationLength=1, int nrblocks=0, bool InDoPadding=true, bool InUseSamplesOr2=true, bool verbose=false, bool noSend=false, int obsID=0, int beam=0);
			  bool processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
			  bool dedisperseData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              bool processData2(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
			  int CalculateBufferSize();
			  void InitDedispersionOffset();
              void InitDedispersionOffset(std::vector<float> FREQvalues);
			  void summary();
			  bool makeplotBuffer(std::string pulselogfilename);
              bool makeplotDedispBlock(ofstream *pulselogfile);
			  bool makeplotDedispBlock(std::string pulselogfilename);
			  std::string blockAnalysisSummary();
			  std::string FoundTriggers();
			  bool SendTriggerMessage(struct triggerEvent trigger);
              bool writeStdDev(ofstream * fsfile);
              bool writeAverage(ofstream * fsfile);

              float itsDM;
			  
		  private:

              
              FRAT::analysis::UDPsend* UDPtransmitter;
              
              bool noSend; 
			  int itsNrChannels;
              int itsTotNrChannels;
			  int itsNrSamples;
			  int itsNrSamplesOr2;
              int itsChannelsPerSubband;
              int itsStartChannel;
              unsigned long int itsStarttime_utc_sec;
              unsigned long int itsStarttime_utc_ns;
			  
			  int itsIntegrationLength;
			  float itsTriggerLevel; // number of stdev above noise
			  float itsStartFreq; // in GHz
			  float itsFreqResolution;  // in GHz
			  float itsTimeResolution; // in s
			  float itsReferenceFreq;
			  int itsReferenceTime;
              
			  unsigned int itsSequenceNumber;
			  int itsBlockNumber;
			  //int itsStartBlock;
			  std::vector<float> DeDispersedBuffer;
			  std::vector<int> dedispersionoffset;
			  std::vector <float> SumDeDispersed;
              std::vector <struct triggerEvent> triggerMessages;
			  int itsBufferLength;
			  bool DoPadding;
			  bool UseSamplesOr2;
			  float itsSBaverage;
			  float itsSBstdev;
              vector <float> itsSBaverageVector;
              vector <float> itsSBstdevVector;
              int itsNrblocks;
			  int itsTotalValidSamples;
			  float itsTriggerThreshold; //value to trigger on
			  float itsPrevTriggerLevel; //value to trigger on
			  long itsTotalZeros;
			  bool verbose;
			  struct triggerEvent trigger;
			  int itsStreamID;
			  int sock;
              struct sockaddr_in server_addr;
			  struct hostent *host;
			  //char send_data[1024];
			  const char* send_data;
			  const char* hostname;
              std::vector<float> itsFREQvalues;
			  
			  std::string itsFoundTriggers;
              /*
              float blocksum;
              int validsamples;
              float SBaverage;
              float SBstdev;
              int SBsumsamples;
              int channeltimeindex;
              int channelindex;
               */
              // Used in processData
              
              float value;
              unsigned long int totaltime;
              int rest;
              
              float blocksum;
              int validsamples;
              int zerocounter;
              float SBaverage;
              float SBstdev;
              int SBsumsamples;
              int channeltimeindex;
              int channelindex;
              
			  		  
		  }; //SubbandTrigger


        class RFIcleaning {
            public:
                ~RFIcleaning();
                RFIcleaning(float* data_start, float* data_end, int nrchannels, int chanspersubband, int nrsamples, float cutlevel, int number_of_divisions, std::string outFileName);
                int itsNrChannels;
                int itsNrSamples;
                vector <float> baseline;
                vector <float> IPbaseline;
                vector <float> sqrBaseline;
                vector <chanIdVal> badChans;
                vector <float> sqrDivBaseline; 
                vector <float> sqrDivBaselineSort;
                vector <int> badSamples;
                vector<float>::iterator it1;
                vector<float>::iterator it2;
                vector<float>::iterator it3;
                vector<float>::iterator it4;
                vector<float>::iterator vec1_end;
                vector<float>::iterator vec2_end;
                vector<chanIdVal>::iterator chit;
                
                vector<int>::iterator iit1;
                vector<int>::iterator iit2;
                vector<int>::iterator iit3;
                int size;

                string outFileName;
                bool calcBaseline();
                bool calcSqrBaseline();
                bool calcBadChannels(int cutlevel, bool useInterpolatedBaseline);
                bool calcBadSamples(int cutlevel);
                bool cleanChannels(std::string method);
                bool cleanChannel0(std::string method);
                bool cleanSamples(std::string method);
                bool interpolateBaseline();
                bool printBadSamples();
                bool printBadChannels();
                bool printBaseline();
                bool divideBaseline(bool useInterPolatedBaseline);
                bool writeBadSamples(ofstream * fsfile, int blockNr);
                bool writeBadChannels(ofstream * fcfile, int blockNr);
                bool writeBaseline(ofstream * blfile, int blockNr);


            private:
                float* itsDataStart;
                float* itsDataEnd;
                int nrOfDivisions; // of the baseline (if it's noncontinuous)
                int ChannelsPerDivision;
                int cutlevel;
                int itsChansPerSubband;


       
        }; // RFIcleaning

  };//StationCU
};//LOFAR
#endif
