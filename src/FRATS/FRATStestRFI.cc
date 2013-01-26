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
//#ifdef _OPENMP
//#include <omp.h>
//#endif
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
 
 \brief A program to trigger on dispersed pulses found in LOFAR beamformed data. (MS files). 
 
 \author Sander ter Veen
 
 \date 2012/09/26
 */

#define CHANNELS        32
#define SAMPLES         2768

/*
    Data structure:
    struct block {
        float sample[SUBBANDS][CHANNELS];
    };
    but usually read in as multiple SAMPLES at the same time. Data is big-endian 32-bit IEEE floats.
    For analysis they are usually swapped.    
*/


//public static unsafe 
/*
 \brief Swaps a number (cnt) of integers from big-endian to little endian
*/
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

// Calculate bad channels that should not be used in the analysis. This function has now been implemented in the RFI cleaning class. I'm checking if this is still used.
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




// Superseded by function in RFIclean in FRATcoincidence.cc ??
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

// Superseded by function in RFIclean in FRATcoincidence.cc ??
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



// superseded by function in RFIclean?
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

// superseded by function in RFIclean ?
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



// check if a file exists (not used in the code at the momemnt, but maybe we should).
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

/*
    The task of the program is to dedisperse data in several bands by summing
    the channels with the correct offset. It does this for several dispersion
    trials. For each dispersion trial it will try to find pulses in access of 
    triggerlevel*standard deviation above the mean. It will do this for different
    pulse lengths.
*/

