
//#  $Id: FRATcoincidence.cc 2010-05-06 14:22:43Z veen $

#include "FRATcoincidence.h"


namespace FRAT {
	namespace coincidence {
		
		CoinCheck::CoinCheck() :
		itsNrTriggers (0),
		itsInitialized (false) {
			// set default parameters for coincidence
			std::cout << FRAT_TASK_BUFFER_LENGTH << std::endl;
			itsNoCoincidenceChannels = 8;
			itsCoincidenceTime = 10.3e-6;
			// Initialize the trigger messages buffer
			for (unsigned int i=0; i<FRAT_TASK_BUFFER_LENGTH; i++){
				trigBuffer[i].next = i+1;
				trigBuffer[i].prev = i-1;
				trigBuffer[i].Time = 0;
				trigBuffer[i].SampleNr = 0;
				trigBuffer[i].date = 0.;
				trigBuffer[i].SBnr = 0;
			};
			first = 0;
			last = FRAT_TASK_BUFFER_LENGTH-1;
			trigBuffer[first].prev = FRAT_TASK_BUFFER_LENGTH; //This means "not there"
            std::cout << "Coincheck constructed" << std::endl;
			// LOG_DEBUG ("FRAT construction");
		}
		
		
		//
		// ~CoinCheck()
		//
		CoinCheck::~CoinCheck()
		{
			// LOG_DEBUG ("FRAT destruction");
		}
		
		
		// Check the contents of the buffer if a coincidence is found
		// make timewindow a double?
		// could also be int
		bool CoinCheck::coincidenceCheck(unsigned int latestindex, unsigned int nChannles, int timeWindow){
			unsigned int i,foundSBs[nChannles],nfound;
			unsigned int startindex,runindex;
			int reftime;
			
			startindex = first;
			while ((startindex!=trigBuffer[latestindex].next) && (startindex < FRAT_TASK_BUFFER_LENGTH)) {
				runindex = trigBuffer[startindex].next;
				reftime=trigBuffer[startindex].Time-timeWindow; //earliest time for coincidence events
				nfound=1;
				foundSBs[0]=trigBuffer[startindex].SBnr;
				while ((runindex < FRAT_TASK_BUFFER_LENGTH) && (trigBuffer[runindex].Time>= reftime))
                {
					//check if current time is higher than the reftime. Stop if that is not the case
					if(trigBuffer[runindex].Time!=0)
                    {  
						for (i=0; i<nfound; i++)
                        { 
							if (foundSBs[i] == trigBuffer[runindex].SBnr) 
                            {
								//i; //prevents i to be nfound so you would trigger on one less.
								//if(i==nfound-1){i++;};
								//std::cout << "i = " << i << std::endl;
								break; //break the for-loop;
							};
						};
						if (i == nfound) 
                        { 
							if (nfound+2 > nChannles)
                            { 
								std::cout << "Subbands found:" ;
								for (i=0; i<nfound; i++)
                                {
									std::cout << " " << foundSBs[i];
								}
								std::cout << " " << trigBuffer[runindex].SBnr << std::endl;
								return true; 
							}; //startindex; 
							foundSBs[nfound] = trigBuffer[runindex].SBnr;
							std::cout << "Subbands found:" ;
							for (i=0; i<nfound; i++)
                            {
								std::cout << " " << foundSBs[i];
							}
							std::cout << " " << trigBuffer[runindex].SBnr << std::endl;
							std::cout << "Not enough. Continuing...." << std::endl;
							nfound++;
						};
						runindex = trigBuffer[runindex].next;
					};
				}; //endwhile
				startindex = trigBuffer[startindex].next;
			};
			//return -1;
			return false;
		};
		// Add a trigger message to the buffer. 
		unsigned int CoinCheck::add2buffer(const triggerEvent& trigger){
			// double date;
			unsigned int newindex,runindex;
			
			std::cout << "Adding new event: SB " << trigger.subband << " at time " << trigger.time << std::endl;
			
			// date = trigger.itsTime + trigger.itsSampleNr/200e6;
			newindex = last;
			last = trigBuffer[last].prev;
			trigBuffer[last].next = FRAT_TASK_BUFFER_LENGTH;
			
			trigBuffer[newindex].SBnr     = trigger.subband;
			// trigBuffer[newindex].SeqNr     = trigger.fitsSeqNr;
			trigBuffer[newindex].Time      = trigger.time;
			trigBuffer[newindex].SampleNr  = trigger.sample;
			trigBuffer[newindex].Sum       = trigger.sum;
			trigBuffer[newindex].NrSamples = trigger.length;
			trigBuffer[newindex].PeakValue = trigger.max;
			//  trigBuffer[newindex].date      = date;
			
			runindex = first;
			while (runindex < FRAT_TASK_BUFFER_LENGTH){
				if (trigBuffer[runindex].Time <= trigger.time) { 
					break; 
				};
				runindex = trigBuffer[runindex].next;
			};
			trigBuffer[newindex].next = runindex;
			if (runindex == first){
				first = newindex;
				trigBuffer[newindex].prev = FRAT_TASK_BUFFER_LENGTH;
				trigBuffer[runindex].prev = newindex;
			} else {
				if (runindex >= FRAT_TASK_BUFFER_LENGTH){
					trigBuffer[last].next = newindex;
					trigBuffer[newindex].prev = last;
					last = newindex;
				} else {
					trigBuffer[(trigBuffer[runindex].prev)].next = newindex;
					trigBuffer[newindex].prev = trigBuffer[runindex].prev;
					trigBuffer[runindex].prev = newindex;
				};
			};
			
			//		for(int index=0; index<FRAT_TASK_BUFFER_LENGTH; index++){
			//			std::cout << "Buffer index " << index << "; time: " << trigBuffer[index].Time << ";SB: " << trigBuffer[index].SBnr << "; prev: " << trigBuffer[index].prev << "; next: " << trigBuffer[index].next << std::endl;
			//		}
			//		std::cout << "first: " << first << "; last: " << last << std::endl;
			return newindex;	
		};
	};
	
	namespace analysis {
    
    UDPsend::UDPsend(){
			hostname=FRAT_HOSTNAME;
            send_data = new char[sizeof(struct triggerEvent)];
            host= (struct hostent *) gethostbyname(hostname);
		    std::cout << "Setting network connection "  << std::endl;	
        
            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                perror("socket");
                exit(1);
            }
			
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
            server_addr.sin_addr = *((struct in_addr *)host->h_addr);
            bzero(&(server_addr.sin_zero),8);

    }

