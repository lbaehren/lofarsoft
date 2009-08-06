/*-------------------------------------------------------------------------*
 | $Id:: snstestlightningskymapping.cc 2009-07-13 swelles                $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
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

#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>
#include <casa/OS/Directory.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <images/Images/PagedImage.h>

using casa::AipsError;
using casa::Array;
using casa::IPosition;
using casa::ObsInfo;
using casa::PagedImage;
using casa::TiledShape;

#include <crtools.h>
#include <Coordinates/SkymapCoordinate.h>
#include <Data/LOFAR_TBB.h>
#include <Analysis/CRinvFFT.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>

using CR::Beamformer;
using CR::ProgressBar;
using CR::SkymapCoordinate;
using CR::Skymapper;
using CR::TimeFreq;

#define HDF5_DISABLE_VERSION_CHECK 1
/*!
  \file snstestnewskymapping.cc

  \ingroup CR_Applications

  \brief Test the Skymapper with different input files with multiple antennas
 
  \author Sander & Sef
 
  \date 2009/08/04

  <h3>Synopsis</h3>

*/

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- input datafile with data input and program configurations
  \param outfile      -- (path-)name of the image-file to be generated
  \param blocksize    -- Number of samples per block of data.

  \return nofFailedTests --  The number of failed tests encountered within this
          fucntion.
*/
//#define upload	//uncomment to upload if it does not compile.

