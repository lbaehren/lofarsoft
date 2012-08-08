 /*-------------------------------------------------------------------------*
 | $Id:: FRATStestRFI.cc                                           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (s.terveen@astro.ru.nl)                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef _OPENMP
#include <omp.h>
#endif
//#include <crtools.h>
//#include <Data/LOFAR_TBB.h>
//#include <Display/SimplePlot.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <cmath>
#include <sstream>
#include <sys/stat.h>
#include "FRATcoincidence.h"
#include "FRATcoincidence.cc"
#include <stdio.h>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace FRAT::coincidence;
using namespace FRAT::analysis;

/*
struct chanIdVal {
    int id;
    int val;
};
*/

/*
 \file ttbbctlIn.cc
 
 \ingroup CR_Applications
 
 \brief A program to trigger on dispersed pulses found in LOFAR pulsar pipeline subband data. (MS files). This version handles multiple subbands as one.
 
 \author Sander ter Veen
 
 \date 2009/09/03
 */

#define CHANNELS        32
#define SAMPLES         2768
//#define STRIDE          (64/CHANNELS)^2

// 256; 768 for the old files or 3056 ipv 3072
/*
 \brief Structure to read the incoherent beam files 
 */
struct stokesdata {
	/* big endian (ppc) */
	unsigned int  sequence_number;
	//char pad[508];	
	/* little endian (intel) */
	float         samples[CHANNELS][SAMPLES|2];
};

//public static unsafe 
void SwapFloats(unsigned char* data, int cnt) {
    //int cnt = data.Length / 4;
    unsigned char* d = data;
    unsigned char* p = d;
    while (cnt-- > 0) {
            char a = *p; //0
            p++;      //p=1
            char b = *p; //1, 
            *p = *(p + 1); //2 --> 1
            p++;   //p=2
            *p = b; // 3--> 2
            p++;   //p=3
            *(p - 3) = *p; //0=3
            *p = a;  //3=0
            p++;
    }
    
}


vector<chanIdVal> CalcBadChannels(float* data_start, float* data_end, int nrchannels, int nrsamples, float cutlevel, int validsamples){
    vector<chanIdVal> badChans;
    chanIdVal tempIdVal;
    vector<float> sumvector;
    vector<float> stdOverPower;
    vector<float> stdOverPowerSort;
    vector<float> std;
    sumvector.resize(nrchannels);
    stdOverPower.resize(nrchannels);
    stdOverPowerSort.resize(nrchannels);

    for(int ch=0; ch<nrchannels; ch++){
        float* it=data_start+ch;
        sumvector[ch]=0.0;
        stdOverPower[ch]=0.0;
        //cout << "iterator " << it << " " << *it;
        for ( ; it<data_end; it+=nrchannels ) {
            sumvector[ch]+=*it;
            stdOverPower[ch]+=(*it)*(*it);
        }
        stdOverPower[ch]/=(sumvector[ch]*sumvector[ch]);  
        stdOverPowerSort[ch]=stdOverPower[ch];
    }
    // Find top 10 and bottom 10 percent of stdOverPower and determine average and standarddeviation of the rest of the values.
    
    sort (stdOverPowerSort.begin(), stdOverPowerSort.end());
    vector<float>::iterator it2;
    double sum=0;
    double sqr=0;
    int val;
    int nrelem=0;
    float sortStd;
    for(it2=stdOverPowerSort.begin()+stdOverPowerSort.size()/10; it2<stdOverPowerSort.end()-stdOverPowerSort.size()/10; it2++){
//        if(!isnan(*it2)){
            sum+=*it2;
            sqr+=(*it2)*(*it2);
            nrelem++;
//        }
    }
    nrelem-=(nrsamples-validsamples); 
    double average=sum/nrelem;
    sortStd=sqrt(sqr/nrelem-average*average);
    if(isnan(sortStd)){
        sortStd=sqrt(average*average-sqr/nrelem);
    }
    cout << " sum " << sum << " nrelem" << nrelem << " average " << average << " std " << sortStd << " sqr " << sqr << " ";
    if(!isnan(sortStd)){
        float limit=average+cutlevel*sortStd;
        int index=0;
        for(it2=stdOverPower.begin(); it2<stdOverPower.end(); it2++){
            if(isnan(*it2)){ 
                cout << " nan " <<  index;
            }
            if(*it2 > limit) {
                tempIdVal.id=index;
                val=(int) round(((*it2)-average)/sortStd);
                tempIdVal.val=val;
                badChans.push_back(tempIdVal);
            }
            index++;
        }
    }
    
    return badChans;
}





