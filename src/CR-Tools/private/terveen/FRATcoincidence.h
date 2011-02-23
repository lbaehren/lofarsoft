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

#ifndef STATIONCU_TBBCONTROL_VHECRTASK_H
#define STATIONCU_TBBCONTROL_VHECRTASK_H

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
// forward declaration
#define FRAT_TASK_BUFFER_LENGTH (20000)

struct triggerEvent {
	int time;
	int length;	
	int sample;
	int block;
	int subband;
	float sum;
	float max;
};

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

float FloatSwap( float f )
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

float SubbandToFreq(int subbandnr, int HBAmode=1, int samplefreq=200) {
    //returns frequency in GHz
	//takes samplefreq in MHz
	return subbandnr*samplefreq/1024000.0+(samplefreq/2000.0)*HBAmode;
}

float CalcFreqResolution(int channels, int frequencyIntegration=1, int samplefreq=200) {
	return samplefreq/1024000.0/channels*frequencyIntegration;
}

float CalcTimeResolution(int channels,  int timeintegration=1, int samplefreq=200){
    return 1e-6/samplefreq*1024*channels*timeintegration;
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
		
        std::string printEvent();
        std::string printLastEvent();
    private:


      int itsNoCoincidenceChannels;
      double itsCoincidenceTime;
      std::stringstream itsEventString;
      std::stringstream itsLastEventString;
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
	  unsigned int	Time;
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
	  
	  class SubbandTrigger 
		  {
		  public:	  
			  //SubbandTrigger();
			  ~SubbandTrigger();
			  SubbandTrigger(int StreamID, int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, long startBlock=0, int IntegrationLength=1, bool InDoPadding=true, bool InUseSamplesOr2=true);
			  bool processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime);
			  int CalculateBufferSize();
			  void InitDedispersionOffset();
			  void summary();
			  bool makeplotBuffer(std::string pulselogfilename);
			  std::string blockAnalysisSummary();
			  std::string FoundTriggers();
			  
		  private:
			  int itsNrChannels;
			  int itsNrSamples;
			  int itsNrSamplesOr2;
			  float itsDM;
			  int itsIntegrationLength;
			  float itsTriggerLevel; // number of stdev above noise
			  float itsStartFreq; // in GHz
			  float itsFreqResolution;  // in GHz
			  float itsTimeResolution; // in s
			  float itsReferenceFreq;
			  float itsReferenceTime;
			  unsigned int itsSequenceNumber;
			  int itsBlockNumber;
			  //int itsStartBlock;
			  std::vector<float> DeDispersedBuffer;
			  std::vector<int> dedispersionoffset;
			  std::vector<float> SumDeDispersed;
			  int itsBufferLength;
			  bool DoPadding;
			  bool UseSamplesOr2;
			  float itsSBaverage;
			  float itsSBstdev;
			  int itsTotalValidSamples;
			  float itsTriggerThreshold; //value to trigger on
			  float itsPrevTriggerLevel; //value to trigger on
			  int itsTotalZeros;
			  bool verbose;
			  struct triggerEvent trigger;
			  int itsStreamID;
			  std::string itsFoundTriggers;
			  		  
		  }; //SubbandTrigger
	  
	  
    
  };//StationCU
};//LOFAR
#endif
