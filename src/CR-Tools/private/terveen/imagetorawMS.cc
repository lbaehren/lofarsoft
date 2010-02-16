
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
//#include <casa/Arrays/Slicer.h>
using namespace casa;
using namespace std;

#define CHANNELS        100
#define SAMPLES         1500


struct stokesdata {
	/* big endian (ppc) */
	unsigned int  sequence_number;
	//char pad[508];	
	/* little endian (intel) */
	float         samples[CHANNELS][SAMPLES|2];
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

int main(int argc, char *argv[])
{
	string infile;
	string datatype;
	int normalize=0;
	int nrfreqsperblock;
	int firstcoordinate;
	int secondcoordinate;
	int thirdcoordinate;
	int startfreq; int endfreq;
	/*
	 Check if filename of the dataset and the format are provided on the command line.
	 */
	if (argc < 3) {
		cerr << "Usage: imagetofits <input.event> <datatype> <nrfreqsperblock> <startfreq #> <nrblocks>\n"
		<< "Supported datatypes are 'hdf5'. 'paged' is not implemented yet.\n"
		<< "This will dedisperse over time, so that after a collapse in time, the frequency series is left."
		<< "/Users/STV/Astro/data/TBB/Crab/pulse1/skymap6x1_2_6khz_start20_1500blocks.h5"
		<< endl;
		return 1;
	} else {
		infile = argv[1];
		datatype = argv[2];
		nrfreqsperblock = CHANNELS;//atoi(argv[3]);
		startfreq = atoi(argv[4]);
		nrblocks = atoi(argv[5]);
		firstcoordinate = 5;//atoi(argv[6]);
		secondcoordinate = 0;//atoi(argv[7]);
		thirdcoordinate = 0;
	}
	//cout<<freqcomp<<endl;
	
	
	//String fitsName = "temp.fits";
	String error;
	String doit;
	//Bool ok;
	
	// Open the image and convert it to fits:
	if (datatype == "hdf5") {
		cout << "This will alter the original file. Are you sure you want to continue (y/n)";
		cin >> doit;
		if(doit=="y"){ 
			
			
			HDF5Image<Float> imageIn (infile);
			// Regridding of input file:
			
			
			IPosition factors (imageIn.shape());
			cout << "-- image shape:" << imageIn.shape() << std::endl;
			
			
			
			Array<casa::Float> data;
			
			cout << "made data" << std::endl;
			//0longitude, 1latitude, 2length, 3frequency, 4time
			IPosition start (5,firstcoordinate,secondcoordinate,thirdcoordinate,0,0), length=factors, stride (5,1,1,1,1,1);
			length(0)=1;
			length(1)=1;
			length(2)=1;
			length(3)=nrfreqsperblock;
			length(4)=SAMPLES;
			
			
			
			//end(3)=1; //end(4)=1;
			//data.resize(end,false);
			//Slicer zeroslicer (start, end, stride, endIsLast); //endIsLast // endIsLength (default)
			int freqoffset;
			int nrfreqs=factors(3);
			int nrtimes=factors(4);
			for(int blocknr=0;blocknr<nrblocks;blocknr++){
				for(
				
				freqoffset=(endfreq-freq)%dispersion_width;
				//cout << "timeoffset = " << timeoffset << endl;
				
				
				start(3)=startfreq;
				length(3)=1;
				length(4)=nrtimes-freqoffset;
				start(4)=freqoffset;
				data.resize(length,false);
				cout << "start = " << start << endl;
				cout << "length = " << length << endl;
				Slicer zeroslicer (start, length, stride);
				imageIn.doGetSlice(data, zeroslicer);
				start(4)=0;
				cout << "start = " << start << endl;
				cout << "length = " << length << endl;
				imageIn.doPutSlice(data,start,stride);
				cout << "put slice" << freq << std::endl;
			}
			
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
			
		}
		else{
			cout << "aborting ... " << std::endl; 
			return 0; 
		}
	
		//ok = CR::image2fits(rb, fitsName);	
		
		//if(ok){
		//cout<<"Image converted to "<<fitsName<<"."<<endl;
		
	} else if (datatype == "paged") {
		//   PagedImage<Float> imageIn (infile);
		//	// Regridding of input file:
		//		IPosition factors (imageIn.shape());
		//	   
		//		factors    = 1;
		//		factors(3) = freqcomp;
		//		RebinImage<Float> rb(imageIn, factors);
		//        
		//	 
		//	/* Summary of image properties */
		//		cout << "-- Summary of the original image:" << std::endl;
		//		DAL::summary (imageIn);
		//		cout << "-- Summary of the image after rebinning:" << std::endl;
		//		DAL::summary (rb);
		//
		//
		//		ok = CR::image2fits(rb, fitsName);	
		//	
		//		if(ok){
		//		cout<<"Image converted to "<<fitsName<<"."<<endl;
	
	//}		
	}    else {
	cerr << "Usage: imagededisperse <input.event> <datatype> <first frequency> <last frequency> <dispersion width> \n"
	<< "Supported datatypes are 'hdf5'. 'paged' is not implemented yet."
	<< endl;
	}
	
	return 0;
	}
