//#  FRATcoincidence.cc: Coincidence and subband scan for FRAT project
//#
//#  Copyright (C) 2009-2010
//#  Sander ter Veen (s.terveen at astro.ru.nl )
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
				while ((runindex < FRAT_TASK_BUFFER_LENGTH) && (trigBuffer[runindex].Time>= reftime)){
					//check if current time is higher than the reftime. Stop if that is not the case
					if(trigBuffer[runindex].Time!=0){  
						for (i=0; i<nfound; i++){ 
							if (foundSBs[i] == trigBuffer[runindex].SBnr) {
								//i; //prevents i to be nfound so you would trigger on one less.
								//if(i==nfound-1){i++;};
								//std::cout << "i = " << i << std::endl;
								break; //break the for-loop;
							};
						};
						if (i == nfound) { 
							if (nfound+2 > nChannles) { 
								std::cout << "Subbands found:" ;
								for (i=0; i<nfound; i++){
									std::cout << " " << foundSBs[i];
								}
								std::cout << " " << trigBuffer[runindex].SBnr << std::endl;
								return true; 
							}; //startindex; };
							foundSBs[nfound] = trigBuffer[runindex].SBnr;
							std::cout << "Subbands found:" ;
							for (i=0; i<nfound; i++){
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
			// trigBuffer[newindex].SeqNr     = trigger.itsSeqNr;
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
		
		
		SubbandTrigger::SubbandTrigger(int StreamID, int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, long startBlock, int IntegrationLength, bool InDoPadding, bool InUseSamplesOr2, int obsID, int beam)
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
			
			itsStreamID=StreamID;
			itsDM = DM;
			itsIntegrationLength=IntegrationLength; 
			itsTriggerLevel = TriggerLevel;
			itsStartFreq = StartFreq;
			itsReferenceFreq = ReferenceFreq;
			itsFreqResolution = FreqResolution;
			itsTimeResolution = TimeResolution;
			itsSequenceNumber = startBlock-1;
			itsBlockNumber = 0;
			itsTotalValidSamples = 0;
			itsSBaverage=1e22;
			itsSBstdev=1e10;
			itsTriggerThreshold=10;
			itsTotalZeros=0;
			itsBufferLength=1000;
			InitDedispersionOffset();
			verbose=true;
			hostname="127.0.0.1";
			send_data = new char[sizeof(struct triggerEvent)];
			host= (struct hostent *) gethostbyname(hostname);
			
			DeDispersedBuffer.resize(itsBufferLength, 0.0);
			SumDeDispersed.resize(itsBufferLength, 0.0);
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{
				perror("socket");
				exit(1);
			}
			
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
			server_addr.sin_addr = *((struct in_addr *)host->h_addr);
			bzero(&(server_addr.sin_zero),8);
			
			
			
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
			
		}
        
        
        SubbandTrigger::SubbandTrigger(int StreamID, int ChannelsPerSubband, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, std::vector<float> FREQvalues, int StartChannel, int NrChannels, int TotNrChannels,  float FreqResolution, float TimeResolution, long startBlock, int IntegrationLength, bool InDoPadding, bool InUseSamplesOr2, int obsID, int beam)
		{
			
			// To be added to input: IntegrationLength, UseSamplesOr2, DoPadding
			DoPadding=InDoPadding;
			UseSamplesOr2=InUseSamplesOr2;
			itsNrChannels= NrChannels; // channels for this stream
            itsTotNrChannels = TotNrChannels; // total channels in this file (nrSBs * ChannelsPerSubband )
            itsChannelsPerSubband=ChannelsPerSubband;
			itsNrSamples = NrSamples;
            itsFREQvalues = FREQvalues;
			if(UseSamplesOr2){
				itsNrSamplesOr2 = NrSamples|2;
			} else {
				itsNrSamplesOr2 = NrSamples;
			}
			
			itsStreamID=StreamID;
			itsDM = DM;
			itsIntegrationLength=IntegrationLength; 
			itsTriggerLevel = TriggerLevel;
			itsStartChannel = StartChannel;
            //itsNrChannels = TotNrChannels;
			itsReferenceFreq = ReferenceFreq;
			itsFreqResolution = FreqResolution;
			itsTimeResolution = TimeResolution;
			itsSequenceNumber = startBlock-1;
			itsBlockNumber = 0;
			itsTotalValidSamples = 0;
			itsSBaverage=1e22;
			itsSBstdev=1e10;
			itsTriggerThreshold=10;
			itsTotalZeros=0;
			itsBufferLength=1000;
			InitDedispersionOffset(FREQvalues);
			verbose=true;
			hostname="127.0.0.1";
			send_data = new char[sizeof(struct triggerEvent)];
			host= (struct hostent *) gethostbyname(hostname);
			
			DeDispersedBuffer.resize(itsBufferLength, 0.0);
			SumDeDispersed.resize(itsBufferLength, 0.0);
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{
				perror("socket");
				exit(1);
			}
			
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
			server_addr.sin_addr = *((struct in_addr *)host->h_addr);
			bzero(&(server_addr.sin_zero),8);
			
			
			
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
			//SumDeDispersed.resize(itsBufferLength, 0.0);
			
			
			
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
		
		bool SubbandTrigger::processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck* cc, int CoinNr, int CoinTime,bool Transposed){
			bool pulsefound=false;
			if( ++itsSequenceNumber!=sequenceNumber && false) { 
				std::cerr << "discontinous data" << std::endl; 
			    return false; 
			} else {
				itsFoundTriggers="";
				
				std::cout << "Processing block " << sequenceNumber << std::endl;
				float value;
				int totaltime;
				int rest;
				float blocksum =0.0;
				int validsamples=0;
				int zerocounter=0;
				itsBlockNumber++;
				float SBaverage=0;
				float SBstdev=0;
				int SBsumsamples=0;
				for(int time=0; time<itsNrSamples; time++){
					//std::cout << std::endl << " time " << time ;
					totaltime=itsSequenceNumber*itsNrSamples+time;
					rest=totaltime%itsBufferLength;
					for(int channel=itsNrChannels-1; channel>=0; channel--){
						//value = data[channel*(itsNrSamplesOr2)+time];
						value = data[time*itsTotNrChannels+itsStartChannel+channel];
						
                        if(DoPadding){value = FloatSwap(value);} //outside BG/P
						if( !isnan( value ) && value!=0 ) {
							//value = 0;
							validsamples++;
							// blockvalidsamples[fc]++;
					    }
					    else { value = 0; }
						if(value == 0 && channel % itsNrChannels ==1) { zerocounter++; std::cout << 0;}
					    blocksum+=value;
						
						
						
					    
						
						
						if(channel!=0){
							DeDispersedBuffer[(totaltime+dedispersionoffset[channel])%itsBufferLength]+=value;
						} else {
							DeDispersedBuffer[rest]+=value;
							SumDeDispersed[rest]=DeDispersedBuffer[rest];
							float subsum=0;
							for(int it=rest; it>rest-itsIntegrationLength; it--){subsum+=SumDeDispersed[it];}
							SBstdev+=(subsum-itsSBaverage)*(subsum-itsSBaverage);
							SBsumsamples++;
							SBaverage+=subsum;
							if(subsum>itsTriggerThreshold) {
								//ADD TRIGGER ALGORITHM OR FUNCTION
								if(totaltime+itsReferenceTime-trigger.time>5){
									//new trigger
									//trigger.average[fc]/=triggerlength[fc];
									//trigger[fc].time=totaltime+fc*channels_p*STRIDE; //correction for dispersion
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
								SendTriggerMessage(trigger);
								
								int latestindex = cc->add2buffer(trigger);
								
								if(cc->coincidenceCheck(latestindex, CoinNr, CoinTime)) {
									
									
									float triggerstrength = (trigger.max-itsSBaverage)/itsSBstdev;
									char output2[400];
									std::string output;
									
									//sprintf(output,"%f4.2 %d %e %e %e %e %e" ,itsDM,itsSequenceNumber,1000*itsStartFreq,itsSBaverage,itsSBstdev,itsSBstdev/itsSBaverage,itsPrevTriggerLevel);

									sprintf(output2,"COINCIDENCE TRIGGER FOUND with DM %f at block %i / %i time: %f strength %f \n",itsDM,itsSequenceNumber,itsBlockNumber,totaltime*itsTimeResolution,triggerstrength);
									itsFoundTriggers=itsFoundTriggers+std::string(output2);
									std::cout << "\n\n\n\n COINCIDENCE TRIGGER FOUND with DM " << itsDM << " at block" << itsSequenceNumber << " / " << itsBlockNumber << " time: " << totaltime*itsTimeResolution   << " strength " << triggerstrength << "\n\n";	
									//usleep(1000000);
									pulsefound=true;
								}
								
								
								
								
							}
							DeDispersedBuffer[rest]=0;

						}
						
						
						
						
					} // for channel
					
					
					
					
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
				//	SBaverageAlt += SumDeDispersed[rest];
				//}
				//SBaverageAlt*=itsIntegrationLength;
				//SBaverageAlt/=SBsumsamples;
				if(verbose){
					std::cerr << "DM " << itsDM << ", SBaverage over "<< SBsumsamples << "samples at block "<<  itsBlockNumber << " / " << itsSequenceNumber <<" at frequency " << itsStartFreq << " is "<< itsSBaverage << " or " << SBaverageAlt << " Standard deviation " << itsSBstdev << " " << itsSBstdev/itsSBaverage << " triggerlevel " << (itsTriggerThreshold-itsSBaverage)/itsSBstdev << std::endl;
				}
				char output[200];
				//std::string output;
				
				itsPrevTriggerLevel = (itsTriggerThreshold-itsSBaverage)/itsSBstdev;
				itsTriggerThreshold = itsSBaverage+itsTriggerLevel*itsSBstdev;
			}
			
			
			return pulsefound;
		}
		
		int SubbandTrigger::CalculateBufferSize(){
			itsBufferLength=2*(dedispersionoffset[itsNrChannels]+itsIntegrationLength+itsNrSamples);
			return itsBufferLength;
		}

		
		void SubbandTrigger::InitDedispersionOffset(){
			dedispersionoffset.resize(itsNrChannels);
            float freq1=itsStartFreq-0.5*itsNrChannels*itsFreqResolution;
			for(int channel=0;channel<itsNrChannels;channel++){
				
				float freq2=freq1+channel*itsFreqResolution;
				float offset=4.15e-3*itsDM*(1/(freq1*freq1)-1/(freq2*freq2))/itsTimeResolution;
				// printf("%f ",offset);
				
				// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
				
				dedispersionoffset[channel]=(int)offset;
			}
			float freqA=itsReferenceFreq+0.5*itsFreqResolution;
			float freqB=itsStartFreq+0.5*itsFreqResolution;
			float offset=4.15e-3*itsDM*(1/(freqA*freqA)-1/(freqB*freqB))/itsTimeResolution;
			itsReferenceTime=(int) offset;
			CalculateBufferSize();
		}
        
        
        void SubbandTrigger::InitDedispersionOffset(std::vector<float> FREQvalues){
			dedispersionoffset.resize(itsNrChannels);
            // Frequency= FREQvalues[itsStartChannel+currentChannel]
            printf("Setting offsets with timeresolution %f", itsTimeResolution);
            float freq1=FREQvalues[itsStartChannel]-0.5*itsChannelsPerSubband*itsFreqResolution;
			for(int channel=0;channel<itsNrChannels;channel++){
				
				float freq2=FREQvalues[itsStartChannel+channel]; //freq2=freq1+channel*itsFreqResolution;
				float offset=4.15e-3*itsDM*(1/(freq1*freq1)-1/(freq2*freq2))*1e18/itsTimeResolution;
                printf("%f ",offset);
				
				// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
				
				dedispersionoffset[channel]=(int)offset;
			}
			float freqA=itsReferenceFreq+0.5*itsFreqResolution;
			float freqB=FREQvalues[itsStartChannel]+0.5*itsFreqResolution;
			float offset=4.15e-3*itsDM*(1/(freqA*freqA)-1/(freqB*freqB))/itsTimeResolution;
			itsReferenceTime=(int) offset;
			CalculateBufferSize();
		}
		
		bool SubbandTrigger::makeplotBuffer(std::string pulselogfilename){
			std::ofstream pulselogfile;
			int totaltime=(itsSequenceNumber+1)*itsNrSamples;
			int intstart=(itsSequenceNumber+1)*itsNrSamples%itsBufferLength;
			pulselogfile.open(pulselogfilename.c_str());
			int buflen=SumDeDispersed.size();
			//pulselogfile.width(11);
                        pulselogfile.precision(10);
			for(int j=intstart+1;j<buflen;j++){
				pulselogfile << (totaltime-buflen+j)*itsTimeResolution <<  " " << SumDeDispersed[j] << std::endl;
				
			}
                        float time;
                        //std::cout.precision(10);
			for(int r=0;r<intstart+1;r++){
				char output3[200];
				sprintf(output3,"%.12f %e",(totaltime+r)*itsTimeResolution,SumDeDispersed[r]  );
				time = (totaltime+r)*itsTimeResolution;
                                
                                pulselogfile << time << " " << SumDeDispersed[r] << std::endl;
                               // std::cout << time << std::endl;
			}
			pulselogfile.close();
			
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
		
		
		
		
		
				//return true;
	};
	
	
}; // StationCU