vector<float> channelcollapse(float* data_start, float* data_end, int nrchannels, int startchannel, int stopchannel, int nrsamples){
    vector<float> sumvector;
    sumvector.resize(nrsamples);
    for(int sa=0; sa<nrsamples; sa++){
        sumvector[sa]=0.0;
        float* it_start=data_start+sa*nrchannels;
        for(int ch=startchannel; ch<stopchannel; ch++){
            float* it=it_start+ch;
            sumvector[sa]+=*it;
        }
    }
    return sumvector;
}

vector<int> CalcBadSamples(float* data_start, float* data_end, int nrchannels, int nrsamples, float cutlevel){
    int subdiv=4;// subdivisions. into how many parts to split the data
    int reqsubdiv=2; // required subdivisions. how many parts should show a peak
    int channelsPerPart=nrchannels/4;
    vector<float> collapsedData[subdiv];
    vector<float> collapsedDataSort[subdiv];
    vector<int> badcollapsedSamples;
    vector<int> badSamples;
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

    for(int div=0; div<subdiv; div++){
        collapsedData[div]=channelcollapse(data_start, data_end, nrchannels, div*channelsPerPart, (div+1)*channelsPerPart, nrsamples);
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
       // cout << "sum" << sum << "nrelem" << nrelem << "average " << average << "\n std " << sortStd;
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
        cout << *it << " " << count << " " << reqsubdiv << " " << prevvalue << " " << badSamples.size() << " \t";
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

    return badSamples;
}




vector<float> channelSum(float* data_start, float* data_end, int nrchannels, int nrsamples){
    vector<float> sumvector;
    sumvector.resize(nrchannels);
    for(int ch=0; ch<nrchannels; ch++){
        float* it=data_start+ch;
        sumvector[ch]=0.0;
    //cout << "iterator " << it << " " << *it;
        for ( ; it<data_end; it+=nrchannels ) {
            sumvector[ch]+=*it;
        }
    }
    return sumvector;
}

vector<float> channelSumSqr(float* data_start, float* data_end, int nrchannels, int nrsamples){
    vector<float> sumvector;
    sumvector.resize(nrchannels);
    for(int ch=0; ch<nrchannels; ch++){
        float* it=data_start+ch;
        sumvector[ch]=0.0;
        //cout << "iterator " << it << " " << *it;
        for ( ; it<data_end; it+=nrchannels ) {
            sumvector[ch]+=(*it) * (*it);
        }
    }
    return sumvector;
}



//_______________________________________________________________________________
//                                                                      show_data



//_______________________________________________________________________________
//                                                                    swap_endian
/*
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
 */
/*
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
 */

bool FileExists(string strFilename) { 
	struct stat stFileInfo; 
	bool blnReturn; 
	int intStat; 
	
	// Attempt to get the file attributes 
	intStat = stat(strFilename.c_str(),&stFileInfo); 
	if(intStat == 0) { 
		// We were able to get the file attributes 
		// so the file obviously exists. 
		blnReturn = true; 
	} else { 
		// We were not able to get the file attributes. 
		// This may mean that we don't have permission to 
		// access the folder which contains this file. If you 
		// need to do that level of checking, lookup the 
		// return values of stat which will give you 
		// more details on why stat failed. 
		blnReturn = false; 
	} 
	
	return(blnReturn); 
}

//_______________________________________________________________________________
//                                                                         main()

int main (int argc,
		  char *argv[])
{
	bool failsafe=0;
	cout<<"This file outputs histograms of pulsar data to /Users/STV/Documents/GiantPulse/."<<endl;
	int integrationlength = 1; //average length
	float triggerlevel = 141/3;
 	unsigned int nofFailedTests=0;
	float average;
	std::string inputfile, configfilename;
	vector<std::string> inputdirs;
	int ninputdirs;
	bool multipledirs=false;
	vector<int> inputstartSBs;
	int firstSB, lastSB, nrblocks=5, startpos=0;
	int CoinNr=10;
	int CoinTime=80;
	float DM=56.8;
	int nDMs=1;
    int nFreqs=0;
    vector<float> DMvalues;
    vector<float> FREQvalues;
    int nBadChannels=0;
    vector<int> extraBadChannels;
    vector<int> BadChannels;
	//	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	std::string logfilename = "/Users/STV/Documents/GiantPulse/excesslog_pdt.txt";
	std::string triggerlogfilename = "/Users/STV/Astro/Programming/pulsar/trigger.log";
	int channels=CHANNELS;
	int samples=SAMPLES;
	int startsubbandnumber=321;
	bool DoPadding=0, verbose=true;
	std::string extension=".stokes";
	int timeintegration=1;
	int nrCombinedSBs = 1;
	int HBAmode = 1;
	string pulsedir = "pulses";
    int sleeptime = 0;
    unsigned long int starttime_sec=0;
    unsigned long int starttime_ns=0;
    bool DoNotSendUDPtriggers=false;
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help" || argc < 2){
			cout << "Options:\n"
			"-c <config file>\n"
			"-i <inputdir>\n"
			"-FSB <first subband>\n"
			"-LSB <last subband>\n"
			"-il <integration length\n"
			"-tl <triggerlevel>\n"
			"-log <logfile>\n"
			"-tlog <triggerlogfile>\n"
			"-n <nrblocks>\n"
			"-Cnr <coincidence nr subbands\n"
			"-Ctime <coincidence time windows\n"
			"-Tnr <number of stored triggers\n"
			"-ch <number of channels, TO BE ADDED>\n"
			"-sa <number of samples, TO BE ADDED>\n"
			"-verbose, display histograms\n"
			"-ext <file extenstion after .MS\n"
			"- pad use padding for newer data files\n"
			"-startSB real subband to set the startfrequency right\n"
			"-DM dispersion measure\n"
			"-nrCSB number of subbands to combine as one"
			"-tInt <time integration>"
			"-LBA LBA mode"
            "-noSend do not send trigger messages over UDP"
            "-sleep <sleeptime (sec)>"
			"Examples:\n"
			"./TransientTrigger -i ~/Astro/data/pulsars/obs/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 51 \n"
			"./TransientTrigger -i ~/Astro/data/pulsars/L2009_13298/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 3595 \n; open /Users/STV/Documents/GiantPulse/excesslog_pdt.txt\n"
			"./TransientTrigger -i ~/Astro/data/pulsars/obs/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 51 -log TransientTrigger.log -tlog trigger.log\n";
			"./TransientTrigger -c TransientTrigger.conf\n";
			return 1;
		} else if(topic == "-c"){
			argcounter++;
			configfilename = string(argv[argcounter]);
			cout << "configfile: " << configfilename << endl;
		} else if(topic == "-i"){
			argcounter++;
			inputfile = string(argv[argcounter]);
			cout << "input directory: " << inputfile << endl;
/*		} else if(topic == "-indirs"){
			multipledirs=true;
			argcounter++;
			ninputdirs = atoi(argv[argcounter]);
			inputdirs.resize(ninputdirs,"");
			inputstartSBs.resize(ninputdirs,0);
			for(int i=0;i<ninputdirs;i++){
				argcounter++;
				inputdirs[i] = string(argv[argcounter]);
				argcounter++;
				inputstartSBs[i]=atoi(argv[argcounter]);
			}
			for(int i=0;i<ninputdirs;i++){
				cout << "input directory: " << inputdirs[i] << " starting at " << inputstartSBs[i] << endl;
			}
*/		} else if(topic == "-FSB"){
			argcounter++;
			firstSB = atoi(argv[argcounter]);
			cout << "First subband: " << firstSB << endl;
		} else if(topic == "-LSB"){
			argcounter++;
			lastSB = atoi(argv[argcounter]);
		    cout << "Last subband: " << lastSB << endl;
		} else if(topic == "-il"){
			argcounter++;
			integrationlength = atoi(argv[argcounter]);
			cout << "integration length: " << integrationlength << endl;
		} else if(topic == "-tl"){
			argcounter++;
			triggerlevel = atof(argv[argcounter]);
			cout << "triggerlevel " << triggerlevel << endl;
		} else if(topic == "-log"){
			argcounter++;
			logfilename = string(argv[argcounter]);
			cout << "log file " << logfilename << endl;
		} else if(topic == "-pulsedir"){
			argcounter++;
			pulsedir = string(argv[argcounter]);
			cout << "pulse plots directory " << pulsedir << endl;
		} else if(topic == "-tlog"){
			argcounter++;
			triggerlogfilename = string(argv[argcounter]);
			cout << "trigger log file " << triggerlogfilename << endl;
		} else if(topic == "-n"){
			argcounter++;
			nrblocks = atoi(argv[argcounter]);
			cout << "nr blocks " << nrblocks << endl;
		} else if(topic == "-Cnr"){
			argcounter++;
			CoinNr = atoi(argv[argcounter]);
			cout << "Coincidence subbands required " << CoinNr << endl;	
		} else if(topic == "-sleep"){
			argcounter++;
            sleeptime=(int) 1000000*atof(argv[argcounter]);
			cout << "Sleep for " << sleeptime << "us" << endl; 
		} else if(topic == "-Ctime"){
			argcounter++;
			CoinTime = atoi(argv[argcounter]);
			cout << "Coincidence timewindow " << CoinTime << endl;
		} else if(topic == "-stb"){
			argcounter++;
			startpos = atol(argv[argcounter]);
			cout << "First block " << startpos << endl;	
		} else if(topic == "-ch"){
			argcounter++;
			channels = atoi(argv[argcounter]);
			cout << "nr of channels " << channels << endl;
		} else if(topic == "-sa"){
			argcounter++;
			samples = atoi(argv[argcounter]);
			cout << "nr of samples " << samples << endl;
		} else if(topic == "-LBA"){
			HBAmode = 0;
			cout << "using LBA data" << endl;
		} else if(topic == "-noSend"){
			DoNotSendUDPtriggers = true;
			cout << "not sending triggers" << endl;
		} else if(topic == "-verbose"){
			verbose = true;
			cout << "using verbose output" << endl;
		} else if(topic == "-pad"){
			DoPadding = true;
			cout << "using padding output" << endl;
		} else if(topic == "-ext"){
			argcounter++;
			extension = argv[argcounter];
			cout << "file extensions: .MS" << extension << endl;
		} else if(topic == "-startSB"){
			argcounter++;
			startsubbandnumber = atoi(argv[argcounter]);
			cout << "startsubband number: " << startsubbandnumber << endl;
		} else if(topic == "-DM"){
			argcounter++;
			nDMs = atoi(argv[argcounter]);
			cout << "dispersion measure: " ;
			DMvalues.resize(nDMs);
			for(int i=0;i<nDMs;i++){
				argcounter++;
			    DMvalues[i]=atof(argv[argcounter]);
                cout << " " << DMvalues[i];
			}		
            cout << endl;	
		} else if(topic == "-freq"){
			argcounter++;
			nFreqs = atoi(argv[argcounter]);
			cout << nFreqs << "frequency values: " ;
			FREQvalues.resize(nFreqs);
			for(int i=0;i<nFreqs;i++){
				argcounter++;
			    FREQvalues[i]=atof(argv[argcounter]);
                if(i<20){ cout << " " << FREQvalues[i]; }
			}		
            cout << endl;	
		} else if(topic == "-badch"){
			argcounter++;
			nBadChannels = atoi(argv[argcounter]);
			cout << nBadChannels << "bad channels that will be skipped " ;
			BadChannels.resize(nBadChannels);
			for(int i=0;i<nBadChannels;i++){
				argcounter++;
			    BadChannels[i]=atoi(argv[argcounter]);
                if(i<20){ cout << " " << BadChannels[i]; }
			}		
            cout << endl;	
		} else if(topic == "-tInt"){
			argcounter++;
			timeintegration = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
        } else if(topic == "-obsstart"){
			argcounter++;
			starttime_sec = atoi(argv[argcounter]);
            argcounter++;
			starttime_ns = atoi(argv[argcounter]);            
			cout << "start time of observation: " << timeintegration << endl;
		} else if(topic == "-nrCSB"){
			argcounter++;
			nrCombinedSBs = atoi(argv[argcounter]);
			cout << "number of subbands to take together: " << nrCombinedSBs << endl;
		} else {
			cout << "specify at least an config file with -c <config file>";
		}
	}
	int samplesOr2 = samples|2;
	
	
	
	cout << logfilename << endl << triggerlogfilename << endl;
	
	//============================= NEW I/O code ======================================================
	
	float ReferenceFreq=SubbandToFreq(startsubbandnumber,HBAmode);
	float FreqResolution = CalcFreqResolution(channels);
	float TimeResolution = CalcTimeResolution(channels,timeintegration);
	//int ninputfiles=lastSB-firstSB+1;
    int ninputfiles=nFreqs/channels;
	int nstreams = ninputfiles/nrCombinedSBs;
	if(ninputfiles%nrCombinedSBs!=0){
		cout << "Not all subbands could be processed in blocks of " << nrCombinedSBs << endl;
		ninputfiles = nstreams*nrCombinedSBs;
		lastSB=firstSB+ninputfiles-1;
		cout << "Last subband: " << lastSB << endl;
		cout << "ninputfiles: " << ninputfiles << endl;
	}
    int NrChPerSB = channels;
	int NrChannels=nrCombinedSBs*channels;
    int TotNrChannels=nFreqs; //nstreams*nrCombinedSBs*channels;
	cout << "Nr of input files: " << ninputfiles << "; Nr of streams: " << nstreams << endl;
	int StartChannel;
	
	// Initialize SBtriggers and coincidence mechanism
	
	float DMval=DM;
    cout << "Making Subband triggers and Coincidence checks" << endl;
	SubbandTrigger* SBTs[nstreams][nDMs];
    cout << "* " <<  sizeof(CoinCheck) << endl;
    //float *data=(float*)malloc(blockdatasize);
    //CoinCheck *cc = (CoinCheck*)malloc(nDMs*sizeof(CoinCheck));
    CoinCheck *cc = new CoinCheck[nDMs];
    //CoinCheck cc[nDMs];
    //for(int i=nDMs-1;i>=0;i--){
    //   cc[i]=new CoinCheck;
    //}
    
    if (cc==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	//CoinCheck cc[nDMs];
	
	
    UDPsend* UDPtransmitter=new UDPsend();
	int StreamCounter=0;
    int SB=firstSB;
    std::string temp;
    for(int stream=0; stream<nstreams; stream++){
        std::cout << "stream " << stream << std::endl;
        SB+=nrCombinedSBs;
		for(int DMcounter=0;DMcounter<nDMs;DMcounter++){
			DMval=DMvalues[DMcounter];
            std::cout << "DMval " << DMval << std::endl;
			cout << "StreamNr " << StreamCounter << " DMcounter " << DMcounter << " DM " << DMval << endl;
			float SBFreq = SubbandToFreq(SB+startsubbandnumber,HBAmode);
            if(nFreqs<1) {
                SBTs[StreamCounter][DMcounter] = new SubbandTrigger(StreamCounter, NrChannels, samples,DMval,triggerlevel,ReferenceFreq, SBFreq, FreqResolution, TimeResolution, starttime_sec, starttime_ns,  startpos, integrationlength, DoPadding, DoPadding, verbose);
            } else {
                StartChannel=stream*NrChannels;
                // start channel of this stream
                // total number of channels in this stream
                printf("Channel, %i, Integration, %i ",channels,timeintegration);
                //if(stream==1 && DMcounter==0)
                SBTs[StreamCounter][DMcounter] = new SubbandTrigger(StreamCounter, NrChPerSB, samples,DMval,triggerlevel,ReferenceFreq, FREQvalues, StartChannel, NrChannels, TotNrChannels, FreqResolution, TimeResolution, starttime_sec, starttime_ns, UDPtransmitter, startpos, integrationlength, DoPadding, DoPadding, verbose); // Add obsid and beam    
                
                //}
            }
	        
			//SBTs[StreamCounter][DMcounter]->summary();
		}
		StreamCounter++;
	}
    std::cout << "initialization done.";
   
	
	// Initialize coincidence

    // Output file	
/*    FILE * pFileOut;
    pFileOut = new FILE;
    pFileOut = fopen((inputfile+".out").c_str(),"wb");
*/	
	//--------------READ IN THE FILES----------------------------------	
    FILE * pFile;
    pFile = new FILE;
    pFile = fopen(inputfile.c_str(),"rb");
    while (pFile==NULL) {
        cout << "File not there yet" << endl;
        usleep(500000);
        pFile = fopen(inputfile.c_str(),"rb");
    } 	
        
        
    
	
    // Create file objects for summary of datastreams
	ofstream pulselogfile;
	int pulsenr=1;
	bool foundpulse;
    int mynDMs=nDMs;
    if(nstreams*nDMs > 50){
        int mynDMs=1;
    } 

    // Create log files        
    
	
            
/*	unsigned int sequence_nr[ninputfiles];
	unsigned int prev_sequence_nr[ninputfiles];
	for(int i=0; i < ninputfiles; i++){
		prev_sequence_nr[i]=0;
	}
	char pad[508];
*/
	unsigned num;
	fpos_t pos;
    // New datasize, allchannels for the samples of the integration time, no header anymore
	long blockdatasize=nFreqs*samples*sizeof(float);
	long stokesdatasize=blockdatasize;
	float *data=(float*)malloc(blockdatasize);
    float *data_end=data+nFreqs*samples;

    bool Transposed=true;

    int number_of_divisions=4;
    int cutlevel=5;
    std:string outFileName="RFIsummary.txt" ;
    RFIcleaning RFIcleaner(data, data_end, nFreqs, NrChPerSB, samples, cutlevel, number_of_divisions, outFileName);
	if (data==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	
	cout << "stokesdatasize " << stokesdatasize << endl;
	
    int ch;
	fseek(pFile, startpos*blockdatasize, SEEK_SET);
    cout << "Current positions before reading " <<  startpos << " " << ftell(pFile)/blockdatasize << " " << startpos*blockdatasize <<  endl;

	for(int blockNr = 0; blockNr< nrblocks; blockNr++){
        // Read data
        fgetpos(pFile,&pos);
        cout << "Current positions " << blockNr << " " << startpos << " " << ftell(pFile)/blockdatasize << endl;
        
        num = fread( &(data[0]), blockdatasize, 1, pFile); //read data
        //flagdata

        /*
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
        }*/
    
        
        if( !num) {
            cout << "*";
            usleep(50000);
            fsetpos(pFile,&pos);
            blockNr--;
            
            //i--; //1 file k--
            continue;
        }
        cout << "About to swap " << blockdatasize << " floats" << endl;
        
        unsigned char *datachar=(unsigned char*) data;

        
        if(DoPadding){
            SwapFloats(datachar,nFreqs*samples);//nFreqs*samples);
            /*
            for(int i=0;i<nFreqs*samples;i++) 
            {
              data[i] = FloatSwap(data[i]);
            }  
            */
        }

        cout << "Done swapping floats" << endl;
//      Find out what the bad channels are
        int validchannels=TotNrChannels;
        int validsamples=samples;
        bool doFlagging=true;
if(doFlagging){
        cout << "Flagging channels in bewtween " << data << " " << data_end << endl; ;
        vector<chanIdVal> badChans15=CalcBadChannels(data,data_end,TotNrChannels,samples,15,validsamples);
        // printing found bad channels
        cout << "\n  bad channels ";
        for(int i=0; i<badChans15.size(); i++){
            cout << badChans15[i].id << " " << badChans15[i].val << " ";
        }
        cout << "\n";
        
        RFIcleaner.calcBaseline();
        RFIcleaner.divideBaseline();
        RFIcleaner.calcBaseline();
        //RFIcleaner.printBaseline();
        RFIcleaner.calcSqrBaseline();
        RFIcleaner.calcBadChannels(15);
        RFIcleaner.printBadChannels();
        RFIcleaner.cleanChannels("1");

        // flagging bad samples
        vector<int> badSamples=CalcBadSamples(data,data_end,TotNrChannels,samples,4);

        cout << "Flagging bad samples " ;
        for(int i=0; i<badSamples.size(); i++){
            cout << badSamples[i] << " ";
        }
        cout << "\n ";

        RFIcleaner.calcBadSamples(4);
        RFIcleaner.printBadSamples();
        RFIcleaner.cleanSamples("1");


        // flagging remaining bad channels
        RFIcleaner.calcBaseline();
        RFIcleaner.calcSqrBaseline();
        RFIcleaner.calcBadChannels(6);
        RFIcleaner.printBadChannels();

        //vector<chanIdVal> badChans=CalcBadChannels(data,data_end,TotNrChannels,samples,5,validsamples);
        cout << "\n";
        cout << "Flagging channels " ;
        vector<chanIdVal> badChans=CalcBadChannels(data,data_end,TotNrChannels,samples,6,validsamples);
        for(int i=0; i<badChans.size(); i++){
            cout << badChans[i].id << " " << badChans[i].val << " ";
        }
        cout << "\n";

        RFIcleaner.cleanChannels("1");
        RFIcleaner.cleanChannel0("1");


/*
        for(ch=0; ch<TotNrChannels; ch+=channels){
            if( data[ch]>0.0001){ // not previously flagged
                validchannels--;                
            }
            for(int sa; sa<samples ; sa++){ 
                data[sa*TotNrChannels+ch]=0.0;
            }
            
        }

        

        for(int i=0; i<BadChannels.size(); i++){
            ch=BadChannels[i];
//            cout << "Flagging channel " << ch << endl;
            if( ch < TotNrChannels ){
                for(int sa; sa<samples ; sa++){ 
                    data[sa*TotNrChannels+ch]=0.0;
                }
            }
            validchannels--;
        }
  */      
   cout << " We are here now . 1 " << sizeof(chanIdVal)<< endl;     

} //do flagging
//   SwapFloats(datachar,nFreqs*samples);//nFreqs*samples);
//   num = fwrite( &(data[0]), blockdatasize, 1, pFileOut);
//   SwapFloats(datachar,nFreqs*samples);//nFreqs*samples);
   

   cout << " We are here now . 2 " << endl;     

		for(int sc=0; sc < nstreams; sc++){
            
            #ifdef _OPENMP
                std::cout<<"Running in parallel mode"<<std::endl;
                #pragma omp parallel for private(foundpulse)
            #else
                std::cout<<"Running in serial mode"<<std::endl;
            #endif // _OPENMP
		    
        	for(int DMcounter=0; DMcounter<nDMs; DMcounter++){	//analyse data of one stream for all DMs
				cout << "Processing " << sc << " " << DMcounter << endl;
				foundpulse=SBTs[sc][DMcounter]->dedisperseData(data, blockNr, &cc[DMcounter], CoinNr, CoinTime,Transposed);
                cout << "f" <<  foundpulse << endl;
                if(nDMs <3){
						stringstream pulselogfn;
						pulselogfn << pulsedir << "/dedisptimeseries_" << SBTs[sc][DMcounter]->itsDM << "_" << sc << ".log";
						string pulselogfilename;
						pulselogfn >> pulselogfilename;
						SBTs[sc][DMcounter]->makeplotDedispBlock(pulselogfilename);
                }
			}
			

		}
        usleep(sleeptime);

	}

    

    //fclose(pFileOut); 
	return nofFailedTests;
}




