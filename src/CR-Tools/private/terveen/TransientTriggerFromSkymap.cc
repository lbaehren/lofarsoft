
/*-------------------------------------------------------------------------*
 | $Id:: imagetofits.cc 															                   
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen ( S.terveen@astro.ru.nl )                             *
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

#include <crtools.h>
#include <Utilities/TestsCommon.h>

//#include <images/Images/ImageFITSConverter.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>
#include <images/Images/RebinImage.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <sys/stat.h>
#include <Analysis/CoincidenceCheck.cc>
#include <stdio.h>


//using namespace FRAT::coincidence;
//#include <casa/Arrays/Slicer.h>
using namespace casa;
using namespace std;

#define CHANNELS 64
#define SAMPLES 128


//int main(int argc, char *argv[])
//{
//	string infile;
//	string datatype;
//	int normalize=0;
//	int channels_p;
//	int firstcoordinate;
//	int secondcoordinate;
//	int thirdcoordinate;
//	int startfreq; int endfreq;
//	int nrblocks;
//	/*
//	 Check if filename of the dataset and the format are provided on the command line.
//	 */
//	if (argc < 3) {
//		cerr << "Usage: TransientTriggerFromSkymap <input.h5> <nrfreqsperblock> <startfreq #> <nrblocks>\n"
//		<< "Supported datatypes are 'hdf5'. 'paged' is not implemented yet.\n"
//		<< "This will dedisperse over time, so that after a collapse in time, the frequency series is left."
//		<< "/Users/STV/Astro/data/TBB/Crab/pulse1/skymap6x1_2_6khz_start20_1500blocks.h5"
//		<< endl;
//		return 1;
//	} else {
//		infile = argv[1];
//		datatype = argv[2];
//		channels_p = CHANNELS;//atoi(argv[3]);
//		startfreq = atoi(argv[4]);
//		nrblocks = atoi(argv[5]);
//		firstcoordinate = 5;//atoi(argv[6]);
//		secondcoordinate = 0;//atoi(argv[7]);
//		thirdcoordinate = 0;
//	}	//cout<<freqcomp<<endl;
//	
//	//testfun (string infile2, int firstSB, int lastSB, int integrationlength, int triggerlevel=20, string logfilename="/Users/STV/Documents/GiantPulse/excesslog_pdt.txt", string triggerlogfilename="/Users/STV/Astro/Programming/pulsar/trigger.log", int nrblocks=51, int CoinNr=10, int CoinTime=20, int startpos=0, int channels_p=CHANNELS, int samples_p=SAMPLES, bool DoPadding=0, string extension=".stokes", bool verbose=0, int startsubbandnumber=321, int timeintegration=1)
//	
//
//	//String fitsName = "temp.fits";
////	
////	String doit;
//	//Bool ok;
//	
//	// Open the image and convert it to fits:
//	return 0;
//}



	
int testfun (string infile2, int firstSB, int lastSB, int integrationlength, int triggerlevel=20, string logfilename="/Users/STV/Documents/GiantPulse/excesslog_pdt.txt", string triggerlogfilename="/Users/STV/Astro/Programming/pulsar/trigger.log", int nrblocks=1, int CoinNr=10, int CoinTime=20, int startpos=0, int channels_p=100, int samples_p=SAMPLES, bool DoPadding=0, string extension=".stokes", bool verbose=0, int startfreqnr=321, int timeintegration=1, int firstcoordinate=4, int secondcoordinate=0, int thirdcoordinate=0)
	{   //change default parameters
	
		
	int nSubbands=lastSB-firstSB+1;
		
		string inputdir;	
	String error;		
			
    HDF5Image<Float> imageIn (infile2);
			// Regridding of input file:
			
			
	IPosition factors (imageIn.shape());
			cout << "-- image shape:" << imageIn.shape() << std::endl;
			
			
			
			Array<casa::Float> data;
			//int nrblocks=1;
			cout << "made data" << std::endl;
			//0longitude, 1latitude, 2length, 3frequency, 4time
			IPosition start (5,firstcoordinate,secondcoordinate,thirdcoordinate,startfreqnr,0), length=factors, stride (5,1,1,1,1,1);
			length(0)=1;
			length(1)=1;
			length(2)=1;
			length(3)=channels_p*nSubbands;
			//length(4)=SAMPLES;			
			
			//end(3)=1; //end(4)=1;
			//data.resize(end,false);
			//Slicer zeroslicer (start, end, stride, endIsLast); //endIsLast // endIsLength (default)
			int freqoffset;
			int nrfreqs=factors(3);
			int nrtimes=factors(4);
		    samples_p=factors(4)/nrblocks;
		    
		    //start(4)=k*samples_p;
		    data.resize(length,false);
		    Slicer zeroslicer (start, length, stride);
		    imageIn.doGetSlice(data, zeroslicer);
//			for(int blocknr=0;blocknr<nrblocks;blocknr++){
//				
//				
//				freqoffset=(endfreq-freq)%dispersion_width;
//				//cout << "timeoffset = " << timeoffset << endl;
//				
//				
//				start(3)=startfreq+freqblock*nrfreqsperblock;
//				start(4)=timeblock*SAMPLES;
//				data.resize(length,false);
//				Slicer zeroslicer (start, length, stride);
//				imageIn.doGetSlice(data, zeroslicer);
//				start(4)=0;
//
//			}
			
			//factors    = 1;
			//factors(3) = freqcomp;
			//factors(4) = timecomp; //quick hax to sum time data instead
			//factors(0) = 60;
			//factors(1) = 60;
			
			//RebinImage<Float> rb(imageIn, factors);
			
			
			/* Summary of image properties */
			//cout << "-- Frequency channels " << firstfreq << " - " << lastfreq <<" are dedispersed " << std::endl;
			
			cout << "-- Summary of the image:" << std::endl;
			DAL::summary (imageIn);
			
	
	
		//ok = CR::image2fits(rb, fitsName);	
		
		//if(ok){
		//cout<<"Image converted to "<<fitsName<<"."<<endl;
	

	int nofFailedTests (0);
	
	cout << "Nr of input files " << nSubbands << endl;
	int nrCHANNELS = channels_p ;  //prev: *ninputfiles, but now each file is treated seperately
	//int STRIDE=(channels_p/64)*( channels_p/64);
	//cout << "STRIDE = " << STRIDE << endl;
	int bufferlength=samples_p*nrblocks;
    //cout << "Bufferlength = " << bufferlength << endl;
	int dedispersionoffset[nSubbands][nrCHANNELS+1];
	//int startsubbandnumber=321;
	float startfreq=startfreqnr*0.1/factors(3)+0.1; //(in GHz, for a samplefrequency of 200 MHz)
	float DM=56.8; //value for Crab pulsar
	float frequencyresolution=0.1/factors(3); // (in GHz)
	float timeresolution=5e-9*2*(factors(3)-1); // (in s)
		cout << "Resolution freq,time " << frequencyresolution << ", " << timeresolution << endl;
	
	// Adjust this part to have all timeoffset with respect to the start frequency?
	//cout << "offsets within a subband: ";
	for(int channel=0;channel<nrCHANNELS;channel++){
		for(int fc=0;fc<nSubbands;fc++){
			float freq1=startfreq+fc*0.1/factors(3)*channels_p;
			float freq2=freq1+channel*frequencyresolution;
			double offset=4.5e-3*DM*(1/(freq1*freq1)-1/(freq2*freq2))/timeresolution;
			// printf("%f ",offset);
			
			// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
			
			dedispersionoffset[fc][channel]=(int)offset;		
		}
	}
	cout << endl;
	cout << "offset between subbands: ";
	for(int fc=0;fc<nSubbands;fc++){
		float freq1=startfreq;
		float freq2=startfreq+fc*0.1/factors(3)*channels_p;
		float offset=4.5e-3*DM*(1/(freq1*freq1)-1/(freq2*freq2))/timeresolution;
		
		printf("%f %f %f ",freq1, freq2, offset);
		
		// dedispersionoffset[fc][channel]=(int) (nrCHANNELS-channel-1)*(64*64)/(channels_p*channels_p);
		
		dedispersionoffset[fc][nrCHANNELS]=(int)offset;		
	}
	cout << endl;
	//return 0;
	/*
	 float *av; 
	 av = new float[ninputfiles];
	 float* maxi;
	 maxi = new float[ninputfiles];
	 */
	
	//--------------READ IN THE FILES----------------------------------	
	FILE ** pFile;
	pFile = new FILE*[nSubbands];	
	
	std::string* infile;
	infile=new std::string[nSubbands];
	
	
	
	std::string * shortinfile;
	shortinfile = new std::string[nSubbands];
	for(int i=firstSB; i<=lastSB; i++){
		std::stringstream ss;
		std::string number;
		ss << i;
		ss >> number;
		infile[i-firstSB]=inputdir + "SB" + number + ".MS" + extension;
		cout << "using file " << infile[i-firstSB] << endl;
		shortinfile[i-firstSB] = "SB" + number;
    }
	//	infile[0]="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS";
	//	infile[1-1]="/Users/STV/Astro/data/pulsars/L2009_13298/SB1.MS";
	//	infile[2-1]="/Users/STV/Astro/data/pulsars/L2009_13298/SB2.MS";
	//	infile[3-1]="/Users/STV/Astro/data/pulsars/L2009_13298/SB3.MS";
	//	infile[4-1]="/Users/STV/Astro/data/pulsars/L2009_13298/SB4.MS";
	//	infile[5-5]="/Users/STV/Astro/data/pulsars/L2009_13298/SB5.MS";
	//	infile[6-5]="/Users/STV/Astro/data/pulsars/L2009_13298/SB6.MS";
	//	infile[7-5]="/Users/STV/Astro/data/pulsars/L2009_13298/SB7.MS";
	//	infile[8-5]="/Users/STV/Astro/data/pulsars/L2009_13298/SB8.MS";
	//	infile[9-5]="/Users/STV/Astro/data/pulsars/L2009_13298/SB9.MS";
	//	infile[10-6]="/Users/STV/Astro/data/pulsars/L2009_13298/SBX.MS";
	/*	av[0]=7.92848e+07;
	 av[1]=7.96872e+07;
	 av[2]=7.90043e+07;
	 av[3]=7.95275e+07;
	 av[4]=7.86694e+07;
	 av[5]=7.87128e+07;
	 av[6]=7.73298e+07;
	 av[7]=7.78624e+07;
	 av[8]=7.75276e+07;
	 av[9]=7.83774e+07;
	 av[10]=7.64856e+07;
	 maxi[0]=1.96146e+08;
	 maxi[1]=1.89357e+08;
	 maxi[2]=2.04084e+08;
	 maxi[3]=1.82849e+08;
	 maxi[4]=1.92037e+08;
	 maxi[5]=1.92135e+08;
	 maxi[6]=2.03838e+08;
	 maxi[7]=4.20479e+08;	
	 maxi[8]=1.76646e+08;
	 maxi[9]=3.47141e+08;
	 maxi[10]=1.85754e+08;
	 */
	
	
	
	
	
	
//	for(int i=0;i<nSubbands;i++){
//		do {
//			pFile[i] = fopen(infile[i].c_str(),"rb");
//			cout << "File not there yet" << endl;
//			usleep(500000);
//		} while (pFile[i]==NULL);
//		
//	}
	
	//------------------set parameters------------------------
	float av=5.9;
	float maxi = 4.205e+08;
	int nrbins=300;
	float bin_width=0.025; //*sqrt(integrationlength);
	
	
	//---------------- set logfile ---------------------------	
	//
	ofstream logfile;
	ofstream triggerlogfile;
	logfile.open(logfilename.c_str());
	triggerlogfile.open(triggerlogfilename.c_str());
	
	//construct the datatype
	
	
	//--------------- set the variables used	
	//	float subsumfw[integrationlength];
	float subsumdedispersed[nSubbands][integrationlength];
    float subsum[nSubbands][integrationlength];
	for(int it=0;it < integrationlength; it++){
		for(int fc=0; fc<nSubbands; fc++){
			subsum[fc][it]=0; subsumdedispersed[fc][it]=0;
		}
	}// subsumfw[it]=0;}
	//	float sumsubsumfw;
	float sumsubsumbw;
	float sumsubsum;	
	
	//    stokesdata* data;
	//	data = new stokesdata[ninputfiles];
	
	//Array<casa::Float> data;
//	float *data=(float*)malloc(ninputfiles*channels_p*(samples_p|2)*sizeof(float));
//	if (data==NULL)
//	{
//		cerr << "Memory could not be allocated\n";
//		return 1;
//	}
	
//	unsigned int sequence_nr[nSubbands];
//	char pad[508];
//	int stokesdatasize=channels_p*(samples_p|2)*sizeof(float)+sizeof(unsigned int);
//	if(DoPadding){stokesdatasize+=sizeof(pad);}
//	cout << "stokesdatasize " << stokesdatasize << endl;
	
	
	
	
	int kmax=nrblocks;//3595;//51;
	//casa::Matrix<double> matrix(CHANNELS,2);//kmax*SAMPLES);
	float sum =0;
	float maximum[nSubbands];
	//	float fwmaximum=0; 
	float bwmaximum[nSubbands];
	//	float fw[nrCHANNELS];
	float bw[nSubbands][bufferlength];
	float blocksum[nSubbands];
	int blockvalidsamples[nSubbands];
	float SBaverage[nSubbands];
	unsigned int histogram[nSubbands][nrbins];
	unsigned int bwhistogram[nSubbands][nrbins];
	for(int j=0; j<nSubbands; j++){ maximum[j]=0; bwmaximum[j]=0; SBaverage[j]=5.9*integrationlength; }
	//	unsigned int fwhistogram[nrbins];
	for(int i=0;i<bufferlength;i++){ 
		for(int j=0; j<nSubbands; j++){
			bw[j][i]=0; 
		}
	} //fw[i]=0;
	for(int i=0;i<nrbins;i++){
		for(int j=0; j<nSubbands; j++){
			histogram[j][i]=0; bwhistogram[j][i]=0;
		}
	}// fwhistogram[i]=0;}
	
	unsigned validsamples=0;
	int zerocounter=0;
	int latestindex=0;
	
	//	std::stringstream excessfwT;
	//	std::stringstream excessfwP;
	//	std::stringstream excessfwB; //block
	//	std::stringstream excessfwS; //sample
	std::stringstream excessbwT;
	std::stringstream excessbwP;
	std::stringstream excessbwB; //block
	std::stringstream excessbwS; //sample
	std::stringstream excessT;
	std::stringstream excessP;
	std::stringstream hgstr; //histogramstring
	
	//	excessfwT << shortinfile[0] <<"-" << shortinfile[ninputfiles-1] << " fwT = [";
	//	excessfwP << shortinfile[0] <<"-" << shortinfile[ninputfiles-1] << " fwP = [";
	//	excessfwB << shortinfile[0] <<"-" << shortinfile[ninputfiles-1] << " fwBlock = [";
	//	excessfwS << shortinfile[0] <<"-" << shortinfile[ninputfiles-1] << " fwSample = [";
	excessbwT << shortinfile[0] <<"-" << shortinfile[nSubbands-1] << " bwT = [";
	excessbwP << shortinfile[0] <<"-" << shortinfile[nSubbands-1] << " bwP = [";
	excessbwB << shortinfile[0] <<"-" << shortinfile[nSubbands-1] << " bwBlock = [";
	excessbwS << shortinfile[0] <<"-" << shortinfile[nSubbands-1] << " bwSample = [";
	excessT << shortinfile << "T = [";
	excessP << shortinfile << "P = [";
	
	
	
	
	
	av = av*integrationlength;
	
	//nrbins = (int) integrationlength*maxi/av/bin_width + 1; 
	cout << "Nr bins = " << nrbins << endl;
    if (nrbins<0){cout << "Negative nr of bins. Quitting....\n values: \nintegration length: " << integrationlength << "\nmaxi " << maxi << "\nav : " << av << "\nbin width " << bin_width << endl ; return 1;}
	
	
	int bin;
	unsigned int rest=0;	
	unsigned num;
	int totchannel;
	int totaltime;
	int triggertime[nSubbands];
	int triggerlength[nSubbands];
	int triggerblock[nSubbands];
	int triggersample[nSubbands];
	float triggeraverage[nSubbands];
	float triggerbin[nSubbands];
	
	
	
	//					 struct triggerEvent {
	//					 int time;
	//					 int length;	
	//					 int sample;
	//					 int block;
	//					 int subband;
	//					 float sum;
	//					 float max;
	//					 };				 
	//					 
  struct triggerEvent trigger[nSubbands];
	//struct triggerEvent* trigger = (struct triggerEvent*)malloc(ninputfiles*sizeof(struct triggerEvent));
	
	for(int fc=0; fc<nSubbands; fc++){
	    trigger[fc].time=0;
		trigger[fc].length=0;
		trigger[fc].block=0;
		trigger[fc].sample=0;
		trigger[fc].sum=0;
		trigger[fc].max=0;
		trigger[fc].subband=fc+firstSB;
	}
	float binfl;
	
	
	fpos_t pos;
	//int samples_pOr2 = samples_p|2;
		CR::CoincidenceCheck cc = CR::CoincidenceCheck();
	int triggercount=0;	
	for(int k=0;k<nrblocks;k++){ 
		
		//int fc=0;//	  
//		for(int i=0; i < ninputfiles; i++){
//			int fc=i;
//			fgetpos(pFile[fc],&pos);
//			
//			//num = fread( &data[fc], sizeof data[fc], 1, pFile[fc]);
//			
//			num = fread( &(sequence_nr[i]), sizeof(sequence_nr[i]), 1, pFile[i]); //read sequence number
//			if( !num) {
//				cout << "Error, EOF reached.\n";
//			}
//			if(DoPadding){
//				num = fread( &(pad),sizeof(pad), 1, pFile[i]); //read padding
//				if( !num) {
//					cout << "Error, EOF reached.\n";
//				}
//			}
//			num = fread( &(data[i*(samples_pOr2)*channels_p]), (samples_pOr2)*channels_p*sizeof(float), 1, pFile[i]); //read date
//			if( !num) {
//				cout << "Error, EOF reached.\n";
//			}
//			if( !num) {
//				cout << "*";
//				usleep(50000);
//				fsetpos(pFile[fc],&pos);
//				fc--; //1 file k--
//				continue;
//			}
//			else {		  
//				swap_endian( (char*)&(sequence_nr[i]) );
//				//if(data[fc].sequence_number%500==0 && fc==0){  
//				cerr << "reading sequence number: " << sequence_nr[i] << endl;
//                blockvalidsamples[i]=0; blocksum[i]=0;
//				//  }
//            }
//		} // for files 
		
		



		
		
		//if(!FileExists(triggerlogfilename.c_str())){					    
		triggerlogfile.close();
		triggerlogfile.open(triggerlogfilename.c_str());
		// } //reopen file if it was removed
		//triggerlogfile << "file reopened for block " << k << endl;
		
		
		//if(k==0){continue;} // data filled with zeros
		//if( (k>500 && k < 511) || (k>730 && k< 933) || (k>730 && k< 933) ){continue;}
		
		// Procedure 1)
		//    Loop over blocks
		//    Check if block exists. If it does check the next file if the block exists
		//		Analyse block:
		//          Loop over time
		//				Loop over channels
		//                  Loop over files
		//						Get the value and add it to the right sum
		//						If the sum is complete put it in the right bin of the histogram
		//						If it is above a certain threshold, write a trigger message
		//                  GOto next file  
		//				goto next channel
		//				Set the vertical sum in a histogram
		//         goto next time
		//     goto next block
		
		float channelsum[nSubbands]; 
		
		for(int time=0;time<samples_p;time++)
		{
			totaltime=k*samples_p+time;
			for(int fc=0; fc < nSubbands; fc++){channelsum[fc]=0;}
			
			//float validcounter = 0;
			
			for(int channel=0; channel<channels_p ;channel++){
		 	    for(int fc=0; fc < nSubbands; fc++){
					

					
					
					
					
					//float value =  data[fc].samples[channel][time] ; //const float before
					float value = data(IPosition(5,0,0,0,channel+fc*channels_p,totaltime));
					//if(DoPadding){value = FloatSwap(value);}
					//if(k==2 && time < 3 ) { cout << "value is: " <<  value << endl; }	
					//value=FloatSwap(value);
					//if(k==2 && time < 3 ) { cout << "value is: " << value << endl; }	
					//matrix(channel,time) = value;
					totchannel = channel;
					//cout <<  endl;
					// if isnan value --> value is zero, add counters for how many good things there are
					if( !isnan( value ) ) {
						//value = 0;
						
						
						validsamples++;
						blockvalidsamples[fc]++;
					}
					else { value = 0; }
					if(value == 0 &&  channel % channels_p  ==0) { zerocounter++; cout << "zero found on time: "<< k*samples_p+time << " in channel " << totchannel << endl;}
					//			if(shortinfile[fc] == "SB7" && (channel==28 || channel >=53 && channel <=55)){value=av/nrCHANNELS; if(k==1 && time==0){cout << "ignoring RFI channels for SB7\n";}} //ignoring channels 28-30
					//			else if(shortinfile[fc] == "SB9" && (channel>=4 && channel <=6)){value=av/nrCHANNELS; if(k==1 && time==0){cout << "ignoring RFI channels for SB9\n";}} //ignoring channels 28-30
					
					channelsum[fc] += value;
					blocksum[fc] += value;
					
					rest=totaltime%bufferlength;
					
					
					if(true || k>2){
						if(totchannel==0){//nrCHANNELS-1){
							//bw[fc][(rest+f*totchannel)%nrCHANNELS]+=value;
							bw[fc][rest]+=value;
							if(bw[fc][rest]>bwmaximum[fc]){
								bwmaximum[fc]=bw[fc][rest]; 
								//cout << "new backward maximum: " <<bwmaximum << endl;
							}
							//bin = (int) bw[rest]/av/bin_width;
							subsumdedispersed[fc][totaltime%integrationlength] = bw[fc][rest];// /bwcounter[rest]*CHANNELS;
							sumsubsumbw=0;
							for(int it=0;it<integrationlength;it++){sumsubsumbw+=subsumdedispersed[fc][it];}
							bin = (int) sumsubsumbw/SBaverage[fc]/bin_width; //changed from av to SBaverage
							if(bin>=triggerlevel){
								binfl = sumsubsumbw/SBaverage[fc]/bin_width; //changed from av to SBaverage 
								excessbwT << totaltime << " "; excessbwP << binfl << " "; excessbwB << k << " "; excessbwS << time << " ";
								if(totaltime+dedispersionoffset[fc][channels_p]-trigger[fc].time>3){
									//new trigger
									//trigger.average[fc]/=triggerlength[fc];
									//trigger[fc].time=totaltime+fc*channels_p*STRIDE; //correction for dispersion
									trigger[fc].time=totaltime+dedispersionoffset[fc][channels_p];//correction for dispersion fc* removed
									
									trigger[fc].sum=binfl;
									trigger[fc].length=1;
									trigger[fc].sample=time;
									trigger[fc].block=k;
									trigger[fc].max=binfl;
								}
								else{
									//old trigger, or trigger accross blocks
									trigger[fc].sum+=binfl;
									trigger[fc].length++;
									//trigger[fc].time=totaltime+fc*channels_p*STRIDE; //correction for dispersion
									trigger[fc].time=totaltime+dedispersionoffset[fc][channels_p]; //correction for dispersion fc* removed
									if(binfl>trigger[fc].max){trigger[fc].max=binfl;} //calculate maximum
								}
								
								
								//triggerlogfile << "trigger " << triggercount << " found in block " << k << " at time " << time << " with level " << bin << " in subband " << shortinfile[fc] << endl; 
								
								triggercount++; 
								
							} else if(totaltime+dedispersionoffset[fc][channels_p]-trigger[fc].time==3 && k>3){ //k>3 to correct for zeros in the first block.
								logfile << "Feeding trigger " << k << " " << fc << " " << totaltime << " " << triggertime[fc] << endl;
								//if(CoincidenceTrigger(triggertable, triggernumber, fc, triggertime[fc], ninputfiles, nrstoredtriggers))
								//cout << "\n\n ADDING TRIGGER \n\n\n";
								
								latestindex = cc.add2buffer(trigger[fc]);
								
								if(cc.CheckNow(latestindex, CoinNr, CoinTime)) {//nChannles, timewindow
									
									triggerlogfile << "COINCIDENCE TRIGGER FOUND! " << k << "\n";
									cout << "\n\n\nCOINCIDENCE TRIGGER FOUND\n" << k << "\n\n";
									triggerlogfile << "trigger " << triggercount << " found in block " << trigger[fc].block << " sample: " << trigger[fc].sample << " time " << trigger[fc].time << " " << " length: " << trigger[fc].length << " strength: " << trigger[fc].max << " average: " << trigger[fc].sum/trigger[fc].length << " subband: " << trigger[fc].subband << endl;
									logfile << "trigger " << triggercount << " found in block " << trigger[fc].block << " sample: " << trigger[fc].sample << " time " << trigger[fc].time << " " << " length: " << trigger[fc].length << " strength: " << trigger[fc].max << " average: " << trigger[fc].sum/trigger[fc].length << " subband: " << trigger[fc].subband << endl;
									usleep(2000000);
								}
							}
							
							if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsumbw << "which is " << bin << "sigma" << endl;}
							else if(/*k>1 ||*/ time >= nrCHANNELS+integrationlength-1){  bwhistogram[fc][bin]++;  }
							
							bw[fc][rest]=0;//value;
							
						}
						else { //if(value!=0) {
							//bw[fc][(rest+STRIDE*totchannel)%bufferlength]+=value;
							
							bw[fc][(rest+dedispersionoffset[fc][channel])%bufferlength]+=value;
							//bw[fc][(rest-(nrCHANNELS-1)+totchannel)%nrCHANNELS]+=value;
							//bwcounter[(rest-63+channel)%64]++;
						}
						
						//		  if(channel==63 && fc==ninputfiles-1){
						//			 if(bw[rest]>bwmaximum){
						//			      bwmaximum=bw[rest]; 
						//				 //cout << "new backward maximum: " <<bwmaximum << endl;
						//			 }
						//			  //bin = (int) bw[rest]/av/bin_width;
						//			  subsumbw[(k*SAMPLES+time)%integrationlength] = bw[rest];// /bwcounter[rest]*CHANNELS;
						//              sumsubsumbw=0;
						//		      for(int it=0;it<integrationlength;it++){sumsubsumbw+=subsumbw[it];}
						//			  bin = (int) sumsubsumbw/av/bin_width;
						//			  if(bin>=triggerlevel){ excessbwT << totaltime << " "; excessbwP << bin << " "; excessbwB << k << " "; excessbwS << time << " ";}
						//			  
						//			  if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsumbw << "which is " << bin << "sigma" << endl;}
						//			  else if(k>1 || time >= nrCHANNELS+integrationlength-1){  bwhistogram[bin]++;  }
						//			  
						//			  bw[rest]=value;
						//			 
						//		  }
						//		  else { //if(value!=0) {
						//			  bw[(rest-nrCHANNELS+1+fc*CHANNELS+channel)%(nrCHANNELS)]+=value;
						//			  //bwcounter[(rest-63+channel)%64]++;
						//		  }
						
						
						
						
						
					} // if k>2
				} // for files
			} // for channels
			//bin = (int) channelsum/av/bin_width;
			for(int fc=0; fc<nSubbands; fc++){
				subsum[fc][(k*samples_p+time)%integrationlength]=channelsum[fc];//  /validcounter*CHANNELS;
				sumsubsum=0;
				for(int it=0;it<integrationlength;it++){sumsubsum+=subsum[fc][it];}
				bin = (int) sumsubsum/SBaverage[fc]/bin_width;
				if(bin>80){ excessT << totaltime << " "; excessP << bin << " ";} //shouldn't be anything above 20 actually
				if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsum << "which is " << bin << "sigma";}
				else{histogram[fc][bin]++  ;}
				
				sum += channelsum[fc];
				
				
				if(channelsum[fc]>maximum[fc]){maximum[fc]=channelsum[fc]; }//cout << "new maximum: " <<maximum<< endl;}
			}//if(channelsum<minimum){minimum=channelsum; cout << "new minimum: " << minimum << endl;}
			//  cout << endl;
			//float average= sum / validsamples;
			/*float average0;
			 if(k==1){average0 = average;}
			 if(average > (average0 + 50)) cout << "------------------------------------------------------------------------\n";
			 cout << "k= " << k << " " << min(matrix) << " min | max " << max(matrix) << " average " << average << endl;
			 */
		} // for time
		for(int fc=0; fc<nSubbands; fc++){
			//if(k>=2){
			
			SBaverage[fc]=blocksum[fc]/blockvalidsamples[fc]*channels_p*integrationlength;
			
			cerr << " SBaverage at block "<<  k <<" in subband " << fc << " is "<< SBaverage[fc] << endl << blocksum[fc] << " " << blockvalidsamples[fc] << endl;
			//}
		}
		
	}
    float average= sum / validsamples;
	cout << "\n\n Average bin data= " << average << endl;
	cout << "Valid sample number = " << validsamples << endl;
	cout << "Number of zeros = " << zerocounter << endl;
	cout << "Average used: " << av << endl;
	cout << "Integration length: " << integrationlength << endl;
	cout << "Average sum data (invalid?)= " << nrCHANNELS*average << endl;
	
	for(int fc=0; fc<nSubbands; fc++){
		cout << "Maximum for SB " << fc << " = " << maximum[fc] << endl;
		//	cout << "Forward diagonal maximum = " << fwmaximum << endl;
		cout << "Backward diagonal maximum for SB " << fc << " = " << bwmaximum[fc] << endl;
		/* Plot the data to a PS file. */
		if(verbose){
			cout << "\n\n bin width " << bin_width <<"\nBin hg bw fw\n";
			for(int i=0;i<nrbins;i++){cout << i << " " << histogram[fc][i] << " " << bwhistogram[fc][i] << " " << endl;} //fwhistogram[i] << endl;}
			//for(int i=40;i<46;i++){cout << i << " " << histogram[fc][i] << " " << bwhistogram[fc][i] << " " << endl;} //fwhistogram[i] << endl;}
		}
		//cout << hgstr.str();
		//logfile << "\n\n" << excessT.str() << "];" << endl << excessP.str() << "];"  << endl  << excessfwT.str() << "];"  << endl << excessfwP.str() << "];" << endl  << excessbwT.str() << "];" << endl << excessbwP.str() << "];"  << endl;
	}
	logfile << "\n\n" << excessbwB.str() << "];" << endl << excessbwS.str() << "];"  << endl  << excessbwT.str() << "];" << endl << excessbwP.str() << "];"  << endl ;// << endl  << excessfwB.str() << "];" << endl << excessfwS.str() << "];"<< excessfwT.str() << "];"  << endl << excessfwP.str() << "];" << endl;
	logfile.close();
	
	
	/*newObject.quickPlot("tSimplePlot-line.ps", xval, yval, empty, empty,
	 "X-axis", "Y-Axis", "plotting-test with lines",
	 4, True, 1, False, False, True);
	 */
	
	
	return nofFailedTests;
}