    bool UDPsend::SendTriggerMessage(struct triggerEvent trigger){
        cout << " SENDING SENDING SENDING SENDING SENDING \n";
        send_data = (char*) &trigger;
        int n = sendto(sock, send_data, sizeof(struct triggerEvent), 0,
               (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        if (n  < 0){
            return false;
        }
        return true;
        cout << " SEND SEND SEND SEND SEND " << n << "\n";
    }
    

    
		
    SubbandTrigger::SubbandTrigger(int StreamID, int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, long startBlock, int IntegrationLength, bool InDoPadding, bool InUseSamplesOr2, bool verbose, bool noSend, int obsID, int beam)
    {
			
			// To be added to input: IntegrationLength, UseSamplesOr2, DoPadding
			DoPadding=InDoPadding;
			UseSamplesOr2=InUseSamplesOr2;
			itsNrChannels= NrChannels;
			itsNrSamples = NrSamples;
			if(UseSamplesOr2){
				itsNrSamplesOr2 = NrSamples|2;
			} else {
				itsNrSamplesOr2 = NrSamples;
			}
		    noSend=noSend;	
			itsStreamID=StreamID;
			itsDM = DM;
			itsIntegrationLength=IntegrationLength; 
			itsTriggerLevel = TriggerLevel;
			itsStartFreq = StartFreq;
			itsReferenceFreq = ReferenceFreq;
			itsFreqResolution = FreqResolution;
			itsTimeResolution = TimeResolution;
			itsSequenceNumber = startBlock-1;
            itsStarttime_utc_sec=starttime_utc_sec;
            itsStarttime_utc_ns=starttime_utc_nanosec;
			itsBlockNumber = 0;
			itsTotalValidSamples = 0;
			itsSBaverage=1e22;
			itsSBstdev=1e10;
			itsTriggerThreshold=10;
			itsTotalZeros=0;
			itsBufferLength=2000;
			InitDedispersionOffset();
			verbose=verbose;
            std::cout << "verbose " << verbose << std::endl;
		    std::cout << "Resizing dedispersionBuffer "  << std::endl;	
			DeDispersedBuffer.resize(itsBufferLength, 0.0);
		    std::cout << "Resizing sum Buffer "  << std::endl;	
			DeDispersed.resize(itsBufferLength, 0.0);
			SumDeDispersed.resize(itsNrSamples, 0.0);
			hostname=FRAT_HOSTNAME;
			if(!noSend){
                send_data = new char[sizeof(struct triggerEvent)];
                host= (struct hostent *) gethostbyname(hostname);
		    std::cout << "Setting network connection "  << std::endl;	
            
                if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                {
                    perror("socket");
                    exit(1);
                }
			
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
                server_addr.sin_addr = *((struct in_addr *)host->h_addr);
                bzero(&(server_addr.sin_zero),8);
		    }	
			
			
			trigger.time=0;
			trigger.length=0;
			trigger.block=0;
			trigger.sample=0;
			trigger.sum=0;
			trigger.max=0;
			trigger.subband=itsStreamID;
			trigger.obsID=obsID;
			trigger.beam=beam;
			trigger.DM=DM;
            trigger.utc_second=5000;
            trigger.utc_nanosecond=10000;
			
    }
        
 /*      
        SubbandTrigger::SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock, int IntegrationLength, int nrblocks, bool InDoPadding, bool InUseSamplesOr2, bool verbose, bool noSend, int obsID, int beam )
        {
            std::vector<int> IntegrationLengthVector;
            IntegrationLengthVector.pusb_back(IntegrationLength);
            return SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock, IntegrationLengthVector, int nrblocks, bool InDoPadding, bool InUseSamplesOr2, bool verbose, bool noSend, int obsID, int beam )
        }
*/

        SubbandTrigger::SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, unsigned long int starttime_utc_sec, unsigned long int starttime_utc_nanosec, FRAT::analysis::UDPsend* UDPtransmitter, long startBlock, std::vector <int> IntegrationLengthVector, bool InDoPadding, bool InUseSamplesOr2, bool verbose, bool noSend, int obsID, int beam, int nrblocks )
		{
			noSend=noSend;
			// To be added to input: IntegrationLength, UseSamplesOr2, DoPadding
			DoPadding=InDoPadding;
			UseSamplesOr2=InUseSamplesOr2;
			itsNrChannels= NrChannels; // channels for this stream
            itsTotNrChannels = TotNrChannels; // total channels in this file (nrSBs * ChannelsPerSubband )
            itsChannelsPerSubband=ChannelsPerSubband;
			itsNrSamples = NrSamples;
            itsFREQvalues = FREQvalues;
            itsNrblocks= nrblocks;
            UDPtransmitter=UDPtransmitter;
            verbose=verbose;
			if(UseSamplesOr2){
				itsNrSamplesOr2 = NrSamples|2;
			} else {
				itsNrSamplesOr2 = NrSamples;
			}
			
			itsStreamID=StreamID;
			itsDM = DM;
            if(IntegrationLengthVector.size()>0){
                itsIntegrationLength=IntegrationLengthVector[0]; 
            } else {
                cerr << "Integration lengths not defined" << endl;
            }
			itsTriggerLevel = TriggerLevel;
			itsStartChannel = StartChannel;
            //itsNrChannels = TotNrChannels;
			itsReferenceFreq = ReferenceFreq;
			itsFreqResolution = FreqResolution;
			itsTimeResolution = TimeResolution;
			itsSequenceNumber = startBlock-1;
            itsStarttime_utc_sec=starttime_utc_sec;
            itsStarttime_utc_ns=starttime_utc_nanosec;            
			itsBlockNumber = 0;
			itsTotalValidSamples = 0;
			itsSBaverage=1e22;
			itsSBstdev=1e10;
			itsTriggerThreshold=10;
			itsTotalZeros=0;
			itsBufferLength=1000;
			InitDedispersionOffset(FREQvalues);
            std::cout << "verbose SBtrigger " << verbose << std::endl;
		    std::cout << "Resizing dedispersionBuffer "  << itsBufferLength << std::endl;	
			DeDispersedBuffer.resize(itsBufferLength, 0.0);
		    std::cout << "Resizing sum Buffer "  << std::endl;	
			DeDispersed.resize(itsBufferLength, 0.0);
			SumDeDispersed.resize(itsNrSamples, 0.0);
			hostname=FRAT_HOSTNAME;
            itsSBaverageVector.reserve(nrblocks);
            itsSBstdevVector.reserve(nrblocks);

            if(!noSend){
                send_data = new char[sizeof(struct triggerEvent)];
                host= (struct hostent *) gethostbyname(hostname);
                
                std::cout << "Setting network connection "  << std::endl;	
                if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                {
                    perror("socket");
                    exit(1);
                }
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
                server_addr.sin_addr = *((struct in_addr *)host->h_addr);
                bzero(&(server_addr.sin_zero),8);
            } 
                
			
			trigger.time=0;
			trigger.length=0;
			trigger.block=0;
			trigger.sample=0;
			trigger.sum=0;
			trigger.max=0;
			trigger.subband=itsStreamID;
			trigger.obsID=obsID;
			trigger.beam=beam;
			trigger.DM=DM;
            trigger.utc_second=1234;
            trigger.utc_nanosecond=56789;
            trigger.lastBadBlock=-10000;
			
		}
	
        void SubbandTrigger::setLastBadBlock(int block){
            trigger.lastBadBlock=block;
        }
		
		void SubbandTrigger::summary()
		{
			
			// To be added to input: IntegrationLength, UseSamplesOr2, DoPadding
			std::cout << "=========SUMMARY =============" << std::endl;
			std::cout << "DoPadding " << DoPadding << std::endl;
			//UseSamplesOr2=InUseSamplesOr2;
			std::cout << "NrChannels " << itsNrChannels << std::endl ;
			std::cout << "NrSamples " << itsNrSamples << std::endl;
			
			std::cout << "StreamID " << itsStreamID << std::endl;
			std::cout << "DM " << itsDM << std::endl;
			std::cout << "Integration Length " << itsIntegrationLength << std::endl;
			std::cout << "Triggerlevel " << itsTriggerLevel << std::endl;
			std::cout << "StartFreq " << itsStartFreq << std::endl;
			std::cout << "ReferenceFreq " << itsReferenceFreq<< std::endl;
			std::cout << "itsReferenceTime " << itsReferenceTime<< std::endl;
			std::cout << "FreqResolution " << itsFreqResolution  << std::endl;
			std::cout << "TimeResolution " << itsTimeResolution  << std::endl;
			std::cout << "startBlock " << itsSequenceNumber<< std::endl;
			std::cout << "BlockNumber " << itsBlockNumber << std::endl;
			std::cout << "TotalValidSamples " << itsTotalValidSamples << std::endl;
			std::cout << "SBaverage " << itsSBaverage << std::endl;
			std::cout << "SBstdev " << itsSBstdev << std::endl;
			std::cout << "TriggerThreshold " << itsTriggerThreshold << std::endl;
			std::cout << "Buffer Length " << itsBufferLength << std::endl;
			std::cout << "verbose mode " << verbose << std::endl;
			std::cout << "Dedispersion offset: ";
			for(int channel=0;channel<itsNrChannels;channel++){
				std::cout << " " << dedispersionoffset[channel];
			}
			std::cout << std::endl;
				
		    std::cout << "=========SUMMARY END===========\n\n\n" << std::endl;
			//DeDispersedBuffer.resize(itsBufferLength, 0.0);
			//DeDispersed.resize(itsBufferLength, 0.0);
			
			
			
			//trigger.time=0;
			//trigger.length=0;
			//trigger.block=0;
			//trigger.sample=0;
			//trigger.sum=0;
			//trigger.max=0;
			//trigger.subband=0;
			
			
			
			
		}
		
		
		
		SubbandTrigger::~SubbandTrigger()
		{
			// LOG_DEBUG ("FRAT CoinCheck destruction");
		}
		
		bool SubbandTrigger::dedisperseData2(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime,bool Transposed){
            //# Version used in the trigger
            //# Input: data, 2-dimensional, axis frequencies and time (dynamic spectrum)
            //# sequenceNumber: block number
            //# ...
            //# The data has already gone through an endianness swap and some RFI mitigation steps.
            //# Task 1: Dedisperse data
            //# How?: Loop over time and frequency
            //# Add data[time][frequency] to the correct sample in DeDispersedBuffer, that is at position (totaltime+dedispersionoffset[channel])%itsBufferLength 
            //# 
			bool pulsefound=false;
			if( ++itsSequenceNumber!=sequenceNumber && false) { 
				std::cerr << "discontinous data" << std::endl; 
			    return false; 
			} else {
				itsFoundTriggers="";
                //itsSequenceNumber==sequenceNumber;
				itsBlockNumber=sequenceNumber;
                if(verbose){
                    std::cout << "Processing block " << sequenceNumber << std::endl;
                }
                blocksum =0.0;
                //validsamples=0;
				//zerocounter=0;
				//SBaverage=0;
				//SBstdev=0;
				//SBsumsamples=0;
                channeltimeindex=-itsTotNrChannels;
                totaltime=itsSequenceNumber*itsNrSamples-1;

				for(int time=0; time<itsNrSamples; time++){
					//std::cout << std::endl << " time " << time ;
					// totaltime=itsSequenceNumber*itsNrSamples+time;
                    totaltime++;

					rest=totaltime%itsBufferLength;
                    channeltimeindex+=itsTotNrChannels; // increase index for each sample by the number of values on the channel axis
                    channelindex=channeltimeindex+itsStartChannel+itsNrChannels; // move to the highest frequency of this band. Then count downwards.
                    
					for(int channel=itsNrChannels-1; channel>0; channel--){
                        channelindex--;
						DeDispersedBuffer[(totaltime+dedispersionoffset[channel])%itsBufferLength]+=data[channelindex];
                    } // for channel
                    //***  channel=0; *******
                    
                    channelindex--;
                    
                    //value = data[channelindex];
                        
                    //DeDispersedBuffer[rest]+=value;
                    //value = data[channelindex];
                        
                    DeDispersedBuffer[rest]+=data[channelindex];
                    blocksum+=DeDispersedBuffer[rest];
                    DeDispersed[rest]=DeDispersedBuffer[rest];

                
                    DeDispersedBuffer[rest]=0;

				} // for time
                    // Calculate sum of integration length

                return pulsefound;
            }
        }

    bool SubbandTrigger::calcAverageStddev(unsigned int sequenceNumber){
        bool pulsefound=false;
        float subsum=0;
        if( itsSequenceNumber!=sequenceNumber && false) { 
            std::cerr << "discontinous data" << std::endl; 
            return false; 
	    } else {
            itsFoundTriggers="";
            if(verbose){
                std::cout << "Processing block " << sequenceNumber << std::endl;
            }
            //blocksum =0.0;
            validsamples=0;
            zerocounter=0;
            SBaverage=0;
            SBstdev=0;
            SBsumsamples=0;
            for(int time=0; time<itsNrSamples; time++){
                //std::cout << std::endl << " time " << time ;
                totaltime=itsSequenceNumber*itsNrSamples+time;
                rest=totaltime%itsBufferLength;
                SBaverage+=DeDispersed[rest];
                SBstdev+=DeDispersed[rest]*DeDispersed[rest];
                SBsumsamples++;
			}	
				// Update class parameters;
				//itsSBstdev = SBstdev;
				
            if(SBsumsamples>0){
                itsSBaverage = SBaverage/SBsumsamples;
                itsSBaverage=blocksum/SBsumsamples;
                SBstdev/=SBsumsamples;
                
            }
            itsSBstdev=sqrt(SBstdev-itsSBaverage*itsSBaverage);
				//for(int it=rest;it>rest-SBsumsamples;it--){
				//	SBaverageAlt += DeDispersed[rest];
				//}
				//SBaverageAlt*=itsIntegrationLength;
				//SBaverageAlt/=SBsumsamples;
            if(verbose && false){
                std::cerr << "DM " << itsDM << ", SBaverage over "<< SBsumsamples << "samples at block "<<  itsBlockNumber << " / " << itsSequenceNumber <<" at frequency " << itsStartFreq << " is "<< itsSBaverage << " Standard deviation " << itsSBstdev << " " << itsSBstdev/itsSBaverage << " triggerlevel " << (itsTriggerThreshold-itsSBaverage)/itsSBstdev << std::endl;
            }
            //std::string output;
            
            itsPrevTriggerLevel = (itsTriggerThreshold-itsSBaverage)/itsSBstdev;
            itsTriggerThreshold = itsSBaverage+itsTriggerLevel*itsSBstdev;
            itsSBaverageVector.push_back(itsSBaverage);
            itsSBstdevVector.push_back(itsSBstdev);
        }
			
			
        return pulsefound;
        
    
    }


    bool SubbandTrigger::setObsid(int obsID){
        //trigger.obsID=obsID;
        return false;
    }

	bool SubbandTrigger::runTrigger(unsigned int sequenceNumber, int IntegrationLength, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime,bool Transposed){
       // for(int i=0;i<itsIntegrationLength.size();i++){
       //     IntegrationLength=itsIntegrationLengthVector[i];
        float subsum=0;
        int samplessummed=1;
        bool pulsefound=false;
        verbose=false;
//# Correct SB average and stddev for the integration length
        SBaverage=itsSBaverage*IntegrationLength;
        SBstdev=itsSBstdev*sqrt(1.0*IntegrationLength);
        float TriggerThreshold=SBaverage+itsTriggerLevel*SBstdev; 
        trigger.SBaverage=SBaverage;
        trigger.SBstdev=SBstdev;
        trigger.Threshold=SBaverage+itsTriggerLevel*SBstdev;

        unsigned long int utc_second;
        unsigned long int utc_nanosecond;
        if( itsSequenceNumber!=sequenceNumber && false) { 
            std::cerr << "discontinous data" << std::endl; 
            return false; 
	    } else {
            itsFoundTriggers="";
            //blocksum =0.0;
            int nextTriggerTimeOffset=1;
            if(IntegrationLength==1){ // make this == 1
                //# no need to buffer
                for(int time=0; time<itsNrSamples; time++){
                    //std::cout << std::endl << " time " << time ;
                    totaltime=itsSequenceNumber*itsNrSamples+time;
                    rest=totaltime%itsBufferLength;
                    subsum=DeDispersed[rest];
                    if(subsum>TriggerThreshold) {
                        //ADD TRIGGER ALGORITHM OR FUNCTION
                        if(trigger.length==IntegrationLength && trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && time!=0){
                            // Next sample, same length
                            trigger.width++;
                            nextTriggerTimeOffset++;
                            if(subsum>trigger.sum) { // new maximum
                                trigger.sum=subsum;
                                trigger.max=(trigger.sum-SBaverage)/SBstdev;
                                trigger.time=totaltime+itsReferenceTime;
                                trigger.sample=time;
                                nextTriggerTimeOffset=1;
                            }
                        } else {
                            // new trigger
                            trigger.time=totaltime+itsReferenceTime;//correction for dispersion fc* removed
                            trigger.sum=subsum;
                            trigger.length=IntegrationLength;
                            trigger.sample=time;
                            trigger.block=itsBlockNumber;
                            trigger.max=(trigger.sum-SBaverage)/SBstdev;
                            trigger.width=IntegrationLength;
                        }     
                        if(time==itsNrSamples-1 && itsBlockNumber>minBlockNumber){
                            // send trigger at last sample of block, as it's overwritten next
                            utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                            utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                            trigger.utc_second=itsStarttime_utc_sec+utc_second;
                            trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                            //if(!nosend){
                            SendTriggerMessage(trigger);
                            nextTriggerTimeOffset=1;

                        }
                    }
                    else if(trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && trigger.length==IntegrationLength && itsBlockNumber>minBlockNumber && time!=0){
                        // send trigger
                        utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                        utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                        trigger.utc_second=itsStarttime_utc_sec+utc_second;
                        trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                        //if(!nosend){
                        SendTriggerMessage(trigger);
                        triggerMessages.push_back(trigger);
                        nextTriggerTimeOffset=1;
                    }

                } // for time
            } else if(IntegrationLength==2){	// make this ==2
                for(int time=0; time<itsNrSamples; time++){
                    //std::cout << std::endl << " time " << time ;
                    totaltime=itsSequenceNumber*itsNrSamples+time;
                    rest=totaltime%itsBufferLength;
                    if(rest>1){
                        subsum=DeDispersed[rest]+DeDispersed[rest-1];
                    } else {
                        subsum=DeDispersed[rest]+DeDispersed[itsBufferLength-1];
                    } 
                    if(subsum>TriggerThreshold) {
                        //ADD TRIGGER ALGORITHM OR FUNCTION
                        if(trigger.length==IntegrationLength && trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && time!=0){
                            // Next sample, same length
                            trigger.width++;
                            nextTriggerTimeOffset++;
                            if(subsum>trigger.sum) { // new maximum
                                trigger.sum=subsum;
                                trigger.max=(trigger.sum-SBaverage)/SBstdev;
                                trigger.time=totaltime+itsReferenceTime;
                                trigger.sample=time;
                                nextTriggerTimeOffset=1;
                            }
                        } else {
                            // new trigger
                            trigger.time=totaltime+itsReferenceTime;//correction for dispersion fc* removed
                            trigger.sum=subsum;
                            trigger.length=IntegrationLength;
                            trigger.sample=time;
                            trigger.block=itsBlockNumber;
                            trigger.max=(trigger.sum-SBaverage)/SBstdev;
                            trigger.width=IntegrationLength;
                        }     
                        if(time==itsNrSamples-1 && itsBlockNumber>minBlockNumber){
                            // send trigger at last sample of block, as it's overwritten next
                            utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                            utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                            trigger.utc_second=itsStarttime_utc_sec+utc_second;
                            trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                            //if(!nosend){
                            SendTriggerMessage(trigger);
                            nextTriggerTimeOffset=1;

                        }
                    }
                    else if(trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && trigger.length==IntegrationLength && itsBlockNumber>minBlockNumber && time!=0){
                        // send trigger
                        utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                        utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                        trigger.utc_second=itsStarttime_utc_sec+utc_second;
                        trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                        //if(!nosend){
                        SendTriggerMessage(trigger);
                        triggerMessages.push_back(trigger);
                        nextTriggerTimeOffset=1;
                    }

                } // for time
              } else if(IntegrationLength>2){ // make this >2	
                rest=itsSequenceNumber*itsNrSamples%itsBufferLength;
                subsum=0;
                samplessummed=1;
                int addindex;
                int subtractindex;
             /*   if(rest >= IntegrationLength){
                    for(int it=rest; it>rest-IntegrationLength; it--){subsum+=DeDispersed[it];}
                } else {
                    for(int it=rest; it>=0; it--){
                        subsum+=DeDispersed[it];
                        samplessummed++;
                    }
                    for(int it=-1; it>-IntegrationLength+1; it--){
                        subsum+=DeDispersed[it+itsBufferLength];
                        samplessummed++;
                    }
                }
               */      
                for(int time=0; time<itsNrSamples; time++){
                    //std::cout << std::endl << " time " << time ;
                    totaltime=itsSequenceNumber*itsNrSamples+time;
                    rest=totaltime%itsBufferLength;
                    //addindex=totaltime%itsBufferLength;
                    //subtractindex=(totaltime-IntegrationLength)%itsBufferLength;
                    samplessummed=0;
                    //subsum+=DeDispersed[addindex];
                    //subsum-=DeDispersed[subtractindex];
                    subsum=0;
                    // rest = 4:il=4: it=4, 3, 2, 1, stop
                    // rest= 3 ; il=4 : it 3 2 1 0, rest-IntegrationLength=-1
                    // rest= 2 ; il=4 it=2 1 0; rest-I=-2, it=-1
                    if(rest >= IntegrationLength){
                        for(int it=rest; it>rest-IntegrationLength; it--){
                            subsum+=DeDispersed[it];
                        }
                    } else {
                        for(int it=rest; it>=0; it--){
                            subsum+=DeDispersed[it];
                            samplessummed++;
                        }
                        for(int it=-1; it>rest-IntegrationLength; it--){
                            subsum+=DeDispersed[it+itsBufferLength];
                            samplessummed++;
                        }
                    }

                    if(subsum>TriggerThreshold) {
                        //ADD TRIGGER ALGORITHM OR FUNCTION
                        if(trigger.length==IntegrationLength && trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && time!=0){
                            // Next sample, same length
                            trigger.width++;
                            nextTriggerTimeOffset++;
                            if(subsum>trigger.sum) { // new maximum
                                trigger.sum=subsum;
                                trigger.max=(trigger.sum-SBaverage)/SBstdev;
                                trigger.time=totaltime+itsReferenceTime;
                                trigger.sample=time;
                                nextTriggerTimeOffset=1;
                            }
                        } else {
                            // new trigger
                            trigger.time=totaltime+itsReferenceTime;//correction for dispersion fc* removed
                            trigger.sum=subsum;
                            trigger.length=IntegrationLength;
                            trigger.sample=time;
                            trigger.block=itsBlockNumber;
                            trigger.max=(trigger.sum-SBaverage)/SBstdev;
                            trigger.width=IntegrationLength;
                        }     
                        if(time==itsNrSamples-1 && itsBlockNumber>minBlockNumber){
                            // send trigger at last sample of block, as it's overwritten next
                            utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                            utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                            trigger.utc_second=itsStarttime_utc_sec+utc_second;
                            trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                            //if(!nosend){
                            SendTriggerMessage(trigger);
                            nextTriggerTimeOffset=1;

                        }
                    }
                    else if(trigger.time-itsReferenceTime+nextTriggerTimeOffset==totaltime && trigger.length==IntegrationLength && itsBlockNumber>minBlockNumber && time!=0){
                        // send trigger
                        utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                        utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                        trigger.utc_second=itsStarttime_utc_sec+utc_second;
                        trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                        //if(!nosend){
                        SendTriggerMessage(trigger);
                        triggerMessages.push_back(trigger);
                        nextTriggerTimeOffset=1;
                    }

                } // for time
            }    
                
                
                
            
        }
    //    }
        return pulsefound;
        
        
    }        

    bool SubbandTrigger::processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime,bool Transposed){
            //# Input: data, 2-dimensional, axis frequencies and time (dynamic spectrum)
            //# sequenceNumber: block number
            //# ...
            //# The data has already gone through an endianness swap and some RFI mitigation steps.
            //# Task 1: Dedisperse data
            //# How?: Loop over time and frequency
            //# Add data[time][frequency] to the correct sample in DeDispersedBuffer, that is at position (totaltime+dedispersionoffset[channel])%itsBufferLength 
            //# 
			bool pulsefound=false;
            float subsum=0;
			if( ++itsSequenceNumber!=sequenceNumber && false) { 
				std::cerr << "discontinous data" << std::endl; 
			    return false; 
			} else {
				itsFoundTriggers="";
				itsBlockNumber=sequenceNumber;
			    if(verbose){	
                    std::cout << "Processing block " << sequenceNumber << std::endl;
                }
                blocksum =0.0;
                validsamples=0;
				zerocounter=0;
				SBaverage=0;
				SBstdev=0;
				SBsumsamples=0;
                channeltimeindex=-itsTotNrChannels;

				for(int time=0; time<itsNrSamples; time++){
					//std::cout << std::endl << " time " << time ;
					totaltime=itsSequenceNumber*itsNrSamples+time;
					rest=totaltime%itsBufferLength;
                    channeltimeindex+=itsTotNrChannels; // increase index for each sample by the number of values on the channel axis
                    channelindex=channeltimeindex+itsStartChannel+itsNrChannels; // move to the highest frequency of this band. Then count downwards.
                    
					for(int channel=itsNrChannels-1; channel>0; channel--){
                        channelindex--;
						DeDispersedBuffer[(totaltime+dedispersionoffset[channel])%itsBufferLength]+=data[channelindex];
                    } // for channel
                    //***  channel=0; *******
                    
                    channelindex--;
                    
                    //value = data[channelindex];
                        
                    //DeDispersedBuffer[rest]+=value;
                    //value = data[channelindex];
                        
                    DeDispersedBuffer[rest]+=data[channelindex];
                    blocksum+=DeDispersedBuffer[rest];
                    DeDispersed[rest]=DeDispersedBuffer[rest];
                    // Calculate sum of integration length
                    subsum=0;
                    if(rest >= itsIntegrationLength){
                        for(int it=rest; it>rest-itsIntegrationLength; it--){subsum+=DeDispersed[it];}
                    } else {
                        for(int it=rest; it>rest-itsIntegrationLength; it--){
                            if(it>=0){
                                subsum+=DeDispersed[it];
                            }
                            else {
                                subsum+=DeDispersed[it+itsBufferLength];
                            }
                        }
                        
                    }
                    SBstdev+=(subsum-itsSBaverage)*(subsum-itsSBaverage);
                    SBsumsamples++;
                    SBaverage+=subsum;
                    if(subsum>itsTriggerThreshold) {
                        //ADD TRIGGER ALGORITHM OR FUNCTION
                        if(totaltime+itsReferenceTime-trigger.time>5){
                            //new trigger
                            //trigger.average[fc]/=triggerlength[fc];
                            trigger.time=totaltime+itsReferenceTime;//correction for dispersion fc* removed
                            
                            trigger.sum=subsum;
                            trigger.length=1;
                            trigger.sample=time;
                            trigger.block=itsBlockNumber;
                            trigger.max=subsum;
                        
                        }
                        else{
                            //old trigger, or trigger accross blocks
                            trigger.sum+=subsum;
                            trigger.length++;
                            //trigger[fc].time=totaltime+fc*channels_p*STRIDE; //correction for dispersion
                            trigger.time=totaltime+itsReferenceTime; //correction for dispersion fc* removed
                            if(subsum>trigger.max){trigger.max=subsum;} //calculate maximum
                        }
                    } else if(totaltime+itsReferenceTime-trigger.time==5 && itsBlockNumber>3){
                        //trigger.utc_second=10000;
                            unsigned long int utc_second=(unsigned long int) trigger.time*itsTimeResolution;
                            unsigned long int utc_nanosecond=(unsigned long int) (fmod(trigger.time*itsTimeResolution,1)*1e9);
                            std::cout << "Time since start " << trigger.time*itsTimeResolution << " " << utc_second << " " << utc_nanosecond << " ";
                            trigger.utc_second=itsStarttime_utc_sec+utc_second;
                            trigger.utc_nanosecond=itsStarttime_utc_ns+utc_nanosecond;
                            // Change trigger max to value in standard deviations.
                            trigger.max=(trigger.max-itsSBaverage)/itsSBstdev;
                            //if(!nosend){
                            SendTriggerMessage(trigger);
                            triggerMessages.push_back(trigger);
                            //UDPtransmitter->SendTriggerMessage(trigger);
                            //}

                           /* 
                            int latestindex = cc->add2buffer(trigger);
                            
                            
                            if(cc->coincidenceCheck(latestindex, CoinNr, CoinTime)) {
                                
                                
                                float triggerstrength = trigger.max;//(trigger.max-itsSBaverage)/itsSBstdev;
                                char output2[400];
                                //std::string output;
                                
                                //sprintf(output,"%f4.2 %d %e %e %e %e %e" ,itsDM,itsSequenceNumber,1000*itsStartFreq,itsSBaverage,itsSBstdev,itsSBstdev/itsSBaverage,itsPrevTriggerLevel);

                                sprintf(output2,"COINCIDENCE TRIGGER FOUND with DM %f at block %i / %i time: %f %f strength %f \n",itsDM,itsSequenceNumber,itsBlockNumber,totaltime*itsTimeResolution,trigger.time*itsTimeResolution,triggerstrength);
                                itsFoundTriggers=itsFoundTriggers+std::string(output2);
                                std::cout << "\n\n\n\n COINCIDENCE TRIGGER FOUND with DM " << itsDM << " at block" << itsSequenceNumber << " / " << itsBlockNumber << " time: " << totaltime*itsTimeResolution   << " " << trigger.time*itsTimeResolution << " strength " << triggerstrength << "\n\n";	
                                //usleep(1000000);
                                pulsefound=true;
                            }
                            */
								
								
								
                        }
                        DeDispersedBuffer[rest]=0;

				} // for time
				
				
				
				
				
				// Update class parameters;
				//itsSBstdev = SBstdev;
				
				if(SBsumsamples>0){
					itsSBaverage = SBaverage/SBsumsamples;
				    //itsSBaverage=blocksum/SBsumsamples;
					SBstdev/=SBsumsamples;
					
				}
				itsSBstdev=sqrt(SBstdev);
				itsTotalZeros += zerocounter;
				float SBaverageAlt = blocksum/SBsumsamples;
				//for(int it=rest;it>rest-SBsumsamples;it--){
				//	SBaverageAlt += DeDispersed[rest];
				//}
				//SBaverageAlt*=itsIntegrationLength;
				//SBaverageAlt/=SBsumsamples;
				if(verbose){
                    std::cout << "verbose " << verbose << endl;
                    
					std::cerr << "verbose" << verbose << "DM " << itsDM << ", SBaverage over "<< SBsumsamples << "samples at block "<<  itsBlockNumber << " / " << itsSequenceNumber <<" at frequency " << itsStartFreq << " is "<< itsSBaverage << " or " << SBaverageAlt << " Standard deviation " << itsSBstdev << " " << itsSBstdev/itsSBaverage << " triggerlevel " << (itsTriggerThreshold-itsSBaverage)/itsSBstdev << std::endl;
				}
				char output[200];
				//std::string output;
				
				itsPrevTriggerLevel = (itsTriggerThreshold-itsSBaverage)/itsSBstdev;
				itsTriggerThreshold = itsSBaverage+itsTriggerLevel*itsSBstdev;
                itsSBaverageVector.push_back(itsSBaverage);
                itsSBstdevVector.push_back(itsSBstdev);
			}
			
			
			return pulsefound;
		}
		