int  simpleImage(string const &infile,
		 string const &outfile,
		 uint const &blocksize=1024)
{
  int nofFailedTests     = 0;

  //_______________________________________________________
  // Read input paramaters from infile
  
     //Opens for reading the file
  ifstream b_file ("../../../src/CR-Tools/apps/snstestnewskymapping.dat"); 
  
  int ninputfiles;
  int nantsinfile;
  int nants=0;
  string pathname;
  int counter = 0;  
  b_file >> ninputfiles;
  b_file >> pathname;
  vector<string> inputfiles(ninputfiles);
  CR::LOFAR_TBB **drstart;
  
  drstart = new CR::LOFAR_TBB*[ninputfiles];
#ifndef upload
  for(int i=0; i<ninputfiles; i++){
  	b_file>> inputfiles[i];
	//cout<<endl<<i<<endl<<endl;
  	drstart[i] = new CR::LOFAR_TBB(pathname+inputfiles[i], blocksize);
	nantsinfile = drstart[i]->fx().shape()[1];
	nants+=nantsinfile;
  }
  cout<<"total number of antennas = "<<nants<<endl;
	cout << "shape of drstart[0]->fx() = " << drstart[0]->fx().shape() << endl;
  cout << "shape of drstart[0]->fft() = " << drstart[0]->fft().shape() << endl;
	cout <<"drstart->fft for antenna "<<0<<" = "<<drstart[0]->fft()[0][0]<<endl;
	cout <<"drstart->fft for antenna "<<1<<" = "<<drstart[0]->fft()[1][0]<<endl;

/*  CR::LOFAR_TBB **dr;
  dr = new CR::LOFAR_TBB*[nants];
  int counter = 0;
  for(int i=0; i<ninputfiles; i++){
		nantsinfile = drstart[i]->fx().shape()[1];
		for(int j=0; j<nantsinfile; j++){
			Vector<Bool> antennasetter(nantsinfile);
			antennasetter = False;
			antennasetter[j]=True;
			cout<<"antennasetter = "<<antennasetter<<endl;
			cout<<"counter = "<<counter<<endl;
		 	//drstart[i]->setSelectedAntennas(antennasetter);
			//cout<<drstart[i]->selectedAntennas();
			dr[counter] = drstart[i];
			counter++;
		}
  }
  
	cout << "shape of dr[0] = " << dr[0]->shape() << endl;
  cout << "shape of drstart[0]->fx() = " << drstart[0]->fx().shape() << endl;
  cout << "shape of dr[0]->fx() = " << dr[0]->fx().shape() << endl;
  cout << "shape of drstart[0]->fft() = " << drstart[0]->fft().shape() << endl;
  cout << "shape of dr[0]->fft() = " << dr[0]->fft().shape() << endl;

	for(int i=0;i<nants;i++){
		cout <<"dr->fft for antenna "<<i<<" = "<<dr[i]->fft()[0][0]<<endl;
	}
*/
	cout <<"drstart->fft for antenna "<<0<<" = "<<drstart[0]->fft()[0][0]<<endl;
	//cout <<"drstart->fft for antenna "<<1<<" = "<<drstart[0]->fft()[1][0]<<endl;

  
  int pixels;
  float increment;
  int depth;
  float startdepth;
  float depthstr;
  
  int nframes;
  int blocksperframe;
  int startblock;

  int ndim;
  int startant;
  int nant;
  int antstr;
  int startfreq;
  int nfreq;
  int freqstr;
  int startcoord;
  int ncoord;
  int coordstr;

  //Read file:
  b_file>> pixels;
  increment = 231./pixels;
  b_file>> depth;
  b_file>> startdepth;
  b_file>> depthstr;
  
  //cout<<"depthstr = " <<depthstr<<endl;
  
  b_file>> nframes;
  b_file>> blocksperframe;
  b_file>> startblock;
  
  //cout<<"startblock = " <<startblock<<endl;
  
  b_file>> ndim;
  b_file>> startant;
  b_file>> nant;
  b_file>> antstr;
  b_file>> startfreq;
  b_file>> nfreq;
  b_file>> freqstr;
  b_file>> startcoord;
  b_file>> ncoord;
  b_file>> coordstr;
  
  cout<<"ncoord = " <<ncoord<<" (should be 3)"<<endl;
  
/*  cout << "pixels: "<<pixels<<endl;
  cout<< "depth: " <<depth<<endl;
  cout<<"blocksperframe: " << blocksperframe<<endl;
  cout<<"starts: " <<start[0]<<" "<<start[1]<<" "<<start[2]<<endl;
*/
	  


  try {    
    
    //________________________________________________________
    cout << "testlightningskymapping::simpleImage reading in event and setting up the pipeline"  << endl;
	
    CR::CRinvFFT pipeline;

    //________________________________________________________
    // Set up the skymap coordinates and infos
     // Observation info
    cout << "testlightningskymapping::simpleImage Setting up ObsInfo"  << endl;
    std::string telescope  = "LOFAR CS302";
    std::string observer   = "John Doe";
    casa::ObsInfo info;
    info.setTelescope(telescope);
    info.setObserver(observer);
    // Spatial coordinates
    cout << "testlightningskymapping::simpleImage Setting up SpatialCoordinate"  << endl;
    std::string refcode    = "AZEL";
    std::string projection = "STG";
    IPosition shape (3,pixels,pixels,depth);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius, refcode,projection);
    spatial.setShape(shape);
    //set reference pixel, reference value, and coord increment
    Vector<double> tmpvec;
    tmpvec = spatial.referencePixel();
    tmpvec(0) = (pixels-1.)/2; tmpvec(1)=(pixels-1.)/2; tmpvec(2)=0.;
    spatial.setReferencePixel(tmpvec);
    tmpvec = spatial.referenceValue();
    tmpvec(0) = 180.; tmpvec(1)=90.;  tmpvec(2)=startdepth;
    spatial.setReferenceValue(tmpvec,true);
    tmpvec = spatial.increment();
    tmpvec(0) = increment; tmpvec(1)=increment;  tmpvec(2)=depthstr;
    spatial.setIncrement(tmpvec,true);
    // Time-Frequency coordinate
    cout << "testlightningskymapping::simpleImage Setting up TimeFreqCoordinate"  << endl;
    uint nofBlocksPerFrame = blocksperframe ;
    uint nofFrames         = nframes;
    TimeFreqCoordinate timeFreq (blocksize, nofBlocksPerFrame, nofFrames,false);
    timeFreq.setNyquistZone(2);
    // Skymap coordinate
    cout << "testlightningskymapping::simpleImage Setting up SkymapCoordinate"  << endl;
    SkymapCoordinate coord (info,
			    spatial,
			    timeFreq);
    
    //________________________________________________________
    // retrieve the antenna positions

    cout << "testlightningskymapping::simpleImage Retrieving the antenna positions"  << endl;
    Matrix<double> antPositions(nants,3);
    for (uInt j=0; j<antPositions.nrow(); j++) {
       for (uInt i=0; i<antPositions.ncolumn(); i++) {
          b_file >> antPositions(j,i);
       }      
    }
		Matrix<double> subantPositions;
		IPosition start_1 (ndim,startant,startcoord), length_1 (ndim,nant,ncoord), stride_1 (ndim,antstr,coordstr);
    Slicer slicer1 (start_1, length_1, stride_1);
		subantPositions = antPositions(slicer1);
		cout << "the antanna positions are: " << subantPositions <<endl;
  

    
    //________________________________________________________
    // Set up the skymapper 
    
    cout << "testlightningskymapping::simpleImage Setting up the Skymapper..."  << endl;
    
    Skymapper skymapper (coord,
			 subantPositions,		// change for number of antennas
			 outfile,
			 Skymapper::HDF5Image);
    
    cout << "                                                         ... done."  << endl;

		skymapper.setFarField();				// Not for imaging lightning!!!
    skymapper.summary();
    
    //________________________________________________________
    // process the event
    
 
		// correct for the offset relative to the last antenna. Be aware that reading from the beginning of the file may give problems.
		
    uint nofBlocks = nofBlocksPerFrame * nofFrames;

    Matrix<casa::DComplex> data(drstart[0]->fftLength(),nants); 
	
		Matrix<casa::DComplex> subdata; 
		IPosition start (ndim,startfreq,startant), length (ndim,nfreq,nant), stride (ndim,freqstr,antstr);
    Slicer slicer (start, length, stride);
		
																							// Sander's new implementation
		casa::Vector<uint> offset(nantsinfile);
		std::vector<int> offset1(nantsinfile);
		int offset_zero;
		offset_zero = drstart[ninputfiles-1]->sample_number()[nantsinfile-1];
		
		cout<<"offset 0 = "<<offset_zero<<endl;
		cout <<"drstart->fft for antenna "<<0<<" = "<<drstart[0]->fft()[0][0]<<endl;
																			
//	casa::Matrix< int > offset(ninputfiles,nantsinfile);
//	casa::Matrix< int > offset1(ninputfiles,nantsinfile);
		
    cout << "Processing block: (out of "<<nofBlocks<<")"<<endl;
    for (uint blocknum=startblock; blocknum<startblock+nofBlocks; blocknum++){
    	//if(blocknum%10==0){
				cout << blocknum-startblock<<endl; 
			//}
			counter = 0;
	 		for(int i=0; i<ninputfiles; i++){
				drstart[i]->setBlock(blocknum);
				nantsinfile = drstart[i]->fx().shape()[1];
				offset = drstart[i]->sample_number();
	  		for(int j=0; j<nantsinfile; j++){
					offset1[j] = -1*offset[j] + offset_zero;
					drstart[i]->setShift(offset1[j]);
					
	  	    cout<<"so far so good. counter i j = "<<counter<<" "<<i<<" "<<j<<endl;
					cout<<drstart[i]->fft()[0][0]<<endl;
    	  	data[counter] = drstart[i]->fft().column(j);
					counter++;
					for(int k=0; k < nfreq*30/100; k++){
						data[i][j] = 0.;
					}
					for(int l=0; l < nfreq*10/100; l++){
						data[i][nfreq-nfreq*10/100+j] = 0.;
					}
 	 		  }
			}
	  	subdata = data (slicer);
    	skymapper.processData(subdata);		// Change if slicer is used!
    };
			
		cout<<"offset = "<<offset<<endl;
		cout<<"offset1 = "<<offset1<<endl;

   cout << "datashape = " << data.shape() << endl;
   cout << "subdatashape = " << subdata.shape() << endl;

  } catch (AipsError x) {
    cerr << "[testlightningskymapping::simpleImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
#endif  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  cout<<"Dit is de door Sef & Sander aangepaste versie van testLOPESskymapping"<<endl;

  uint nofFailedTests=0, blocksize=1024;
  std::string infile, outfile="snstestnewskymapping.img";

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cout << "Usage: testnewskymapping <inputfile.dat> [<output-image>]. Now using snstestnewskymapping.dat" << endl;
	infile = "../../../src/CR-Tools/apps/seftestnewskymapping.dat"; 
  } else {
    infile = argv[1];
     if (argc > 2) {
       outfile  = argv[2];
     };
  }
  
  nofFailedTests += simpleImage (infile,
				 outfile,
				 blocksize);
  
  return nofFailedTests;
}

