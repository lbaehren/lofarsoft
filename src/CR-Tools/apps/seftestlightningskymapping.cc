/*-------------------------------------------------------------------------*
 | $Id:: seftestlightningskymapping.cc 2009-07-13 swelles                $ |
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
#include <IO/LOFAR_TBB.h>
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
  \file seftestlightningskymapping.cc

  \ingroup CR_Applications

  \brief Test the Skymapper with a lightning event
 
  \author Sef Welles
 
  \date 2009/07/13

  <h3>Synopsis</h3>

*/

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- lightning data set to use as data input
  \param outfile      -- (path-)name of the image-file to be generated
  \param blocksize    -- Number of samples per block of data.

  \return nofFailedTests --  The number of failed tests encountered within this
          fucntion.
*/
int  simpleImage(string const &infile,
		 string const &outfile,
		 uint const &blocksize=1024)
{
  int nofFailedTests     = 0;

  try {    
    
    //________________________________________________________
    cout << "testlightningskymapping::simpleImage reading in event and setting up the pipeline"  << endl;
    CR::LOFAR_TBB dr(infile, blocksize); 
	cout << "shape of dr = " << dr.shape() << endl;
	cout << "shape of dr.fx() = " << dr.fx().shape() << endl;
    CR::CRinvFFT pipeline;
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);
    pipeline.SetObsRecord(obsrec);
    pipeline.InitEvent(&dr);
	  
	// correct for the offset relative to antenna 0. Be aware that reading from the beginning of the file may give problems.
	 casa::Vector< uint > offset = dr.sample_number();
     std::vector<int> offset1(offset.shape()[0]);
	 for(int i=0; i<offset.shape()[0]; i++){
	 	offset1[i] = -1*offset[i] + offset[0];
	}
	cout<<"offset = "<<offset<<endl;
	cout<<"offset1 = "<<offset1<<endl;
 	cout<<"shift before = "<<dr.shift(3)<<endl;
	dr.setBlock(230000);
	cout << "somewhere in dr = " << dr.fx().row(0)[3] << endl;

	dr.setShift(offset1);
	cout<<"shift after = "<<dr.shift(3)<<endl;
	dr.setBlock(230000);
	cout << "somewhere in dr = " << dr.fx().row(0)[3] << endl;
	
  //_______________________________________________________
  // Read input paramaters from inputfile seftestlightningskymapping.dat
	  
  int pixels;
  float increment;
  int depth;
  int blocksperframe;
  int start[3]; 
  int length[3];
  int stride[3];
  int  start1[3];
  int length1[3];
  int  stride1[3];
  //Opens for reading the file
  ifstream b_file ( "../../../src/CR-Tools/apps/seftestlightningskymapping.dat" );
  //Read file:
  b_file>> pixels;
  b_file>> increment;
  b_file>> depth;
  b_file>> blocksperframe;
  for (int i=0;i<3;i++){
	  b_file>> start[i];
  }for (int i=0;i<3;i++){
	  b_file>> length[i];
  }for (int i=0;i<3;i++){
	  b_file>> stride[i];
  }for (int i=0;i<3;i++){  
	  b_file>> start1[i];
  }for (int i=0;i<3;i++){  
	  b_file>> length1[i];
  }for (int i=0;i<3;i++){  
	  b_file>> stride1[i];
  }
  
/*  cout << "pixels: "<<pixels<<endl;
  cout<< "depth: " <<depth<<endl;
  cout<<"blocksperframe: " << blocksperframe<<endl;
  cout<<"starts: " <<start[0]<<" "<<start[1]<<" "<<start[2]<<endl;
*/
	  

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
    tmpvec(0) = 180.; tmpvec(1)=90.;  tmpvec(2)=1000.;
    spatial.setReferenceValue(tmpvec,true);
    tmpvec = spatial.increment();
    tmpvec(0) = increment; tmpvec(1)=increment;  tmpvec(2)=10000.;
    spatial.setIncrement(tmpvec,true);
    // Time-Frequency coordinate
    cout << "testlightningskymapping::simpleImage Setting up TimeFreqCoordinate"  << endl;
    uint nofBlocksPerFrame = blocksperframe ;
    uint nofFrames         = 1;
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
    Matrix<double> antPositions(6,3);


    /*
	 0: 3827944.292,459792.072,5063991.294
	 1: 3827944.292,459792.072,5063991.294
	 2: 3827942.397,459789.467,5063992.951
	 3: 3827946.675,459782.252,5063990.392
	 4: 3827956.998,459784.497,5063982.442
	 5: 3827894.790,459786.107,5064028.98
	*/
	
    for (uInt j=0; j<antPositions.nrow(); j++) {
       for (uInt i=0; i<antPositions.ncolumn(); i++) {
          b_file >> antPositions(j,i);
       }      
    }
	
	Matrix<double> subantPositions;
	cout<<start1[0]<<" "<<start1[1]<<" "<<start1[2]<<endl;
	IPosition start_1 (start1[0],start1[1],start1[2]), length_1 (length1[0],length1[1],length1[2]), stride_1 (stride1[0],stride1[1],stride1[2]);
	cout<<"bla1"<<endl;
    Slicer slicer1 (start_1, length_1, stride_1);
	cout<<"bla2"<<endl;
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
	//skymapper.setAntPositions(antPositions)
    skymapper.summary();
    
    //________________________________________________________
    // process the event
    
    
    
    uint nofBlocks = nofBlocksPerFrame * nofFrames;

    Matrix<casa::DComplex> data; 
	Matrix<casa::DComplex> subdata; 
	IPosition start_ (start[0],start[1],start[2]), length_ (length[0],length[1],length[2]), stride_ (stride[0],stride[1],stride[2]);
    Slicer slicer (start_, length_, stride_);
	uint startblocknum = 226170;
    for (uint blocknum=startblocknum; blocknum<startblocknum+nofBlocks; blocknum++){
    	cout << "testlightningskymapping::simpleImage Processing block: " << blocknum << " out of: " << nofBlocks << endl;
      dr.setBlock(blocknum);
	  	cout<<"so far so good..." << endl;
      data = dr.fft();
			cout<<"data.shape() = "<<data.shape()<<endl;
	  	int nfreq = length[1];
			cout<<"nfreq = "<< nfreq<<endl;
			for (int i=0; i < 6; i++){
				for(int j=0; j < nfreq*30/100; j++){
					data.row(i)[j] = 0.;
				}
				for(int j=0; j < nfreq*10/100; j++){
					data.row(i)[nfreq-nfreq*10/100+j] = 0.;
				}
			}
			cout<<"bad data filtered out"<<endl;
      subdata = data (slicer);
      skymapper.processData(subdata);
    };
	
   	cout << "datashape = " << data.shape() << endl;
   	cout << "subdatashape = " << subdata.shape() << endl;

  } catch (AipsError x) {
    cerr << "[testlightningskymapping::simpleImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  cout<<"Dit is de door Sef aangepaste versie van testLOPESskymapping"<<endl;

  uint nofFailedTests=0, blocksize=1024;
  std::string infile, outfile="seftestlightningskymapping.img";

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cout << "Usage: testlightningskymapping <input.event> [<output-image>]. Now using lightning_17_23.h5" << endl;
	infile = "/mnt/lofar/lightning/lightning_17_23.h5"; //contains 6 dipoles, other files are _16_48.h5 and _17_46.h5
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

