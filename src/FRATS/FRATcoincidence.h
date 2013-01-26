//#  FRATcoincidence.cc: Coincidence and subband scan for FRAT project
//#
//#  Copyright (C) 2009-2012
//#  Sander ter Veen (s.terveen@astro.ru.nl) 
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
#ifdef _OPENMP
#include <omp.h>
#endif

//# Common Includes
//#include <Common/LofarLogger.h>
//#include <Common/lofar_string.h>
//#include <Common/lofar_datetime.h>

#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>


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
#define FRAT_TRIGGER_PORT_1 (31661) // ports 0x7BA0 - 0x7BAF (31648-31663) are reserved for TBB triggers
#define DM_CONSTANT (4.148808e-3)
#define FRAT_HOSTNAME ("127.0.0.1") // localhost
//#define FRAT_HOSTNAME ("10.135.252.101") //central computer: lhn001

using namespace std;

// Define several structures used in the analysis

// used to store flagged channels NOTE: check if this is true
struct chanIdVal {
    int id;
    int val;
};

// trigger message from each band, transmitted to a coincidence program
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
    float SBaverage;
    float SBstdev;
    float Threshold;
    int nrFlaggedChannels;
    int nrFlaggedSamples;
    int width;
    int lastBadBlock;
};


// Message used to trigger the TBBs
struct TBBtriggerMessage {
    char Magic0;
    char Magic1;
    
    unsigned char utc_second[4];
    unsigned char utc_nanosecond[4];

    char MagicProject;
};

// swap endianness
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

// swap endianness of a float
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

// Convert subband to frequency, NOTE check how it works with middle of the band frequencies 
inline float SubbandToFreq(int subbandnr, int HBAmode=1, int samplefreq=200) {
    //returns frequency in GHz
	//takes samplefreq in MHz
	return subbandnr*samplefreq/1024000.0+(samplefreq/2000.0)*HBAmode;
}

// Calculates bandwidth of a single channel
inline float CalcFreqResolution(int channels, int frequencyIntegration=1, int samplefreq=200) {
    // input: * channels * channels per subband
    //        * frequencyIntegration *   how many frequency channels are integrated
    //                                   before the data is stored
    //        * samplefreq *   sample frequency in MHz (200 or 160 for LOFAR)
	return samplefreq/1024000.0/channels*frequencyIntegration;
}

// Calculates time resolution
inline float CalcTimeResolution(int channels,  int timeintegration=1, int samplefreq=200){
    // input: * channels * channels per subband
    //        * timeIntegration *   how many timesamples are integrated
    //                                   before the data is stored
    //        * samplefreq *   sample frequency in MHz (200 or 160 for LOFAR)
    return 1e-6/samplefreq*1024*channels*timeintegration;
}