int main (int argc,
		  char *argv[])
{
	bool failsafe=0; // Do not continu if a sequence number is missing. This would hint
    // to dropped data. But now sequence numbers are not used anymore, but data is padded
    // with zeros for new blocks
    
/* Set default values that will mostly be filled by commandline options. */

// Length to integrate over for the trigger
	std::vector<int> integrationlength;
    integrationlength.resize(1);
    integrationlength[0]=1;
    int nrFlaggedSamples=0;
    int nrFlaggedChannels=0;
// Trigger level in each band
	float triggerlevel = 141/3;
	float average; // obsolote?
    int obsID=0;
    int beam=-1;
// data files to read. Configfile is not yet implemented
	std::string inputfile, configfilename;
	vector<std::string> inputdirs;
	int ninputdirs;
	bool multipledirs=false;
    int Rstartchan;
    int Rendchan;

// start subband of the data. Mostly used in a previous version of the program
	vector<int> inputstartSBs;
	int firstSB, lastSB, nrblocks=5, startpos=0;

// How many bands should trigger simultaneously?
	int CoinNr=10;
// Time difference for triggers between bands in samples
	int CoinTime=80;
// Default DM value, currently mostly more DMvalues are used.
	float DM=56.8;
// Count and value of dispersion measure trials.
	int nDMs=1;
    vector<float> DMvalues;
// Count and values of frequencies of all the channels
    int nFreqs=0;
    vector<float> FREQvalues;
// Count and channel numbers that are contaminated by RFI
    int nBadChannels=0;
    vector<int> extraBadChannels;
    vector<int> BadChannels;
// log file for trigger messages and other information
	std::string logfilename = "/Users/STV/Documents/GiantPulse/excesslog_pdt.txt";
	std::string triggerlogfilename = "/Users/STV/Astro/Programming/pulsar/trigger.log";
// channels in each subband
	int channels=CHANNELS;
// samples used for reading one block of data
	int samples=SAMPLES;
// resampling in time of raw data, before doing all the analysis
    int resampleFactorTime=1;
// resampling in frequency of raw data, before doing all the analysis
    int resampleFactorFreq=1;
// lowest subband of the data
	int startsubbandnumber=321;
// padding now used only for swapping floats. 
// NOTE: Verbose output needs some more work 
	bool DoPadding=0, verbose=false;
// ZeroDM substracts the average of each time sample. Effect on detection efficiency
// and RFI mitigation needs to be investigated.
    bool DoZeroDM=false;
// file extensions used. Obsolete? 
	std::string extension=".stokes";
// integration on the correlator
	int timeintegration=1;
// how many subbands form one band?
	int nrCombinedSBs = 1;
// using HBA or LBA data?
	int HBAmode = 1;
// where to store output 
	string pulsedir = "pulses";
    bool cleanRFI = true;
// extra sleep time between blocks
    int sleeptime = 0;
// starttime is used to calculate actual trigger time. Not calculated at te moment when the 
// program is called.
    unsigned long int starttime_sec=0;
    unsigned long int starttime_ns=0;
// Send UDP messages with trigger information?
    bool DoNotSendUDPtriggers=false;
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
// Input from the commandline is written to the appropriate keywords.	
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
			"-DMrange start, step, end dispersion measure, overwrites -DM\n"
			"-nrCSB number of subbands to combine as one"
			"-tInt <time integration>"
			"-LBA LBA mode"
            "-nocleanRFI"
            "-noSend do not send trigger messages over UDP"
            "-sleep <sleeptime (sec)>"
            "-obsID <obsID>"
            "-beam <beam>"
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
			integrationlength[0] = atoi(argv[argcounter]);
			cout << "integration length: " << integrationlength[0] << endl;
		} else if(topic == "-ilrange"){
			argcounter++;
            int intlenlen=atoi(argv[argcounter]);
            integrationlength.resize(intlenlen); 
            cout << "integration lengths: ";
            for(int i=0;i<intlenlen;i++){
                argcounter++;
                integrationlength[i] = atoi(argv[argcounter]);    
                cout << integrationlength[i] << " ";
            }
            cout << endl;
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
		} else if(topic == "-resampleT"){
			argcounter++;
			resampleFactorTime = atoi(argv[argcounter]);
			cout << "time resampling factor" << resampleFactorTime << endl;
		} else if(topic == "-resampleF"){
			argcounter++;
			resampleFactorFreq = atoi(argv[argcounter]);
			cout << "frequency resampling factor" << resampleFactorFreq << endl;
		} else if(topic == "-LBA"){
			HBAmode = 0;
			cout << "using LBA data" << endl;
		} else if(topic == "-nocleanRFI"){
			cleanRFI = false;
			cout << "not cleaning RFI" << endl;
		} else if(topic == "-noSend"){
			DoNotSendUDPtriggers = true;
			cout << "not sending triggers" << endl;
		} else if(topic == "-verbose"){
			verbose = true;
			cout << "using verbose output" << endl;
		} else if(topic == "-silent"){
			verbose = false;
			cout << "not using verbose output" << endl;
		} else if(topic == "-zeroDM"){
			DoZeroDM = true;
			cout << "using zeroDMing to divide data by average timeseries" << endl;
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
		} else if(topic == "-DMrange"){
			argcounter++;
			float firstDM=atof(argv[argcounter]);
			argcounter++;
			float stepDM=atof(argv[argcounter]);
			argcounter++;
			float endDM=atof(argv[argcounter]);
            float nDMsf = (endDM-firstDM)/stepDM;
            cout << firstDM << " " << stepDM << " " << endDM << " " << (endDM-firstDM)/stepDM << endl;
			nDMs = (int) nDMsf+1;
			cout << "dispersion measure ("<< nDMs <<"): " ;
			DMvalues.resize(nDMs);
			for(int i=0;i<nDMs;i++){
			    DMvalues[i]=firstDM+i*stepDM;
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
			cout << "start time of observation: " << starttime_sec << " " << starttime_ns << endl;
		} else if(topic == "-nrCSB"){
			argcounter++;
			nrCombinedSBs = atoi(argv[argcounter]);
			cout << "number of subbands to take together: " << nrCombinedSBs << endl;
		} else if(topic == "-obsID"){
			argcounter++;
			obsID = atoi(argv[argcounter]);
			cout << "observation ID: " << obsID << endl;
		} else if(topic == "-beam"){
			argcounter++;
			beam = atoi(argv[argcounter]);
			cout << "beam: " << beam << endl;
		} else {
			cout << "specify at least an config file with -c <config file>";
		}
	}
	
	
	
	//cout << logfilename << endl << triggerlogfilename << endl;
	
	//============================= NEW I/O code ======================================================
	
	
	
	
	
	float ReferenceFreq=SubbandToFreq(startsubbandnumber,HBAmode);
	float FreqResolution = CalcFreqResolution(channels);
	float TimeResolution = CalcTimeResolution(channels,timeintegration)*resampleFactorTime;
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
    if(samples%resampleFactorTime != 0){
        samples-=samples%resampleFactorTime;
        cout << "new samples per datablock " << samples << ", to enable resampling";
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
                SBTs[StreamCounter][DMcounter] = new SubbandTrigger(StreamCounter, NrChannels, samples,DMval,triggerlevel,ReferenceFreq, SBFreq, FreqResolution, TimeResolution, starttime_sec, starttime_ns,  startpos, integrationlength[0], DoPadding, DoPadding, verbose, DoNotSendUDPtriggers, obsID, beam);
            } else {
                StartChannel=stream*NrChannels;
                // start channel of this stream
                // total number of channels in this stream
                printf("Channel, %i, Integration, %i ",channels,timeintegration);
                //if(stream==1 && DMcounter==0)
                SBTs[StreamCounter][DMcounter] = new SubbandTrigger(StreamCounter, NrChPerSB, samples/resampleFactorTime,DMval,triggerlevel,ReferenceFreq, FREQvalues, StartChannel, NrChannels, TotNrChannels, FreqResolution, TimeResolution, starttime_sec, starttime_ns, UDPtransmitter, startpos, integrationlength, DoPadding, DoPadding, verbose, DoNotSendUDPtriggers, obsID, beam); // Add obsID and beam    
                
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
	string alltriggerlogfilename;
	alltriggerlogfilename=pulsedir+"/found_triggers.log";
	string flaggedchansfilename=pulsedir+"/flaggedchans.log";
	string flaggedsamplesfilename=pulsedir+"/flaggedsamples.log";
	string baselinefilename=pulsedir+"/baseline.log";
	string sqrtimeseriesfilename=pulsedir+"/sqrtimeseries.log";
	string avfilename=pulsedir+"/average.log";
	string stdfilename=pulsedir+"/stddev.log";
    string offsetfilename=pulsedir+"/offset.log";
    
    ofstream * fcfile = new ofstream;
    fcfile->open(flaggedchansfilename.c_str(), ios::binary  | ios::out);
    ofstream * fsfile = new ofstream;
    fsfile->open(flaggedsamplesfilename.c_str(), ios::binary  | ios::out);
    ofstream * basefile = new ofstream;
    basefile->open(baselinefilename.c_str(), ios::binary |  ios::out);
    ofstream * sqrtimeseriesfile = new ofstream;
    sqrtimeseriesfile->open(sqrtimeseriesfilename.c_str(), ios::binary |  ios::out);
    ofstream * avfile = new ofstream;
    avfile->open(avfilename.c_str(), ios::binary  | ios::out);
    ofstream * stdfile = new ofstream;
    stdfile->open(stdfilename.c_str(), ios::binary  | ios::out);
    ofstream * offsetfile = new ofstream;
    offsetfile->open(offsetfilename.c_str(), ios::binary  | ios::out);
	ofstream alltriggerlogfile;
	//logfile.open(logfilename.c_str());
	//triggerlogfile.open(triggerlogfilename.c_str());
	alltriggerlogfile.open(alltriggerlogfilename.c_str(),ios::out | ios::app);

    //writing offsets to file	
    for(int DMcounter=0; DMcounter<nDMs; DMcounter++){
        for(int fc2=0;fc2<nstreams;fc2++){
            	SBTs[fc2][DMcounter]->writeOffset(offsetfile);
        }
    }
    offsetfile->close();
    // used for reading files        
	unsigned num;
	fpos_t pos;
    // Datasize is total number of channels (frequencies) * number of samples
	long blockdatasize=nFreqs*samples*sizeof(float);
    long blockdatasizeResampled=nFreqs*samples*sizeof(float)/resampleFactorTime;
    // allocate memory
	float *data=(float*)malloc(blockdatasize);
    // get pointer to the end of a data block
    float *data_end=data+blockdatasize/sizeof(float);
	if (data==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
    cout << "blockdatasize " << blockdatasize << endl;
    float *resamp_data;
    float *resamp_data_end;
    if(resampleFactorTime>=1){
        resamp_data=(float*)malloc(blockdatasizeResampled);
        resamp_data_end=resamp_data+blockdatasizeResampled/sizeof(float);
        if (resamp_data==NULL)
        {
            cerr << "Memory could not be allocated\n";
            return 1;
        }
    } else {
        resamp_data=data;
        resamp_data_end=data_end;
    }
    cout << "resampled blockdatasize " << blockdatasizeResampled << endl;



    // indicator for new data format
    bool Transposed=true;

    // Setting up RFI clean task.
    // Number of divisions to search for short time RFI
    int number_of_divisions=nstreams;
    // default cutlevel
    int cutlevel=5;
    // should put this in pulsedir? Should check what is actually logged.
    std:string outFileName="RFIsummary.txt" ;
    RFIcleaning RFIcleaner(resamp_data, resamp_data_end, nFreqs, NrChPerSB, samples/resampleFactorTime, cutlevel, number_of_divisions, outFileName);
	
	
    int ch;
    // Data can start at another block
	fseek(pFile, startpos*blockdatasize, SEEK_SET);
    cout << "Current positions before reading " <<  startpos << " " << ftell(pFile)/blockdatasize << " " << startpos*blockdatasize <<  endl;
    int lastBadBlock=-10000;
    // Read all blocks
	for(int blockNr = 0; blockNr< nrblocks; blockNr++){
        // Read data
        cout << " Analyzing block " << blockNr << endl;
        fgetpos(pFile,&pos);
      //  if(verbose){
            cout << "Current positions " << blockNr << " " << startpos << " " << ftell(pFile)/blockdatasize << endl;
      //  }
        num = fread( &(data[0]), blockdatasize, 1, pFile); //read data
        
        if( !num) {
            // Data could not be read. Wait a little and try again.
            cout << "*";
            cout.flush();
            usleep(50000);
            fsetpos(pFile,&pos);
            blockNr--;
            
            //i--; //1 file k--
            continue;
        }
        // swap endianness of the data. NOTE see if we can make this parallel?
        if(verbose) {
            cout << "About to swap " << blockdatasize << " floats" << endl;
        }

        unsigned char *datachar=(unsigned char*) data;
        if(DoPadding){
            SwapFloats(datachar,nFreqs*samples);//nFreqs*samples);
        }
        if(verbose) {
            cout << "Done swapping floats" << endl;
        }

        // Resample data

        for(float *it3=resamp_data; it3<resamp_data_end; it3++){
            *it3=0;
        }
        
        if(resampleFactorTime>=1){
            float *dptr;
            float *dptrRes;
            #ifdef _OPENMP
                #pragma omp parallel for private(dptr,dptrRes)
            #else
            #endif // _OPENMP  
            for(int ch=0; ch<TotNrChannels;ch++){
                for(int step=0;step<resampleFactorTime;step++){
                    dptr=data+step*TotNrChannels+ch;
                    dptrRes=resamp_data+ch;
                    for ( ; dptr<data_end ; dptr+=resampleFactorTime*TotNrChannels){
                        if(step==0){*dptrRes=0.0;}
                        *dptrRes+=*dptr; 
                        dptrRes+=TotNrChannels;


                    }
                }
            }
        }
        
   /* 
        float sum1=0;
        float sum2=0;
        float sum3=0;
        int sam2=0;
        int numsam=0;
        if(resampleFactorTime>=1){
            for(int ch=0; ch<TotNrChannels;ch++){
                sam2=0;

                for(int sam=0; sam<samples/resampleFactorTime; sam++){
                //for(int sam=0; sam<samples; sam++){
                    //*(resamp_data+sam*TotNrChannels+ch)=0;
                    for(int samcnt=0; samcnt<resampleFactorTime; samcnt++){
                        sum2+=*(data+sam2*TotNrChannels+ch);
                        *(resamp_data+sam*TotNrChannels+ch)+=*(data+sam2*TotNrChannels+ch);
                        sam2++;
                        numsam++;
                    }
                }
            }
        }

        for(float *it3=data; it3<data_end; it3++){
            sum1+=*it3;
        }
        for(float *it3=resamp_data; it3<resamp_data_end; it3++){
            sum3+=*it3;
        }
        if((sum3-sum1)/sum1>0.001 || (sum1-sum3)/sum1>0.001){
            cout << "Breaking, " << sum3 << " != " << sum1 << " " << sum1-sum3 << " " << endl;
            return 155;
        }
    */


//      Find out what the bad channels are NOTE check if we can skip this, as this is already done with the RFI cleaner
        int validchannels=TotNrChannels;
        int validsamples=samples;
        bool doFlagging=true;

if(doFlagging){
        if(verbose){
            cout << "Flagging channels in bewtween " << resamp_data << " " << resamp_data_end << endl; 
        }

// RFI cleaning. Calculate baseline (sum over time for each channel) and divide by it. 
// This makes the average one.
// Calculate the sum of the square of the samples
// This should be roughly equal for pure noise
// RFI channels stick out here, and are cut off by calculating and cleaning bad channels 
        if(RFIcleaner.checkDataloss(3)){
            lastBadBlock=blockNr;
        }


        RFIcleaner.calcBaseline();
        RFIcleaner.writeBaseline(basefile, blockNr);
        RFIcleaner.divideBaseline();
    if(cleanRFI){
        RFIcleaner.calcBaseline(); // NOTE this should be one? Do we need to calculate it?
        //RFIcleaner.printBaseline();
        RFIcleaner.calcSqrBaseline();
        nrFlaggedChannels=RFIcleaner.calcBadChannels(15); // 15 sigma cut, because bad samples may also give rise to 
        // seeing a lot of 'bad' channels.
        if(verbose){
            RFIcleaner.printBadChannels();
        }
        RFIcleaner.cleanChannels("1"); // replaced by 1
        RFIcleaner.writeBadChannels(fcfile, blockNr); // write bad channels to file



// Calculate bad samples, by summing in time over a quarter of the bandwidth
// samples that are more than cutlevel*sigma above average in at least 2 of the quarters are flagged
        nrFlaggedSamples=RFIcleaner.calcBadSamples(4); // cutlevel = 4 sigma
        if(verbose){
            
            RFIcleaner.printBadSamples();
        }
        RFIcleaner.cleanSamples("1"); // replace by 1
        RFIcleaner.writeBadSamples(fsfile, blockNr); // write bad samples to file


        // flagging remaining bad channels, see steps above
        RFIcleaner.calcBaseline();
        RFIcleaner.calcSqrBaseline();
        nrFlaggedChannels+=RFIcleaner.calcBadChannels(6); // 6 sigma cut
        if(verbose){
            RFIcleaner.printBadChannels();
        }
        RFIcleaner.writeBadChannels(fcfile, blockNr);

        // cleans channels
        RFIcleaner.cleanChannels("1");
        // removes the bad 0th channel of each subband.
        nrFlaggedChannels+=RFIcleaner.cleanChannel0("1");
        // Subtract average timeseries, a technique known as ZeroDMing. Effectiveness needs to be investigated.
        if(DoZeroDM){
            //RFIcleaner.calcAverageTimeseries();
            //RFIcleaner.subtractAverageTimeseries();
            //# Per stream
            for(int sc=0; sc<nstreams; sc++){
                Rstartchan=NrChannels*sc;
                Rendchan=NrChannels*(sc+1);
                RFIcleaner.calcAverageTimeseriesStream(Rstartchan,Rendchan);
                RFIcleaner.subtractAverageTimeseriesStream(Rstartchan,Rendchan);
                if(blockNr<2){
                    cout << blockNr << " " << sc << " ";
                    RFIcleaner.printAverageTimeseriesStream(false);
                    cout << blockNr << ";" << sc << " ";
                    RFIcleaner.calcAverageTimeseriesStream(Rstartchan,Rendchan);
                    RFIcleaner.printAverageTimeseriesStream(false);
                }
                
            }

        }
    } // if clean

        // Write baseline and sqrt timeseries to files
        RFIcleaner.calcBaseline();
        RFIcleaner.writeSqrTimeseries(sqrtimeseriesfile, blockNr);

        //RFIcleaner.writeBaseline(basefile, blockNr);

        if(verbose){
            cout << " We are here now . 1 " << sizeof(chanIdVal)<< endl;     
        }

} //do flagging
        if(verbose){
            cout << " We are here now . 2 " << endl;     
        }

        // For each stream
		for(int sc=0; sc < nstreams; sc++){
            

            // Do the DMs in parallel
            #ifdef _OPENMP
                //std::cout<<"Running in parallel mode"<<std::endl;
                #pragma omp parallel for private(foundpulse)
            #else
                //std::cout<<"Running in serial mode"<<std::endl;
            #endif // _OPENMP
		    
            
        	for(int DMcounter=0; DMcounter<nDMs; DMcounter++){	//analyse data of one stream for all DMs
                if(verbose){
                    cout << "Processing " << sc << " " << DMcounter << endl;
                }
                //# Processing data now split up into three separate tasks:
                //# dedisperseData2 : calculates dedispersed data
                SBTs[sc][DMcounter]->setNrFlaggedChannels(nrFlaggedChannels);
                SBTs[sc][DMcounter]->setNrFlaggedSamples(nrFlaggedSamples);
                SBTs[sc][DMcounter]->setLastBadBlock(lastBadBlock);
                
				foundpulse=SBTs[sc][DMcounter]->dedisperseData2(resamp_data, blockNr, &cc[DMcounter], CoinNr, CoinTime,Transposed);
                //# calcAverageStddev : calculates average, stddev and thresholdlevel
				foundpulse=SBTs[sc][DMcounter]->calcAverageStddev(blockNr);
                int intLength;
                // loop over different pulse search lengths
                for(int i=0;i<integrationlength.size();i++){
                    intLength=integrationlength[i];
                    //# runTrigger : sums data and compares it to the trigger threshold.
                    foundpulse=SBTs[sc][DMcounter]->runTrigger(blockNr, intLength, &cc[DMcounter], CoinNr, CoinTime,Transposed);
                }
                if(verbose){
                    cout << "f" <<  foundpulse << endl;
                }
              /*  if(DMcounter<mynDMs){
                    datamonitor[sc][DMcounter] << SBTs[sc][DMcounter]->blockAnalysisSummary() << "\n";
                } */
				if(foundpulse){ // actually finding pulses is handled by a separate program, or maybe we want to incorporate it here again.
                    
                    // output information and send it to a logfile 
                    cout << "Found pulse " << pulsenr << " " << SBTs[sc][DMcounter]->FoundTriggers();
                    alltriggerlogfile << "pulse " << pulsenr << " " <<SBTs[sc][DMcounter]->FoundTriggers();
                    alltriggerlogfile.flush();
                    // output dedispersed timeseries for the detected pulse
					for(int fc2=0;fc2<nstreams;fc2++){
						
						stringstream pulselogfn;
						pulselogfn << pulsedir << "/pulse" << pulsenr << "_" << fc2 << ".log";
						string pulselogfilename;
						pulselogfn >> pulselogfilename;
						
						SBTs[fc2][DMcounter]->makeplotBuffer(pulselogfilename);

					}
					pulsenr++;
				}
                // output dedispersed timeseries, if number of DMs is less than 3. NOTE: make this configurable?
                if(nDMs <=3){
						stringstream pulselogfn;
                        pulselogfn.precision(3);
                        pulselogfn.setf(ios::fixed,ios::floatfield);
						pulselogfn << pulsedir << "/dedisptimeseries_" << SBTs[sc][DMcounter]->itsDM << "_" << sc << ".log";
						string pulselogfilename;
						pulselogfn >> pulselogfilename;
						SBTs[sc][DMcounter]->makeplotDedispBlock(pulselogfilename);
                }
			}
			

		}
        usleep(sleeptime);

	}

    // Write averages  and standard deviations of each block to the file
    for(int DMcounter=0; DMcounter<nDMs; DMcounter++){
        for(int fc2=0;fc2<nstreams;fc2++){
            	SBTs[fc2][DMcounter]->writeAverage(avfile);
            	SBTs[fc2][DMcounter]->writeStdDev(stdfile);
        }
    }

    // close the files
    fcfile->close();
    fsfile->close();
    avfile->close();
    stdfile->close();
    //triggerlogfile.close();
    alltriggerlogfile.close();
    basefile->close();
    sqrtimeseriesfile->close();
    //fclose(pFileOut); 
    // NOTE close pFile?
	return 0;
}




