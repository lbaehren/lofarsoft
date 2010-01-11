/*-------------------------------------------------------------------------*
 | $Id:: testLOPESskymapping.cc 2403 2009-03-05 15:03:49Z baehren        $ |
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
#include <IO/LopesEventIn.h>
#include <Analysis/CRinvFFT.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>

using CR::Beamformer;
using CR::ProgressBar;
using CR::SkymapCoordinate;
using CR::Skymapper;
using CR::TimeFreq;

#include <iostream> 
#include <fstream>

/*!
  \file testLOPESskymapping.cc

  \ingroup CR_Applications

  \brief Test the Skymapper with a LOPES event
 
  \author Andreas Horneffer
 
  \date 2009/03/09

  <h3>Synopsis</h3>

*/

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- LOPES data set to use as data input
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
    cout << "testLOPESskymapping::simpleImage reading in event and setting up the pipeline"  << endl;
    CR::LopesEventIn dr(infile, blocksize); 
	cout << "shape of dr = " << dr.shape() << endl;
    CR::CRinvFFT pipeline;
	// cout << "shape of pipeline = " << pipeline.shape() << endl; Not possible!
    Record obsrec;
    obsrec.define("LOPES",caltable_lopes);
	//cout << "shape of caltable = " << caltable_lopes.shape() << endl; Not possible!
    pipeline.SetObsRecord(obsrec);
    pipeline.InitEvent(&dr);

	  //_______________________________________________________
	  // Read input paramaters from inputfile seftestLOPESskymapping.dat
	  
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
  ifstream b_file ( "../../../src/CR-Tools/apps/seftestLOPESskymapping.dat" );
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
    cout << "testLOPESskymapping::simpleImage Setting up ObsInfo"  << endl;
    std::string telescope  = "LOPES";
    std::string observer   = "John Doe";
    casa::ObsInfo info;
    info.setTelescope(telescope);
    info.setObserver(observer);
    // Spatial coordinates
    cout << "testLOPESskymapping::simpleImage Setting up SpatialCoordinate"  << endl;
    std::string refcode    = "AZEL";
    std::string projection = "STG";
    IPosition shape (3,pixels,pixels, depth);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius, refcode,projection);
    spatial.setShape(shape);
    //set reference pixel, reference value, and coord increment
    Vector<double> tmpvec;
    tmpvec = spatial.referencePixel();
    tmpvec(0) = (pixels-1.)/2; tmpvec(1)=(pixels-1.)/2;  tmpvec(2)=0.;
    spatial.setReferencePixel(tmpvec);
    tmpvec = spatial.referenceValue();
    tmpvec(0) = 180.; tmpvec(1)=90.;  tmpvec(2)=100000.;
    spatial.setReferenceValue(tmpvec,true);
    tmpvec = spatial.increment();
    tmpvec(0) = increment; tmpvec(1)=increment;  tmpvec(2)=10000.;
    spatial.setIncrement(tmpvec,true);
    // Time-Frequency coordinate
    cout << "testLOPESskymapping::simpleImage Setting up TimeFreqCoordinate"  << endl;
    uint nofBlocksPerFrame = blocksperframe ;
    uint nofFrames         = (dr.headerRecord().asInt("Filesize")/blocksize)/nofBlocksPerFrame;
    TimeFreqCoordinate timeFreq (blocksize, nofBlocksPerFrame, nofFrames,true);
    timeFreq.setNyquistZone(2);
    // Skymap coordinate
    cout << "testLOPESskymapping::simpleImage Setting up SkymapCoordinate"  << endl;
    SkymapCoordinate coord (info,
			    spatial,
			    timeFreq);
    
    //________________________________________________________
    // retrieve the antenna positions

    cout << "testLOPESskymapping::simpleImage Retrieving the antenna positions"  << endl;
    Matrix<double> antPositions;
    Matrix<double> subantPositions;
	cout<<start1[0]<<" "<<start1[1]<<" "<<start1[2]<<endl;
	IPosition start_1 (start1[0],start1[1],start1[2]), length_1 (length1[0],length1[1],length1[2]), stride_1 (stride1[0],stride1[1],stride1[2]);
    Slicer slicer1 (start_1, length_1, stride_1);
    antPositions = pipeline.GetAntPositions(&dr);
	subantPositions = antPositions(slicer1);
    cout << subantPositions <<endl;		// change for number of antennas
    
    //________________________________________________________
    // Set up the skymapper 
    
    cout << "testLOPESskymapping::simpleImage Setting up the Skymapper..."  << endl;
	
	Skymapper skymapper (coord,
			 subantPositions,		// change for number of antennas
			 outfile,
			 Skymapper::HDF5Image);
	skymapper.setFarField();
    cout << "                                                         ... done."  << endl;
    skymapper.summary();
    
    //________________________________________________________
    // process the event for 2 antennas
	
     

    uint nofBlocks = nofBlocksPerFrame * nofFrames;

    Matrix<casa::DComplex> data; 
    Matrix<casa::DComplex> subdata; 
	IPosition start_ (start[0],start[1],start[2]), length_ (length[0],length[1],length[2]), stride_ (stride[0],stride[1],stride[2]);
    Slicer slicer (start_, length_, stride_);
    for (uint blocknum=0; blocknum<nofBlocks; blocknum++){
      cout << "testLOPESskymapping::simpleImage Processing block: " << blocknum << " out of: " 
	   << nofBlocks << endl;
      dr.setBlock(blocknum);
      data = dr.fft();
      subdata = data (slicer);
      skymapper.processData(subdata);	// change for number of antennas
    };
	
   cout << "datashape = " << data.shape() << endl;
   cout << "subdatashape = " << subdata.shape() << endl;

  } catch (AipsError x) {
    cerr << "[testLOPESskymapping::simpleImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  cout<<"Dit is de door Sef aangepaste versie van testLOPESskymapper"<<endl;

  uint nofFailedTests=0, blocksize=1024;
  std::string infile, outfile="seftestLOPESskymapping.img";

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cout << "Usage: testLOPESskymapping <input.event> [<output-image>]. Now using the example.event" << endl;
	infile = "$LOFARSOFT/data/lopes/example.event";
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