// Sum Array over the specified axis
// NOTE: convert fully to pointers to make this faster?
// NOTE: I don't see it used in the code at the moment.
inline bool SumArray(float *array, float* summedaxis, int dimArray1, int dimArray2, int dimVector, uint sumaxis) {
    // * array *   pointer to two dimensional array
    // * summedaxis *  pointer to one dimension array used as output
    // * dimArray1 *   length of first dimension of the array
    // * dimArray2 *   length of the second dimension of the array
    // * sumaxis   *   sum 0th or 1st axis
    // returns if summing was a success
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
        CoinCheck();  // constructor
        ~CoinCheck(); // destructor

        // add a trigger to the buffer, returns index of last trigger
		unsigned int add2buffer(const triggerEvent& trigger); 
		
        // check if there is a coincidence with the last trigger added
		bool coincidenceCheck(unsigned int latestindex, unsigned int nChannles, int timeWindow);
        	
	    // Copy coincheck	
		CoinCheck(const CoinCheck&);
		CoinCheck& operator=(const CoinCheck&);
		
        std::string printEvents(int reftime, int timewindow);
    private:


        int itsNoCoincidenceChannels; // how many bands should trigger simultaneously
        double itsCoincidenceTime; // time difference allowed between triggers in different bands

      
        unsigned int itsNrTriggers;	// just for statistics
        bool   itsInitialized; // check if initialized
      
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

    }; // end CoinCheck
  }; // end coincidence
	namespace analysis { //FRAT::analysis
	 
      class UDPsend
         {
            public:
              ~UDPsend(); // destructor
              UDPsend(); // constructor
              // send a trigger message over udp
			  bool SendTriggerMessage(struct triggerEvent trigger);
            private:
              const char* send_data;
              struct sockaddr_in server_addr;
			  struct hostent *host;
			  const char* hostname;
              int sock;
         }; // UDP send


	  class SubbandTrigger // FRAT::analysis::SubbandTrigger 
		  {
		  public:	  
			  ~SubbandTrigger(); //destructor
              // constructors
              // old verstion
			  SubbandTrigger(int StreamID, int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, long startBlock=0, int IntegrationLength=1, bool InDoPadding=true, bool InUseSamplesOr2=true, bool verbose=false, bool doSend=false, int obsID=0, int beam=0);
              // current version that can deal with non-continuous frequency axis
              SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock, vector <int> IntegrationLengthVector, bool InDoPadding=true, bool InUseSamplesOr2=true, bool verbose=false, bool noSend=false, int obsID=0, int beam=0, int nrblocks=0);
              //SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock, std::vector<int> IntegrationLength, int nrblocks=0, bool InDoPadding=true, bool InUseSamplesOr2=true, bool verbose=false, bool noSend=false, int obsID=0, int beam=0); // NOTE: obsolete?

              // do dedispersion and triggering, now split in separate steps
			  bool processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              // dedisperse data for the DM of the constructor
			  bool dedisperseData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              // dedisperse data for the DM of the constructor
              // NOTE: what is the difference between 1 and 2?
			  bool dedisperseData2(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              // search for pulses
			  bool runTrigger(unsigned int sequenceNumber, int IntegrationLength, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              // calculate average and standard deviation of dedispersed data
			  bool calcAverageStddev(unsigned int sequenceNumber);
              // do dedispersion and triggering, now split in separate steps
              // NOTE: what is the difference between 1 and 2
              bool processData2(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime, bool Transposed=false);
              // Calculate how large the buffer for dedispersed data should be
			  int CalculateBufferSize();
              // Set number of bad samples and channels in the trigger message 
			  int setNrFlaggedChannels(int nrFlaggedChannels);
			  int setNrFlaggedSamples(int nrFlaggedSamples);
              // Calculate dedispersion offset of the different channels, based on settings in the SubbandTrigger
			  void InitDedispersionOffset();
              // Calculate dedispersion offset of the different channels from the FREQvalues.
              void InitDedispersionOffset(std::vector<float> FREQvalues);
              // summary of SubbbandTrigger parameters
			  void summary();
              // output dedispersed buffer to file
			  bool makeplotBuffer(std::string pulselogfilename);
              // output dedispersed buffer to file NOTE: check if this is true
              bool makeplotDedispBlock(ofstream *pulselogfile);
			  bool makeplotDedispBlock(std::string pulselogfilename);
              // summary of parameters for this block
			  std::string blockAnalysisSummary();
              // triggers found sofar
			  std::string FoundTriggers();
              // send UDP message with from this trigger
			  bool SendTriggerMessage(struct triggerEvent trigger);
              // write standard deviation and average from buffer to file
              bool writeStdDev(ofstream * fsfile);
              bool writeAverage(ofstream * fsfile);
              bool writeOffset(ofstream * fsfile);
              // set observation ID in triggermessage, NOTE check if this is true
              bool setObsid(int obsID);
              // dispersion measure used for dedispersing the data
              float itsDM;
              void setLastBadBlock(int block);
			  
		  private:

              
              FRAT::analysis::UDPsend* UDPtransmitter;
              
              bool noSend; // do not send messages 
			  int itsNrChannels; // channels per band (NOTE check this)
              int itsTotNrChannels; // channels per subband x nof(subbands)
			  int itsNrSamples; // samples per block
			  int itsNrSamplesOr2; // for old data
              int itsChannelsPerSubband; // channels per subband
              int itsStartChannel; // start channel for this block, relative to data start
              unsigned long int itsStarttime_utc_sec; // start time of observation, second part
              unsigned long int itsStarttime_utc_ns; // start time of observation, nanosecond part
			  
			  int itsIntegrationLength; // find pulses with a sliding window of this length 
			  float itsTriggerLevel; // number of stdev above noise
			  float itsStartFreq; // in GHz
			  float itsFreqResolution;  // in GHz
			  float itsTimeResolution; // in s
			  float itsReferenceFreq; // compared to which frequency is data move
			  int itsReferenceTime; // relative time to the reference frequency
              
			  int itsSequenceNumber; // block number
			  int itsBlockNumber; // block number

			  //int itsStartBlock;
			  std::vector<float> DeDispersedBuffer; // ringbuffer of dedispersed data, also with partial sum 
              // over the channels
			  std::vector<int> dedispersionoffset; //offset of each channel w.r.t. the lowest frequency
			  std::vector <float> DeDispersed; // ringbuffer of dedispersed data. Only with full dedispersed data
              std::vector <float> SumDeDispersed; // not used at the moment? NOTE
              std::vector <struct triggerEvent> triggerMessages; // stores trigger messages
              std::vector<int> integrationLength; // values for different sliding window length for the trigger
              int minBlockNumber; // only send triggers for blocks later than this number, to prevent sending false triggers due to partially summed data leading to low averages.
			  int itsBufferLength; // length of DeDispersedBuffer
			  bool DoPadding; // Current task: swap endianness of data, if True
			  bool UseSamplesOr2; // Old datastructure used SAMPLES|2 as dimensions
			  float itsSBaverage; // average of the previous block of the dedispersed data
			  float itsSBstdev; // stddev of the previous block of the dedispersed data
              vector <float> itsSBaverageVector; // averages for all blocks
              vector <float> itsSBstdevVector; // stddev for all blocks
              int itsNrblocks; // total blocks to analyse
			  int itsTotalValidSamples; // non-flagged samples
			  float itsTriggerThreshold; //send trigger if value is higher than this threshold, calculated from previous block (NOTE check this)
			  float itsPrevTriggerLevel; //value to trigger on
			  long itsTotalZeros; // count zeros in each block, NOTE not done anymore, but maybe we should?
			  bool verbose; // verbose output ( NOTE: needs better implementation )
			  struct triggerEvent trigger; // single trigger message, re-used during analysis
			  int itsStreamID; // which band is analysed?
              // network socket data. Now put into UDPsend? NOTE check if this is still used
			  int sock; 
              struct sockaddr_in server_addr;
			  struct hostent *host;
			  //char send_data[1024];
			  const char* send_data;
			  const char* hostname;
                
              std::vector<float> itsFREQvalues; // values of the frequency axis
			  
			  std::string itsFoundTriggers; // string with data of triggers that our found. NOTE: obsolete?
             
              // used in process data 
              float value; // single sample
              unsigned long int totaltime; // time since start
              int rest; // index in buffer
              
              float blocksum; // sum over samples in a complete block
              int validsamples; // samples that are non-zero/ not flagged
              int zerocounter; // how many zeros in the block?
              float SBaverage; // used to calculate average of the block, then later stored in itsSBaverage
              float SBstdev; // used to calculate stddev of the block, then later stored in itsSBstdev
              int SBsumsamples; // 
              int channeltimeindex; // total index in one-dimensional representation of data 
              int channelindex; // channel index
              
			  		  
		  }; //SubbandTrigger


        // Class used to clean a two dimensional array from interference along the time and frequency axis
        class RFIcleaning {
            public:
                ~RFIcleaning(); // destructor
                RFIcleaning(float* data_start, float* data_end, int nrchannels, int chanspersubband, int nrsamples, float cutlevel, int number_of_divisions, std::string outFileName); // constructor
                /*
                    * data_start * , pointer to start of data array
                    * data_end *,    pointer to after the data array
                    * nrchannels *,  total amount of channels in this data array
                    * chanspersubband *, channels for each subband
                    * nrsamples *,       samples in this data block
                    * cutlevel *,        threshold for claiming something is RFI
                    * number_of_division *, into how many parts should the band be divided trigger on time domain RFI
                    * outFileName *,        for logging purposes? NOTE check this



                */
                int itsNrChannels; // total number of channels
                int itsNrSamples; // total number of samples
                vector <float> baseline; // collapse over time
                vector <float> timeseries; // collapse over channels
                vector <float> avgtimeseries; 
                vector <float> avgtimeseriesstream; 
                vector <float> sqrTimeseries;
                vector <float> IPbaseline;
                vector <float> sqrBaseline; // sum over time of samples^2 for each channel
                vector <chanIdVal> badChans; // channels that contain RFI
                vector <float> sqrDivBaseline;  // sqrBaseline / Baseline
                vector <float> sqrDivBaselineSort; // sorted array of the previous one
                vector <int> badSamples; // samples that contain RFI
                // several iterators used throughout to save time creating time every time.
                // care should be taken by this way of programming it
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
                bool calcBaseline(); // sum over time of the sample for each channel
                bool calcSqrBaseline(); // sum over time of samples^2 for each channel
                int calcBadChannels(int cutlevel, bool useInterpolatedBaseline); // True if SqrBaseline/Baseline > average+cutlevel*stddev for a certain frequencychannel
                bool checkDataloss(int requiredsamples);
                int calcBadSamples(int cutlevel); // True if 
                bool cleanChannels(std::string method); // replace bad channels
                int cleanChannel0(std::string method); // replace 0th channel of each subband/ return number of 0th channels that are cleaned
                bool cleanSamples(std::string method); // replace bad samples
                bool calcTimeseries(); // sum over channels for each timestep
                bool calcAverageTimeseries(); // sum over channels for each timestep divided by number of channels
                bool calcAverageTimeseriesStream(int startchan, int endchan); // sum over channels for each timestep divided by number of channels
                bool averageTimeseries(); // calculate average timeseries by dividing the timeseries by itsNrChannels
                bool calcSqrTimeseries(); // sum over channels of samples^2 for each timestep
                bool interpolateBaseline(); // not yet implemented
                bool printBadSamples(); //print content of badSamples
                bool printBadChannels(); // print content of badChannels
                bool printData(); // print complete data
                bool printBaseline(bool printindex); // print baseline with or without channelindex
                bool printTimeseries(bool printindex); // print timeseries with or without sampleindex
                bool printAverageTimeseries(bool printindex); // print average timeseries with or without sampleindex
                bool printAverageTimeseriesStream(bool printindex); // print average timeseries with or without sampleindex

                bool printSqrTimeseries(bool printindex); // print sqrTimeseries with or without sampleindex
                bool divideBaseline(bool useInterPolatedBaseline); // divide data at each sample by the baseline
                bool subtractAverageTimeseries(); // subtract average timeseries, ZeroDMing
                bool subtractAverageTimeseriesStream(int startchan, int endchan); // subtract average timeseries, ZeroDMing
                bool writeBadSamples(ofstream * fsfile, int blockNr); // write bad samples to file
                bool writeBadChannels(ofstream * fcfile, int blockNr); // write bad channels to file
                bool writeBaseline(ofstream * blfile, int blockNr); // write baseline to file
                bool writeSqrTimeseries(ofstream *stfile, int blockNr); // write square of timeseries to file


            private:
                float* itsDataStart; // start of data
                float* itsDataEnd; // end of data
                int nrOfDivisions; // of the baseline (if it's noncontinuous)
                int ChannelsPerDivision; // channels in each division
                int cutlevel; // threshold for determining RFI
                int itsChansPerSubband; // channels in each subband, used for flagging 0-th channel


       
        }; // RFIcleaning

  };//StationCU
};//LOFAR
#endif