int main (int argc,
		  char *argv[])
{
	cout<<"This file outputs histograms of pulsar data to /Users/STV/Documents/GiantPulse/."<<endl;
	int integrationlength = 1; //average length
	int triggerlevel = 141/3;
 	unsigned int nofFailedTests=0;
	float average;
	std::string inputdir, configfilename;
	int firstSB, lastSB, nrblocks=5, startpos=0;
	int CoinNr=10;
	int CoinTime=80;
	//	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	std::string logfilename = "/Users/STV/Documents/GiantPulse/excesslog_pdt.txt";
	std::string triggerlogfilename = "/Users/STV/Astro/Programming/pulsar/trigger.log";
	int channels=CHANNELS;
	int samples=SAMPLES;
	int startsubbandnumber=321;
	bool pad=0, verbose=false;
	std::string extension=".stokes";
	int timeintegration=1;
	int firstcoordinate=4;
	int secondcoordinate=0;
	int thirdcoordinate=0;
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help" || argc < 2){
			cout << "Options:\n"
			"-c <config file>\n"
			"-i <inputfile>\n"
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
			"-v verbose, display histograms\n"
			"-ext <file extenstion after .MS\n"
			"- pad use padding for newer data files\n"
			"-startSB real subband to set the startfrequency right\n"
			"-C1 first coordinate\n"
			"-C2 second coordinate\n"
			"-C3 third coordinate\n"
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
			inputdir = string(argv[argcounter]);
			cout << "input directory: " << inputdir << endl;
		} else if(topic == "-FSB"){
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
			triggerlevel = atoi(argv[argcounter]);
			cout << "triggerlevel " << triggerlevel << endl;
		} else if(topic == "-log"){
			argcounter++;
			logfilename = string(argv[argcounter]);
			cout << "log file " << logfilename << endl;
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
		} else if(topic == "-Ctime"){
			argcounter++;
			CoinTime = atoi(argv[argcounter]);
			cout << "Coincidence timewindow " << CoinTime << endl;
		} else if(topic == "-stb"){
			argcounter++;
			startpos = atoi(argv[argcounter]);
			cout << "Coincidence subbands required " << CoinNr << endl;	
		} else if(topic == "-ch"){
			argcounter++;
			channels = atoi(argv[argcounter]);
			cout << "nr of channels " << channels << endl;
		} else if(topic == "-sa"){
			argcounter++;
			samples = atoi(argv[argcounter]);
			cout << "nr of samples " << samples << endl;
		} else if(topic == "-verbose"){
			verbose = true;
			cout << "using verbose output" << endl;
		} else if(topic == "-pad"){
			pad = true;
			cout << "using padding output" << endl;
		} else if(topic == "-ext"){
			argcounter++;
			extension = argv[argcounter];
			cout << "file extensions: .MS" << extension << endl;
		} else if(topic == "-startSB"){
			argcounter++;
			startsubbandnumber = atoi(argv[argcounter]);
			cout << "startsubband number: " << startsubbandnumber << endl;
		} else if(topic == "-tInt"){
			argcounter++;
			timeintegration = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
		} else if(topic == "-C1"){
			argcounter++;
			firstcoordinate = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
		} else if(topic == "-C2"){
			argcounter++;
			secondcoordinate = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
		} else if(topic == "-C3"){
			argcounter++;
			thirdcoordinate = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
		} else {
			cout << "specify at least an config file with -c <config file>";
		}
	}
	
	
	
	
	/*	if (argc < 4) {
	 std::cout << "Usage: test <inputdir> <first subband> <last subband> < [<int length of integration>] [triggerlevel] [logfilename] [triggerlogfilename] \n Now using input /Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS \n and output /Users/STV/Documents/GiantPulse/histogram.ps"<< endl;
	 std::cout << "Example: ./test ~/Astro/data/pulsars/obs/ 0 5 20 50 ; open /Users/STV/Documents/GiantPulse/excesslog_pdt.txt";
	 } else {
	 inputdir = argv[1];
	 firstSB = atoi(argv[2]);
	 lastSB = atoi(argv[3]);
	 if (argc > 4) {
	 integrationlength  = atoi(argv[4]);
	 };
	 if (argc > 5) {
	 triggerlevel = atoi(argv[5]);
	 };
	 if (argc > 6) {
	 logfilename = argv[6];
	 }
	 if (argc > 7) {
	 triggerlogfilename = argv[7];
	 }
	 */	
	cout << logfilename << endl << triggerlogfilename << endl;
	nofFailedTests += testfun (inputdir, firstSB, lastSB, integrationlength, triggerlevel, logfilename, triggerlogfilename, nrblocks, CoinNr, CoinTime, startpos, channels, samples, pad, extension, verbose, startsubbandnumber, timeintegration, firstcoordinate, secondcoordinate, thirdcoordinate); //change default parameters
	
	return nofFailedTests;
}

