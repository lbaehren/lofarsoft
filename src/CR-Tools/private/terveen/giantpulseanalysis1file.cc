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

#include <crtools.h>
#include <Data/LOFAR_TBB.h>
#include <Display/SimplePlot.h>
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
#define SAMPLES         3056
#define BLOCKS          140205


// 256; 768 for the old files
/*
  \brief Structure to read the incoherent beam files 
*/
struct stokesdata {
  /* big endian (ppc) */
  unsigned int  sequence_number;

  /* little endian (intel) */
  float         samples[CHANNELS][SAMPLES|2];
};

//_______________________________________________________________________________
//                                                                      show_data

/*!
  \brief Show the contents of the data array
  
  \param data -- Array with the ADC or voltage data per antenna
*/
void show_data (Matrix<Double> const &data)
{
  std::cout << "  Data shape : " << data.shape() << std::endl;
  std::cout << "  Data ..... : "
	    << data(1,1) << " "
	    << data(2,1) << " "
	    << data(3,1)
	    << std::endl;
}

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

//_______________________________________________________________________________
//                                                                         main()

int testfun (string infile, string outfile, int maxblock, int startblock=0, int integrationlength=1)
{
	//int integrationlength=1;
  cerr << "Starting tesfun\n";
  int nofFailedTests =0;
  FILE * pFile;
  //pFile = fopen("/Users/STV/Astro/data/L2009_11117/SB26.MS","rb");
  //pFile = fopen("/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS","rb");
  pFile = fopen(infile.c_str(),"rb");
  if (pFile==NULL) {cout << "File error" << endl;} 
  cout << "Reading file " << infile << endl;
  cerr << "Reading file " << infile << endl;
  std::string shortinfile(infile,42,3);
	float av=0, maxi=0;
	int nrbins=60;
	float bin_width=0.1; //0.1
	
	float subsumfw[integrationlength];
	float subsumbw[integrationlength];
    float subsum[integrationlength];
	for(int it=0;it < integrationlength; it++){subsumfw[it]=0; subsumbw[it]=0; subsum[it]=0;}
	float sumsubsumfw;
	float sumsubsumbw;
	float sumsubsum; 
	int totaltime;
	

   if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS"){
   //Average bin data= 1.23883e+06
	   av =7.92848e+07;
   //Maximum = 1.52434e+08
   //Forward diagonal maximum = 1.36479e+08
	   maxi = 1.96146e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB1.MS"){
   //Average bin data= 1.24511e+06
	av =7.96872e+07;
   //Maximum = 1.73193e+08
   //Forward diagonal maximum = 1.69446e+08
	maxi = 1.89357e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB2.MS"){
   
   //Average bin data= 1.23444e+06
	   av =7.90043e+07;
   //Maximum = 1.51013e+08
   //Forward diagonal maximum = 1.34941e+08
   maxi = 2.04084e+08;
   }

   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB3.MS"){
   
   
   //Average bin data= 1.24262e+06
	   av =7.95275e+07;
   //Maximum = 1.53423e+08
   //Forward diagonal maximum = 1.41693e+08
   maxi = 1.82849e+08;
   }

   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB4.MS"){
   
   //Average bin data= 1.22921e+06
	   av =7.86694e+07;
   //Maximum = 1.5079e+08
   //Forward diagonal maximum = 1.39223e+08
   maxi = 1.92037e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB5.MS"){
   
   
   //Average bin data= 1.22989e+06
	   av =7.87128e+07;
   //Maximum = 1.56443e+08
   //Forward diagonal maximum = 1.59848e+08
   maxi = 1.92135e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB6.MS"){
   
   //Average bin data= 1.20828e+06
	   av =7.73298e+07;
   //Maximum = 1.52073e+08
   //Forward diagonal maximum = 1.52871e+08
	   maxi = 2.03838e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB7.MS"){
   //Average bin data= 1.2166e+06
	   av =7.78624e+07;
   //Maximum = 3.04752e+08
   //Forward diagonal maximum = 4.21222e+08
	   maxi = 4.20479e+08;		
   }

   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB8.MS"){
   //Average bin data= 1.21137e+06
   av =7.75276e+07;
   //Maximum = 1.43714e+08
   //Forward diagonal maximum = 1.48645e+08
   maxi = 1.76646e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB9.MS"){
   //Average bin data= 1.22465e+06
	   av =7.83774e+07;
   //Maximum = 3.20819e+08
   //Forward diagonal maximum = 3.58423e+08
	   maxi = 3.47141e+08;
   }

   
   
   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SB10.MS"){
   //Average bin data= 1.19509e+06
   av =7.64856e+07;
   //Maximum = 1.49343e+08
   //Forward diagonal maximum = 1.55559e+08
   maxi = 1.85754e+08;

   }
	av = av*integrationlength;
	nrbins = (int) integrationlength*maxi/av/bin_width + 1; 
	cout << "Nr bins = " << nrbins << endl;
    if (nrbins<0){cout << "Negative nr of bins. Quitting....\n values: \nintegration length: " << integrationlength << "\nmaxi " << maxi << "\nav : " << av << "\nbin width " << bin_width << endl ; return 1;}
	
		
  //construct the datatype
  unsigned num;
  unsigned time,channel;
  unsigned validsamples=0;
  stokesdata data;
	int kmax=maxblock;//3595
	casa::Matrix<double> matrix(CHANNELS,2);//kmax*SAMPLES);
	float sum =0;
	float maximum =0, minimum=1e40;
	float fwmaximum=0, fwminimum=1e40; 
	float bwmaximum=0, bwminimum=1e40;
	float fw[CHANNELS];
	float bw[CHANNELS];
	float pulseprofile[101];
	float pulseprofilebw[101];
	float pulseprofilefw[101];
	//int fwcounter[CHANNELS];
	//int bwcounter[CHANNELS];
	int zerocounter=0;
	uint histogram[nrbins];
	uint bwhistogram[nrbins];
	uint fwhistogram[nrbins];
	for(int i=0;i<CHANNELS;i++){fw[i]=0; bw[i]=0;}
	for(int i=0;i<nrbins;i++){histogram[i]=0; bwhistogram[i]=0; fwhistogram[i]=0;}
	for(int i=0;i<101;i++){pulseprofile[i]=0; pulseprofilebw[i]=0; pulseprofilefw[i]=0;}
	int bin;
	uint rest=0;
	int totSamples = kmax*SAMPLES;
    int histocounter;
	cout << "Before creating fitsdata\n";
	
	/*  allocate storage for an array of pointers */
	short *fitsdata = (short*)malloc(totSamples * (CHANNELS+2) * sizeof(short));
	
	if (fitsdata==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	cerr << "copying array ...\n";
	/* for each pointer, allocate storage for an array of ints */
	
	for(int j=0;j<totSamples;j++){
		for(int i=0;i<CHANNELS+2;i++){
			fitsdata[j*(CHANNELS+2)+i]=0;
		}
	}
	
	//move to the right postition when reading in the file.
//	for(int k=0;k<startblock;k++){ 
//	num = fread( &data, sizeof data, 1, pFile);
//    if( !num) {
//		cout << "Error, EOF reached.\n";
//    }
	
//  }
	fseek(pFile, startblock*(sizeof data), SEEK_SET);
	cout << "After filling fitsdata\n";
    for(int k=0;k<kmax;k++){ 
    num = fread( &data, sizeof data, 1, pFile);
    if( !num) {
      cout << "Error, EOF reached.\n";
    }
		
		
	  
    swap_endian( (char*)&(data.sequence_number) );
	  if(data.sequence_number%500==0){  
	     cerr << "sequence number: " << data.sequence_number << endl;
	  }
    //cout << data.sequence_number << endl;
	if(k==0 && startblock==0){continue;} // data filled with zeros for block 0;
    for(int time=0;time<SAMPLES;time++)
	{
	    totaltime=k*SAMPLES+time;
	//DAL::swapbytes((char*)&(data->sequence_number), 4);
	//  cout << &data->samples[0][j] << " ";
		float channelsum = 0;
		float validcounter = 0;
	    for(int channel=0;channel<CHANNELS;channel++){
	     float value =  data.samples[channel][time] ; //const float before
	     //matrix(channel,time) = value;
		
	     //cout <<  endl;
		// if isnan value --> value is zero, add counters for how many good things there are
	     if( !isnan( value ) ) {
			 //value = 0;
              
		 	 
			 validsamples++;
		 }
	     else { value = 0; }
			bin = value/av/bin_width*CHANNELS;	//CHANNELS is to normalize with the total average
			fitsdata[totaltime + totSamples*channel] = (short) bin;
		 if(value == 0) { zerocounter++; cout << "zero found on time: "<< k*SAMPLES+time << " in channel " << channel << endl;}
		 channelsum += value;
		 
		 rest=(k*SAMPLES+time)%CHANNELS;
		 if(channel==0){
			 if(fw[rest]>fwmaximum){
			    fwmaximum=fw[rest]; 
				 //cout << "new forward maximum: " <<fwmaximum << endl;
			 }
			 bin = (int) fw[rest]/av/bin_width;
			 //pulseprofilefw[(totaltime+totaltime/3333)%101]+=fw[rest];
			 /*
			 subsumfw[(k*SAMPLES+time)%integrationlength] = fw[rest];
			 
			 sumsubsumfw=0;
			 for(int it=0;it<integrationlength;it++){sumsubsumfw+=subsumfw[it];}
			 bin = (int) sumsubsumfw/av/bin_width;
			 
			 if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsumfw << "which is " << bin << "sigma" << endl;}
			 else 
			  */
			 //if(k>1 || time >= CHANNELS+integrationlength-1){fitsdata[CHANNELS+1][totaltime]=bin;}
			
			 
			 fw[rest]=value;
			 //fwcounter[rest]=1;
		  }
		 else { //if(value!=0) {
			 fw[(rest-channel)%CHANNELS]+=value;
			 //fwcounter[(rest-channel)%64]++;
		  }
		  if(channel==63){
			 if(bw[rest]>bwmaximum){
			      bwmaximum=bw[rest]; 
				 //cout << "new backward maximum: " <<bwmaximum << endl;
			 }
			  bin = (int) bw[rest]/av/bin_width;
			  //pulseprofilebw[(totaltime+totaltime/3333)%101]+=bw[rest];
			  
			  subsumbw[(k*SAMPLES+time)%integrationlength] = bw[rest];// /bwcounter[rest]*CHANNELS;
			  
              sumsubsumbw=0;
		      for(int it=0;it<integrationlength;it++){sumsubsumbw+=subsumbw[it];}
			  bin = (int) sumsubsumbw/av/bin_width;
              
			  if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsumbw << "which is " << bin << "sigma" << endl;}
			  else if(k>1 || time >= CHANNELS+integrationlength-1){  fitsdata[totaltime + totSamples*(CHANNELS+1)] = (short) bin;  }
			  
			  bw[rest]=value;
			 
		  }
		  else { //if(value!=0) {
			  bw[(rest-63+channel)%CHANNELS]+=value;
			  //bwcounter[(rest-63+channel)%64]++;
		  }
		 
		   
			 
		 
		 
	   } // for channels
	   
	   bin = (int) channelsum/av/bin_width;
		//pulseprofile[(totaltime+totaltime/3333)%101]+=channelsum;
		/*
	    subsum[(k*SAMPLES+time)%integrationlength]=channelsum;//  /validcounter*CHANNELS;

		sumsubsum=0;
		for(int it=0;it<integrationlength;it++){sumsubsum+=subsum[it];}
		bin = (int) sumsubsum/av/bin_width;
		if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsum << "which is " << bin << "sigma";}
		else 
		 */
		 //if(k>1 || time >= CHANNELS+integrationlength-1){fitsdata[CHANNELS+5][totaltime]=bin;}
	    
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
	cout << "Average sum data= " << 64*average << endl;
	cout << "Maximum = " << maximum << endl;
	cout << "Forward diagonal maximum = " << fwmaximum << endl;
	cout << "Backward diagonal maximum = " << bwmaximum << endl;
  /* Plot the data to a PS file. */
  
#ifdef HAVE_PLPLOT_REMOVE_THIS_TO_USE_IT
	/*
	Vector<Double> empty;
  	Vector<Double> xx(nrbins);
	Vector<Double> yy(nrbins);
	Vector<Double> yy2(nrbins);
	Vector<Double> yy3(nrbins);
    SimplePlot plot = SimplePlot();	
	for (int i=0; i < nrbins; i++){
		xx[i] = (i+0.5)*bin_width;
		yy[i] = histogram[i];
		yy2[i]=fwhistogram[i];
		yy3[i]=bwhistogram[i];
	}
	
	cout << "\n\n\n\n Histograms normal, forward, backword " << endl << yy << endl << yy2 << endl << yy3 << endl;
	yy = log(yy+0.01)/log(10);
	yy2 = log(yy2+0.01)/log(10);
	yy3 = log(yy3+0.01)/log(10);
    std::string shortinfile(infile,42,3);
	cout << " short infile " << shortinfile << endl;
	
	std::stringstream il;
	il << integrationlength;
	std::stringstream avg;
	avg << av;
	std::string outfile2 = outfile + "histogram_"+ shortinfile + "_intl_" + il.str() + ".ps";
	
	  std::string title = "Histogram of " + shortinfile + " with a sigma level of " + avg.str();
	cout << "writing file " << outfile2 << "....\n with title " << title<<endl; 
	plot.CR::SimplePlot::quickPlot( outfile2, xx, yy, empty, empty, "nrsigma", "log counts",	title, 1, false, 1, false, true);
	plot.CR::SimplePlot::PlotLine( xx, yy2,6); //brown
	plot.CR::SimplePlot::PlotLine( xx, yy3,5); //purple
	*/
	
	
	
	Vector<Double> empty;
  	Vector<Double> px(101);
	Vector<Double> py(101);
	Vector<Double> py2(101);
	Vector<Double> py3(101);
    SimplePlot plot = SimplePlot();	
	for (int i=0; i < 101; i++){
		px[i] = i*5*1024*64/1000000;
		py[i] = pulseprofile[i];
		py2[i]=pulseprofilebw[i];
		py3[i]=pulseprofilefw[i];
	}
	
	cout << "\n\n\n\n Pulseprofile normal, forward, backword " << endl << py << endl << py2 << endl << py3 << endl;
/*	yy = log(yy+0.01)/log(10);
	yy2 = log(yy2+0.01)/log(10);
	yy3 = log(yy3+0.01)/log(10);
*/  
	cout << " short infile " << shortinfile << endl;
	
	std::stringstream il;
	il << integrationlength;
	std::stringstream avg;
	avg << av;
	std::string outfile2 = outfile + "pulseprofile_"+ shortinfile + ".ps";
	
	std::string title = "Histogram of " + shortinfile + " with a sigma level of " + avg.str();
	cout << "writing file " << outfile2 << "....\n with title " << title<<endl; 
	plot.CR::SimplePlot::quickPlot( outfile2, px, py, empty, empty, "nrsigma", "log counts",	title, 1, false, 1, false, true);
	plot.CR::SimplePlot::PlotLine( px, py2,6); //brown
	plot.CR::SimplePlot::PlotLine( px, py3,5); //purple
	
	
	/*
	 
	 
	 
	title = "Dedispersed " + title;
 
	outfile2 = outfile + "histogram_"+ shortinfile + "_bw_intl_"+ il.str()  + ".ps";
	
	cout << "writing file " << outfile2 << "....\n with title " << title <<endl;
	plot.CR::SimplePlot::quickPlot( outfile2, xx, yy3, empty, empty, "nrsigma", "log counts",	title, 1);
	outfile2 = outfile + "histogram_"+ shortinfile + "_fw_intl_"+ il.str()  + ".ps";
	title = "Anti " + title;
	cout << "writing file " << outfile2 << "....\n with title " << title <<endl;
	plot.CR::SimplePlot::quickPlot( outfile2, xx, yy2, empty, empty, "nrsigma", "log counts",	title, 1);
   */
	
	
	
	
#endif
	/*newObject.quickPlot("tSimplePlot-line.ps", xval, yval, empty, empty,
	 "X-axis", "Y-Axis", "plotting-test with lines",
	 4, True, 1, False, False, True);
	 */
	
	
  // routines to write to a FITS file.
#ifdef HAVE_CFITSIO			/*
		 * Convert to floats and take the logarithm. If that value is zero, replace
		 * it with the minimumvalue.
		 */
		//float minimum = (float) min(dynamicSpectrum_p);		casa::Matrix<float> dynamicSpectrum_float(dynamicSpectrum_p.shape());
	

		
		
		int status            = 0;
		long naxis            = 2;
		uint nelements        = (CHANNELS+2)*(totSamples);
		long naxes[2]         = { totSamples, CHANNELS+2};
		long firstelement = 1;
		char *ctype[] = {"TIME","freq channel"};
		// char *cunit[] = {"sec","Hz"};
		// get the incrementvalues and units for the axes 
	    /*
		std::string cd1(casa::String::toString(timeAxis_p.increment()(0)));
		std::string cd2(casa::String::toString(freqAxis_p.increment()(0)));
		
		
		std::string tunit(timeAxis_p.worldAxisUnits()(0));
		std::string funit(freqAxis_p.worldAxisUnits()(0));
		std::string fnul(casa::String::toString(freqAxis_p.referenceValue()(0)));
		
		// copy the values to char*, as it's the only thing CFITSIO can handle;
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
		fitsoutfile = "!" + outfile + shortinfile + "_" + blockinfo.str() + ".fits";
		std::cout << "write to: " << fitsoutfile.c_str() << std::endl;
		//* Create FITS file on disk 
		fits_create_file (&fptr, fitsoutfile.c_str(), &status);
		
		// Create primary array to take up the image data 
		fits_create_img (fptr, SHORT_IMG, naxis, naxes, &status);
		
		
		//NOTE: getStorage() is used to get a pointer to the actual data as the matrix also has MetaData. 


		fits_write_img (fptr, TSHORT, firstelement, nelements, fitsdata, &status);
		//clean up the storage
				
		//set keywords for the axis
		//std::cout << " -- Set the keywords for the coordinate type" << std::endl;
		ffukys (fptr, "CTYPE1", ctype[0], "Time axis", &status);
		ffukys (fptr, "CTYPE2", ctype[1], "Frequency axis", &status);
		/*
		//std::cout << " -- Set the keywords for the units on the coordinate axes"<< std::endl;
		ffukys (fptr, "CUNIT1", cunit0, "Time axis units", &status);
		ffukys (fptr, "CUNIT2", cunit1, "Frequency axis units", &status);
		
		ffukys (fptr, "CD1_1", cd11, "Time axis scaling", &status);
		ffukys (fptr, "CD2_2", cd12, "Frequency axis scaling", &status);
		
		ffukys (fptr, "CRVAL2", cval1, "Frequency start", &status);
		*/
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
	int maxblock = 3595; //maximum of blocks processed
    int startblock = 0;
	int integrationlength;
 	uint nofFailedTests=0;
	float average;
	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	if (argc < 2) {
		std::cout << "Usage: testlightningskymapping [<inputfile.MS>] [nrblocks to process] [startblock] [<length of integration>] \n Now using input /Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS \n and output /Users/STV/Documents/GiantPulse/histogram.ps"<< endl;
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
	}
	
	nofFailedTests += testfun (infile, outfile, maxblock, startblock, integrationlength);
	
	return nofFailedTests;
}