		int SubbandTrigger::setNrFlaggedChannels(int nrFlaggedChannels){
            trigger.nrFlaggedChannels=nrFlaggedChannels; 
            return 0;
        }

		int SubbandTrigger::setNrFlaggedSamples(int nrFlaggedSamples){
            trigger.nrFlaggedSamples=nrFlaggedSamples; 
            return 0;
        }

		int SubbandTrigger::CalculateBufferSize(){
            printf("Started ... %i %i %i ",dedispersionoffset[itsNrChannels-1],itsIntegrationLength,itsNrSamples);
            int requiredLength=2*(dedispersionoffset[itsNrChannels-1]+itsIntegrationLength+itsNrSamples+itsReferenceTime);
            if(requiredLength < 4096) itsBufferLength=4096;
            else if(requiredLength < 8192) itsBufferLength=8192;
            else if(requiredLength < 16384) itsBufferLength=16384;
            else if(requiredLength < 32768) itsBufferLength=32768;
            else {
                itsBufferLength=std::max(4096,2*(dedispersionoffset[itsNrChannels-1]+itsIntegrationLength+itsNrSamples+itsReferenceTime));
            }
            //itsBufferLength=std::max(2000,500*(itsBufferLength/500+1);
            printf("Done.");
			return itsBufferLength;
		}

		
		void SubbandTrigger::InitDedispersionOffset(){
			dedispersionoffset.resize(itsNrChannels);
            float freq1=itsStartFreq-0.5*itsNrChannels*itsFreqResolution;
			for(int channel=0;channel<itsNrChannels;channel++){
				
				float freq2=freq1+channel*itsFreqResolution;
				float offset=DM_CONSTANT*itsDM*(1/(freq1*freq1)-1/(freq2*freq2))/itsTimeResolution;
			//	 printf("%f ",offset);
				
				// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
				
				dedispersionoffset[channel]=(int)offset;
			}
            minBlockNumber=(dedispersionoffset[0]-dedispersionoffset[itsNrChannels-1])/itsNrSamples;
            if(minBlockNumber<0) {
                minBlockNumber=-minBlockNumber+2;
            } else {
                minBlockNumber+=2;
            }
             

			float freqA=itsReferenceFreq+0.5*itsFreqResolution;
			float freqB=itsStartFreq+0.5*itsFreqResolution;
			float offset=DM_CONSTANT*itsDM*(1/(freqA*freqA)-1/(freqB*freqB))/itsTimeResolution;
			itsReferenceTime=(int) offset;
			CalculateBufferSize();
		}
        
        
        void SubbandTrigger::InitDedispersionOffset(std::vector<float> FREQvalues){
            std::cout << itsNrChannels << std::endl;
            dedispersionoffset.resize(itsNrChannels);
            // Frequency= FREQvalues[itsStartChannel+currentChannel]
            printf("Setting offsets with timeresolution %f start %i number %i tot number %i ", itsTimeResolution,itsStartChannel, itsNrChannels, itsTotNrChannels);	
            float freq1=FREQvalues[itsStartChannel]-0.5*itsChannelsPerSubband*itsFreqResolution;
			for(int channel=0;channel<itsNrChannels;channel++){
				float freq2=FREQvalues[itsStartChannel+channel]; //freq2=freq1+channel*itsFreqResolution;
				float offset=DM_CONSTANT*itsDM*(1/(freq1*freq1)-1/(freq2*freq2))*1e18/itsTimeResolution;
                //printf("%f %i ",offset,int(offset));
				
				// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
				
				dedispersionoffset[channel]=(int)offset;
			}
            minBlockNumber=(dedispersionoffset[0]-dedispersionoffset[itsNrChannels-1])/itsNrSamples;
            if(minBlockNumber<0) {
                minBlockNumber=-minBlockNumber+2;
            } else {
                minBlockNumber+=2;
            }

            cout << endl << "minBlocknumber for DM, stream " << itsDM << " " << itsStreamID << " = " << minBlockNumber <<  " " << dedispersionoffset[0] << " " << dedispersionoffset[itsNrChannels-1] << " " << itsNrSamples << endl;
			float freqA=FREQvalues[0];//itsReferenceFreq+0.5*itsChannelsPerSubband*itsFreqResolution;
			float freqB=FREQvalues[itsStartChannel];//+0.5*itsChannelsPerSubband*itsFreqResolution;
            freqA/=1e9;
            freqB/=1e9;
			float offset=DM_CONSTANT*itsDM*(1/(freqA*freqA)-1/(freqB*freqB))/itsTimeResolution;
            printf("ref Freq A, %g B %g, offset %f,",freqA,freqB,offset);
			itsReferenceTime=(int) offset;
			CalculateBufferSize();
		}
		
