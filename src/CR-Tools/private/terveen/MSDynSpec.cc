/*-------------------------------------------------------------------------*
 | $Id:: tDynSpecInc.cc                                                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (s.terveen@astro.ru.nl)                                              *
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

#define HAVE_CFITSIO
//#include <crtools.h>
#include <fitsio.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>



using namespace std;
/*
 \file ttbbctlIn.cc
 
 \ingroup CR_Applications
 
 \brief A program to analyse the Giant Pulse dataset L2009_13298
 
 \author Sander ter Veen
 
 \date 2009/03/30
 */

#define CHANNELS        64 
#define SAMPLES         3072 
#define BLOCKS          140205


// 256; 768 for the old files
/*
 \brief Structure to read the incoherent beam files 
 */
struct stokesdata {
	/* big endian (ppc) */
	unsigned int  sequence_number;
	char pad[508];	
	/* little endian (intel) */
	float data[CHANNELS][SAMPLES|2];
};

//_______________________________________________________________________________
//                                                                      show_data


//_______________________________________________________________________________
//                                                                    swap_endian

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

//_______________________________________________________________________________
//                                                                         main()

int testfun (string inputdir, string outfile, int maxblock, int startblock=0, int integrationlength=1, int firstSB=0, int lastSB=10, int channels_p=CHANNELS, int samples_p=SAMPLES, bool DoPadding=1, string extension="")
{
	
	
	float freqAxisOffset=200*2e8/1024+1e8;
	//int integrationlength=1;
	cerr << "Starting tesfun\n";
	int nofFailedTests =0;
	int ninputfiles=lastSB-firstSB+1;
	//int ninputfiles=11;
	int nrCHANNELS = ninputfiles*channels_p;
	FILE ** pFile;
	pFile = new FILE*[ninputfiles];	
	std::string* infile;
	infile=new std::string[ninputfiles];
	//	infile[0]=infile2+"SB0.MS";
	//	infile[1]=infile2+"SB1.MS";
	//	infile[2]=infile2+"SB2.MS";
	//	infile[3]=infile2+"SB3.MS";
	//	infile[4]=infile2+"SB4.MS";
	//	infile[5]=infile2+"SB5.MS";
	//	infile[6]=infile2+"SB6.MS";
	//	infile[7]=infile2+"SB7.MS";
	//	infile[8]=infile2+"SB8.MS";
	//	infile[9]=infile2+"SB9.MS";
	//	infile[10]=infile2+"SB10.MS";
	
	
	
	std::string * shortinfile;
	shortinfile = new std::string[ninputfiles];
	
	
	for(int i=firstSB; i<=lastSB; i++){
		stringstream ss;
	    string number;
		ss << i;
		ss >> number;
		infile[i-firstSB]=inputdir + "SB" + number + ".MS" + extension;
		cout << "using file " << infile[i-firstSB] << endl;
		shortinfile[i-firstSB] = "SB" + number;
    }
	
	
	for(int i=0;i<ninputfiles;i++){
		pFile[i] = fopen(infile[i].c_str(),"rb");
		if (pFile==NULL) {cout << "File error" << endl;} 
		cout << "Reading file " << i << " " << infile[i] << endl;
		cerr << "Reading file " << i << " " << infile[i] << endl;
		//		shortinfile[i] = std::string(infile[i],infile[i].length()-6,3);
		//		if(i>=10){shortinfile[i] = std::string(infile[i],infile[i].length()-7,4);}
	}
	
	
	
	//pFile = fopen("/Users/STV/Astro/data/L2009_11117/SB26.MS","rb");
	//pFile = fopen("/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS","rb");
	
	float av=ninputfiles*7.94e+07, maxi=ninputfiles*2.5e+08;
	int nrbins=60;
	float bin_width=0.1; //0.1
	cout << "average= " << av << " maximum= " << maxi << endl;
    float subsum[integrationlength];
	for(int it=0;it < integrationlength; it++){subsum[it]=0;} 
	int totaltime;
	
	av = av*integrationlength;
	//nrbins = (int) integrationlength*maxi/av/bin_width + 1; 
	cout << "Average = " << av << endl;
	cout << "Nr bins = " << nrbins << endl;
    if (nrbins<0){cout << "Negative nr of bins. Quitting....\n values: \nintegration length: " << integrationlength << "\nmaxi " << maxi << "\nav : " << av << "\nbin width " << bin_width << endl ; return 1;}
	
	
	//construct the datatype
	unsigned num;
	unsigned validsamples=0;
	float *data=(float*)malloc(ninputfiles*channels_p*(samples_p|2)*sizeof(float));
	if (data==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	
//	for(int jk=0;jk<ninputfiles*CHANNELS*(SAMPLES|2);jk++){
//			data[jk]=0.0;
//	}
		
	//data = new float[ninputfiles*CHANNELS*(SAMPLES|2)];
	
	unsigned int sequence_nr[ninputfiles];
	
	char pad[508];
	int stokesdatasize=channels_p*(samples_p|2)*sizeof(float)+sizeof(unsigned int);
	if(DoPadding){stokesdatasize+=508*sizeof(char);}
	cout << "stokesdatasize " << stokesdatasize << endl;
    //data = new stokesdata[ninputfiles];
	
	
	
	int kmax=maxblock;//3595
	float sum =0;
	float maximum =0;		
	int zerocounter=0;
    int bin;
	int totSamples = kmax*samples_p;
	int totchannel=0;
	cout << "Before creating fitsdata\n";
	cout << "totSamples: " << totSamples << "\n nrCHANNELS: " << nrCHANNELS << endl;
	/*  allocate storage for an array of pointers */
	short *fitsdata = (short*)malloc(totSamples * (nrCHANNELS) * sizeof(short));
	
	if (fitsdata==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	cerr << "copying array ...\n";
	/* for each pointer, allocate storage for an array of ints */
	
	for(int j=0;j<totSamples;j++){
		for(int i=0;i<nrCHANNELS;i++){
			fitsdata[j*(nrCHANNELS)+i]=0;
		}
	}
	
	cout << "After filling fitsdata\n";
	for(int i=0;i<ninputfiles;i++){
		fseek(pFile[i], startblock*(stokesdatasize), SEEK_SET);
	}
	
    for(int k=0;k<kmax;k++){ 
		cout << k << endl;
		for(int i=0;i<ninputfiles;i++){
			//cout << "reading file: " << i << endl;
			num = fread( &(sequence_nr[i]), sizeof(sequence_nr[i]), 1, pFile[i]); //read sequence number
			if( !num) {
				cout << "Error, EOF reached.\n";
			}
			if(DoPadding){
			num = fread( &(pad),sizeof(pad), 1, pFile[i]); //read padding
			if( !num) {
				cout << "Error, EOF reached.\n";
			}
			}
			num = fread( &(data[i*(samples_p|2)*channels_p]), (samples_p|2)*channels_p*sizeof(float), 1, pFile[i]); //read date
			if( !num) {
				cout << "Error, EOF reached.\n";
			}
			
			
			
			if(i==0){// && data[i].sequence_number%10==0){  
				swap_endian( (char*)&(sequence_nr[i]) );
				cerr << "sequence number: " << sequence_nr[i] << endl;
			}
		}
		
		
		
		int counter=0;
		
		//cout << data.sequence_number << endl;
		if(k==0 && sequence_nr[0]==0){continue;} // data filled with zeros for block 0;
		for(int time=0;time<samples_p;time++)
		{
			
			totaltime=k*samples_p+time;
			//DAL::swapbytes((char*)&(data->sequence_number), 4);
			//  cout << &data->samples[0][j] << " ";
			float channelsum = 0;
			
			for(int sb=0;sb<ninputfiles;sb++){
				for(int channel=0;channel<channels_p;channel++){
					totchannel=sb*channels_p+channel;
					float value = data[sb*channels_p*(samples_p|2)+channel*(samples_p|2)+time];//data[sb].samples_p[channel][time];
					//float value = value2;
					if(DoPadding){value = FloatSwap(value);}
					if(counter<10) { cout << value << endl; counter++;}
					// if isnan value --> value is zero, add counters for how many good things there are
					if( !isnan( value ) ) {
						//value = 0;
						validsamples++;
					}
					else { value = 0; }
					if(sb==0 && channel==0 && time<10) {cout << "value is " << value << endl;}
					bin = value/av/bin_width*nrCHANNELS;	//CHANNELS is to normalize with the total average
					fitsdata[totaltime + totSamples*totchannel] = (short) bin;
					if(value == 0) { zerocounter++; cout << "zero found on time: "<< k*samples_p+time << " in channel " << totchannel << endl;}
					channelsum += value;
					
				} // for channels
				
			} //for subbands
			sum += channelsum;
			
			
			//if(channelsum>maximum){maximum=channelsum; cout << "new maximum: " <<maximum<< endl;}
			//if(channelsum<minimum){minimum=channelsum; cout << "new minimum: " << minimum << endl;}
		} // for samples
		//  cout << endl;
		/* float average= sum / validsamples;
		 float average0;
		 if(k==1){average0 = average;}
		 if(average > (average0 + 50)) cout << "------------------------------------------------------------------------\n";
		 cout << "k= " << k << " " << min(matrix) << " min | max " << max(matrix) << " average " << average << endl;
		 */
	}
    float average= sum / validsamples;
	cout << "\n\n Average bin data= " << average << endl;
	cout << "Valid sample number = " << validsamples << endl;
	cout << "Number of zeros = " << zerocounter << endl;
	cout << "Average used: " << av << endl;
	cout << "Integration length: " << integrationlength << endl;
	cout << "Average sum data= " << nrCHANNELS*average << endl;
	cout << "Maximum = " << maximum << endl;
	//	cout << "Forward diagonal maximum = " << fwmaximum << endl;
	//	cout << "Backward diagonal maximum = " << bwmaximum << endl;
	/* Plot the data to a PS file. */
	
	
	
	// routines to write to a FITS file.
#ifdef HAVE_CFITSIO			/*
/* Convert to floats and take the logarithm. If that value is zero, replace
* it with the minimumvalue.
*/
	//float minimum = (float) min(dynamicSpectrum_p);		casa::Matrix<float> dynamicSpectrum_float(dynamicSpectrum_p.shape());
	
	cout << "nrChannel = " << nrCHANNELS << endl;
	cout << "totSamples = " << totSamples << endl;
	
	
	int status            = 0;
	long naxis            = 2;
	uint nelements        = (nrCHANNELS)*(totSamples);
	long naxes[2]         = { totSamples, nrCHANNELS};
	long firstelement = 1;
	char *ctype[] = {"TIME","freq channel"};
	// char *cunit[] = {"sec","Hz"};
	// get the incrementvalues and units for the axes 
//	float fVal = 65;
//	char cVal[32];
//	sprintf(cVal,"%f",fVal);
//	
	
	
	
	float timeAxisIncrement=5e-9*1024*channels_p;
	float freqAxisIncrement=1/timeAxisIncrement;
	
	char TAS[32];
	char FAS[32];
	char FST[32];
	sprintf(TAS,"%f",timeAxisIncrement);
	sprintf(FAS,"%f",freqAxisIncrement/1e6);
	sprintf(FST,"%f",freqAxisOffset/1e6);

	 

	 
	 // copy the values to char*, as it's the only thing CFITSIO can handle;
	
	/*
	cout << "timeunit: " << tunit << "   frequnit: " << funit << std::endl;
	 char *cunit0, *cunit1, *cd11, *cd12, *cval1;
	 cunit0 = new char[tunit.length()+1];
	 tunit.copy(cunit0, string::npos);
	 cunit0[tunit.length()] = '\0';
	 
	 cunit1 = new char[funit.length()+1];
	 funit.copy(cunit1, string::npos);
	 cunit1[funit.length()] = '\0';
	 
	 cd11 = new char[cd1.length()+1];
	 cd1.copy(cd11, string::npos);
	 cd11[cd1.length()]='\0';
	 
	 cd12 = new char[cd2.length()+1];
	 cd2.copy(cd12, string::npos);
	 cd12[cd2.length()]='\0';
	 
	 cval1 = new char[fnul.length()+1];
	 fnul.copy(cval1, string::npos);
	 cval1[fnul.length()] = '\0';
	 */
	std::stringstream blockinfo;
	blockinfo << startblock << "_" << startblock + maxblock;
	
	
	std::string fitsoutfile;
	fitsfile *fptr;
	std::cout << "nelements: " << nelements << std::endl;  
	//* Adjust the filename such that existing data are overwritten
	fitsoutfile = "!" + outfile + shortinfile[0] + "_" + shortinfile[ninputfiles-1] + "_" + blockinfo.str() + ".fits";
	std::cout << "write to: " << fitsoutfile.c_str() << std::endl;
	//* Create FITS file on disk 
	fits_create_file (&fptr, fitsoutfile.c_str(), &status);
	cout << "fits file created" << endl;
	// Create primary array to take up the image data 
	fits_create_img (fptr, SHORT_IMG, naxis, naxes, &status);
	cout << "fits image created" << endl;
	
	//NOTE: getStorage() is used to get a pointer to the actual data as the matrix also has MetaData. 
	
	
	fits_write_img (fptr, TSHORT, firstelement, nelements, fitsdata, &status);
	//clean up the storage
	
	//set keywords for the axis
	//std::cout << " -- Set the keywords for the coordinate type" << std::endl;
	ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
	ffukys (fptr, "CTYPE2", ctype[1], "Frequency axis", &status);
	
	 //std::cout << " -- Set the keywords for the units on the coordinate axes"<< std::endl;
	 ffukys (fptr, "CUNIT1", "s", "Time axis units", &status);
	 ffukys (fptr, "CUNIT2", "MHz", "Frequency axis units", &status);
	 
	 ffukys (fptr, "CD1_1", TAS, "Time axis scaling", &status);
	 ffukys (fptr, "CD2_2", FAS, "Frequency axis scaling", &status);
	 
	 ffukys (fptr, "CRVAL2", FST, "Frequency start", &status);
	 
	//ADD start value CRVAL1 by start pixel CRPIX1 to complete the picture. This is needed for the second nyquist zone
	//CRVAL = (PIXEL - CRPIX1)*CD1_1 + CRVAL1
	//
	
	
	
	/*		
	 
	 //Add frequency spectrum
	 Vector<double> avSpectrum = averageSpectrum(); //log(averageSpectrum());
	 
	 casa::IPosition shape_F = avSpectrum.shape();
	 long naxis_F            = shape_F.nelements();
	 uint nelements_F        = avSpectrum.nelements();
	 long naxes_F[2]         = { shape_F(0), shape_F(1)};
	 long firstelement_F = 1; 
	 
	 casa::Bool storage_F;
	 casa::Double* datapointer_F = avSpectrum.getStorage(storage_F);
	 
	 fits_create_img (fptr, DOUBLE_IMG, naxis_F, naxes_F, &status);
	 fits_write_img (fptr, TDOUBLE, firstelement_F, nelements_F, datapointer_F, &status);
	 avSpectrum.putStorage(datapointer_F, storage_F);	  
	 
	 
	 
	 
	 ffukys (fptr, "CTYPE1", ctype[1], "Frequency axis", &status);
	 ffukys (fptr, "CUNIT1", cunit1, "Frequency axis units", &status);
	 ffukys (fptr, "CD1_1", cd12, "Frequency axis scaling", &status);
	 ffukys (fptr, "BUNIT", "log(averageSpectrum)", "Power axis", &status);
	 ffukys (fptr, "CRVAL1", cval1, "Frequency start", &status);
	 ffukys (fptr, "EXTNAME", "Average Spectrum (no logscale atm)", "Frequency spectrum averaged over time in logarithmic scale", &status);
	 
	 //Add powerspectrum
	 Vector<double> totPower = totalPower();//log(totalPower());
	 
	 casa::IPosition shape_P = totPower.shape();
	 long naxis_P            = shape_P.nelements();
	 uint nelements_P        = totPower.nelements();
	 long naxes_P[2]         = { shape_P(0), shape_P(1)};
	 long firstelement_P = 1; 
	 
	 casa::Bool storage_P;
	 casa::Double* datapointer_P = totPower.getStorage(storage_P);
	 
	 
	 fits_create_img (fptr, DOUBLE_IMG, naxis_P, naxes_P, &status);
	 fits_write_img (fptr, TDOUBLE, firstelement_P, nelements_P, datapointer_P, &status);
	 totPower.putStorage(datapointer_P, storage_P);	 
	 ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
	 ffukys (fptr, "CUNIT1", cunit0, "Time axis units", &status);
	 ffukys (fptr, "CD1_1", cd11, "Time axis scaling", &status);
	 ffukys (fptr, "BUNIT", "log(totalPower)", "Power axis", &status);
	 ffukys (fptr, "EXTNAME", "Total Power (no logscale atm)", "Total power for each time in logarithmic scale", &status);
	 
	 
	 */		
	fits_close_file (fptr, &status);  
	if (fitsdata!=NULL) { 
		free(fitsdata);
		fitsdata = NULL;
	}
	
#endif
	
	
	
	return nofFailedTests;
}

int main (int argc,
		  char *argv[])
{
	cout<<"This file outputs histograms of pulsar data to /Users/STV/Documents/GiantPulse/."<<endl;
	int maxblock = 3; //maximum of blocks processed
    int startblock = 10;
	int integrationlength=1;
 	uint nofFailedTests=0;
	float average;
	int firstSB=0, lastSB=10;
	int channels=CHANNELS;
	int samples=SAMPLES;
	bool pad=1;
	std::string extension="";
	
	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	if (argc < 2) {
		std::cout << "Usage: MSDynSpec [<inputfile.MS>] [nrblocks to process] [startblock] [<length of integration>] [firstSB] [lastSB] [#channels] [#samples] [use padding 0/1] [file extension] \n Now using input /Users/STV/Astro/data/pulsars/L2009_13298/*.MS \n and output /Users/STV/Documents/GiantPulse/"<< endl;
	} else {
		infile = argv[1];
		if (argc > 2) {
			maxblock  = atoi(argv[2]);
		};
		if (argc > 3) {
			startblock = atoi(argv[3]);
		};
		if (argc > 4) {
			integrationlength = atoi(argv[4]);
		};
		if (argc > 5) {
			firstSB = atoi(argv[5]);
		}
		if (argc > 6) {
			lastSB = atoi(argv[6]);
		}
		if (argc > 7) {
			channels = atoi(argv[7]);
		}
		if (argc > 8) {
			samples = atoi(argv[8]);
		}
		if (argc > 9) {
			pad= (bool) atoi(argv[9]);
		}
		if (argc > 10) {
			extension= argv[10];
		}
		nofFailedTests += testfun (infile, outfile, maxblock, startblock, integrationlength, firstSB, lastSB, channels, samples, pad, extension);
		
	}
	
    
	
	
	
	return nofFailedTests;
}
