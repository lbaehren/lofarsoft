/*-------------------------------------------------------------------------*
 | $Id:: tDynSpecInc.cc                                                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (<mail>)                                              *
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

  \brief A program to play around with functions for me. Currently to deal with subband tied array pipeline data.
 
  \author Sander ter Veen
 
  \date 2009/03/30
*/

#define CHANNELS        64
#define SAMPLES         3056

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

int testfun (string infile, string outfile, int integrationlength, int triggerlevel=20)
{
	
  
  int nofFailedTests (0);
  FILE * pFile;
  //pFile = fopen("/Users/STV/Astro/data/L2009_11117/SB26.MS","rb");
  //pFile = fopen("/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS","rb");
  pFile = fopen(infile.c_str(),"rb");
  if (pFile==NULL) {cout << "File error" << endl;} 
  cout << "Reading file " << infile << endl;
  cerr << "Reading file " << infile << endl;
     
	ofstream logfile;
	std::string shortinfile(infile,42,3);
	std::string logfilename;
	std::stringstream il;
	il << integrationlength;
	logfilename = "/Users/STV/Documents/GiantPulse/excesslog_" + shortinfile + + "_intl_" + il.str() + ".txt";
	logfile.open(logfilename.c_str());
	float av=0, maxi=0;
	int nrbins=60;
	float bin_width=0.1; //*sqrt(integrationlength);
	
	float subsumfw[integrationlength];
	float subsumbw[integrationlength];
    float subsum[integrationlength];
	for(int it=0;it < integrationlength; it++){subsumfw[it]=0; subsumbw[it]=0; subsum[it]=0;}
	float sumsubsumfw;
	float sumsubsumbw;
	float sumsubsum;
	

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

   else if(infile == "/Users/STV/Astro/data/pulsars/L2009_13298/SBX.MS"){
	   //Average bin data= 1.19509e+06
	   av =7.64856e+07;
	   //Maximum = 1.49343e+08
	   //Forward diagonal maximum = 1.55559e+08
	   maxi = 1.85754e+08;
	   
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
	int kmax=3595;
	casa::Matrix<double> matrix(CHANNELS,2);//kmax*SAMPLES);
	float sum =0;
	float maximum =0, minimum=1e40;
	float fwmaximum=0, fwminimum=1e40; 
	float bwmaximum=0, bwminimum=1e40;
	float fw[CHANNELS];
	float bw[CHANNELS];
	//int fwcounter[CHANNELS];
	//int bwcounter[CHANNELS];
	int zerocounter=0;
	uint histogram[nrbins];
	uint bwhistogram[nrbins];
	uint fwhistogram[nrbins];
	for(int i=0;i<CHANNELS;i++){fw[i]=0; bw[i]=0;}
	for(int i=0;i<nrbins;i++){histogram[i]=0; bwhistogram[i]=0; fwhistogram[i]=0;}
	int bin;
	uint rest=0;
	
    int histocounter;
	std::stringstream excessfwT;
	std::stringstream excessfwP;
	std::stringstream excessbwT;
	std::stringstream excessbwP;
	std::stringstream excessbwB; //block
	std::stringstream excessbwS; //sample
	std::stringstream excessT;
	std::stringstream excessP;
	
	excessfwT << shortinfile << "fwT = [";
	excessfwP << shortinfile << "fwP = [";
	excessbwT << shortinfile << "bwT = [";
	excessbwP << shortinfile << "bwP = [";
	excessbwB << shortinfile << "bwBlock = [";
	excessbwS << shortinfile << "bwSample = [";
	excessT << shortinfile << "T = [";
	excessP << shortinfile << "P = [";
	int totaltime;
	int triggertime=0;
	int triggerlength=0;
	int triggerblock=0;
	int triggersample=0;
	float triggeraverage=0;
	float triggerbin=0;
	float binfl;
	
	
	
	 cout << "Ignoring channels 28-30 atm";
	
	
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
	if(k==0){continue;} // data filled with zeros
	  //if( (k>500 && k < 511) || (k>730 && k< 933) || (k>730 && k< 933) ){continue;}
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
		 if(value == 0) { zerocounter++; cout << "zero found on time: "<< k*SAMPLES+time << " in channel " << channel << endl;}
			//if(shortinfile == "SB7" && (channel==28 || channel >=53 && channel <=55)){value=av/CHANNELS; if(k==0){cout << "ignoring RFI channels for SB7\n";}} //ignoring channels 28-30
			//else if(shortinfile == "SB9" && (channel>=4 || channel <=6)){value=av/CHANNELS; if(k==0){cout << "ignoring RFI channels for SB9\n";}} //ignoring channels 28-30
			
			channelsum += value;
		
		 rest=(k*SAMPLES+time)%CHANNELS;
		
			
		
		 if(channel==0){
			 if(fw[rest]>fwmaximum){
			    fwmaximum=fw[rest]; 
				 //cout << "new forward maximum: " <<fwmaximum << endl;
			 }
			 //bin = (int) fw[rest]/av/bin_width;
			 subsumfw[(k*SAMPLES+time)%integrationlength] = fw[rest];
			 sumsubsumfw=0;
			 for(int it=0;it<integrationlength;it++){sumsubsumfw+=subsumfw[it];}
			 bin = (int) sumsubsumfw/av/bin_width;
             if(bin>25){ excessfwT << totaltime << " "; excessfwP << bin << " ";}
			 if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsumfw << "which is " << bin << "sigma" << endl;}
			 else if(k>1 || time >= CHANNELS+integrationlength-1){fwhistogram[bin]++;}
			 fw[rest]=value;
			 //fwcounter[rest]=1;
		  }
		 else { //if(value!=0) {
			 fw[(rest-channel)%64]+=value;
			 //fwcounter[(rest-channel)%64]++;
		  }
		  if(channel==63){
			 if(bw[rest]>bwmaximum){
			      bwmaximum=bw[rest]; 
				 //cout << "new backward maximum: " <<bwmaximum << endl;
			 }
			  //bin = (int) bw[rest]/av/bin_width;
			  subsumbw[(k*SAMPLES+time)%integrationlength] = bw[rest];// /bwcounter[rest]*CHANNELS;
              sumsubsumbw=0;
		      for(int it=0;it<integrationlength;it++){sumsubsumbw+=subsumbw[it];}
			  bin = (int) sumsubsumbw/av/bin_width;
			  if(bin>=triggerlevel){
			     binfl = sumsubsumbw/av/bin_width; 
			     excessbwT << totaltime << " "; excessbwP << binfl << " "; excessbwB << k << " "; excessbwS << time << " ";
				  if(totaltime-triggertime>10){
					//new trigger
					  triggeraverage/=triggerlength;
					  logfile << "bl: " << triggerblock << " " << triggersample << " " << triggertime << " " << " " << triggerlength << " " << triggerbin << " " << triggeraverage << " " << shortinfile << endl;
				    triggertime=totaltime;
					triggeraverage=binfl;
				    triggerlength=1;
				    triggersample=time;
				    triggerblock=k;
					triggerbin=binfl;
				  }
				  else{
					  //old trigger, or trigger accross blocks
					  triggeraverage+=binfl;
					  triggerlength++;
					  triggertime=totaltime;
					  if(binfl>triggerbin){triggerbin=binfl;} //calculate maximum
				  }
			  
			  }
			  
			  
			  if(bin>nrbins){ cout <<" bin number too high. Value = " << sumsubsumbw << "which is " << bin << "sigma" << endl;}
			  else if(k>1 || time >= CHANNELS+integrationlength-1){  bwhistogram[bin]++;  }
			  
			  bw[rest]=value;
			 
		  }
		  else { //if(value!=0) {
			  bw[(rest-63+channel)%64]+=value;
			  //bwcounter[(rest-63+channel)%64]++;
		  }
		 
		   
			 
		 
		 
	   } // for channels
	   
	   //bin = (int) channelsum/av/bin_width;
	    subsum[(k*SAMPLES+time)%integrationlength]=channelsum;//  /validcounter*CHANNELS;
		sumsubsum=0;
		for(int it=0;it<integrationlength;it++){sumsubsum+=subsum[it];}
		bin = (int) sumsubsum/av/bin_width;
		if(bin>25){ excessT << totaltime << " "; excessP << bin << " ";} //shouldn't be anything above 20 actually
		if(bin>nrbins){cout <<" bin number too high. Value = " << sumsubsum << "which is " << bin << "sigma";}
		else if(k>1 || time >= CHANNELS+integrationlength-1){histogram[bin]++  ;}
	   
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
  
	//logfile << "\n\n" << excessT.str() << "];" << endl << excessP.str() << "];"  << endl  << excessfwT.str() << "];"  << endl << excessfwP.str() << "];" << endl  << excessbwT.str() << "];" << endl << excessbwP.str() << "];"  << endl;
	logfile << "\n\n" << excessbwB.str() << "];" << endl << excessbwS.str() << "];"  << endl  << excessbwT.str() << "];" << endl << excessbwP.str() << "];"  << endl;
	logfile.close();
	
	