		bool SubbandTrigger::makeplotBuffer(std::string pulselogfilename){
			std::ofstream pulselogfile(pulselogfilename.c_str(), std::ios::out | std::ios::binary);
			int totaltime=(itsSequenceNumber+1)*itsNrSamples;
			int intstart=(itsSequenceNumber+1)*itsNrSamples%itsBufferLength;
			//pulselogfile.open;
			int buflen=DeDispersed.size();
			//pulselogfile.width(11);
            //printf("SeqNr, NrSam, BufLen, SumDeDispSize %i %i %i %i ",itsSequenceNumber, itsNrSamples, itsBufferLength, buflen);
            //pulselogfile.precision(10);
            pulselogfile.write((char*) &totaltime, sizeof(totaltime));
            pulselogfile.write((char*) &intstart, sizeof(intstart));
            pulselogfile.write((char*) &itsTimeResolution,sizeof(itsTimeResolution));
            pulselogfile.write((char*) &buflen, sizeof(buflen));
            pulselogfile.write((char*) &itsReferenceTime, sizeof(itsReferenceTime));

            pulselogfile.write((char*) &DeDispersed[0], DeDispersed.size()*sizeof(float));
            /*
			for(int j=intstart+1;j<buflen;j++){
				pulselogfile << (totaltime-buflen+j)*itsTimeResolution <<  DeDispersed[j] << std::endl;
				
			}
                        float time;
                        //std::cout.precision(10);
			for(int r=0;r<intstart+1;r++){
				char output3[200];
				sprintf(output3,"%.12f %e",(totaltime+r)*itsTimeResolution,DeDispersed[r]  );
				time = (totaltime+r)*itsTimeResolution;
                                
                                pulselogfile << time << " " << DeDispersed[r] << std::endl;
                               // std::cout << time << std::endl;
			}
            */
			pulselogfile.close();
			
			return true;
		
		
		
		}


