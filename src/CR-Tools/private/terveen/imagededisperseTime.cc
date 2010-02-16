
/*-------------------------------------------------------------------------*
 | $Id:: imagetofits.cc 															                   
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sef Welles                                                            *
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

int main(int argc, char *argv[])
{
	string infile;
	string datatype;
	int normalize=0;
	int dispersion_width;
	/*
	 Check if filename of the dataset and the format are provided on the command line.
	 */
	if (argc < 3) {
		cerr << "Usage: imagetofits <input.event> <datatype> <dispersion_width> \n"
		<< "Supported datatypes are 'hdf5'. 'paged' is not implemented yet.\n"
		<< "This will dedisperse over time, so that after a collapse in time, the frequency series is left."
		<< endl;
		return 1;
	} else {
		infile = argv[1];
		datatype = argv[2];
		dispersion_width = atoi(argv[3]);
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
			IPosition start (5,0,0,0,0,0), length=factors, stride (5,1,1,1,1,1);
			
			
			
			//end(3)=1; //end(4)=1;
			//data.resize(end,false);
			//Slicer zeroslicer (start, end, stride, endIsLast); //endIsLast // endIsLength (default)
			int timeoffset;
			int nrfreqs=factors(3);
			for(int time=0; time<factors(4); time++){
				
				
				timeoffset=time%dispersion_width;
				//cout << "timeoffset = " << timeoffset << endl;
				
				
				start(3)=0;
				length(3)=nrfreqs-timeoffset;
				length(4)=1;
				start(4)=time;
				data.resize(length,false);
				cout << "start = " << start << endl;
				cout << "length = " << length << endl;
				Slicer zeroslicer (start, length, stride);
				imageIn.doGetSlice(data, zeroslicer);
				start(3)=timeoffset;
				cout << "start = " << start << endl;
				cout << "length = " << length << endl;
				imageIn.doPutSlice(data,start,stride);
				cout <<"put slice" << time << std::endl;
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