#ifdef HAVE_PLPLOT
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
    
	cout << " short infile " << shortinfile << endl;
	

	std::stringstream avg;
	avg << av;
	std::string outfile2 = outfile + "histogram_"+ shortinfile + "_intl_" + il.str() + ".ps";
	
	  std::string title = "Histogram of " + shortinfile + " with a sigma level of " + avg.str();
	cout << "writing file " << outfile2 << "....\n with title " << title<<endl; 
	
	plot.CR::SimplePlot::quickPlot( outfile2, xx, yy, empty, empty, "nrsigma", "log counts",	title, 1, false, 1, false, true);
	plot.CR::SimplePlot::PlotLine( xx, yy2,6); //brown
	plot.CR::SimplePlot::PlotLine( xx, yy3,5); //purple
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
	
  
  return nofFailedTests;
}

int main (int argc,
		  char *argv[])
{
	cout<<"This file outputs histograms of pulsar data to /Users/STV/Documents/GiantPulse/."<<endl;
	int integrationlength = 1; //average length
	int triggerlevel = 20;
 	uint nofFailedTests=0;
	float average;
	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	if (argc < 2) {
		std::cout << "Usage: testlightningskymapping [<inputfile.MS>] [<int length of integration>] \n Now using input /Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS \n and output /Users/STV/Documents/GiantPulse/histogram.ps"<< endl;
	} else {
		infile = argv[1];
		if (argc > 2) {
			integrationlength  = atoi(argv[2]);
		};
		if (argc > 3) {
			triggerlevel = atoi(argv[3]);
		};
	}
	
	nofFailedTests += testfun (infile, outfile, integrationlength, triggerlevel);
	
	return nofFailedTests;
}