        bool SubbandTrigger::writeAverage(ofstream * fsfile){
            fsfile->write((const char*)&itsDM, sizeof(itsDM));
            fsfile->write((const char*)&itsStreamID, sizeof(itsStreamID));
            int size2=itsSBaverageVector.size();
            fsfile->write((const char*)&size2, sizeof(size2));
            fsfile->write((const char*)&itsSBaverageVector[0], size2 * sizeof(itsSBaverageVector[0]));
        }
    
       
        bool SubbandTrigger::writeStdDev(ofstream * fsfile){
            fsfile->write((const char*)&itsDM, sizeof(itsDM));
            fsfile->write((const char*)&itsStreamID, sizeof(itsStreamID));
            int size2=itsSBstdevVector.size();
            fsfile->write((const char*)&size2, sizeof(size2));
            fsfile->write((const char*)&itsSBstdevVector[0], size2 * sizeof(itsSBstdevVector[0]));
        }

        bool SubbandTrigger::writeOffset(ofstream * fsfile){
            fsfile->write((const char*)&itsDM, sizeof(itsDM));
            fsfile->write((const char*)&itsStreamID, sizeof(itsStreamID));
            int size2=dedispersionoffset.size();
            fsfile->write((const char*)&size2, sizeof(size2));
            fsfile->write((const char*)&dedispersionoffset[0], size2 * sizeof(dedispersionoffset[0]));
        }

        bool SubbandTrigger::makeplotDedispBlock(string pulselogfilename){
            std::ofstream pulselogfile(pulselogfilename.c_str(), std::ios::out | std::ios::app | std::ios::binary);
            if(itsBlockNumber<1){
                std::ofstream pulselogfile(pulselogfilename.c_str(), std::ios::out |  std::ios::binary);
            }
			int totaltime=(itsSequenceNumber+1)*itsNrSamples;
			int intstart=(itsSequenceNumber+1)*itsNrSamples%itsBufferLength;

			//pulselogfile.open;
			int buflen=DeDispersed.size();
			//pulselogfile.width(11);
            //printf("SeqNr, NrSam, BufLen, SumDeDispSize %i %i %i %i ",itsSequenceNumber, itsNrSamples, itsBufferLength, buflen);
            //pulselogfile.precision(10);
            if(intstart>=itsNrSamples){
                pulselogfile.write((char*) &DeDispersed[intstart-itsNrSamples], itsNrSamples*sizeof(float));
            } else {
                int offset=itsNrSamples-intstart;
                pulselogfile.write((char*) &DeDispersed[buflen-offset], offset*sizeof(float));
                pulselogfile.write((char*) &DeDispersed[0], intstart*sizeof(float));
            }
	    	pulselogfile.close();
			
			return true;
		
		
		
		}

