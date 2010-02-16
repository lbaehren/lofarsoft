
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
  int freqcomp, timecomp;
  int normalize=0;
  
  /*
    Check if filename of the dataset and the format are provided on the command line.
  */
  if (argc < 2) {
    cerr << "Usage: imagetofits <input.event> <datatype> <frequency compression factor> <time compression factor> <normolize 0/1>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
    return 1;
  } else {
      infile = argv[1];
      if (argc < 3) {
      datatype = "hdf5";
      freqcomp = 513;
	  timecomp = 1;
      } else {
	datatype = argv[2];
	if (argc < 4) {
	  freqcomp = 513;
	  timecomp = 1;
	} else {
	    freqcomp = atoi(argv[3]);
			if (argc < 5) {
				timecomp = 1;
			} else {
				timecomp = atoi(argv[4]);
			} 
			if (argc > 5) {
				normalize = atoi(argv[5]);
			}
		     
          }
        }
    }
  cout<<freqcomp<<endl;

  
  String fitsName = "new.fits";
  String error;
  Bool ok;
  
  // Open the image and convert it to fits:
  if (datatype == "hdf5") {
    HDF5Image<Float> imageIn (infile);
		// Regridding of input file:
		IPosition factors (imageIn.shape());
	    cout << "-- image shape:" << imageIn.shape() << std::endl;

	  
   if(normalize){		
		Array<casa::Float> data;
	   float Naverage;
		cout << "made data" << std::endl;
		IPosition start (5,0,0,0,0,0), end=factors, stride (5,1,1,1,1,1);
	   end(3)=1;
	   data.resize(end,false);
	   cout << "-- data shape: " << data.shape() << std::endl;
	   for(int freqctr=0; freqctr<factors(3); freqctr++){
		start(3)=freqctr;
		//end(3)=1; //end(4)=100;
	    //cout << start(0) << start(1) << start(2) << start(3) << start(4) << endl;
	    //cout << end(0) << end(1) << end(2) << end(3) << end(4) << endl;
		Slicer zeroslicer (start, end, stride);
		
		//cout << "made slicer" << data.shape() << std::endl;
		imageIn.doGetSlice(data,zeroslicer);
		//   cout << data << std::endl;
		//cout << "got slice" << std::endl;
		   Naverage=0;
		//for(int timectr;timectr<end(4);timectr++){
			Naverage=sum(data)/end(4);
		//	cout << "average is: " << Naverage << std::endl;
	     
		   if(Naverage!=0.0){
		      data/=Naverage;
		   }
		imageIn.doPutSlice(data,start,stride);
		   if(freqctr%10==0){cout <<"put slice " << freqctr << " " << data.shape() << std::endl;}
		}
	}
	  
	  		
	  Array<casa::Float> data;
	  
	  cout << "made data" << std::endl;
	  IPosition start (5,0,0,0,0,0), end=factors, stride (5,1,1,1,1,1);
	  
	  
	  
	  end(3)=1; //end(4)=1;
	  data.resize(end,false);
	  Slicer zeroslicer (start, end, stride);
	  
	  cout << "made slicer" << data.shape() << std::endl;
	  imageIn.doGetSlice(data,zeroslicer);
	  cout << "got slice" << std::endl;
	  for(int SB=146;SB<=161;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=317;SB<=323;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=344;SB<=345;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=91;SB<=93;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=309;SB<=310;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=376;SB<=379;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
	  for(int SB=410;SB<=411;SB++){
		  start(3)=SB;
		  imageIn.doPutSlice(data,start,stride);
		  cout <<"put slice" << SB << std::endl;
	  }
	  
		factors    = 1;
		factors(3) = freqcomp;
	    factors(4) = timecomp; //quick hax to sum time data instead
		//factors(0) = 60;
		//factors(1) = 60;

		RebinImage<Float> rb(imageIn, factors);
	    
	
	/* Summary of image properties */
		cout << "-- Summary of the original image:" << std::endl;
		DAL::summary (imageIn);
		cout << "-- Summary of the image after rebinning:" << std::endl;
		DAL::summary (rb);
		
	
		ok = CR::image2fits(rb, fitsName);	
	
		if(ok){
		cout<<"Image converted to "<<fitsName<<"."<<endl;
	  }		
  } else if (datatype == "paged") {
    PagedImage<Float> imageIn (infile);
	// Regridding of input file:
		IPosition factors (imageIn.shape());
	   
		factors    = 1;
		factors(3) = freqcomp;
		RebinImage<Float> rb(imageIn, factors);
        
	 
	/* Summary of image properties */
		cout << "-- Summary of the original image:" << std::endl;
		DAL::summary (imageIn);
		cout << "-- Summary of the image after rebinning:" << std::endl;
		DAL::summary (rb);


		ok = CR::image2fits(rb, fitsName);	
	
		if(ok){
		cout<<"Image converted to "<<fitsName<<"."<<endl;
	  }		
  } else {
    cerr << "Usage: imagetofits <input.event> <datatype> <frequency compression factor>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
  }
 
  return 0;
}