        bool SubbandTrigger::makeplotDedispBlock(ofstream *pulselogfile){
            //std::ofstream pulselogfile(pulselogfilename.c_str(), std::ios::out | std::ios::app | std::ios::binary);
            //if(itsBlockNumber<1){
            //    std::ofstream pulselogfile(pulselogfilename.c_str(), std::ios::out |  std::ios::binary);
            //}
			int totaltime=(itsSequenceNumber+1)*itsNrSamples;
			int intstart=(itsSequenceNumber+1)*itsNrSamples%itsBufferLength;

			//pulselogfile.open;
			int buflen=DeDispersed.size();
			//pulselogfile.width(11);
            //printf("SeqNr, NrSam, BufLen, SumDeDispSize %i %i %i %i ",itsSequenceNumber, itsNrSamples, itsBufferLength, buflen);
            //pulselogfile.precision(10);
            if(intstart>itsNrSamples){
                pulselogfile->write((char*) &DeDispersed[intstart-itsNrSamples], itsNrSamples*sizeof(float));
            } else {
                int offset=itsNrSamples-intstart;
                pulselogfile->write((char*) &DeDispersed[buflen-offset], offset*sizeof(float));
                pulselogfile->write((char*) &DeDispersed[0], intstart*sizeof(float));
            }
	    	//pulselogfile.close();
			
			return true;
		
		
		
		}

        	
		std::string SubbandTrigger::blockAnalysisSummary(){
			char output[200];
			//std::string output;
			sprintf(output,"%f4.2 %d %e %e %e %e %e" ,itsDM,itsSequenceNumber,1000*itsStartFreq,itsSBaverage,itsSBstdev,itsSBstdev/itsSBaverage,itsPrevTriggerLevel);
			//std::cerr << "DM " << itsDM << ", SBaverage over "<< SBsumsamples << "samples at block "<<  itsBlockNumber << " / " << itsSequenceNumber <<" at frequency " << itsStartFreq << " is "<< itsSBaverage << " or " << SBaverageAlt << " Standard deviation " << itsSBstdev << " " << itsSBstdev/itsSBaverage << " triggerlevel " << (itsTriggerThreshold-itsSBaverage)/itsSBstdev << std::endl;
			return std::string(output);
		}
		
        




		std::string SubbandTrigger::FoundTriggers(){
			return itsFoundTriggers;
		}
		
		bool SubbandTrigger::SendTriggerMessage(struct triggerEvent trigger){
			send_data = (char*) &trigger;
			int n = sendto(sock, send_data, sizeof(struct triggerEvent), 0,
				   (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
			if (n  < 0){
				return false;
		    }
			return true;
	    }


		bool SubbandTrigger::dedisperseData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime,bool Transposed){
            // Mostly the same as process data, but now without triggering
			bool pulsefound=false;
			if( ++itsSequenceNumber!=sequenceNumber && false) { 
				std::cerr << "discontinous data" << std::endl; 
			    return false; 
			} else {
				itsFoundTriggers="";
				itsBlockNumber++;
			    if(verbose) {	
                    std::cout << "Processing block " << sequenceNumber << std::endl;
                }

                blocksum =0.0;
                validsamples=0;
				zerocounter=0;
				SBaverage=0;
				SBstdev=0;
				SBsumsamples=0;
                channeltimeindex=-itsTotNrChannels;



				for(int time=0; time<itsNrSamples; time++){
					//std::cout << std::endl << " time " << time ;
					totaltime=itsSequenceNumber*itsNrSamples+time;
					rest=totaltime%itsBufferLength;
                    channeltimeindex+=itsTotNrChannels;
                    channelindex=channeltimeindex+itsStartChannel+itsNrChannels;
                    
					for(int channel=itsNrChannels-1; channel>0; channel--){
                        channelindex--;
						
						DeDispersedBuffer[(totaltime+dedispersionoffset[channel])%itsBufferLength]+=data[channelindex];
					    
						
                        
                    
                    } // for channel
                    //***  channel=0; *******
                    
                    channelindex--;
                    
                    value = data[channelindex];
                        
                    DeDispersedBuffer[rest]+=value;
                    blocksum+=DeDispersedBuffer[rest];
                    DeDispersed[rest]=DeDispersedBuffer[rest];
                    float subsum=0;
                    if(rest >= itsIntegrationLength){
                        for(int it=rest; it>rest-itsIntegrationLength; it--){subsum+=DeDispersed[it];}
                    } else {
                        for(int it=rest; it>rest-itsIntegrationLength; it--){
                            if(it>=0){
                                subsum+=DeDispersed[it];
                            }
                            else {
                                subsum+=DeDispersed[it+itsBufferLength];
                            }
                        }
                        
                    }
                    SBstdev+=(subsum-itsSBaverage)*(subsum-itsSBaverage);
                    SBsumsamples++;
                    SBaverage+=subsum;
                    DeDispersedBuffer[rest]=0;

				} // for time
				
				
				
				
				
				// Update class parameters;
				//itsSBstdev = SBstdev;
				
				if(SBsumsamples>0){
					itsSBaverage = SBaverage/SBsumsamples;
				    //itsSBaverage=blocksum/SBsumsamples;
					SBstdev/=SBsumsamples;
					
				}
				itsSBstdev=sqrt(SBstdev);
				itsTotalZeros += zerocounter;
				float SBaverageAlt = blocksum/SBsumsamples;
				//for(int it=rest;it>rest-SBsumsamples;it--){
				//	SBaverageAlt += DeDispersed[rest];
				//}
				//SBaverageAlt*=itsIntegrationLength;
				//SBaverageAlt/=SBsumsamples;
				if(verbose){
                    std::cout << "verbose "  << verbose << endl;
					std::cout << "verbose" << verbose << "DM " << itsDM << ", SBaverage over "<< SBsumsamples << "samples at block "<<  itsBlockNumber << " / " << itsSequenceNumber <<" at frequency " << itsStartFreq << " is "<< itsSBaverage << " or " << SBaverageAlt << " Standard deviation " << itsSBstdev << " " << itsSBstdev/itsSBaverage << " triggerlevel " << (itsTriggerThreshold-itsSBaverage)/itsSBstdev << std::endl;
				}
				//char output[200];
				//std::string output;
				
				itsPrevTriggerLevel = (itsTriggerThreshold-itsSBaverage)/itsSBstdev;
				itsTriggerThreshold = itsSBaverage+itsTriggerLevel*itsSBstdev;
                itsSBaverageVector.push_back(itsSBaverage);
                itsSBstdevVector.push_back(itsSBstdev);
        	}
			
            return pulsefound;
	    }		
			
         
    
        /*			
		 SubbandTrigger::setNrChannels(int NrChannels){ itsNrChannels = NrChannels; }
		 SubbandTrigger::setNrSamples(int NrSamples){ itsNrSamples = NrSamples; }
		 //inline void setDM( float DM ){ itsDM = DM; }
		 SubbandTrigger::setIntegrationLength( int IntegrationLength ){ itsIntegrationLength = IntegrationLength; }
		 SubbandTrigger::setTriggerlevel( float TriggerLevel){ itsTriggerlevel = TriggerLevel; }
		 SubbandTrigger::setStartFreq( float StartFreq){ itsStartFreq = StartFreq; }
		 SubbandTrigger::setFreqResolution(float FreqResolution) { itsFreqResolution = FreqResolution; }
		 SubbandTrigger::setTimeResolution(float TimeResolution) { itsTimeResolution = TimeResolution; }
		 */	    
		//Vector commands: size, resize, front, back, at, [], push_back, pop_back, insert, erase, swap, clear	
		
		


//##################################################################################

/*
Data properties:
float* itsDataStart;
float* itsDataEnd;
int nrChannels;
int nrSamples;
int nrOfDivisions;  of the baseline (if it's noncontinuous)
int ChannelsPerDivision;
int cutlevel;

Methods:

Calculate baseline

vector <float> baseline;



Calculate interpolated baseline

vector <float> interpolated_baseline;

Check for spiked RFI
vector <int> badSamples;


Check for narrow frequency RFI
vector <int> badChannels;


Cleaning:

Clean narrow Frequency:
    replace by 1
    replace by next frequency
    replace by clean frequency
    put entry in flagged samples

Clean timeseries:
    replace by 1
    replace by next time stamp
    replace by clean timestamp
    put entry in flagged samples

Create Clean spectrum ...



Save timeseries (append)
    string outFileName;
*/








        //#################################################################################

        RFIcleaning::RFIcleaning(float* data_start, float* data_end, int nrchannels, int chanspersubband, int nrsamples, float cutlevel, int number_of_divisions, std::string outFileName) {
                itsDataStart=data_start;
                itsDataEnd=data_end;
                itsNrChannels=nrchannels;
                itsNrSamples=nrsamples;
                itsChansPerSubband=chanspersubband;
                nrOfDivisions=number_of_divisions;
                ChannelsPerDivision=itsNrChannels/nrOfDivisions;
                cutlevel=cutlevel;
                baseline.resize(itsNrChannels);
                IPbaseline.resize(itsNrChannels); //interpolated baseline
                sqrBaseline.resize(itsNrChannels);
                sqrDivBaseline.resize(itsNrChannels);
                sqrDivBaselineSort.resize(itsNrChannels);
                timeseries.resize(itsNrSamples);
                avgtimeseries.resize(itsNrSamples);
                avgtimeseriesstream.resize(itsNrSamples);
                sqrTimeseries.resize(itsNrSamples);
                
        }


        bool RFIcleaning::calcBaseline(){
             float * it;
             #ifdef _OPENMP
                #pragma omp parallel for private(it)
             #else
             #endif // _OPENMP 
             for(int ch=0; ch<itsNrChannels; ch++){
                    it=itsDataStart+ch;
                    baseline[ch]=0.0;
                    for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                        baseline[ch]+=*it;
                    }
                }
             return true;
        }
        
        
        
        bool RFIcleaning::calcSqrBaseline(){
             float * it;
            #ifdef _OPENMP
                #pragma omp parallel for private(it)
            #else
            #endif // _OPENMP
             for(int ch=0; ch<itsNrChannels; ch++){
                    it=itsDataStart+ch;
                    sqrBaseline[ch]=0.0;
                    for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                        sqrBaseline[ch]+=(*it)*(*it);
                    }
                }
            return true;
        }
        
        bool RFIcleaning::calcTimeseries(){
             // calculate average timeseries
             float * it;
             float * vec_end=itsDataStart;
             it=itsDataStart;
            #ifdef _OPENMP
                #pragma omp parallel for private(it,vec_end)
            #else
            #endif // _OPENMP
             for(int sa=0; sa<itsNrSamples; sa++){
                    it=itsDataStart+sa*itsNrChannels;
                    vec_end+=itsNrChannels;
                    // cout << it << " " << vec_end << endl;
                    timeseries[sa]=0.0;
                    for ( ; it<vec_end; it++ ) {
                        timeseries[sa]+=*it;
                    }
                }
            return true;
        }
        
        bool RFIcleaning::calcAverageTimeseries(){
             // calculate average timeseries
             float * it;
             float * vec_end=itsDataStart;
             it=itsDataStart;
             #ifdef _OPENMP
                #pragma omp parallel for private(it,vec_end)
            #else
            #endif // _OPENMP
             for(int sa=0; sa<itsNrSamples; sa++){
                    it=itsDataStart+sa*itsNrChannels;
                    vec_end+=itsNrChannels;
                    avgtimeseries[sa]=0.0;
                    // cout << it << " " << vec_end << endl;
                    for ( ; it<vec_end; it++ ) {
                        avgtimeseries[sa]+=*it;
                    }
                    avgtimeseries[sa]/=itsNrChannels;
                }
            return true;
        }
        
        bool RFIcleaning::calcAverageTimeseriesStream(int startchan, int endchan){
             // calculate average timeseries
             float * it;
             float * vec_end=itsDataStart+endchan;
             it=itsDataStart;
    /*         #ifdef _OPENMP
                #pragma omp parallel for private(it,vec_end)
            #else
            #endif // _OPENMP
    */         for(int sa=0; sa<itsNrSamples; sa++){
                    it=itsDataStart+sa*itsNrChannels+startchan;
                    avgtimeseriesstream[sa]=0.0;
                    for ( ; it<vec_end; it++ ) {
                        avgtimeseriesstream[sa]+=*it;
                    }
                    vec_end+=itsNrChannels;
                    avgtimeseriesstream[sa]/=(endchan-startchan);//itsNrChannels;
                }
            return true;
        }




        bool RFIcleaning::averageTimeseries(){
            it1=timeseries.begin();
            it2=avgtimeseries.begin();
            if(timeseries.size() != avgtimeseries.size()){
                cerr << " timeseries and avgtimeseries vector not of equal size ";
                return false;
            }
            vec1_end=timeseries.end();
            while(it1<vec1_end){
                (*it2)=(*it1)/itsNrChannels;
                it1++;
                it2++;
            }
            return true;
        }
        
        bool RFIcleaning::calcSqrTimeseries(){
             float * it;
             float * vec_end=itsDataStart;            
             it=itsDataStart;
             #ifdef _OPENMP
                #pragma omp parallel for private(it,vec_end)
            #else
            #endif // _OPENMP
             for(int sa=0; sa<itsNrSamples; sa++){
                    it=itsDataStart+sa*itsNrChannels;
                    vec_end+=itsNrChannels;
                    // cout << it << " " << vec_end << endl;
                    sqrTimeseries[sa]=0.0;
                    for ( ; it<vec_end; it++ ) {
                        sqrTimeseries[sa]+=(*it)*(*it);
                    }
                }
            return true;
        }
        
        

        int RFIcleaning::calcBadChannels(int cutlevel, bool useInterpolatedBaseline=false) {
                // divide sqrBaseline by baseline^2
                if(useInterpolatedBaseline){
                    it1=IPbaseline.begin();
                    vec1_end=IPbaseline.end();
                } else {
                    it1=baseline.begin();
                    vec1_end=baseline.end();
                }
                it2=sqrBaseline.begin();
                vec2_end=sqrBaseline.end();
                it3=sqrDivBaseline.begin();
                

                while( it1 != vec1_end ) {
                    *it3 = *it2 / ((*it1)*(*it1));
                    ++it1; ++it2; ++it3;
                }
            
                copy(sqrDivBaseline.begin(), sqrDivBaseline.end(), sqrDivBaselineSort.begin());
                sort(sqrDivBaselineSort.begin(),  sqrDivBaselineSort.end() );
                
           // Find top 10 and bottom 10 percent of stdOverPower and determine average and standarddeviation of the rest of the values.
                double sum=0;
                double sqr=0;
                int val;
                int nrelem=0;
                float sortStd;
                for(it2=sqrDivBaselineSort.begin()+sqrDivBaselineSort.size()/10; it2<sqrDivBaselineSort.end()-sqrDivBaselineSort.size()/10; it2++){

                        sum+=*it2;
                        sqr+=(*it2)*(*it2);
                        nrelem++;
                }
                double average=sum/nrelem;
                sortStd=sqrt(sqr/nrelem-average*average);
    
                //cout << " sum " << sum << " nrelem" << nrelem << " average " << average << " std " << sortStd << " sqr " << sqr << " ";

                chanIdVal tempIdVal;
                badChans.resize(0);
                if(!isnan(sortStd)){
                    float limit=average+cutlevel*sortStd;
                    float lowerlimit=average-cutlevel*sortStd;
                    int index=0;
                    for(it2=sqrDivBaseline.begin(); it2<sqrDivBaseline.end(); it2++){
                        if(isnan(*it2)){ 
                            cout << " nan " <<  index;
                        }
                        if(*it2 > limit || *it2 < lowerlimit) {
                            tempIdVal.id=index;
                            val=(int) round(((*it2)-average)/sortStd);
                            tempIdVal.val=val;
                            badChans.push_back(tempIdVal);
                        }
                        index++;
                    }
                }



            return badChans.size();
        }

        bool RFIcleaning::checkDataloss(int requiredsamples){
            int lossCount=0;
            for(int sa=0; sa<itsNrSamples; sa++){
                if(*(itsDataStart+sa*itsNrChannels)==0.0){
                    lossCount++;
                }
            }
            if(lossCount>=requiredsamples){
                cout << "Dataloss occured" << endl;
                return true;
            }
            else{
                return false;
            }
        }




        int RFIcleaning::calcBadSamples(int cutlevel) {
// Channel collapse:
        

            int subdiv=8;// subdivisions. into how many parts to split the data
            int reqsubdiv=2; // required subdivisions. how many parts should show a peak
            int channelsPerPart=itsNrChannels/subdiv;
            vector<float> collapsedData[subdiv];
            vector<float> collapsedDataSort[subdiv];
            vector<int> badcollapsedSamples;
            badSamples.resize(0);
            float sum=0;
            float sqr=0;
            int val;
            int nrelem=0;
            float sortStd;
            float average;
            float limit;
            int index;
            vector<int>::iterator it;
            vector<float>::iterator it2;
            int startchannel;
            int stopchannel;


            for(int div=0; div<subdiv; div++){
                //collapsedData[div]=channelcollapse(data_start, data_end, nrchannels, div*channelsPerPart, (div+1)*channelsPerPart, nrsamples);
                startchannel=div*channelsPerPart;
                stopchannel=(div+1)*channelsPerPart;

                collapsedData[div].resize(itsNrSamples);
                #ifdef _OPENMP
                    #pragma omp parallel for 
                #else
                #endif // _OPENMP                
                for(int sa=0; sa<itsNrSamples; sa++){
                    collapsedData[div][sa]=0.0;
                    float* it_start=itsDataStart+sa*itsNrChannels;
                    for(int ch=startchannel; ch<stopchannel; ch++){
                        float* it=it_start+ch;
                        collapsedData[div][sa]+=*it;
                    }
                }
                collapsedDataSort[div].resize(collapsedData[div].size());
                copy(collapsedData[div].begin(),collapsedData[div].end(),collapsedDataSort[div].begin());
                sort(collapsedDataSort[div].begin(),collapsedDataSort[div].end());
                sum=0;
                sqr=0;
                val;
                nrelem=0;
                for(it2=collapsedDataSort[div].begin()+collapsedDataSort[div].size()/10; it2<collapsedDataSort[div].end()-collapsedDataSort[div].size()/10; it2++){
                    sum+=*it2;
                    sqr+=(*it2)*(*it2);
                    nrelem++;
                }
                average=sum/nrelem;
                sortStd=sqrt(sqr/nrelem-average*average);
                //cout << "sum" << sum << "nrelem" << nrelem << "average " << average << "\n std " << sortStd;
                limit=average+cutlevel*sortStd;
                index=0;
                for(it2=collapsedData[div].begin(); it2<collapsedData[div].end(); it2++){
                    if(*it2 > limit) {
                        badcollapsedSamples.push_back(index);
                    }
                    index++;
                } 
            }
            
            sort(badcollapsedSamples.begin(),badcollapsedSamples.end()); 
            int prevvalue=-1;
            int count=0;
            for(it=badcollapsedSamples.begin(); it<badcollapsedSamples.end();it++){
                //cout << *it << " " << count << " " << reqsubdiv << " " << prevvalue << " " << badSamples.size() << " \t";
                if( *it == prevvalue){
                    count++;
                } else {
                    if(count >= reqsubdiv) {
                        badSamples.push_back(prevvalue);
                    }
                    count=1;
                    prevvalue=*it;
                }
            }

            // check if any value appears at least reqsubdiv times

            return badSamples.size();
        }
        
        bool RFIcleaning::cleanChannels(std::string method) {
        /*  Replaces the values in the channels in the badChans list
            according to the method. Options:
            method "1" replaces values with value 1
            method "prevfreq" replaces values with that from a channel closeby
            method "clean" replaces with values from a clean spectrum.
        */
            
            int ch;                
            int choffset;


            if ( method == "1" ) {
                #ifdef _OPENMP
                    #pragma omp parallel for private(ch,choffset)
                #else
                #endif // _OPENMP
                for(int i=0; i<badChans.size(); i++){
                    ch=badChans[i].id;
                    float* it=itsDataStart+ch;
                    for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                        *it = 1.0;
                    }
                }
            } else if ( method == "0" ) {
                #ifdef _OPENMP
                    #pragma omp parallel for private(ch)
                #else
                #endif // _OPENMP
                for(int i=0; i<badChans.size(); i++){
                    ch=badChans[i].id;
                    float* it=itsDataStart+ch;
                    for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                        *it = 0.0;
                    }
                }
            } else if ( method == "prevfreq" ){
                choffset=-3*itsNrChannels;
                if(ch<3){
                    choffset=3;
                }
                if((ch-3)%itsChansPerSubband==0){
                    choffset=-1;
                }
                #ifdef _OPENMP
                    #pragma omp parallel for private(ch)
                #else
                #endif // _OPENMP
                for(int i=0; i<badChans.size(); i++){
                    ch=badChans[i].id;
                    float* it=itsDataStart+ch;
                    for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                        *it=*(it+choffset);
                    }
                }
            }
            

            return true;
        }
        
        int  RFIcleaning::cleanChannel0(std::string method) {
        /*  Replaces the values in the channels in the badChans list
            according to the method. Options:
            method "1" replaces values with value 1
            method "prevfreq" replaces values with that from a channel closeby
            method "clean" replaces with values from a clean spectrum.
        */
            
            int ch;                
            int choffset;
            #ifdef _OPENMP
                #pragma omp parallel for private(ch,choffset)
            #else
            #endif // _OPENMP
            for(int ch=0; ch<itsNrChannels; ch+=itsChansPerSubband){
                float* it=itsDataStart+ch;
                for ( ; it<itsDataEnd; it+=itsNrChannels ) {
                    if ( method == "1" ) {
                        *it = 1.0;
                    } else if ( method == "prevfreq" ){
                        choffset=-3*itsNrChannels;
                        if(ch<3){
                            choffset=3;
                        }
                        if((ch-3)%itsChansPerSubband==0){
                            choffset=-1;
                        }
                        *it=*(it+choffset);
                    }
                }
            }

            return itsChansPerSubband/itsNrChannels;
        }
        

                
        bool RFIcleaning::cleanSamples(std::string method){
            int sam;
            int samoffset=-3;

            #ifdef _OPENMP
                #pragma omp parallel for private(sam)
            #else
            #endif // _OPENMP
        
            for(int i=0; i<badSamples.size(); i++){
   //             validsamples--;                
                sam=badSamples[i];
                if(sam<3){ //beware, this is hardcoded
                    samoffset=3;
                } else{
                    samoffset=-3;
                }
                if( sam < itsNrSamples ){
                    for(int ch=0; ch<itsNrChannels ; ch++){ 
                        if ( method == "1" ) {
                            *(itsDataStart+sam*itsNrChannels+ch)=1.0;
                        } else if (method == "previous" ) {
                            *(itsDataStart+sam*itsNrChannels+ch)=*(itsDataStart+(sam+samoffset)*itsNrChannels+ch);
                        } 
                    }
                }
            }
            return true;
        }

        bool RFIcleaning::printBadSamples(){
            cout << " Bad samples " ;
            for(int i=0; i<badSamples.size(); i++){
                cerr << badSamples[i] << " ";
            }
            cerr << endl;
            return true;  
        }

        bool RFIcleaning::printBadChannels(){
            cout << " Bad Channels ";
            for(int i=0; i<badChans.size(); i++){
                cout << badChans[i].id << " " << badChans[i].val << " ";
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::printData(){
            float *it=itsDataStart;
            cout << " Data from ";
            cout << *it << " " << it << " ";
            while(++it!=itsDataEnd){
                cout << *it << " " << it << " ";
            }
            cout << endl;
            return 0;
        }

        bool RFIcleaning::printBaseline(bool printindex=true){
            cout << " Baseline     ";
            if(printindex){
                for(int i=0; i<baseline.size(); i++){
                    cout << i << " " << baseline[i] << " " ; 
                }
            } else {
                for(int i=0; i<baseline.size(); i++){
                    cout << baseline[i] << " " ; 
                }
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::printTimeseries(bool printindex=true){
            cout << " Timeseries     ";
            if(printindex){
                for(int i=0; i<timeseries.size(); i++){
                    cout << i << " " << timeseries[i] << " " ; 
                }
            } else {
                for(int i=0; i<timeseries.size(); i++){
                    cout << timeseries[i] << " " ; 
                }
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::printAverageTimeseries(bool printindex=true){
            cout << " Average Timeseries     ";
            if(printindex){
                for(int i=0; i<avgtimeseries.size(); i++){
                    cout << i << " " << avgtimeseries[i] << " " ; 
                }
            } else {
                for(int i=0; i<avgtimeseries.size(); i++){
                    cout << avgtimeseries[i] << " " ; 
                }
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::printAverageTimeseriesStream(bool printindex=true){
            cout << " Average Timeseries     ";
            if(printindex){
                for(int i=0; i<avgtimeseriesstream.size()/10; i++){
                    cout << i << " " << avgtimeseriesstream[i] << " " ; 
                }
            } else {
                for(int i=0; i<avgtimeseriesstream.size()/10; i++){
                    cout << avgtimeseriesstream[i] << " " ; 
                }
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::printSqrTimeseries(bool printindex=true){
            cout << " Square sum Timeseries     ";
            
            if(printindex){
                for(int i=0; i<sqrTimeseries.size(); i++){
                    cout << i << " " << sqrTimeseries[i] << " " ; 
                }
            } else {
                for(int i=0; i<sqrTimeseries.size(); i++){
                    cout << sqrTimeseries[i] << " " ; 
                }
            }
            cout << endl;
            return true;  
        }

        bool RFIcleaning::divideBaseline(bool useInterpolatedBaseline=false){
            float * dataptr = itsDataStart;
            if(useInterpolatedBaseline){
               it1=IPbaseline.begin();
               vec1_end=IPbaseline.end();
            } else {
                it1=baseline.begin();
                vec1_end=baseline.end();
            }
            it2=baseline.begin();
            while(dataptr!=itsDataEnd){
                *dataptr /= (*it2/itsNrSamples);
                dataptr++;
                it2++;
                if(it2==vec1_end){
                    it2=baseline.begin();
                }
            }
        }
        
        bool RFIcleaning::subtractAverageTimeseries(){
            float * dataptr = itsDataStart;
            it1=avgtimeseries.begin();
            vec1_end=avgtimeseries.end();
            it2=avgtimeseries.begin();
            int chindex=0;
            //cout << it1 << " " << vec1_end << endl;
            while(dataptr!=itsDataEnd){
                while(chindex<itsNrChannels && dataptr!=itsDataEnd){
                    *dataptr -= *it2;
                    dataptr++;
                    chindex++;
                }
                chindex=0;
                
                it2++;
                if(it2==vec1_end){
                    it2=avgtimeseries.begin();
                }
            }
            return true;
        }
        
        bool RFIcleaning::subtractAverageTimeseriesStream(int startchan, int endchan){
             float * it;
             float * vec_end=itsDataStart+endchan;
             it=itsDataStart;
    /*         #ifdef _OPENMP
                #pragma omp parallel for private(it,vec_end)
            #else
            #endif // _OPENMP
    */         for(int sa=0; sa<itsNrSamples; sa++){
                    it=itsDataStart+sa*itsNrChannels+startchan;
                    for ( ; it<vec_end; it++ ) {
                        *it-=avgtimeseriesstream[sa];

                    }
                    vec_end+=itsNrChannels;
                }
            return true;
            
            /*
            float * dataptr = itsDataStart+startchan;
            it1=avgtimeseriesstream.begin();
            vec1_end=avgtimeseriesstream.end();
            it2=avgtimeseriesstream.begin();
            int chindex=0;
            //cout << it1 << " " << vec1_end << endl;
            while(dataptr<itsDataEnd){
                while(chindex<endchan && dataptr<itsDataEnd){
                    *dataptr -= *it2;
                    dataptr++;
                    chindex++;
                }
                chindex=startchan;
                dataptr+=itsNrChannels-endchan+startchan; 
                
                it2++;
                if(it2==vec1_end){
                    it2=avgtimeseriesstream.begin();
                }
            }
            */
            return true;
        }

        bool RFIcleaning::writeBadSamples(ofstream * fsfile, int blockNr){
            fsfile->write((const char*)&blockNr, sizeof(blockNr));
            size=badSamples.size();
            fsfile->write((const char*)&size, sizeof(size));
            fsfile->write((const char*)&badSamples[0], size * sizeof(badSamples[0]));
        }


        bool RFIcleaning::writeBadChannels(ofstream * fcfile, int blockNr){
            fcfile->write((const char*)&blockNr, sizeof(blockNr));
            size=badChans.size();
            fcfile->write((const char*)&size, sizeof(size));
            fcfile->write((const char*)&badChans[0], size * sizeof(badChans[0]));
        }


        bool RFIcleaning::writeBaseline(ofstream * blfile, int blockNr){
            float blockNrFloat=(float) blockNr;
            blfile->write((const char*)&blockNrFloat, sizeof(blockNrFloat));
            float sizefloat=(float) baseline.size();
            blfile->write((const char*)&sizefloat, sizeof(sizefloat));
            blfile->write((const char*)&baseline[0], baseline.size() * sizeof(baseline[0]));
        }


        bool RFIcleaning::writeSqrTimeseries(ofstream * stfile, int blockNr){
            float blockNrFloat=(float) blockNr;
            stfile->write((const char*)&blockNrFloat, sizeof(blockNrFloat));
            float sizefloat=(float) sqrTimeseries.size();
            stfile->write((const char*)&sizefloat, sizeof(sizefloat));
            stfile->write((const char*)&sqrTimeseries[0], sqrTimeseries.size() * sizeof(sqrTimeseries[0]));
        }




        RFIcleaning::~RFIcleaning(){

            // RFI cleaning destructor
        }
		
				//return true;
	}; // namespace analysis
	
	
}; // namespace FRAT

